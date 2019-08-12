/*===============================================================================================
 * FILE:        strncasecmp.c
 *
 * DESCRIPTION: Implementation of standard string API strncasecmp.
 *
 *              Copyright (c) 1999 - 2010 Qualcomm Technologies Incorporated.
 *              All Rights Reserved. QUALCOMM Proprietary and Confidential.
 *===============================================================================================*/
 
/*===============================================================================================
 *
 *                            Edit History
 * $Header: //components/rel/rpm.bf/2.1.1/core/kernel/libstd/src/strncasecmp.c#1 $ 
 * $DateTime: 2015/02/25 02:53:43 $
 *
 *===============================================================================================*/

 #include <stringl/stringl.h>

static __inline int cmp_char_i(unsigned char c1, unsigned char c2)
{
    /* Convert UC to LC */
    if (('A' <= c1) && ('Z' >= c1))
    {
        c1 = c1 - 'A' + 'a';
    }
    if (('A' <= c2) && ('Z' >= c2))
    {
        c2 = c2 - 'A' + 'a';
    }
    return (c1 - c2);
}

int strncasecmp(const char * s1, const char * s2, size_t n)
{
    unsigned char c1, c2;
    int diff;
    if (n > 0)
    {
        do
        {
            c1 = (unsigned char)(*s1++);
            c2 = (unsigned char)(*s2++);
            diff = cmp_char_i(c1, c2);
            if (0 != diff)
            {                
                return diff;
            }
            if ('\0' == c1)
            {
                break;
            }
        } while (--n);
    }
    return 0;
}
