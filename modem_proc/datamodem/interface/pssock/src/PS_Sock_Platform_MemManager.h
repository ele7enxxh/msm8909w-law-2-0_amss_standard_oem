#ifndef PS_SOCK_PLATFORM_MEMMANAGER_H
#define PS_SOCK_PLATFORM_MEMMANAGER_H
/*===========================================================================
  @file PS_Sock_Platform_MemManager.h

  This file defines functions for translation of Class id to pool id macros.
  It also provides default initializations of PS mem pool objects for all
  the different DSNet objects.

  Copyright (c) 2008 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/PS_Sock_Platform_MemManager.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-06-30 hm  Created module.

===========================================================================*/


/*===========================================================================

                     PUBLIC DATA DEFINITIONS

===========================================================================*/
namespace PS
{
  namespace Sock
  {
    namespace Platform
    {
      namespace MemManager
      {
        void Init
        (
          void
        );

      } /* namespace MemManager */
    } /* namespace Platform */
  } /* namespace Sock */
} /* namespace PS */

#endif /* PS_SOCK_PLATFORM_MEMMANAGER_H */
