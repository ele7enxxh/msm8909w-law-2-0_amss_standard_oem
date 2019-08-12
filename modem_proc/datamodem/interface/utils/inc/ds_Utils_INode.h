#ifndef DS_UTILS_INODE_H
#define DS_UTILS_INODE_H

/*==========================================================================*/ 
/*! 
  @file 
  ds_Utils_INode.h

  @brief
  This file provides the definition of ds::Utils::INode interface.

  @details
  The ds::Utils::INode interface exports a single function to perform
  object-specific process logic for traversal. See ds::Utils::ITraverser
  interface to see how the Traversal design pattern is implemented.
  INode interface supports weak references, ds_Utils_IWeakRef.h

  @note
  Do not derive from the INode interface directly. The INode interface
  does not provide methods of Creation and Destruction which are provided by 
  the ds::Utils::Node object, which provides a concrete implementation of
  the INode interface. Use ds::Utils::Node class for all purposes.

  @see ds::Utils::Node        - Concrete implementation of INode interface.
  @see ds::Utils::ITraverser  - Traverser interface.
  @see DS::Utils::IWeakRef    - Weak reference interface.

  Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ds_Utils_INode.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-04-30 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_IWeakRef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Utils
{
/*!
  @class ds::Utils::INode

  @brief
  Provides the definition of the INode interface.

  @details
  The ds::Utils::INode interface exports a single function to perform
  object-specific process logic for traversal. See ds::Utils::ITraverser
  interface to see how the Traversal design pattern is implemented.

  Method exported:
  Process() - To perform object-specific processing during traversal.

  @see ds::Utils::ITraverser
*/
class INode : public IWeakRef
{
public:

  /*!
  @brief
  This method performs the object-specific processing during traversal.

  @params[in]   pUserData - User Data associated with traversal.

  @return TRUE  If the traversal can proceed forward.
  @return FALSE Traversal should stop after this object's processing.

  @note
  The return values of Process() should not related to if the processing
  of a certain object passed or failed. Rather it should indicate whether
  the traversal can continue or not. Rarely FALSE is returned (in cases
  where user data is invalid or the Process function is not expected.)
  */
  virtual boolean Process
  (
    void*  pUserData
  ) = 0;

  /*!
  @brief
  Destructor of the INode interface.

  @details
  The reason for having a virtual destructor in the interface is to allow
  operations of the sort "delete pINode" etc. In this case, if the 
  destructor is not explicitly defined to be virtual, the base class 
  destructor is used and subsequent derived class destructors are not called.

  @params   None.
  @return   None.
  */
  virtual ~INode
  (
    void
  )
  throw()
  {
    /* No-op */
  }

}; /* class INode */

} /* namespace Utils */
} /* namespace ds */

#endif /* DS_UTILS_INODE_H */

