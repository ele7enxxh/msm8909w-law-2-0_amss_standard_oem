#ifndef __DSS_MAP_OBJECT_2_DESCRIPTOR_H__
#define __DSS_MAP_OBJECT_2_DESCRIPTOR_H__

/*======================================================

FILE:  DSS_MapObject2Descriptor.h

SERVICES:
Template class which implements a map (array) data structure associating
objects with descriptors (handlers, file descriptors, net handlers).
Internally a descriptor is associated with an index in the array.
It allows:
  Inserting new object in a free cell in the array, returning a descriptor.
  Removing an object IAW (in accordance with) a descriptor.
  Getting an object IAW a descriptor.
  Other capabilities.

GENERAL DESCRIPTION:

=====================================================

Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_MapObject2Descriptor.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-11-15 yl  File created.

===========================================================================*/

//===================================================================
//   Includes and Public Data Declarations
//===================================================================

//-------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------
#include "ds_Utils_StdErr.h"
#include "ps_handle_mgr.h"

//-------------------------------------------------------------------
// Constant / Define Declarations
//-------------------------------------------------------------------
#define DSS_MAP_OBJECT_2_DESCRIPTOR_INDEX_INVALID (-1)
#define DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID (PS_HANDLE_MGR_INVALID_HANDLE)

//-------------------------------------------------------------------
// Type Declarations (typedef, struct, enum, etc.)
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Constant Data Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Forward Declarations
//-------------------------------------------------------------------


//===================================================================
//              Macro Definitions
//===================================================================


//===================================================================
//              Class Definitions
//===================================================================

//===================================================================
//  CLASS:      DSS_MapObject2Descriptor
//
//  DESCRIPTION:
//    This class is a template class implementing a list of objects.
//    Accessing the objects in the list is possible via:
//    - its index in the array
//    - its descriptor.
//    Descriptors a managed by "ps_handle_mgr" module.
//    This class template uses a type T in its definition, yet it
//    holds objects of T* (pointers to T) in its list.
//
//  HOW TO USE:
//    Define a new object of type DSSMapObject2Descriptor.
//    Use DSSMapObject2Descriptor::Insert to insert new objects to
//    to the list.
//    Use DSSMapObject2Descriptor::Remove to remove existing objects
//    from the list
//    Other methods exist as well (e.g. accessing objects).
//
//===================================================================

template<typename T, int16 MAX>
class DSSMapObject2Descriptor
{
//-------------------------------------------------------------------
//  Constructors/Desctructors
//-------------------------------------------------------------------
  public:

    // C-tor
    DSSMapObject2Descriptor(
       ps_handle_mgr_client_enum_type clientId,
       int16 minBaseDescriptor,
       int32 maxBaseDescriptor);

    // D-tor
    virtual ~DSSMapObject2Descriptor();

//-------------------------------------------------------------------
//  Interface members
//-------------------------------------------------------------------
  public:
    void Init();
    int32 Insert(T* object);
    AEEResult Remove(int32 descriptor, T** ppObject);
    AEEResult GetObjectByDescriptor(int32 descriptor, T** ppObject);
    bool DescriptorIsWithinBoundaries(int32 descriptor) const;
    AEEResult DescriptorExist(int32 descriptor, bool* pExist) const;
    bool Full() const;

//-------------------------------------------------------------------
//  Get/Set functions for protected members access
//-------------------------------------------------------------------
    T* GetObjectByIndexUnchecked(int16 index) const;
    int16 GetMaxIndex() const;

//-------------------------------------------------------------------
//  Protected members
//-------------------------------------------------------------------


//-------------------------------------------------------------------
//  Private structures
//-------------------------------------------------------------------
  private:

    struct DSSMapItem
    {
      T* mpObject;
      int32 descriptor;
    };

//-------------------------------------------------------------------
//  Private methods
//-------------------------------------------------------------------
  private:

    DSSMapObject2Descriptor();
    int16 FindFirstFreeIndex();
    AEEResult GenerateNewDescriptor();
    AEEResult GetIndexByDescriptor(int32 descriptor, int16* index) const;
    bool IndexIsValid(int16 index) const;

//-------------------------------------------------------------------
//  Data members
//-------------------------------------------------------------------
  private:

    // Array holding all T* objects
    DSSMapItem mArray[MAX];

    // ps_handle client id
    ps_handle_mgr_client_enum_type mPsHandleClientId;

    // ps_handle min base descriptor;
    // Note: defined as int16 and not int32 like mPsHandleMaxBaseDescriptor
    // since ps_handle_mgr_init_client defines it this way 
    int16 mPsHandleMinBaseDescriptor;

    // ps_handle max base descriptor;
    int32 mPsHandleMaxBaseDescriptor;

    // Number of T* held by this list 
    int32 mCounter;
      
//-------------------------------------------------------------------
//  Memory management
//-------------------------------------------------------------------

};

template <typename T,int16 MAX>
DSSMapObject2Descriptor<T, MAX>::
DSSMapObject2Descriptor(
  ps_handle_mgr_client_enum_type clientId,
  int16 minBaseDescriptor,
  int32 maxBaseDescriptor):
mPsHandleClientId(clientId),
mPsHandleMinBaseDescriptor(minBaseDescriptor),
mPsHandleMaxBaseDescriptor(maxBaseDescriptor),
mCounter(0)
{
  // initialize array
  for (int i = 0; i < MAX; i++)
  {
    mArray[i].mpObject = NULL;
    mArray[i].descriptor = DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID;
  }
}

template <typename T,int16 MAX>
DSSMapObject2Descriptor<T, MAX>::
~DSSMapObject2Descriptor()
{
  // Empty
}

template <typename T,int16 MAX>
inline void
DSSMapObject2Descriptor<T, MAX>::
Init()
{
  // initialize ps_handle_mgr used for FD allocation
  ps_handle_mgr_init_client(
    mPsHandleClientId,
    MAX,
    mPsHandleMinBaseDescriptor,
    mPsHandleMaxBaseDescriptor);
}

template <typename T,int16 MAX>
int32
DSSMapObject2Descriptor<T, MAX>::
Insert(T* pObject)
{
  int32 newDescriptor = DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID;

  // Check if object ptr is NULL
  if (NULL == pObject)
  {
    LOG_MSG_ERROR_0(
"Insert - object ptr is NULL %d");

    return QDS_EFAULT;
  }

  // find the first empty cell in the array
  int16 index = FindFirstFreeIndex();

  if (DSS_MAP_OBJECT_2_DESCRIPTOR_INDEX_INVALID == index)
  {
    LOG_MSG_ERROR_0(
"Insert cannot find free index for new object");

    goto bail;
  }

  // Allocate a new object descriptor
  newDescriptor = ps_handle_mgr_get_handle(
    mPsHandleClientId,
    index);

  // Check that descriptor is valid
  if (!DescriptorIsWithinBoundaries(newDescriptor))
  {
      LOG_MSG_ERROR_1(
"Insert cannot allocate the descriptor %d for the new object", newDescriptor);
      
      goto bail;
  }

  // Set the new object into a free location
  mCounter++;
  mArray[index].mpObject = pObject;
  mArray[index].descriptor = newDescriptor;

  return mArray[index].descriptor;
bail:
  return DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID;
}

template <typename T,int16 MAX>
AEEResult
DSSMapObject2Descriptor<T, MAX>::
Remove(int32 descriptor, T** ppObject)
{
  AEEResult res = AEE_SUCCESS;
  int16 index = DSS_MAP_OBJECT_2_DESCRIPTOR_INDEX_INVALID;

  // Check if object ptr is NULL
  if (NULL == ppObject)
  {
    LOG_MSG_ERROR_0(
"Remove - object ptr is NULL %d");

    return QDS_EFAULT;
  }
  
  // Check that descriptor is valid
  if (!DescriptorIsWithinBoundaries(descriptor))
  {
    LOG_MSG_ERROR_1(
"Remove - bad descriptor %d", descriptor);

    return QDS_EBADF;
  }

  // Get index according to its descriptor
  res = GetIndexByDescriptor(descriptor, &index);
  if (AEE_SUCCESS != res)
  {
    LOG_MSG_ERROR_1(
"Remove - error finding descriptor %d", descriptor);

    return res;
  }

  if (DSS_MAP_OBJECT_2_DESCRIPTOR_INDEX_INVALID == index)
  {
    LOG_MSG_ERROR_1(
"Remove - bad descriptor %d", descriptor);

    return QDS_EBADF;
  }

  // Object is set to an out parameter
  // for the user to be able to use the object (example: free its memory)
  *ppObject = mArray[index].mpObject;

  // The array cell at location index is available again
  mCounter--;
  mArray[index].mpObject = NULL;
  mArray[index].descriptor = DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID;

  return res;
}

template <typename T,int16 MAX>
AEEResult
DSSMapObject2Descriptor<T, MAX>::
GetObjectByDescriptor(int32 descriptor, T** ppObject)
{
  AEEResult res = AEE_SUCCESS;
  int16 index = DSS_MAP_OBJECT_2_DESCRIPTOR_INDEX_INVALID;

  // Check if object ptr is NULL
  if (NULL == ppObject)
  {
    LOG_MSG_ERROR_0(
"GetObjectByDescriptor - object ptr is NULL %d");

    return QDS_EFAULT;
  }

  // Check that descriptor is valid
  if (!DescriptorIsWithinBoundaries(descriptor))
  {
    LOG_MSG_ERROR_1(
"GetObjectByDescriptor - bad descriptor %d", descriptor);

    return QDS_EBADF;
  }

  // Get the index according to its descriptor
  res = GetIndexByDescriptor(descriptor, &index);
  if (AEE_SUCCESS != res)
  {
    // LOG_MSG_ERROR is redundant here
    return res;
  }

  if (DSS_MAP_OBJECT_2_DESCRIPTOR_INDEX_INVALID == index)
  {
    LOG_MSG_ERROR_1(
"GetObjectByDescriptor - bad descriptor %d", descriptor);

    return QDS_EBADF;
  }

  // Set object to output parameter
  *ppObject = mArray[index].mpObject;

  return res;
}

template <typename T,int16 MAX>
inline T*
DSSMapObject2Descriptor<T, MAX>::
GetObjectByIndexUnchecked(int16 index) const
{
  return mArray[index].mpObject;
}

template <typename T,int16 MAX>
inline int16
DSSMapObject2Descriptor<T, MAX>::
GetMaxIndex() const
{
  return MAX;
}

template <typename T,int16 MAX>
int16
DSSMapObject2Descriptor<T, MAX>::
FindFirstFreeIndex()
{
  for (int i = 0; i < MAX; i++)
  {
    if (DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID ==
      mArray[i].descriptor)
    {
      return i;
    }
  }

  return DSS_MAP_OBJECT_2_DESCRIPTOR_INDEX_INVALID;
}

template <typename T,int16 MAX>
AEEResult
DSSMapObject2Descriptor<T, MAX>::
GetIndexByDescriptor(int32 descriptor, int16* pIndex) const
{
  int16 internalIndex = DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID;

  // Check the index pointer
  if (NULL == pIndex)
  {
    LOG_MSG_ERROR_0(
"GetIndexByDescriptor - internal error - NULL index");

    ASSERT(0);
    return QDS_EFAULT;
  }

  // No need to validate descriptor, since this method is private
  // verification of descriptor should be preformed by the calling method

  // Transform descriptor to index
  internalIndex = ps_handle_mgr_get_index(
    mPsHandleClientId,
    descriptor);

  // Check that index isn't -1 (invalid by definition)
  if (internalIndex == DSS_MAP_OBJECT_2_DESCRIPTOR_INDEX_INVALID)
  {
    // LOG_MSG_ERROR is redundant here
    return QDS_EBADF;
  }

  // Check that index value is valid
  if (!IndexIsValid(internalIndex))
  {
    LOG_MSG_ERROR_1(
"GetIndexByDescriptor - internal error - bad index %d", internalIndex);

    ASSERT(0);
  }

  // Make sure that this index does indeed correspond to an object 
  if (descriptor != mArray[internalIndex].descriptor)
  {
    *pIndex = DSS_MAP_OBJECT_2_DESCRIPTOR_INDEX_INVALID;
  }
  else
  {
    // Set the index into the callers address
    *pIndex = internalIndex;
  }

  return AEE_SUCCESS;
}

template <typename T,int16 MAX>
inline bool
DSSMapObject2Descriptor<T, MAX>::
DescriptorIsWithinBoundaries(int32 descriptor) const
{
  // Check that descriptor is within boundaries
  return ((static_cast<int32>(mPsHandleMinBaseDescriptor) <= descriptor) &&
    (mPsHandleMaxBaseDescriptor > descriptor));
}

template <typename T,int16 MAX>
inline AEEResult
DSSMapObject2Descriptor<T, MAX>::
DescriptorExist(int32 descriptor, bool* pExist) const
{
  AEEResult res = AEE_SUCCESS;
  int16 index = DSS_MAP_OBJECT_2_DESCRIPTOR_INDEX_INVALID;

  // Check that descriptor is valid
  if (NULL == pExist)
  {
    LOG_MSG_ERROR_0(
"DescriptorExist - NULL pointer: pExist");

    return QDS_EFAULT;
  }

  // Check that descriptor is valid
  if (!DescriptorIsWithinBoundaries(descriptor))
  {
    LOG_MSG_ERROR_1(
"DescriptorExist - bad descriptor %d", descriptor);

    return QDS_EBADF;
  }

  // Get the index according to its descriptor
  res = GetIndexByDescriptor(descriptor, &index);
  if (AEE_SUCCESS != res)
  {
    LOG_MSG_ERROR_1(
"DescriptorExist - result error, res %d", res);

    return res;
  }

  // Check received index
  if (DSS_MAP_OBJECT_2_DESCRIPTOR_INDEX_INVALID == index)
  {
    *pExist = false;
  }
  else
  {
    // Set object to output parameter
    *pExist = true;
  }

  return res;
}

template <typename T,int16 MAX>
inline bool
DSSMapObject2Descriptor<T, MAX>::
Full() const
{
  // Check that descriptor is valid
  return (mCounter >= MAX);
}

template <typename T,int16 MAX>
inline bool
DSSMapObject2Descriptor<T, MAX>::
IndexIsValid(int16 index) const
{
  // Check that index is valid
  return ((0 <= index) && (MAX > index));
}

//===================================================================

#endif // __DSS_MAP_OBJECT_2_DESCRIPTOR_H__
