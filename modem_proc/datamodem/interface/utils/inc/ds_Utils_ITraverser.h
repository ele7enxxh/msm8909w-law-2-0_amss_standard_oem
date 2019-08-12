#ifndef DS_UTILS_ITRAVERSER_H
#define DS_UTILS_ITRAVERSER_H
/*==========================================================================*/ 
/*! 
  @file ds_Utils_ITraverser.h

  @details
  This file provides the ds::Utils::ITraverser interface. The traversal design
  pattern allows clients to perform a common operation on the individual 
  objects of the set. The operation to be performed is given provided by a 
  Process() function that is implemented by individual objects. The ITraverser
  interface provides a single method:

  Traverse()
  Perform a complete traversal of the set and calls the Process() function
  on each node of the set. Each node of the set implements the process 
  function of the INode interface. 
  
  @section Traverser Design Pattern
  The Traverser design pattern allows to specific functions on each item 
  of a collection. The operation to be performed can be defined by the 
  individual elements of the collection. In this case, Traverser operates
  on collection on INode elements. The INode interface defines a virtual
  Process() function, which can be implemented to suit the requirements
  of the collection. 

  We also allow passing some user data to the items of the collection while
  performing Traversal. This user data allows INode elements to determine 
  the type of processing that needs to be performed.

  Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ds_Utils_ITraverser.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-04-30 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Utils
{
/*!
  @class ds::Utils::ITraverser

  @brief
  This class provides the ITraverser interface.

  @details
  The ITraverser interface exports a single method for performing 
  traversal on a collection. The individual objects of the collection
  implement the Process() method of the INode interface. 
  
  Functions exported:
  Traverse() - Iterate through the collection calling Process() method
  on individual objects.

  @see ds::Utils::INode 
  @see ds::Utils::List  - This class implements an ITraverser interface.
*/
class ITraverser
{
public:
  /*!
  @brief
  Perform traversal on a collection.

  @params   pUserData - User data associated with traversal.
  @return   TRUE - If traversal completes on all objects.
  @return   FALSE - If traversal was not completed.
  @see      ds::Utils::INode::Process()
  */
  virtual boolean Traverse 
  (
    void *pUserData
  ) = 0;

  /*!
  @brief
  Destructor of the ITraverser interface.

  @details
  The reason for having a virtual destructor in the interface is to allow
  operations of the sort "delete pIInterface" etc. In this case, if the 
  destructor is not explicitly defined to be virtual, the base class 
  destructor is used and subsequent derived class destructors are not called.

  @params   None.
  @return   None.
  */
  virtual ~ITraverser
  (
    void 
  ) 
  throw() 
  {
    /* No-op */
  }

}; /* class ITraverser */

} /* namespace Utils */
}/* namespace ds */

#endif /* DS_UTILS_ITRAVERSER_H */

