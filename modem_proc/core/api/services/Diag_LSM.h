#ifndef DIAG_LSM_H
#define DIAG_LSM_H
/*!
@ingroup diag_misc
  @file Diag_LSM.h 
  @brief
  Diagnostic Services API definitions specific to Windows devices

  This file contains APIs necessary to initialize Diagnostic services on 
  Windows devices. This additional initialization is necessary to support
  Diagnostic services on multiple process-spaces. On Windows phones, 
  Diagnostic Service API definitions are provided by the dynamic link library,
  Diag_LSM.dll.
  
  @note
  This file should be used only on Windows devices (Win-mobile 6.x and Windows Phone 7).
*/  

/*
Copyright (c) 2007-2010 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/Diag_LSM.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/30/10   mad     Modified comment for WINDIAG_TASK_INIT_COMPLETE_EVT_NAME
06/02/10   mad     Doxygenated
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

/*!
@defgroup diag_misc Miscellaneous Diagnostic Service interfaces
This module describes some miscellaneous interfaces to diagnostic services.
*/

/*!
@ingroup diag_misc
Name of Windows synchronization event that clients can wait on, to get 
notification that Diag is ready for use. Clients that come up earlier in the
boot-up sequence can wait on this event, before calling Diag_LSM_Init() or 
using other Diag APIs. 
 
Clients need to obtain event HANDLE with the flag settings given as follows:
CreateEvent(NULL, FALSE, FALSE, WINDIAG_TASK_INIT_COMPLETE_EVT_NAME);
*/
#define WINDIAG_TASK_INIT_COMPLETE_EVT_NAME    TEXT("Windiag_Task_Initialized")

/*!
@ingroup diag_misc
@brief
   Initializes the Diag API Layer for the client process-space

   @param[in] *pParam  Not used. Place-holder for any future parameters.
   
   @dependencies
   Successful initialization requires Diag driver (windiag.dll) to be loaded 
   and available in the system.

   @return
   <ul>
      <li> FALSE - Initialization of Diag API layer failed
      <li> TRUE  - Initialization of Diag API layer succeeded
   </ul>

   @sideeffects
   None

   @sa
   Diag_LSM_DeInit

   @note
   Clients should call Diag_LSM_DeInit() to clean-up the memory structures on exit, 
   to avoid memory leaks.
*/
#ifdef __cplusplus
extern "C" {
#endif
boolean Diag_LSM_Init (byte* pParam);

/*!
@ingroup diag_misc
@brief
   De-Initialize the Diag API layer for the client process-space.
   This function should be called, to re-claim memory allocated in Diag_LSM_Init().

   @dependencies
   None

   @return
   <ul>
      <li> FALSE - De-initialization of Diag API layer failed
      <li> TRUE  - De-initialization of Diag API layer succeeded
   </ul>

   @sideeffects
   None

   @sa
   Diag_LSM_Init
*/
boolean Diag_LSM_DeInit(void);
#ifdef __cplusplus
}
#endif

#endif /* DIAG_LSM_H */
