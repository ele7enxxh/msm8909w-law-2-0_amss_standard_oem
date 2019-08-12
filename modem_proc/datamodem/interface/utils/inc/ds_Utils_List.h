#ifndef DS_UTILS_LIST_H
#define DS_UTILS_LIST_H

/*==========================================================================*/
/*!

  @file
  ds_Utils_List.h

  @brief
  This file provides a generic List mechanism.

  @details
  This file provides the ds::Utils::List class. The List class is currently
  implemented as a wrapper on top of the QCT list API. The implementation
  could be changed to use other list API without affecting clients using the
  ds::Utils::List API.

  There are special reasons why List is not defined as an interface.
  1. List is used by internal objects, and not remoted.
  2. If defined as interface, a lot of messy typecasting need to happen
     in the implementation.

  The current implementation of having list as a concrete object by itself
  allows portability and removes the messy typecasting in the implementation.

  The following functions are exported from List API. Note that List API
  also implements ITraverser and IIterator interfaces.

  List()
    Constructor for the list object.

  ~List()
    Destructor for the list object. Does NOT call destructor on each of
    its elements.

  PushBack()
    Insert an element at the end of the list.

  PushFront()
    Insert an element at the beginning of the list.

  PopFront()
    Remove an element from the beginning of the list.

  RemoveItem()
    Removes an item specified by the argument from the list.

  GetTraverser()
    Returns a traverser for the list.
    See ds::Utils::ITraverser interface.

  Count()
    Returns the number of elements in the list.

  Get()
    Returns the element at the given index.

  @see      ds::Utils::IIterator
  @see      ds::Utils::ITraverser
  @see      list.h (This provides the QCT list, is released as part of
            services/utils VU.)

  Copyright (c) 2008 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ds_Utils_List.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-02 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_ITraverser.h"
#include "ds_Utils_IIterator.h"
#include "ds_Utils_INode.h"
#include "ds_Utils_ICritSect.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Utils
{
/*!
  @class
  ds::Utils::List

  @brief
  This class provides a generic list implementation.

  @details
  The List class is implemented as a wrapper on top of the QCTL list API.
  This class provides very lightweight API to perform various list
  operations.

  The items that can be added to this list have to be derived-classes of
  ds::Utils::INode interface. This restriction is added to provide
  ITraverser implementation.

  Note: Current implementation does NOT manage ref count for objects
  stored inside of the list.

  @todo
  The file is not greatly efficient. The methods implemented in this file
  can definitely be improved tremendously.

  @see      ds::Utils::INode
  @see      ds::Utils::IIterator
  @see      ds::Utils::ITraverser
  @see      list.h (This provides the QCT list, is released as part of
            services/utils VU.)

*/
class List : public ITraverser,
             public IIterator
{

public:
  /*!
    @brief
    Constructor of the List object.

    @details
    This method constructs the list object as an empty list.

    @params     None.
    @see        None.
    @return     The constructed list object.
  */
  List
  (
    void
  )
  throw();


  /*!
    @brief
    Destructor of the list object.

    @details
    Deletes all the elements of the list and the storage allocated to hold
    them. It does not delete the individual objects held though.

    @params     None.
    @see        None.
    @return     The constructed list object.
  */
  virtual ~List
  (
    void
  )
  throw();

  /*!
    @brief
    Add an item to the end of the list.

    @params[in] pItem - Pointer to the INode item to be added.
    @return     AEE_SUCCESS on SUCCESS
    @return     AEE_ENOMEMORY No memory to add the item to the list.
  */
  int32 PushBack
  (
    INode* item
  )
  throw();

  /*!
    @brief
    Add an item to the front of the list.

    @params[in] pItem - Pointer to the INode item to be added.
    @return     AEE_SUCCESS on SUCCESS
    @return     AEE_ENOMEMORY No memory to add the item to the list.
  */
  int32 PushFront
  (
    INode* item
  )
  throw();

  /*!
    @brief
    Deletes an item from the front of the list.

    @param      None.
    @return     Pointer to the returned item.
    @return     NULL if item was not found.
  */
  INode* PopFront
  (
    void
  )
  throw();

  /*!
    @brief
    Remove a specified item from the list.

    @params[in] pItem - INode item to be removed.
    @return     None.
  */
  void RemoveItem
  (
    INode* item
  )
  throw();

  /*!
    @brief
    Return the current number of elements in the list.

    @param      None.
    @return     Number of elements in the list.
  */
  int32 Count
  (
    void
  )
  throw();

  /*!
    @brief
    Return the item at a specified index. The item is not removed from
    the list.

    @param[in]  index - Index
    @return     INode pointer to the item.
  */
  INode* Get
  (
    int32 index
  )
  throw();

  /* The ds::Utils::List class implements the ITraverser interface */
  /*!
    @brief
    Traverse the list and call Process() method on each INode.

    @param      pUserData - User data to be passed to Process()
    @return     TRUE - If traversal was successful.
    @return     FALSE - If traversal failed.
    @see        ds::Utils::ITraverser
  */
  virtual boolean Traverse
  (
    void* pUserData
  )
  throw();

  /* The ds::Utils::List class implements the IIterator interface */

  /*!
    @brief
    Set the iterator to the first element of the list.

    @param      None.
    @return     None.
    @see        ds::Utils::IIterator
  */
  virtual void First
  (
    void
  )
  throw();

  /*!
    @brief
    Move the traverser to the next element in the list.

    @param      None.
    @return     None.
    @see        ds::Utils::IIterator
  */
  virtual void Next
  (
    void
  )
  throw();

  /*!
    @brief
    Return the item at the current iteration index.

    @param      None.
    @return     None.
    @see        ds::Utils::IIterator
  */
  virtual INode* CurrentItem
  (
    void
  )
  throw();

  /*!
    @brief
    Check if the iteration is complete or not.

    @param      None.
    @return     TRUE - If iteration is complete.
    @return     FALSE - If iteration is still in progress.
    @see        ds::Utils::IIterator
  */
  virtual boolean IsDone
  (
    void
  )
  throw();


  /*-------------------------------------------------------------------------
    Overloaded new/delete operators.
  -------------------------------------------------------------------------*/
  /*!
    @brief
    Overloaded new operator.

    @return
    Pointer to object created.
  */
  static void* operator new
  (
    unsigned int numBytes
  )
  throw();

  /*!
    @brief
    Overloaded delete operator.

    @return
    None.
  */
  static void operator delete
  (
    void* bufPtr
  )
  throw();

private:
  int32 PreparePush
  (
    INode *item,
    void **listNodePtr
  )
  throw();

  /*!
    @brief
    Copies information from a List object.

    @details
    Copies members from passed in List object.

    @param[in] listPtr  List object from which members are to be copied
    @return     TRUE - If creating clone of the list was successful.
    @return     FALSE - If clone list failed.
  */
  boolean CloneList
  (
    List *  listPtr
  )
  throw();

  /*-------------------------------------------------------------------------
    Internal data structure to implement List in a platform independent way.
  -------------------------------------------------------------------------*/
  class ListInternal*     pListInternal;

  /*-------------------------------------------------------------------------
    Critical section.
  -------------------------------------------------------------------------*/
  ICritSect*              pCritSect;

}; /* class List */

}  /* namespace Utils */
}  /* namespace ds */

#endif /* DS_UTILS_LIST_H */

