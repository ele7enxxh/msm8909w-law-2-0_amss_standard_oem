#ifndef DS_UTILS_IFACTORY_H
#define DS_UTILS_IFACTORY_H
/*==========================================================================*/ 
/*! 
  @file 
  ds_Utils_IFactory.h

  @brief
  This file exports the ds::Utils::IFactory interface.

  @details
  The ds::Utils::IFactory interface provides a generic factory interface for 
  the DSNET and DSSOCK objects. It provides methods to add/remove items from
  the factory.

  ds::Utils::IFactory interface methods are only exposed to objects that 
  derive from ds::Utils::INode interface. When the INode objects are created,
  they call the AddItem() method on the IFactory object to associate itself
  with the IFactory object. When the INode objects get released, they call
  RemoveItem() method of the IFactory interface to remove themselves from
  the factory. The ds::Utils::Factory and ds::Utils::Node objects provide
  concrete implementation for the IFactory and INode interfaces. Below is an
  illustration on how the creation and destruction of Node objects.

  class ds::Utils::Factory              class ds::Utils::Node
  AddItem(INode * node)                 Node(IFactory *fac)
  {                                     {
    Adds node to list;                    Create self and add to fac.
  }                                     }

  RemoveItem()                          ~Node(IFactory *fac)                        
  {                                     { 
    Remove node from list;                Delete and remove from fac
  }                                     } 

  The IFactory interface also exports methods for traversing and iterating 
  through the list of objects maintained by it. See ds::Utils::IIterator and
  ds::Utils::ITraverser interfaces for details.

  The classes ds::Utils::Factory and ds::Utils::Node provide implementation 
  of common methods of IFactory and INode interfaces. Usually the only 
  implementation that needs to be overriden for the derived objects
  is the INode interface's Process() method. 

  @see ds::Utils::INode       - INode interface.
  @see ds::Utils::ITraverser  - Traverser interface.
  @see ds::Utils::IIterator   - Iterator interface.
  @see ds::Utils::Factory     - Implementation of IFactory interface.
  @see ds::Utils::Node        - Implementation of INode interface.

  Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ds_Utils_IFactory.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who  what, where, why
  ---------- ---  ------------------------------------------------------------
  2008-04-30 hm   Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_INode.h"
#include "ds_Utils_IIterator.h"
#include "ds_Utils_ITraverser.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

namespace ds
{
namespace Utils
{

/*!
  @class ds::Utils::IFactory

  @brief
  This class defines the IFactory interface.

  @details
  ds::Utils::IFactory interface provides an interface to manage creation 
  and destruction of objects in the DSNET/DSSOCK layers. The implementation 
  of IFactory/INode interfaces should guarantee the following for the 
  management of lifecycles of objects:
  1. As soon as the object is created, it has been added to a container
     for the object. The object is always aware of its container (Factory).
  2. When the object is destroyed, to make sure that the container removes
     the reference it is holding to the object.

  The methods of IFactory interface are only exported to objects deriving 
  from INode class. When the INode object, is getting created, it calls 
  AddItem() method on the IFactory. When the INode object is getting deleted,
  it calls RemoveItem().

  In addition, IFactory also derives from ITraverser and IIterator 
  interfaces. These methods are available to all the objects.

  @see ds::Utils::INode       - INode interface.
  @see ds::Utils::ITraverser  - Traverser interface.
  @see ds::Utils::IIterator   - Iterator interface.
  @see ds::Utils::Factory     - Implementation of IFactory interface.
  @see ds::Utils::Node        - Implementation of INode interface.

*/
class IFactory : public ITraverser,
                 public IIterator
{

public:
  /*!
  @brief
  To add an item to the parent factory during creation of the object.

  @details
  This method adds an INode object to the factory. This method is only
  exported to the ds::Utils::Node object. This function is meant to be
  called during the construction of the Node object.

  @params[in]   pINode - INode object to add to the factory.

  @return       AEE_SUCCESS - On success
  @return       EFAULT  - Invalid parameters.

  @see          ds::Utils::INode interface.
  @see          ds::Utils::Node object.
  */
  virtual int32 AddItem
  (
    INode* pINode
  )  
  throw() = 0;


  /*!
  @brief
  To disassociate an object from the parent factory object during object's
  destruction.

  @details
  This function is meant to be called when the Node object is getting
  destroyed. This function updates the state of the Factory to remove the
  reference to the Node object.

  @params[in]   pINode - INode object to remove from the factory.

  @return       None. 

  @see          ds::Utils::INode interface.
  @see          ds::Utils::Node object.
  */
  virtual void RemoveItem
  (
    INode* pINode
  )
  throw() = 0;

  /*!
  @brief
  Destructor of the IFactory interface.

  @details
  The reason for having a virtual destructor in the interface is to allow
  operations of the sort "delete pIFactory" etc. In this case, if the 
  destructor is not explicitly defined to be virtual, the base class 
  destructor is used and subsequent derived class destructors are not called.

  @params   None.
  @return   None.
  */
  virtual ~IFactory
  (
    void 
  ) 
  throw() 
  {
    /* No-op */
  }

}; /* class Factory */

} /* namespace Utils */
}/* namespace ds */


#endif /* DS_UTILS_IFACTORY_H */

