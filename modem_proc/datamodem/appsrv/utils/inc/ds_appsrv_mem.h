/*==============================================================================

                            ds_appsrv_mem.h

GENERAL DESCRIPTION
  Operator overload for default new and delete. This is for default data types
  and c-structs ONLY. C++ structs and classes should overload new and delete.

  Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/12/15    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_APPSRV_MEM_H
#define DS_APPSRV_MEM_H

#include "comdef.h"
#include "modem_mem.h"
#include <stringl/stringl.h>


enum ds_appsrv_mem_e_type { APPSRV_MEM };

// Placement new
void* operator new(unsigned int size, void* alloc, ds_appsrv_mem_e_type mem_type) throw();
void  operator delete(void* obj, void* alloc, ds_appsrv_mem_e_type mem_type) throw();

/* Note1: Prefer to use non-template functions for array allocate/deallocate for
          default types and c-structs since template functions have additional
          overhead (and loop for array allocation) for constructor/destructor
          calls.

   Note2: Alloc/dealloc pairs
    Allocator                     Deallocator
    ----------------------------------------------------------------
    ds_appsrv_alloc(uint)         ds_appsrv_free(void*)
                                  ds_appsrv_free<>(void*)

    ds_appsrv_alloc(uint, uint)   ds_appsrv_free(void*)
                                  ds_appsrv_free<>(void*)

    ds_appsrv_alloc<>()           ds_appsrv_free(void*)
                                  ds_appsrv_free<>(void*)

    ds_appsrv_alloc<>(uint)       ds_appsrv_free<>(void*, uint)
*/

/* =============================== Allocators =============================== */
void* ds_appsrv_alloc(unsigned int obj_size);
void* ds_appsrv_alloc(unsigned int obj_size, unsigned int array_size);

/* Note: Does not call constructor. Use placement new for constructor call. */
template<typename T>
T* ds_appsrv_alloc() throw();

/* Note: Loops through the array and calls the default constructor. */
template<typename T>
T* ds_appsrv_alloc(unsigned int array_size) throw();

/* ============================== Deallocators ============================== */
void ds_appsrv_free(void*& obj) throw();

template<typename T>
void ds_appsrv_free(T*& obj) throw();

template<typename T>
void ds_appsrv_free(T*& obj, unsigned int array_size) throw();



/* =========================== void specialization ========================== */
/* ds_appsrv_alloc() should be used instead of ds_appsrv_alloc<void>
   ds_appsrv_alloc<void> will return NULL */
template<>
void* ds_appsrv_alloc() throw();

template<>
void* ds_appsrv_alloc(unsigned int array_size) throw();

template<>
void ds_appsrv_free(void*& obj) throw();

template<>
void ds_appsrv_free(void*& obj, unsigned int array_size) throw();






/* ========================= Template implementation ======================== */
template<typename T>
T* ds_appsrv_alloc() throw()
{
  T* obj = static_cast<T*>(modem_mem_alloc(sizeof(T), MODEM_MEM_CLIENT_DATA));
  if(NULL != obj)
  {
    memset((void*)obj, 0, sizeof(T));
  }
  return obj;
}


template<typename T>
T* ds_appsrv_alloc(unsigned int array_size) throw()
{
  if(0 == array_size)
    return NULL;

  T* obj = static_cast<T*>(modem_mem_alloc(sizeof(T) * array_size, MODEM_MEM_CLIENT_DATA));
  if(NULL != obj)
  {
    memset((void*)obj, 0, sizeof(T) * array_size);
    for (int i = 0; i < array_size; ++i)
    {
      new((void*)&(obj[i]), APPSRV_MEM) T();
    }
  }
  return obj;
}


template<typename T>
void ds_appsrv_free(T*& obj) throw()
{
  if(NULL != obj)
  {
    obj->~T();
    modem_mem_free((void*)obj, MODEM_MEM_CLIENT_DATA);
  }
  obj = NULL;
}


template<typename T>
void ds_appsrv_free(T*& obj, unsigned int array_size) throw()
{
  if(NULL != obj)
  {
    for(uint32 i = 0; i < array_size; ++i)
    {
      obj[i].~T();
    }
    modem_mem_free((void*)obj, MODEM_MEM_CLIENT_DATA);
  }
  obj = NULL;
}

#endif /* DS_APPSRV_MEM_H */
