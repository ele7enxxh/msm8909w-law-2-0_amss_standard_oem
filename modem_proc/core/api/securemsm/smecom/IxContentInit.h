#ifndef __IXCONTENTINIT_H__
#define __IXCONTENTINIT_H__


/**
   @file  IxContentInit.h

   @brief 
   Interface defination for IxContent Initialization class.

   This header file defines the interface for IxContent Initialization class.

   Externalized Functions:
   None.

   Initialization and sequencing requirements:
   None.
*/

/*===========================================================================
                    I X  C O N T E N T  I N I T
                       H E A D E R  F I L E

Copyright (c) 2006-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/IxContentInit.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/11/11   leo     (Tech Pubs) Edited Doxygen comments and markup.
12/10/10   nk      Doxygenated.
01/19/09   yh      Merge CL819969 Remove IxTimeofDayServer task by collapsing it with IxTimeofDay change back
01/19/09   yh      CR168013 only, signal overlapping correction

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

============================================================================*/

#include "comdef.h"
#include "rtedef.h"

/** @addtogroup SMECOMUTIL_ENCODERS 
@{ 
*/

/** @name IxContent Server Signal Constants 
@{
*/

/**
  Defines the IPC signal for the content service task.
*/
#define IXCONTENTSERVER_IPC_SIG    0x100

/**
   Defines the signal used by the IxContent initialization timer.
*/
#define IXCONTENT_INIT_TIMER_SIG   0x200 

/**
  Defines the watchdog report signal for the content service task.
*/
#define IXCONTENT_RPT_TIMER_SIG    0x400

/**
  Defines the signal used by content-related tasks to ACK the content service
   task.
*/
#define IXCONTENT_ACK_SIG          0x800

/**
   Defines the signal used by the Content task to signal completion of NV
   commands.
*/
#define IXCONTENT_NV_SIG          0x1000

/**
  Defines the signal used by the content task to access time services.
*/
#define IXCONTENT_TIME_WAIT_SIG   0x0001

/** @}  */ /* end_name IxContent Server Signal Constants */

#ifdef __cplusplus
__CPLUSPLUS_GUARD_BEGIN
#endif

/**
   Content IPC signal for synchronization.

   This function blocks until the IxContent main task is done with the IPC
   initialization.

   @return
   None.

   @dependencies 
   None.
*/
void IxContentIPCSync (void);

/**
   Content main function.

   This function is an entry point for the main IxContent task.

   @param[in] dword Double word variable.

   @return
   None.

   @dependencies 
   None.
*/
void IxContentMain (dword);

/** @} */  /* end_addtogroup SMECOMUTIL_ENCODERS */

#ifdef __cplusplus
__CPLUSPLUS_GUARD_END
#endif

#endif /* IXCONTENTINIT_H */

