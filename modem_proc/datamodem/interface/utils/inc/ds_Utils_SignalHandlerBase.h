#ifndef DS_UTILS_SIGNALHANDLERBASE_H
#define DS_UTILS_SIGNALHANDLERBASE_H

/*!
  @file
  ds_Utils_SignalHandler.h

  @brief
  Declares the ds::Utils::SignalHandlerBase class.

  Copyright (c) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ds_Utils_SignalHandlerBase.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-06-21  mt Add IWeakRef support.
  2010-01-11  mt Created the module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "ds_Utils_CSSupport.h"
#include "ds_Utils_IWeakRefSupport.h"
#include "ds_Utils_IWeakRef.h"
#include "ds_Utils_SignalHandler.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Utils
{

/*lint -esym(1510, IQI) */
/*lint -esym(1510, IWeakRefSupport) */

/**
  @class
  ds::Utils::SignalHandlerBase

  @brief
  Implements the convenience base class for users of SignalHandler.

  @details
  This class provides implementation of the common functions and specifies
  the prototypes of other functions required from users
  of the ds::Utils::SignalHandler class, i.e. everyone working with
  a ds::Utils::SignalFactory:

  - Implements IQI (AddRef, Release, QI of IQI interface),

  - Implements IWeakRef (AddWeakRef, ReleaseWeak, GetStrongRef),

  - Has and initializes a SignalHandler member,

  - marks operator delete as protected so that attempt to use it by other
    classes will produce an error (they should probably Release() instead).

  Your Destructor() must call SignalHandlerBase's Destructor().
*/
class SignalHandlerBase : public IWeakRef
{
public:
   SignalHandlerBase();

   virtual void Destructor
   (
      void
   ) throw();

   virtual ~SignalHandlerBase() throw();

   /*-------------------------------------------------------------------------
      Defintions of IQI and IWeakRef Methods
   -------------------------------------------------------------------------*/
   DS_UTILS_IWEAKREF_IMPL_DEFAULTS()

   /*-------------------------------------------------------------------------
      Operator delete should not be used by any descendant of this class -
      Release() should be used instead. However, it must be visible to
      descendants in order to use operator new.
   -------------------------------------------------------------------------*/
protected:
   static void operator delete
   (
      void *  bufPtr
   ) throw();

protected:
   ds::Utils::SignalHandler signalHandler;

}; /* class SignalHandlerBase */

} /* namespace Utils */
}/* namespace DS */

#endif /* DS_UTILS_SIGNALHANDLERBASE_H */

