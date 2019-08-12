/*===============================================================================================
 * FILE:        strcasecmp.c
 *
 * DESCRIPTION: Implementation of standard string API strcasecmp.
 *
 *              Copyright (c) 1999 - 2010 Qualcomm Technologies Incorporated.
 *              All Rights Reserved. QUALCOMM Proprietary and Confidential.
 *===============================================================================================*/
 
/*===============================================================================================
 *
 *                            Edit History
 * $Header: //components/rel/rpm.bf/2.1.1/core/kernel/libstd/src/strcasecmp.c#1 $ 
 * $DateTime: 2015/02/25 02:53:43 $
 *
 *===============================================================================================*/
#include <limits.h> 
#include <stringl/stringl.h>

int strcasecmp(const char * s1, const char * s2)
{
    return strncasecmp(s1, s2, UINT_MAX);
}
