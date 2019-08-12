/*==============================================================================

                              ds_ASVector.h

GENERAL DESCRIPTION
  Dynamic array container that grows in size when necessary.
  Insert O(1) if no resize needed (best, average)
         O(n) if resize needed    (worst)
  Remove O(1) for end removal
         location removal NOT implemented
  Access O(1)

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
06/03/15    ml     Remove memory allocation on default constructor
05/20/15    ml     Use memory from modem heap
04/21/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_AS_VECTOR_H
#define DS_AS_VECTOR_H

#include "comdef.h"
#include "ds_appsrv_mem.h"
#include <stringl/stringl.h> // memscpy

#define DEFAULT_VECTOR_SIZE 16 // Must be 1+, else makes heap allocation of size 0

// uint32 max 4294967295
#define DS_ASVECTOR_MAX_SIZE 20000


/*==============================================================================
                             ASVector Declaration
==============================================================================*/

/*==============================================================================
CLASS ASVector

DESCRIPTION
  Class for a dynamic array.

  Whenever the vector needs to resize its container, it will be making
  deep copies of the object, so for large objects, it is recommended to use
  pointers instead.

  If using custom class as object in the vector instead of pointers, base
  constructor and assignment operator functions are REQUIRED. Also, if any
  memory is released in destructor, make sure to make new copies of the pointer
  object in the copy/assignment as the pattern is :
    1. allocate memory for new container
    2. copy old objects to new container
    3. delete old objects / container
  meaning destructors will be called whenever the container is resized.
  Implementation wise, the calls will be :
    1. base constructor n times
    2. assignment operator m times
    3. destructor m times
    where n and m is the new and old size of the container
==============================================================================*/
template <typename T>
class ASVector
{
public:
  ASVector();
  ASVector(uint32 cap_size);
  ~ASVector();

  /*===========================================================================
  FUNCTION ASVector::operator[]

  DESCRIPTION
    Returns the object at index location
    WARNING!!! - Undefined behavior for index value equal or larger than size()

  PARAMETERS
    [In] index - The location of the object.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  T& operator [](uint32 index);
  const T& operator[](uint32 index) const;


  /*===========================================================================
  FUNCTION ASVector::size

  DESCRIPTION
    Returns the number of objects inside the container

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  uint32 size() const;


  /*===========================================================================
  FUNCTION ASVector::capacity

  DESCRIPTION
    Returns the object at index location

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  uint32 capacity() const;


  /*===========================================================================
  FUNCTION ASVector::back

  DESCRIPTION
    Returns the last element inside the container
    WARNING!!! - Undefined behavior for calling this function on an empty container

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  T& back();
  const T& back() const;


  /*===========================================================================
  FUNCTION ASVector::push_back

  DESCRIPTION
    Adds the object the the end of the container. Container capacity will be
    resized if it does not have enough capacity to add

  PARAMETERS
    [In] object - The object to add to the container

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  bool push_back(const T& object);


  /*===========================================================================
  FUNCTION ASVector::pop_back

  DESCRIPTION
    Removes the last element inside the container.
    WARNING!!! - Undefined behavior if this function is called on an empty vector.

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  void pop_back();

  /*===========================================================================
  FUNCTION ASVector::clear

  DESCRIPTION
    Clears the vector container to size 0.

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  void clear();


private:
  /*===========================================================================
  FUNCTION ASVector::resize_array

  DESCRIPTION
    Resize the current container capacity

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  bool resize_array();

  /* prevent deep copy */
  ASVector(const ASVector &src);
  ASVector& operator=(const ASVector &rhs);


protected:
  uint32 cur_size;     // The current number of objects occupying the container
  uint32 cur_capacity; // The capacity of the container
  T*     container;    // Container containing the objects
};


/*===========================================================================
FUNCTION clear_ptr_vector

DESCRIPTION
  Releases all the memory that the objects in the vector holds and clears the
  vector array.

PARAMETERS
  vec - The vector to clear.

DEPENDENCIES
  ASVector with pointer objects

SIDE EFFECTS
  None
===========================================================================*/
template <typename T>
void clear_ptr_vector(ASVector<T*>& vec);






/*==============================================================================
                           ASVector Implementation
==============================================================================*/
template <typename T>
ASVector<T>::ASVector()
: cur_size(0), cur_capacity(DEFAULT_VECTOR_SIZE), container(NULL)
{ }



template <typename T>
ASVector<T>::ASVector(uint32 cap_size)
: cur_size(0), cur_capacity(cap_size), container(NULL)
{
  if(0 == cur_capacity)
  {
    cur_capacity = DEFAULT_VECTOR_SIZE;
  }
}



template <typename T>
ASVector<T>::~ASVector()
{
  if(NULL != container)
  {
    ds_appsrv_free<T>(container, cur_capacity);
    container = NULL;
  }
}



template <typename T>
const T& ASVector<T>::operator[] (uint32 index) const
{
  return container[index];
}

template <typename T>
T& ASVector<T>::operator[] (uint32 index)
{
  return container[index];
}


template <typename T>
uint32 ASVector<T>::capacity() const
{
  return cur_capacity;
}


template <typename T>
uint32 ASVector<T>::size() const
{
  return cur_size;
}


template <typename T>
T& ASVector<T>::back()
{
  return container[cur_size-1];
}


template <typename T>
const T& ASVector<T>::back() const
{
  return container[cur_size-1];
}



template <typename T>
bool ASVector<T>::push_back(const T& object)
{
  // Allocate memory
  if(NULL == container)
  {
    if(0 == cur_capacity)
      return false;

    container = ds_appsrv_alloc<T>(cur_capacity);

    if(NULL == container)
    {
      return false;
    }
  }

  if(cur_size+1 >= cur_capacity)
  {
    if(!resize_array())
    {
      return false;
    }
  }

  container[cur_size++] = object;
  return true;
}



template <typename T>
void ASVector<T>::clear()
{
  if(NULL != container)
  {
    ds_appsrv_free<T>(container, cur_capacity);
    container = NULL;
  }
  cur_size = 0;
}



template <typename T>
void ASVector<T>::pop_back()
{
  if(0 != cur_size)
  {
    cur_size--;
  }
}



template <typename T>
bool ASVector<T>::resize_array()
{
  uint32 new_capacity = cur_capacity * 2;

  if(DS_ASVECTOR_MAX_SIZE < new_capacity)
    return false;

  T* new_container = ds_appsrv_alloc<T>(new_capacity);

  if(NULL == new_container)
    return false;

  memset(new_container, 0, sizeof(T)*new_capacity);
  for(uint32 i=0; i<cur_size; i++)
  {
    new_container[i] = container[i];
  }

  ds_appsrv_free<T>(container, cur_capacity);
  cur_capacity = new_capacity;
  container = new_container;

  return true;
}



template <typename T>
void clear_ptr_vector(ASVector<T*>& vec)
{
  uint32 size = vec.size();
  for(uint32 i = 0; i < size; ++i)
  {
    if(NULL != vec[i])
    {
      ds_appsrv_free<T>(vec[i]);
    }
  }

  vec.clear();
}


#endif /* DS_AS_VECTOR_H */
