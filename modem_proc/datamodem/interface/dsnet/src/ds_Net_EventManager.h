#ifndef DS_NET_EVENT_MANAGER_H
#define DS_NET_EVENT_MANAGER_H

/*=========================================================================*/
/*!
  @file
  ds_Net_EventManager.h

  @brief
  This file provides the class ds::Net::EventManager which is used for 
  processing and propagating of the network events.

  @detail
  The ds::Net::EventManager is a singleton object. It is initialized during
  after-task-start initialization to register for all iface/flow/phys link
  events. The EventManager singleton maintains a set of lists for each 
  different "event groups" in the system. Each list is a set of active objects
  corresponding to the event group. For example, all the network objects
  are put in a networkObjList, the QoS objects are put in qosObjList etc.

  @section Event Framework

  @subsection Objectives
  The event framework is designed with the following objectives:
  1. Addition of new events should be straightforward.
  2. Event framework is not dependent upon the events themselves (generic 
     framework). No special logic needs to be used to handle and propogate 
     individual events. It should be straightforward to add new events
     to the framework.

  @subsection Event Registration
  The EventManager singleton allows the objects to register for events 
  from the underlying layers. There is no explicit registration involved. 
  Registrations happen automatically, when the objects are created. For 
  example, when the network object is created, it is added to the 
  EventManager::networkObjList, and would start receiving network events 
  immediately. 

  @subsection Event Propogation
  The following illustration how the event is propogated to the objects. We
  will start from the event indication from the PS iface layer and will 
  trace it all the way to the application.

  PS Iface layer                   IFACE_DOWN_IND 
                                          |
                                          V
  Event Manager               networkOjectList.Traverse()
                                          |
                                          V
                    +------+      +------+      +------+      +------+ 
  networkObjList    | net1 |----->| net2 |----->| net3 |----->| net4 |
                    +------+      +------+      +------+      +------+
                        |             |             |             |
                        V             V             V             V  
                    Process()     Process()     Process()     Process()
                    handle!=      handle!=      handle==      handle!=
                    evHandle      evHandle      evHandle      evHandle
                        |             |            |              |
                        V             V            |              V  
                    Continue      Continue         V          Continue
                                              +----------+
                                              | Notify() |
                                              +----------+

  In the above illustration, the iface handle on which the event occurred
  matches with the net3's object handle. Hence Notify() would be called on
  net3. However, net3 may not be interested in the event that has occurred
  right now. net3 only processes the event if app has registered for it.

  The Handle object maintains the event registrations performed by the 
  application in the form of EventNode objects. When Handle::Notify() is 
  called, the EventNode objects for the Handle are traversed. If the current 
  event matches that of a event node, the corresponding signal is set.

  +------------+
  | Handle obj |
  | +-------+  |   
  | | Event |  |     +----------+      +----------+      +----------+ 
  | | Node  |--|---->| Node1    |----->| Node2    |----->| Node3    |
  | | List  |  |     | ev1/sig1 |      | ev2/sig2 |      | ev3/sig3 |
  | +-------+  |     +----------+      +----------+      +----------+
  |Traverse(ev)|      Process(),        Process(),        Process(),
  +------------+      ev != ev1,        ev == ev2         ev != ev3,
                      return                |             return
                                            |
                                            V
                                      +-------------+
                                      | sig2->Set() |
                                      +-------------+


  @see      ds::Net::Handle
  @see      ds::Net::EventNode
  @see      ds::Utils::INode
  @see      ds::Utils::IFactory
  @see      ds::Utils::ITraverser

  @todo
  Cleanup event management logic. Currently it is going through network 
  factory, then through network objects etc. A lot of event dependent 
  infomration is being maintained. Make sure everyone uses userHandle if 
  required. Check to see where psEventInfo is getting used and we should
  get rid of that.


  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_EventManager.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-02 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_Utils.h"
#include "ds_Net_MemManager.h"
#include "ds_Net_Platform.h"
#include "ds_Utils_Factory.h"
#include "ds_Utils_Singleton.h"
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Net
{

/*!
  @class ds::Net::EventManager

  @brief
  This class implements the event framework for the DSNET objects.

  @todo
  Write details about the event manager design.

*/
  class EventManager : public ds::Utils::Singleton<EventManager>
{
  friend class ds::Utils::Singleton<EventManager>;

private:

  /*!
  @brief
  Constructor of the ds::Net::EventManager object.

  @details
  EventManager is a singleton. The constructor is hence declared as a 
  private method. Call ds::Net::EventManager::Instance() method to get
  access to the Event Manager.

  @param      None.
  @see        ds::Net::EventManager::Instance()
  @return     Pointer to the EventManager object.
  */
  EventManager
  (
    void
  );

  /*!
  @brief
  EventManager destructor. 

  @details
  Since EventManager is a singleton that exists in the system for the
  entire duration, the destructor is never called. 

  @param      None.
  @see        None.
  @return     None.
  */
  ~EventManager
  (
    void 
  ) 
  throw();

  /*!
  @brief
  This method processes the events occurring from the PS iface layer.

  @details
  This method processes the events from the PS iface layer. The EventManager
  registers with the Network platform for all events from all ifaces, flows 
  and phys links during initialization using this method as callback. Since
  this method is registered as a callback with PS iface layer, it has
  to be a static method.

  Depending upon the current event, this method goes through the relevent 
  list of objects waiting on the event and informs the same to them. It 
  performs a tranlsation from the PS iface event names to DS Net event names
  and calls traverse on the appropriate event group.

  @note
  This method is called as a callback function from the PS iface layer. Hence
  there is a great possibility that this callback is tasklocked etc. Keep
  the amount of processing to minimum levels in this function.

  @param[in]  handle - iface handle on which the event occurred.
  @param[in]  eventName - Name of the event in PS iface format.
  @param[in]  pEventInfo - Event info union associated with the event.
  @param[in]  pUserData - User data pointer with the callback.

  @return     None.
  */
  static void ProcessEvent
  (
    int32                                 handle,
    int32                                 eventName,
    NetPlatform::IfaceEventInfoUnionType  pEventInfo,
    void*                                 pUserData
  );

  /*-------------------------------------------------------------------------
    Functions to overload the new and delete operators of this class.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS(PS_MEM_DS_NET_EVENT_MANAGER)

public:
  /*-------------------------------------------------------------------------
    Public lists that allow automatic factory/node registration management.
  -------------------------------------------------------------------------*/
  static ds::Utils::Factory*   networkObjList;
  static ds::Utils::Factory*   qosObjList;
  static ds::Utils::Factory*   physLinkObjList;
  static ds::Utils::Factory*   mcastObjList;
  static ds::Utils::Factory*   networkMBMSObjList;
  static ds::Utils::Factory*   networkIPv6ObjList;
  static ds::Utils::Factory*   network1XObjList;
  static ds::Utils::Factory*   mtpdObjList;


  /*-------------------------------------------------------------------------
    Exported methods.
  -------------------------------------------------------------------------*/
  /*!
  @brief
  This function initializes the event manager instance.

  @details
  This function initializes the event manager instance. It registers 
  callback functions for iface, flow and phys link events for all handles 
  with the PS iface layer.
  This function is a static method that is called only once during 
  after-task-start initialization.

  @dependencies
  The initialization performed in this function assumes that the PS iface
  layers have already been initialized.

  @todo
  This framework needs some design changes to perform initializations on
  3rd party OS platforms. 

  @param      None.
  @see        psi_init() - After task start initialization of PS task.
  @see        ds::Net::Init() 
  @see        ds::Net::PowerupInit()
  @return     None.
  */
  static void Init
  (
    void
  );

  /*!
  @brief      
  This function cleansup the event manager instance.

  @param      None.
  @see        ds::Net::Init() 
  @return     None.
  */
  static void Deinit
  (
    void
  );

}; /* class EventManager */

} /* namespace Net */
}/* namespace ds */

#endif /* DS_NET_EVENT_MANAGER_H */

