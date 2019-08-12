/*==============================================================================

                            ds_ASAutoPointer.h

GENERAL DESCRIPTION
  Helper class to automatically release memory when it goes out of scope.
  Operations behave similarly to std::auto_ptr
  Note: TODO: If C++11 is supported, add move operations or
              add support for unique_ptr and shared_ptr

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/24/15    ml     Added move constructor/operator
05/20/15    ml     Use memory from modem heap
04/21/21    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_AS_AUTO_POINTER_H
#define DS_AS_AUTO_POINTER_H

#include "comdef.h"
#include "ds_appsrv_mem.h"

/*==============================================================================
CLASS ASAutoPointer

DESCRIPTION
  Object to give a pointer RAII behavior. In general, it can be used as if it is
  a pointer object (e.g. obj->member1)
==============================================================================*/
template <class T>
class ASAutoPointer
{
public:
  /*===========================================================================
  FUNCTION ASAutoPointer CONSTRUCTOR

  DESCRIPTION
    Creates a new ASAutoPointer. The pointer it holds can be assigned later by
    using the assignemnt operator.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  ASAutoPointer()
  : auto_ptr(NULL) { }
  ASAutoPointer(T *ptr)
  : auto_ptr(ptr) { }

  ~ASAutoPointer();

  // Note: Below functions are MOVE operation instead of copy
  ASAutoPointer(ASAutoPointer &src);
  ASAutoPointer& operator=(ASAutoPointer &rhs);

  /*============================================================================
  FUNCTION ASAutoPointer::operator=

  DESCRIPTION
    Assign the auto-release pointer object to the given pointer.
    WARNING - If this object already contained a pointer to something else, make
              sure it is release elsewhere as this object will only release the
              last assigned pointer.

  PARAMETERS
    [In] rhs - The pointer to release memory upon object destruction.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  ASAutoPointer& operator=(T *rhs);

  /*============================================================================
  FUNCTION ASAutoPointer::operator*
  FUNCTION ASAutoPointer::operator->

  DESCRIPTION
    Operator overload to enable the object to be used as if it is a pointer.

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  T& operator*()
  { return *auto_ptr;}

  T* operator->()
  { return auto_ptr; }

  const T* operator->() const
  { return auto_ptr; }

  /*============================================================================
  FUNCTION ASAutoPointer::get

  DESCRIPTION
    Returns the raw pointer of the object

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  T* get()
  { return auto_ptr; }

  const T* get() const
  { return auto_ptr; }

  /*============================================================================
  FUNCTION ASAutoPointer::is_null

  DESCRIPTION
    Check if the pointer it holds is null

  PARAMETERS
    True if auto_ptr is null, else false

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ============================================================================*/
  bool is_null() const
  { return NULL == auto_ptr; }

private:
  /* prevent deep copy */
  ASAutoPointer(const ASAutoPointer &src);
  ASAutoPointer& operator=(const ASAutoPointer &rhs);

  T* auto_ptr; // The pointer to release when ASAutoPointer goes out of scope
};


template <class T>
ASAutoPointer<T>::ASAutoPointer(ASAutoPointer &src)
{
  auto_ptr = src.auto_ptr;
  src.auto_ptr = NULL;
}


template <class T>
ASAutoPointer<T>& ASAutoPointer<T>::operator=(ASAutoPointer &rhs)
{
  T* tmp       = auto_ptr;
  auto_ptr     = rhs.auto_ptr;
  rhs.auto_ptr = tmp;
}

template <class T>
ASAutoPointer<T>& ASAutoPointer<T>::operator=(T *ptr)
{
  auto_ptr = ptr;
  return *this;
}



template <class T>
ASAutoPointer<T>::~ASAutoPointer()
{
  if(NULL != auto_ptr)
  {
    ds_appsrv_free<T>(auto_ptr);
  }
}

#endif /* DS_AS_AUTO_POINTER_H */
