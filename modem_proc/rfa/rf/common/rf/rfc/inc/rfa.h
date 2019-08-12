#ifndef RFA_H
#define RFA_H
/*!
  @file
  rfc.h

  @brief
  This file contains the class definition for the rfa, which will be the base class
  for all classes within RFA 

*/

/*===========================================================================

Copyright (c) 2011-12 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/rfc/inc/rfa.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------   
05/09/12   sr      init version
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
#include "rfcom.h"
#ifdef __cplusplus
}
#endif

/* Definition of RFC Common base class */
class rfa 
{
public:
  void* operator new(size_t size);
  void operator delete(void* ptr);

  // Destructor
  virtual ~rfa();

protected:
  // Protected constructor for singleton implemenation
  rfa();

private:

};

#endif /* RFA_H */

