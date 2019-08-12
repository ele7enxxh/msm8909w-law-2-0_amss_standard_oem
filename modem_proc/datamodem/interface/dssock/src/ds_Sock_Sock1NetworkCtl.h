#ifndef DS_SOCK_1_NETWORK_CTL_H
#define DS_SOCK_1_NETWORK_CTL_H

/*===========================================================================
@file ds_Sock_Sock1NetworkCtl.h

This file defines the Sock1NetworkCtl class.

This class extends NetworkCtl class
with Socket1 specific network functionality

Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
EDIT HISTORY FOR MODULE

Please notice that the changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_Sock1NetworkCtl.h#1 $
$DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

when       who what, where, why
---------- --- ------------------------------------------------------------
2010-01-05 vm  Created module.

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
    class Sock1NetworkCtl : public NetworkCtl
    {

    public:

      /**
        @brief Returns a Sock1NetworkCtl object.

        @param None

        @retval address  Sock1NetworkCtl is created successfully
        @retval 0        Out of memory
      */
      static Sock1NetworkCtl * CreateInstance
      (
        void
      );

      /**
        @brief Sock1NetworkCtl class constructor

        Initializes a Sock1NetworkCtl object

        @param None

        @retval None
      */
      Sock1NetworkCtl
      (
        void
      );

      /**
        @brief Destructor for Sock1NetworkCtl class.

        @param None

        @retval None
      */
      virtual ~Sock1NetworkCtl
      (
        void
      )
      throw();

      /**
        @brief Clones Sock1NetworkCtl object properties to provided
        NetworkCtl object properties.

        Virtual constructor design pattern implementation

        @param[in] critSect socket critical section
        @param[out] networkCtlPtr resulting Sock1NetworkCtl object

        @retval AEE_SUCCESS      Sock1NetworkCtl is cloned
        successfully
        @retval AEE_ENOMEMORY   No memory for Sock1NetworkCtl
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

        Operator new is overloaded so that memory for Sock1NetworkCtl is
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

        Operator delete is overloaded so that memory for Sock1NetworkCtl is
        de-allocated and is put back in to ps_mem

        @param[in] bufPtr  The object to be deleted

        @retval None
      */
      static void operator delete
      (
        void *  bufPtr
      ) throw();

    };/* class Sock1NetworkCtl */
  } /* namespace Sock */
} /* namespace ds */

#endif /* DS_SOCK_1_NETWORK_CTL_H */
