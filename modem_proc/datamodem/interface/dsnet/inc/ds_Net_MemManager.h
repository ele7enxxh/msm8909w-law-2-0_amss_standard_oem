#ifndef DS_NET_MEMORY_MANAGER_H
#define DS_NET_MEMORY_MANAGER_H

/*==========================================================================*/ 
/*! 
  @file 
  ds_Net_MemManager.h

  @brief
  This file provides methods to initialize the PS Mem pools used by
  DSNET module during powerup. It also provides a wrapper function
  on top of ps_mem_get_buf() to get the DSNET buffers.

  Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/inc/ds_Net_MemManager.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
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
namespace Net
{

/*!
  @brief      
  Static class to provide memory management functionality to DSNET objects.

  @details
  This class acts as a wrapper on top of PS Mem API to provide memory for
  DSNET objects. It also initializes the PS MEM pools for DSNET during
  powerup.

  @see        ps_mem.h
  @see        ps_mem_ext.h
*/
class MemoryManager
{
public:
  /*!
  Initializes the PS Mem pools used by DSNET during powerup. 

  @param      None.
  @see        ps_mem_pool_init()
  @return     None.
  @note       PS Mem library should be initialized.
  */
  static void MemPoolInit 
  (
    void
  );
      
}; /* class MemoryManager */  
} /* namespace Net */
} /* namespace ds */

#endif /* DS_NET_MEMORY_MANAGER_H */
