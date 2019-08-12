/*===========================================================================

                       Boot Heap Memory Source File

GENERAL DESCRIPTION
  This file contains functions which override the default c++ new and delete
  operators into DAL malloc and free functions

Copyright 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_heap.cpp#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/12/11   dh      Rename file to boot_heap.cpp, switch all DAL calls to boot extern
                   interface calls. Add check for succesful operation for delete as well.
08/04/11   aus     Fixed the BL_VERIFY operation
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
extern "C"
{
  #include "boot_extern_dal_interface.h"
  #include "boot_error_if.h"
}

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/
void *operator new (size_t s)
{
  void *retval;
  DALResult result;

  result = boot_DALSYS_Malloc(s, &retval);
  BL_VERIFY(result == DAL_SUCCESS, BL_ERR_OTHERS);
  
  return retval;
}


void operator delete (void *p)
{
  DALResult result;
  result = boot_DALSYS_Free(p);
  BL_VERIFY(result == DAL_SUCCESS, BL_ERR_OTHERS);
}


void *operator new[] (size_t s)
{
  void *retval;
  DALResult result;

  result = boot_DALSYS_Malloc(s, &retval);
  BL_VERIFY(result == DAL_SUCCESS, BL_ERR_OTHERS);
  
  return retval;
}


void operator delete[] (void *p)
{
  DALResult result;
  result = boot_DALSYS_Free(p);
  BL_VERIFY(result == DAL_SUCCESS, BL_ERR_OTHERS);
}


