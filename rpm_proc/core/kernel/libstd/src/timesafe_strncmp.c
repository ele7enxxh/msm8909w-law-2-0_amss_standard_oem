/*===============================================================================================
 * FILE:        timesafe_strncmp.c
 *
 * DESCRIPTION: Implementation of standard string API timesafe_strncmp.
 *
 *              Copyright (c) 2013 Qualcomm Technologies Incorporated.
 *              All Rights Reserved. Qualcomm Confidential and Proprietary.
 *===============================================================================================*/
 
/*===============================================================================================
 *
 *                            Edit History
 * $Header: //components/rel/rpm.bf/2.1.1/core/kernel/libstd/src/timesafe_strncmp.c#1 $ 
 * $DateTime: 2015/02/25 02:53:43 $
 *
 *===============================================================================================*/

/* Implementations of timesafe strncmp
   This should compare strings in constant time
*/

#include <stdint.h>
#include <stringl/stringl.h>

/* timesafe_strncmp() should perform a strncmp() in constant time (proportional to len) */
/* Function returns 1 if the strings are different and 0 if the strings are the same    */
/* This prevents any information about difference between two strings being leaked      */

int timesafe_strncmp(const char* ptr1, const char* ptr2, size_t len)
{
    uint8_t rc[2] = {0};
    uint8_t index=0;
    uint8_t val;
    uint8_t* p1 = (uint8_t*) ptr1;
    uint8_t* p2 = (uint8_t*) ptr2;

    while (len > 0)
    {
        /* The XOR will be zero iff the two pointers contain the same value.             */
        rc[index] = rc[index] | (*p1 ^ *p2);

        /* If *p1 was null then set index to 1 in perpetuity.                            */
        /* We don't have to worry about *p2 as if *p2 == NULL and *p2 != *p1             */
        /* then the function returns a value which declares this to be non-equal anyway. */
        val = *p1;
        val = (val >> 4) | (val & 0x0F);
        val = (val >> 2) | (val & 0x03);
        val = (val >> 1) | (val & 0x01);
        index = index | (val ^ 0x01);

        /* Setup for next iteration.                                                      */
        len--;
        p1++;
        p2++;
    }

    /* Reduce rc to 1 if rc is non-zero and 0 if rc is zero.                          */
    rc[0] = (rc[0] >> 4) | (rc[0] & 0x0F);
    rc[0] = (rc[0] >> 2) | (rc[0] & 0x03);
    rc[0] = (rc[0] >> 1) | (rc[0] & 0x01);

    return (int) rc[0];
}
