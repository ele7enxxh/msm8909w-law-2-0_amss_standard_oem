#ifndef DS_UTILS_ONDEMANDSIGNALBUS_H
#define DS_UTILS_ONDEMANDSIGNALBUS_H

/*===========================================================================
  @file ds_Utils_OnDemandSignalBus.h

  Copyright (c) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ds_Utils_OnDemandSignalBus.h#1 $
  $DateTime: 2016/12/13 08:00:03 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-07-19 mt  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_ICritSect.h"
#include "ds_Utils_ISignalBus.h"
#include "ds_Utils_ISignal.h"
#include "ds_Errors_Def.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

namespace ds
{
  namespace Utils
  {

    /*!
      @class
      ds::Utils::OnDemandSignalBus

      @brief
      "Lazy" wrapper around the ISignalBus interface, which will create a real
      SignalBus when necessary.

      @details
      Provides a "lazy" signal bus wrapper that creates an actual
      underlying signal bus only when necessary, with the expectation that
      many signal buses will actually never be used (no signals will be added
      to them) but the signal bus state must still be tracked.

      The lazy optimization allows us to save ~5 SignalBuses per socket (which
      figures to at least 250 SignalBuses less needed when using static pools).
      With dynamic pools, this object is still smaller by some 20 bytes, and
      multiplying by 7 buses in every socket, the memory savings add up.

      An instance of this class must be initialized by calling Init() before
      use.

      This class is intended to be used as a class member, not as
      a dynamically created object.

      This class does not inherit from ISignalBus, so you cannot pass
      an instance of this class outside of your object to someone else
      expecting to hold onto an ISignalBus. Use GetBus() instead.

      @see AEEISignalBus.h
    */
    /*lint -esym(1510, ISignalBus) */
    /*lint -esym(1510, IQI) */
    class OnDemandSignalBus
    {
    public:
      /*---------------------------------------------------------------------
        Constructor and destructor
      ---------------------------------------------------------------------*/
      OnDemandSignalBus(
        void
      );

      /*! @brief Initialize the bus

      @retval If not AEE_SUCCESS, initialization has failed.
      */
      ds::ErrorType Init(
        void
      );

      ~OnDemandSignalBus(
        void
      );

      /*! @brief Release statically held resources

      This function should be called once during powerdown sequence.
      */
      static void DeInit(
        void
      );

      /*---------------------------------------------------------------------
        Public members
      ---------------------------------------------------------------------*/
      /*! @brief Obtain the real SignalBus

      If a bus didn't exist yet, it will be created. It is your responsibility
      to release the obtained reference.

      @retval Pointer to the bus on success, 0 if a bus could not be allocated.
      */
      ISignalBus* GetBus(
        void
      );

      /*! @brief Release the resources held by the bus

      If the user of this object is weak reference-enabled, this function
      should be called from its Destructor() to explicitly release
      the internally-held and -allocated resources.

      For other users, the resources will be cleaned by the
      OnDemandSignalBus destructor.

      @note An instance may no longer be used after Release() was called.
      */
      void Release(
        void
      )
      throw();

      /*---------------------------------------------------------------------
        The syntax of the following members is the same as in ISignalBus.
        They will create an underlying SignalBus if necessary.
      ---------------------------------------------------------------------*/
      ds::ErrorType Add
      (
        ISignal* piSignal
      );

      ds::ErrorType Remove
      (
        ISignal* piSignal
      );

      ds::ErrorType Set
      (
        void
      );

      ds::ErrorType Clear
      (
        void
      );

      ds::ErrorType Strobe
      (
        void
      );

      ds::ErrorType OnEnable
      (
        ISignal* piSignal
      );

    private:
      /*---------------------------------------------------------------------
        Private members
      ---------------------------------------------------------------------*/
      /* The real SignalBus implementation */
      ISignalBus *busPtr;

      /* State of the bus (set / cleared).

      The state is only tracked until the real SignalBus is created. After
      that, following the state is no longer necessary */
      boolean  isSet;

      /* Cached Enable signal */
      ISignal *onEnablePtr;

      static ICritSect *critSectPtr;

      /* Ensure this class is not dynamically allocated */
      void* operator new (
        unsigned int num_bytes
      )
      throw();

      /* Helper to check and allocate a signal bus member */
      ds::ErrorType CheckAndAllocateRealBus(
        void
      );

    }; /* class OnDemandSignalBus */

  } /* namespace Utils */
}/* namespace ds */

#endif /* DS_UTILS_ONDEMANDSIGNALBUS_H */

