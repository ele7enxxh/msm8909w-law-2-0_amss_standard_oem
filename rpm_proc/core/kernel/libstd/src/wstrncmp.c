/*===============================================================================================
 * FILE:        wstrncmp.c
 *
 * DESCRIPTION: Implementation of API wstrcmp.
 *
 *              Copyright (c) 2011 Qualcomm Technologies Incorporated.
 *              All Rights Reserved. QUALCOMM Proprietary and Confidential.
 *===============================================================================================*/
 
/*===============================================================================================
 *
 *                            Edit History
 * $Header: //components/rel/rpm.bf/2.1.1/core/kernel/libstd/src/wstrncmp.c#1 $ 
 * $DateTime: 2015/02/25 02:53:43 $
 *
 *===============================================================================================*/
#include <stringl/stringl.h>

int wstrncmp(const wchar *s1, const wchar *s2, size_t n)
{
    wchar ch1, ch2;
    int diff;
    if (n > 0)
    {
        do 
        {
            ch1 = *s1++;
            ch2 = *s2++;
            diff = ch1 - ch2;
            if (0 != diff)
            {
                return diff;
            }
            if ('\0' == ch1)
            {
                break;
            }
        } while (--n);
    }
    return 0;
}
