/*============================================================================
@file CoreHeap.c

Implements CoreHeap.h

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.mpss/3.5.c12.3/power/utils/src/CoreHeap.c#1 $
============================================================================*/
#include "CoreHeap.h"
#include "DALSys.h"

#ifdef __cplusplus
extern "C" {
#endif

void* Core_Malloc( unsigned int size )
{
  void *ptr = NULL;
  return ( DALSYS_Malloc( size, &ptr ) == DAL_SUCCESS ? ptr : NULL );
}

void Core_Free( void *ptr )
{
  if ( ptr )
  {
    DALSYS_Free( ptr );
  }
}

#ifdef __cplusplus
}
#endif
