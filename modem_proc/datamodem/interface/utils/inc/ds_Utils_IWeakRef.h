#ifndef DS_UTILS_I_WEAK_REF_H
#define DS_UTILS_I_WEAK_REF_H

/*==========================================================================*/
/*!
  @file
  ds_Utils_IWeakRef.h

  @brief
  This file provides the definition of ds::Utils::IWeakRef interface.

  @details
  ds::Utils::IWeakRef is DS-style interface which is used to support
  weak references. The concept is based on CS weak references
  (http://qwiki.qualcomm.com/qct-ipa/CS_Weak_References), but the interfaces
  are different than CS weak reference interfaces (AEEIWeakRef.h and
  AEEISupportsWeakRef.h). This is an internal interface, not to be
  exported to applications.

  Methods exported:
  AddRefWeak()    - Add a weak reference to this object.
  ReleaseWeak()   - Release a weak reference.
  QueryReferent() - Get a strong reference from the weak reference.

  How to use and other documentation: TODO

  Copyright (c) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ds_Utils_IWeakRef.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-05-25 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_IQI.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds 
{
namespace Utils
{
/*lint -esym(1510, IQI) */
class IWeakRef : public IQI
{
public:

  /*!
  @brief
  Increments the weak reference.

  @params
  None.

  @return
  The incremented weak reference count.
  */
  virtual uint32 AddRefWeak
  (
    void
  ) throw() = 0;


  /*!
  @brief
  Decrements the weak reference. If weak ref-count reaches 0, memory for
  the object would be freed.

  @return
  The weak reference count after release. 0 if obj memory is freed.
  */
  virtual uint32 ReleaseWeak
  (
    void
  ) throw() = 0;

  /*!
  @brief
  Get a strong reference to the object.

  @return
  TRUE  - if a strong reference can be obtained.
  FALSE - if object is destructed.

  @side_effects
  The strong ref count of the object would be incremented if return value
  is TRUE. The caller should call Release() to release this strong
  reference.
  */
  virtual boolean GetStrongRef
  (
    void
  ) throw() = 0;

  /*!
  @brief
  Destructor of the IWeakRef base object.

  @details
  The reason for having a virtual destructor in the interface is to allow
  operations of the sort "delete pIWeakRef" etc. In this case, if the
  destructor is not explicitly defined to be virtual, the base class
  destructor is used and subsequent derived class destructors are not called.
  */
  virtual ~IWeakRef
  (
    void
  )
  throw()
  {
    /* No-op */
  }


}; /* class IWeakRef */

} /* namespace Utils */
}/* namespace ds */

#endif /* DS_UTILS_I_WEAK_REF_H */


