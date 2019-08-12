/*==============================================================================

                              ds_ASAssignType.h

GENERAL DESCRIPTION
  Utility class for variables that needs 'unset' state.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/27/15    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_AS_ASSIGN_TYPE_H
#define DS_AS_ASSIGN_TYPE_H

#include "comdef.h"

namespace Appsrv
{
namespace Utils
{

template <typename T>
class ASAssignType
{
public:
  ASAssignType();
  ASAssignType(const T& val);
  ASAssignType(const ASAssignType& orig);

  bool is_set() const;
  void unset();

  void operator =(const T& val);
  void operator =(const ASAssignType& val);
  void set(const T& val);

  T& get();
  const T& get() const;

  bool operator ==(const ASAssignType& rhs) const;
  bool operator !=(const ASAssignType& rhs) const;
  bool operator >(const ASAssignType& rhs) const;
  bool operator >=(const ASAssignType& rhs) const;
  bool operator <(const ASAssignType& rhs) const;
  bool operator <=(const ASAssignType& rhs) const;


private:
  bool assigned;
  T    value;
};








template <typename T>
ASAssignType<T>::ASAssignType()
: assigned(false)
{ }



template <typename T>
ASAssignType<T>::ASAssignType(const T& val)
: assigned(true), value(val)
{ }


template <typename T>
ASAssignType<T>::ASAssignType(const ASAssignType& orig)
: assigned(orig.assigned), value(orig.value)
{ }


template <typename T>
void ASAssignType<T>::operator =(const T& val)
{
  set(val);
}


template <typename T>
void ASAssignType<T>::operator =(const ASAssignType& orig)
{
  assigned = orig.assigned;
  value    = orig.value;
}

template <typename T>
void ASAssignType<T>::set(const T& val)
{
  assigned = true;
  value    = val;
}


template <typename T>
bool ASAssignType<T>::is_set() const
{
  return assigned;
}


template <typename T>
void ASAssignType<T>::unset()
{
  assigned = false;
}


template <typename T>
T& ASAssignType<T>::get()
{
  return value;
}


template <typename T>
const T& ASAssignType<T>::get() const
{
  return value;
}


template <typename T>
bool ASAssignType<T>::operator ==(const ASAssignType& rhs) const
{
  return value == rhs.value;
}


template <typename T>
bool ASAssignType<T>::operator !=(const ASAssignType& rhs) const
{
  return value != rhs.value;
}


template <typename T>
bool ASAssignType<T>::operator >(const ASAssignType& rhs) const
{
  return value > rhs.value;
}


template <typename T>
bool ASAssignType<T>::operator >=(const ASAssignType& rhs) const
{
  return value >= rhs.value;
}


template <typename T>
bool ASAssignType<T>::operator <(const ASAssignType& rhs) const
{
  return value < rhs.value;
}


template <typename T>
bool ASAssignType<T>::operator <=(const ASAssignType& rhs) const
{
  return value <= rhs.value;
}



} /* namespace Utils */
} /* namespace Appsrv */

#endif /* DS_AS_ASSIGN_TYPE_H */
