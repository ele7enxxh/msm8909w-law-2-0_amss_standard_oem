#ifndef IPCERROR_H
#define IPCERROR_H

/**
   @file ipcerror.h

   @brief 
   Generic IPC Error Services Header File.

   Externalized Functions:
   IPC_ERROR.

   Initialization and sequencing requirements:
   None.
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   I P C    E R R O R    S E R V I C E S

Copyright (c) 2005-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/ipcerror.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/11/11   leo     (Tech Pubs) Edited Doxygen comments and markup.
12/07/10   nk      Doxygenated.
11/29/05   ws      Initial Revision.

===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdio.h>

#ifdef __cplusplus
__CPLUSPLUS_GUARD_BEGIN
#endif


/*===========================================================================

                        PUBLIC DATA DECLARATIONS

===========================================================================*/
/** @addtogroup SMECOMUTIL_IPC 
@{
*/

/**
   Maximum IPC message length.
*/
#define IPC_MSG_LENGTH_MAX       256

/**
   Posts an IPC error message.

   @param[in] ipc_msg Message describing the IPC-related error.

   @sideeffects 
   Notifies potential clients.
*/
#define IPC_ERROR( format, ipc_arg1, ipc_arg2, ipc_arg3 )  \
   do {  \
         static char ipc_msg[IPC_MSG_LENGTH_MAX];  \
         snprintf( ipc_msg, sizeof(ipc_msg), format, (uint32)(ipc_arg1), (uint32)(ipc_arg2), (uint32)(ipc_arg3));  \
         __ipc_error( __FILE__, __LINE__, ipc_msg );  \
   } while (0)

/**
   Reports an IPC-related error.

   @note1hang Do not call this function directly; use IPC_ERROR() instead.

   @param[in] filename Character pointer to the file name.
   @param[in] line     Line number of type long.
   @param[in] message  Character pointer to the message.

   @return
   None.

   @dependencies 
   None.

   @sideeffects 
   Notifies potential clients.
*/
void __ipc_error( char* filename, long line, char* message);

/** @} */  /* end_addtogroup SMECOMUTIL_IPC */

#ifdef __cplusplus
__CPLUSPLUS_GUARD_END
#endif

#endif /* IPCERROR_H */
