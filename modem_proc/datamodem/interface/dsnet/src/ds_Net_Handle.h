#ifndef DS_NET_HANDLE_H
#define DS_NET_HANDLE_H

/*==========================================================================*/
/*!
  @file
  ds_Net_Handle.h

  @brief
  This file defines the class that implements the IEventManager interface.

  @details
  This file exports the network handle object, from which all network
  related objects like Network, Qos, Mcast etc., derive.

  The Handle exports the concept of handle (or identifier) for any
  network objects. Instances of the Handle may not be directly created.
  Only instances of derived classes like Network, Qos, Mcast etc.,
  can be created.

  The Handle class also exports Event registration and reporting mechanism.
  The applications wishing to receive event notification from a certain
  interface call RegEvent() method of that interface (that interface should
  derive from IHandle). The signal registered is set when the event is
  triggered.

  Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_Handle.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-03-10 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_ISignal.h"
#include "ds_Utils_ISignalBus.h"
#include "ds_Utils_StdErr.h"
#include "ds_Errors_Def.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_INode.h"
#include "ds_Utils_Factory.h"
#include "ds_Utils_ICritSect.h"
#include "ds_Net_IEventManager.h"
#include "ps_system_heap.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Net
{

/*lint -esym(1510, IQI) */
/*lint -esym(1510, IEventManager) */

/*!
  @class ds::Net::Handle

  @brief
  This class provides the implementation of the IEventManager interface.

  @details
  The Handle class acts as a base class for many of the network objects.
  The primary purpose of the handle class is to provide mechanism for
  event registration and signalling. The IEventManager interface exports
  the following methods:
  1. RegEvent() : Allow registration of a certain event with a given
     ISignal object. There is no function for de-registration. To dereg
     a signal, Release() should be called on the signal object itself.

  Apart from the methods of IEventManager interface, Handle class also
  supports couple of protected methods. These methods are only available
  to internal DSNET objects and are not exported via IEventManager
  interface:
  1. Notify(): This function is to notify the client when an event occurs.
     The client is notified via the signal registered for the given event.
  2. SetHandle(): This method sets the handle in the object. The Handle
     is usually set during creation of the object. The handle can also be
     set when the underlying network handle becomes invalid for the object.
  3. GetHandle(): This method returns the underlying handle identifier for
     the object. If this is a Network object, then this handle would refer
     to the PS iface pointer. If the handle is QoS object, then this handle
     would represent the PS flow pointer associated with the network.

  @see      IEventManager interface.
  @see      ds::Utils::INode
  @see      ds::Utils::Node
*/
class Handle: public IEventManager,
              public ds::Utils::INode
{

private:
  int32                     mObjHandle;
  ds::Utils::IFactory      *mpParentFactory;  /* Parent for this handle */

  /**
    This class implements the IQI interface and used
    to create regObj in RegEvent function.
  */
  class RegBound : public IQI
  {
    public:
      RegBound():refCnt(1){};
      virtual ~RegBound() throw() {};
      void * operator new
      (
        unsigned int numBytes
      ) throw()
      {
        void * buf = NULL;
        PS_SYSTEM_HEAP_MEM_ALLOC(buf, sizeof (RegBound),
                                 void*);
        return buf;
      }

     void operator delete
     (
       void *  bufPtr
     ) throw()
     {
       if(NULL != bufPtr)
       {
         PS_SYSTEM_HEAP_MEM_FREE (bufPtr);
           return;
       }
     }

     DSIQI_DECL_LOCALS()
     DSIQI_ADDREF()
     DSIQI_RELEASE()
     virtual int CDECL QueryInterface (AEEIID iid, void **ppo)
     {
       switch (iid)
       {
         case AEEIID_IQI:
           *ppo = reinterpret_cast <void *> (this);
           (void) AddRef ();
           break;

         default:
           return AEE_ECLASSNOTSUPPORT;
       }

       return AEE_SUCCESS;
     }
  };

protected:
  /*!
    This is the critical section used by all objects derived from handle.
    DO NOT define multiple crit sects per object
  */
  ICritSect*                mpICritSect;
  int32                     mStaleIfaceHandle;

  /*!
  @brief
  Handle object constructor.

  @details
  Constructs the object with a known handle.

  @param[in]  objHandle - Object handle.

  @see        ds::Net::EventManager's different object lists.
  @return     None
  */
  Handle
  (
    int32 objHandle = 0
  )
  throw();

  /*!
  @brief
  Init method for handle object.

  @params     pIFactory - Factory object which this handle
  would be part of.

  @return     AEE_SUCCESS   - if successful
  @return     AEE_ENOMEMORY - no memory 
  @return     QDS_EFAULT    - invalid parameters
  */
  ds::ErrorType Init
  (
    ds::Utils::IFactory *pIFactory
  )
  throw();

  /*!
  @brief
  Handle object's destructor.

  @params     None.
  @return     None.
  */
  virtual void Destructor
  (
    void
  )
  throw();

  /*!
  @brief
  Dummy destructor. Only used to free memory.

  @details
  For objects that support weak references, destruction of the object and
  freeing up of memory can happen at different times. Hence Destructor()
  is the one which performs actual destruction of the object, where as
  ~Object() would perform memory cleanup.
  @params     None.
  @return     None.
  */
  virtual ~Handle
  (
    void
  )
  throw();

  /*!
  @brief
  Protected function to set the objects handle during initialization.

  @details
  Some times the handle is not known during the time of construction
  of the object. For example, before performing network lookup, the
  underlying PS iface that the object would get associated is not known.
  Hence, when the lookup is complete, the network object calls the SetHanel()
  method to set the object handle.

  @params     mObjHandle - Handle that needs to be set.
  @return     None.
  */
  void SetHandle
  (
    int32 objHandle
  );

  /*!
  @brief
  An event specific to this handle has happened. Inform the application if
  it is interested in this event.

  @details
  The Notify() event is called during event processing. The Notify() method
  is specific to a certain handle and means that event corresponding to this
  handle has occurred. The Notify method further traverses its internal
  list to figure out if APP had registered for this event and if so, it
  sets the signal on the same.

  @param[in]  eventName - Event occurred.

  @see        ds::Net::EventManager - For details on the event framework.
  @return     None.
  */
  virtual void Notify
  (
    int32 eventName
  );

public:
  /*---------------------------------------------------------------------
    Inherited functions from IEventManager interface.
  ---------------------------------------------------------------------*/
  /*!
  @brief
  Register for an event using a given signal object.

  @details
  This function registers the signal provided by the application for the
  given event. The different events that can be registered are dependent
  upon the individual interfaces.

  @param[in]  pISignal - Signal object to be registered.
  @param[in]  eventName - Name of the event to be registered.
  @return     AEE_SUCCESS - if successful
  @return     QDS_EFAULT - On invalid arguments.
  @return     QDS_EINVAL - Registering for unsupported event.
  */
  virtual ds::ErrorType CDECL OnStateChange
  (
    ISignal             *pISignal,
    ds::Net::EventType   eventName,
    IQI** regObj
  );

  /*---------------------------------------------------------------------
    Update IEventManager interface to export RegEvent() instead of
    OnStateChange()
  ---------------------------------------------------------------------*/
  ds::ErrorType  RegEvent
  (
    ISignal   *pISignal,
    int32      eventName,
    IQI**      regObj
  );

  /*---------------------------------------------------------------------
    Abstract IQI/IWeakRef methods, these are defined in the sub-classes.
  ---------------------------------------------------------------------*/
  virtual int CDECL QueryInterface(AEEIID iid, void **ppo) = 0;
  virtual uint32 CDECL AddRef(void) = 0;
  virtual uint32 CDECL Release(void) = 0;
  virtual uint32 AddRefWeak(void) throw() = 0;
  virtual uint32 ReleaseWeak(void) throw() = 0;
  virtual boolean GetStrongRef(void) throw() = 0;


  /*---------------------------------------------------------------------
    Exported only to internal DSNET objects.
  ---------------------------------------------------------------------*/
  /*!
  @brief
  Returns the object handle of the object.

  @param      None.
  @see        None.
  @return     The object handle.
  */
  inline int32 GetHandle
  (
    void
  )
  throw()
  {
    return mObjHandle;
  }

  /*!
  @brief
  This method overrides the Process() method of the ds::Utils::Node class.

  @details
  This method provides generic method for processing events. For many
  event where user handles need not be matched, this function can be used.
  For events where certain special processing needs to happen, the derived
  object further overrides this method to perform the required logic.

  @param      pUserData - User data along with the event.
  @see        ds::Net::EventManager
  @see        ds::Net::EventInfoType
  @return     TRUE if the Process() method completes successfully.
  @return     FALSE if the Process() method has to halt traversal.
  */
  virtual boolean Process
  (
    void   *pUserData
  );


  /*!
  @brief
  Gets the signal bus object corresponding to the event.

  @details
  The signal bus pointer for supported events are stored in derived
  objects. For example, ds::Net::Network class stores the signal
  buses for BEARER_TECH_CHANGED, IP_ADDR_CHANGED, STATE_CHANGED etc.
  This function, gets the correct signal bus through which we can
  register and notify. This is an abstract method for Handle class,
  it is implemented in various derived classes.

  @param[in]  eventID     - Event ID
  @param[out] ppISignalBus- Signal bus returned.

  @return     SUCCESS - If signal bus is successfully returned.
  @return     DSS_EIVAL - If event is not valid for given object.
  @return     QDS_EFAULT - Invalid args.
  */
  virtual ds::ErrorType GetSignalBus
  (
    ds::Net::EventType  eventID,
    ISignalBus **       ppISigBus
  ) = 0;

};
} /* namespace Net */
}/* namespace ds */
#endif /* DS_NET_HANDLE_H */

