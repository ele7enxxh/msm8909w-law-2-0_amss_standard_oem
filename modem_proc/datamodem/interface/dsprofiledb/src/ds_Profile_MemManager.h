#ifndef DS_PROFILE_MEMORY_MANAGER_H
#define DS_PROFILE_MEMORY_MANAGER_H

/*==========================================================================*/ 
/*! 
  @file 
  ds_Profile_MemManager.h

  @brief
  This file provides methods to initialize the PS Mem pools used by
  DS PROFILE DB module during powerup. It also provides macro to be used 
  in classes that wish to use managed memory usage

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsprofiledb/src/ds_Profile_MemManager.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-06-08 am  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ps_mem_ext.h"
#include "ps_mem.h"
#include "comdef.h"
/*===========================================================================

                     PUBLIC DATA DEFINITIONS

===========================================================================*/
#define DS_PROFILE_DB_USE_MEMORY_MANAGEMENT(ps_mem_pool_id)                 \
  inline void* operator new                                                 \
  (                                                                         \
    unsigned int num_bytes                                                  \
  )                                                                         \
  throw()                                                                   \
  {                                                                         \
    (void) num_bytes;                                                       \
    return ps_mem_get_buf(ps_mem_pool_id);                                  \
  }                                                                         \
                                                                            \
  inline void operator delete                                               \
  (                                                                         \
    void* objPtr                                                            \
  )                                                                         \
    throw()                                                                 \
  {                                                                         \
    PS_MEM_FREE(objPtr);                                                    \
    objPtr = NULL;                                                          \
  }

namespace ds
{
  namespace Profile
  {

    /*!
      @brief      
      Static class to provide memory management functionality to DSNET objects.

      @details
      This class acts as a wrapper on top of PS Mem API to provide memory for
      DS PROFILE DB objects. It also initializes the PS MEM pools for DSNET during
      powerup.

      @see        ps_mem.h
      @see        ps_mem_ext.h
    */
    class MemoryManager
    {
      public:
        /*!
        Initializes the PS Mem pools used by DS PROFILE DB during powerup. 

        @param      None.
        @see        ps_mem_pool_init()
        @return     0 in case of SUCCESS, any other value in case of ERROR
        @note       PS Mem library should be initialized.
        */
        static int MemPoolInit 
        (
          void
        );
      
    }; /* class MemoryManager */  
  } /* namespace Net */
} /* namespace ds */

#endif /* DS_PROFILE_MEMORY_MANAGER_H */
