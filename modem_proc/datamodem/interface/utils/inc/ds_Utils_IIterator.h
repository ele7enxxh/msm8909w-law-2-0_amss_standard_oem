#ifndef DS_UTILS_IITERATOR_H
#define DS_UTILS_IITERATOR_H

/*==========================================================================*/ 
/*! 
  @file 
  ds_Utils_IIterator.h

  @details
  This file provides the ds::Utils::IIterator interface. This is the
  minimal iterator interface that exports methods for the following
  operations:

  First()
    Move to the beginning of the range of elements in the collection.

  Next()
    Advance to the next element of the collection.

  CurrentItem()
    Return the current object referred to by the iterator.

  IsDone()
    Interrogate it to see if the iteration is complete.

  Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ds_Utils_IIterator.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-08-29 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_INode.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Utils
{
/*!
  @class ds::Utils::IIterator

  @brief
  This class provides the IIterator interface.

  @details
  The iterator logic is provided by the 4 methods exported by this interface.
  First()       - Sets the iterators internal state to the first element.
  Next()        - Move to the next element.
  IsDone()      - Returns true if the iteration is complete.
  CurrentItem() - Returns the INode object at the current item.

  To use an iterator interface, the client can use the following loop:
  for (First(); !IsDone(); Next())
  {
    pINode = CurrentItem();
    ProcessItem (pINode);
  }
  The example above assumes that the object in which the iteration is being
  performed is derived from the IIterator interface. 

  @see ds::Utils::INode 
  @see ds::Utils::List  - This class implements an iterator.
*/
class IIterator
{
public:
  /*!
  @brief
  Destructor of the IIterator interface.

  @details
  The reason for having a virtual destructor in the interface is to allow
  operations of the sort "delete pIInterface" etc. In this case, if the 
  destructor is not explicitly defined to be virtual, the base class 
  destructor is used and subsequent derived class destructors are not called.

  @params   None.
  @return   None.
  */
  virtual ~IIterator
  (
    void 
  ) 
  throw() 
  {
    /* No-op */
  }

  /*!
  @brief
  Sets the iterators internal state to point to the first element of the set.
  @params   None.
  @return   None.
  */
  virtual void First
  (
    void 
  ) = 0;

  /*!
  @brief
  Increments the iterator's state to move to the next element of the set.
  @params   None.
  @return   None.
  */
  virtual void Next 
  (
    void
  ) = 0;

  /*!
  @brief
  Returns the current item pointed to by the iterator.
  @params   None.
  @return   Pointer to the INode object at the current iteration point.
  */
  virtual ds::Utils::INode* CurrentItem
  (
    void 
  ) = 0;

  /*!
  @brief
  Interrogate to see if the iteration is complete.

  @params   None.
  @return   TRUE - If iteration is complete.
  @return   FALSE - If iteration is not yet complete.
  */
  virtual boolean IsDone 
  (
    void 
  ) = 0;

}; /* class Iterator */

} /* namespace Utils */
} /* namespace ds */

#endif /* DS_UTILS_IITERATOR_H */

