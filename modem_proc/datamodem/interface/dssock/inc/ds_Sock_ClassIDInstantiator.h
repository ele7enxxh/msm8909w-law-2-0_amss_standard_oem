#ifndef DS_SOCK_CLASSIDINSTANTIATOR_H
#define DS_SOCK_CLASSIDINSTANTIATOR_H
/*===========================================================================
  @file ds_Sock_ClassIDInstantiator.h

  This file defines various methods which are used to create instances of
  socket interfaces.

  Copyright (c) 2008 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/inc/ds_Sock_ClassIDInstantiator.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "ds_Utils_StdErr.h"
#include "ds_Utils_CSSupport.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
DSIQI_DECL_CREATE_INSTANCE2( DS, Sock, SocketFactory)
DSIQI_DECL_CREATE_INSTANCE2( DS, Sock, SocketFactoryPriv)

#endif /* DS_SOCK_CLASSIDINSTANTIATOR_H */
