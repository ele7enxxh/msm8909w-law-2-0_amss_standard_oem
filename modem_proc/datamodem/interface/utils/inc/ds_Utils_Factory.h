#ifndef DS_UTILS_FACTORY_H
#define DS_UTILS_FACTORY_H

/*==========================================================================*/
/*!
  @file
  ds_Utils_Factory.h

  @brief
  This file exports the ds::Utils::Factory class which implements the 
  ds::Utils::IFactory interface.

  @details
  Concrete implementation of ds::Utils::IFactory interface. The IFactory 
  interface contains generic methods such as AddItem() and RemoveItem(). 
  This class provides the standard implementation for the same. Objects
  which need Factory functionality can directly derive from this class.

  @see ds::Utils::IFactory

  Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ds_Utils_Factory.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who  what, where, why
  ---------- ---  ------------------------------------------------------------
  2008-04-30 hm   Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "ds_Utils_IFactory.h"
#include "ds_Utils_INode.h"
#include "ds_Utils_List.h"
#include "ps_system_heap.h"
#include "amssassert.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Utils
{
/*!
  @class
  ds::Utils::Factory

  @brief
  Concrete implementation for ds::Utils::IFactory interface.
  IMPORTANT: Current implementation does NOT manage ref count for objects stored inside

  @details
  Since this is a concrete implementation of IFactory, this object also ends
  up implementing the ITraverser and IIterator methods.

  @see ds::Utils::IFactory
*/
class Factory : public IFactory
{
protected:
  List                    _list;

public:

  /*-------------------------------------------------------------------------
    Constructor/Destructor
  -------------------------------------------------------------------------*/
  /*!
  @brief
  Constructor of the Factory object.

  @params       None.
  */
  Factory
  (
    void
  )
  throw();


  /*!
  @brief
  Constructor of the Factory object.

  @params       None.
  */
  virtual ~Factory
  (
    void
  )
  throw();

  /*-------------------------------------------------------------------------
    Methods from IFactory interface.
  -------------------------------------------------------------------------*/
  /*!
  @brief
  Adds an INode object to the factory.

  @params[in]   pINode - Pointer to the INode object to be added.
  @see          ds::Utils::INode
  @see          ds::Utils::IFactory
  */
  virtual int32 AddItem
  (
    INode* pINode
  )
  throw();

  /*!
  @brief
  Removes an INode object from the factory.

  @params[in]   pINode - Pointer to the INode object to be added.
  @see          ds::Utils::INode
  @see          ds::Utils::IFactory
  */
  virtual void RemoveItem
  (
    INode* pINode
  )
  throw();

  /*!
  @brief
  Removes and deletes all items in the factory.

  @params       None.
  @see          ds::Utils::INode
  @see          ds::Utils::IFactory
  */
  virtual void DeleteAllItems
  (
    void
  )
  throw();

  /*-------------------------------------------------------------------------
    Methods from ITraverser interface.
  -------------------------------------------------------------------------*/
  /*!
  @brief
  Method to perform traversal of the objects in the factory.

  @params[in]   pUserData
  User data that gets passed to each INode during traversal.

  @see          ds::Utils::INode::Process()
  @see          ds::Utils::ITraverser::Traverse()
  */
  virtual boolean Traverse
  (
    void* pUserData
  )
  throw();

  /*-------------------------------------------------------------------------
    Methods from IIterator interface.
  -------------------------------------------------------------------------*/
  /*!
  @brief
  Sets the internal state of the iterator to point to the first element.

  @params       None.
  @return       None.
  @see          ds::Utils::IIterator
  */
  virtual void First
  (
    void
  )
  throw();

  /*!
  @brief
  Increments the internal state of the iterator to point to the next element.

  @params       None.
  @return       None.
  @see          ds::Utils::IIterator
  */
  virtual void Next
  (
    void
  )
  throw();

  /*!
  @brief
  Returns the item corresponding to the current state of the iterator.

  @params       None.
  @return       INode item returned.
  @see          ds::Utils::IIterator
  */
  virtual INode * CurrentItem
  (
    void
  )
  throw();

  /*!
  @brief
  Check condition for iteration.

  @details
  Depending upon the state of the iterator, the function returns true or false
  if iteration is complete or not.

  @params       None.
  @return       TRUE - If iteration is complete.
  @return       FALSE - If iteration is not complete yet.
  @see          ds::Utils::IIterator
  */
  virtual boolean IsDone
  (
    void
  )
  throw();

  /* Overload new/delete operators */
  inline void* operator new                       
  (                                               
    unsigned int num_bytes                        
  )                                               
  throw()                                         
  {                                               
    void * buf = NULL;
    if (sizeof(ds::Utils::Factory) != num_bytes)
    {
      ASSERT (0);
      return NULL;
    }
    PS_SYSTEM_HEAP_MEM_ALLOC (buf, num_bytes, void*);
    return buf;
  }                                               

  inline void operator delete
  (                          
    void* objPtr             
  )                          
  throw()                    
  {                          
    PS_SYSTEM_HEAP_MEM_FREE (objPtr);
  }


}; /* class Factory */

} /* namespace Utils */
}/* namespace ds */

#endif /* DS_UTILS_FACTORY_H */

