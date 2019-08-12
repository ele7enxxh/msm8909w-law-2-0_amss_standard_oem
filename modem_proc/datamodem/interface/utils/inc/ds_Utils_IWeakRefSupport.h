#ifndef DS_UTILS_IWEAKREFSUPPORT_H
#define DS_UTILS_IWEAKREFSUPPORT_H

/*!
  @file
  ds_Utils_IWeakRefSupport.h

  @details
  This file defines the ds::Utils::IWeakRefSupport interface, which provides
  support for basic weak reference semantics. It is not complete as opposed to
  CS ISupportsWeakRef or IWeakRef, but sufficient to implement (for example)
  SignalHandler functionality.

  The following methods are exported.

  AEEResult Lock()
    AEE_SUCCESS if a strong reference was obtained.

  void Free()
    Releases the memory held by the object.

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ds_Utils_IWeakRefSupport.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-11-24  mt Created the module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "ds_Utils_ISignalHandler.h"

#include "ds_Utils_StdErr.h"
#include "ds_Utils_CSSupport.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Utils
{

/*lint -esym(1510, IQI) */

/*!
  @class
  ds::Utils::IWeakRefSupport

  @brief
  Implements the IWeakRefSupport interface.

  @details
  TODO
*/
class IWeakRefSupport : public IQI
{
public:
  /*!
    @brief
    Returns AEE_SUCCESS if a strong reference has been obtained.

    An existing pointer to the object can be used after calling Lock(),
    and Release()d afterwards.
  */
  virtual AEEResult Lock
  (
  ) throw() = 0;

  /*!
    @brief
    Frees the memory used by an object.
  */
  virtual void Free
  (
  ) throw() = 0;

}; /* class IWeakRefSupport */

} /* namespace Utils */
}/* namespace ds */

#endif /* DS_UTILS_IWEAKREFSUPPORT_H */

