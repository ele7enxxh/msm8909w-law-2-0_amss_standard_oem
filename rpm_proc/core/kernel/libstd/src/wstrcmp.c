/*===============================================================================================
 * FILE:        wstrcmp.c
 *
 * DESCRIPTION: Implementation of API wstrcmp.
 *
 *              Copyright (c) 2011 Qualcomm Technologies Incorporated.
 *              All Rights Reserved. QUALCOMM Proprietary and Confidential.
 *===============================================================================================*/
 
/*===============================================================================================
 *
 *                            Edit History
 * $Header: //components/rel/rpm.bf/2.1.1/core/kernel/libstd/src/wstrcmp.c#1 $ 
 * $DateTime: 2015/02/25 02:53:43 $
 *
 *===============================================================================================*/
#include <stringl/stringl.h>

int wstrcmp(const wchar *s1, const wchar *s2)
{
    wchar ch1, ch2;

    do
    {
        ch1 = *s1++;
        ch2 = *s2++;
    } while (ch1 && (ch1 == ch2));

    return ch1 - ch2;
}
