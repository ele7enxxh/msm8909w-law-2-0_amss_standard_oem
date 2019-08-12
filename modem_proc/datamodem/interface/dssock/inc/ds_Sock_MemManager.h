#ifndef DS_SOCK_MEMMANAGER_H
#define DS_SOCK_MEMMANAGER_H

/*===========================================================================
  @file SocketMemManager.h

  This file defines functions for translation of Class id to pool id macros.
  It also provides default initializations of PS mem pool objects for all
  the different DSSOCK objects.

  Copyright (c) 2008,2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/inc/ds_Sock_MemManager.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-01-10 ts  Adding SCRATCHPAD for ancillary data buffers.
  2008-06-30 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "ps_mem_ext.h"
#include "ps_mem.h"

/*===========================================================================

                     PUBLIC DATA DEFINITIONS

===========================================================================*/
namespace ds
{
namespace Sock
{

/*!
  @brief      
  Static class to provide memory management functionality to DSSOCK objects.

  @details
  This class acts as a wrapper on top of PS Mem API to provide memory for
  DSSOCK objects. It also initializes the PS MEM pools for DSSOCK during
  powerup.

  @see        ps_mem.h
  @see        ps_mem_ext.h
*/
    class MemManager
    {
      public:
        /*!
        Initializes the PS Mem pools used by DSSOCK during powerup. 
      
        @param      None.
        @see        ps_mem_pool_init()
        @return     None.
        @note       PS Mem library should be initialized.
        */
        static void MemPoolInit  
        (
          void
        );

    }; /* class MemManager */  
  } /* namespace Sock */
} /* namespace DS */

#endif /* DS_SOCK_MEMMANAGER_H */
