#ifndef DS_SOCK_2_NETWORK_CTL_H
#define DS_SOCK_2_NETWORK_CTL_H

/*===========================================================================
@file ds_Sock_Sock2NetworkCtl.h

This file defines the Sock2NetworkCtl class.

This class extends NetworkCtl class
with Socket2 specific network functionality

Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
EDIT HISTORY FOR MODULE

Please notice that the changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_Sock2NetworkCtl.h#1 $
$DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

when       who what, where, why
---------- --- ------------------------------------------------------------
2010-01-06 vm  Created module.

===========================================================================*/
/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_StdErr.h"
#include "ds_Sock_NetworkCtl.h"
#include "ds_Utils_ICritSect.h"

/*===========================================================================

FORWARD DATA DECLARATIONS

===========================================================================*/
/*===========================================================================

PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Sock
  {
    class Sock2NetworkCtl : public NetworkCtl
    {

    public:

      /**
        @brief Returns a Sock2NetworkCtl object.

        @param None

        @retval address  Sock2NetworkCtl is created successfully
        @retval 0        Out of memory
      */
      static Sock2NetworkCtl * CreateInstance
      (
        void
      );

      /**
        @brief Sock2NetworkCtl class constructor

        Initializes a Sock2NetworkCtl object

        @param None

        @retval None
      */
      Sock2NetworkCtl
      (
        void
      );

      /**
        @brief Destructor for Sock2NetworkCtl class.

        @param None

        @retval None
      */
      virtual ~Sock2NetworkCtl
      (
        void
      )
      throw();

      /**
        @brief Clones Sock2NetworkCtl object properties to provided
        NetworkCtl object properties.

        Virtual constructor design pattern implementation

        @param[in] critSect socket critical section
        @param[out] networkCtlPtr resulting Sock2NetworkCtl object

        @retval AEE_SUCCESS      Sock2NetworkCtl is cloned
        successfully
        @retval AEE_ENOMEMORY   No memory for Sock2NetworkCtl
        clone

        @see ds::Error
      */
      virtual ds::ErrorType Clone
      (
        ICritSect          *critSect,
        NetworkCtl*        *networkCtlPtr
      );

      /**
        @brief Tells if Network Policy object can be updated.

        @param None

        @retval TRUE  Policy object can be updated
        @retval FALSE Policy object can't be updated

      */
      virtual boolean IsPolicyUpdateEnabled
      (
        void
      );

    private:

      /**
        @brief Overloaded operator new.

        Operator new is overloaded so that memory for Sock2NetworkCtl is
        allocated from ps_mem instead of from the system heap.

        As the C Runtime Library throws a std::bad_alloc exception if the
        memory allocation fails, and since memory allocation could fail as
        memory is allocated from ps_mem, this method also handles the
        exception.

        @param[in] numBytes  The amount of memory to be allocated

        @retval address  Ptr to object if memory is allocated
        @retval 0        If memory couldn't be allocated
      */
      static void * operator new
      (
        unsigned int numBytes
      ) throw();

      /**
        @brief Overloaded operator delete.

        Operator delete is overloaded so that memory for Sock2NetworkCtl is
        de-allocated and is put back in to ps_mem

        @param[in] bufPtr  The object to be deleted

        @retval None
      */
      static void operator delete
      (
        void *  bufPtr
      ) throw();

    };/* class Sock2NetworkCtl */
  } /* namespace Sock */
} /* namespace ds */

#endif /* DS_SOCK_2_NETWORK_CTL_H */
