/*==============================================================================

                            ds_appsrv_mem.cpp

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
#include "ds_appsrv_mem.h"

/*==============================================================================
                            C++ Memory alloc/free
==============================================================================*/
// Placement new
void* operator new(unsigned int size, void* alloc, ds_appsrv_mem_e_type mem_type) throw()
{
  // Do nothing
  return alloc;
}

void operator delete(void* obj, void* alloc, ds_appsrv_mem_e_type mem_type) throw()
{
  // Do nothing
}

void* ds_appsrv_alloc(unsigned int obj_size)
{
  void* obj = modem_mem_alloc(obj_size, MODEM_MEM_CLIENT_DATA);
  if(NULL != obj)
  {
    memset(obj, 0, obj_size);
  }
  return obj;
}

void* ds_appsrv_alloc(unsigned int obj_size, unsigned int array_size)
{
  if(0 == obj_size || 0 == array_size)
    return NULL;

  void* obj = modem_mem_alloc(obj_size * array_size, MODEM_MEM_CLIENT_DATA);
  if(NULL != obj)
  {
    memset(obj, 0, obj_size * array_size);
  }
  return obj;
}



/* Specialization for void type */

/* Allocators return NULL because void has size 0 */
template<>
void* ds_appsrv_alloc() throw()
{
  return NULL;
}

template<>
void* ds_appsrv_alloc(unsigned int array_size) throw()
{
  return NULL;
}

/* Deallocator specialization for void since destructor can't be called on void type */
template<>
void ds_appsrv_free(void*& obj) throw()
{
  if(NULL != obj)
  {
    modem_mem_free(obj, MODEM_MEM_CLIENT_DATA);
  }
  obj = NULL;
}

/* Deallocator specialization for void since destructor can't be called on void type */
template<>
void ds_appsrv_free(void*& obj, unsigned int array_size) throw()
{
  if(NULL != obj)
  {
    modem_mem_free(obj, MODEM_MEM_CLIENT_DATA);
  }
  obj = NULL;
}
