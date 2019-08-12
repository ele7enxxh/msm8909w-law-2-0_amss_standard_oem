/*===============================================================================================
 * FILE:        wstrlen.c
 *
 * DESCRIPTION: Implementation of API wstrlen.
 *
 *              Copyright (c) 2011 Qualcomm Technologies Incorporated.
 *              All Rights Reserved. QUALCOMM Proprietary and Confidential.
 *===============================================================================================*/
 
/*===============================================================================================
 *
 *                            Edit History
 * $Header: //components/rel/rpm.bf/2.1.1/core/kernel/libstd/src/wstrlen.c#1 $ 
 * $DateTime: 2015/02/25 02:53:43 $
 *
 *===============================================================================================*/
#include <stringl/stringl.h>

size_t wstrlen(const wchar *s)
{
   size_t i;
   for (i=0; *s; s++, i++);
   return i;
}
