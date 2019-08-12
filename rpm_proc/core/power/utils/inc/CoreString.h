#ifndef CORE_STRING_H
#define CORE_STRING_H

/*==============================================================================
  @file CoreString.h

  String related Function/Structure Declarations for portability.

  Copyright (c) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/rpm.bf/2.1.1/core/power/utils/inc/CoreString.h#1 $ */


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#if defined (WINSIM) || defined (WIN_DEBUGGER_EXTENSION)

#include "stdio.h"
#define core_snprintf sprintf_s 

#elif defined (__GNUC__) || defined(USE_STDLIB)

#include "stdio.h"
#define core_snprintf snprintf 

#else

#include "AEEstd.h"
#define core_snprintf std_strlprintf 

#endif


#endif /* CORE_STRING_H */
