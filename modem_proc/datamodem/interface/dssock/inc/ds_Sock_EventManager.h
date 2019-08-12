#ifndef DS_SOCK_EVENTMANAGER_H
#define DS_SOCK_EVENTMANAGER_H

/*===========================================================================
  @file EventManager.h

  This file provides the EventManager class.

  TODO: Write detailed explanation.

  Copyright (c) 2008 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/inc/ds_Sock_EventManager.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-02 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Sock
  {
    namespace EventManager
    {
      void Init
      (
        void
      );

      void Deinit
      (
        void
      );

    } /* namespace EventManager */
  } /* namespace Sock */
}/* namespace ds */

#endif /* DS_SOCK_EVENTMANAGER_H */
