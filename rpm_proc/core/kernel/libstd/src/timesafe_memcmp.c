/*===============================================================================================
 * FILE:        timesafe_memcmp.c
 *
 * DESCRIPTION: Implementation of standard string API timesafe_memcmp.
 *
 *              Copyright (c) 2013 Qualcomm Technologies Incorporated.
 *              All Rights Reserved. Qualcomm Confidential and Proprietary.
 *===============================================================================================*/
 
/*===============================================================================================
 *
 *                            Edit History
 * $Header: //components/rel/rpm.bf/2.1.1/core/kernel/libstd/src/timesafe_memcmp.c#1 $ 
 * $DateTime: 2015/02/25 02:53:43 $
 *
 *===============================================================================================*/

/* Implementations of timesafe memcmp
   This should compare memory in constant time
*/

#include <stdint.h>
#include <stringl/stringl.h>

/* timesafe_memcmp() should perform a memcmp() in constant time (proportional to len) */
/* Function returns 1 if the strings are different and 0 if the strings are the same  */
/* This prevents any information about difference between two buffers being leaked.   */

int timesafe_memcmp(const void* ptr1, const void* ptr2, size_t len)
{
    uint8_t rc = 0;
    uint8_t* p1 = (uint8_t*) ptr1;
    uint8_t* p2 = (uint8_t*) ptr2;

    while (len > 0)
    {
        /* *p1 XOR * p2 will be zero if and only if *p1 == *p2                  */
        /* The OR means that once rc becomes non-zero, it will remain that way. */
        rc = rc | (*p1 ^ *p2);

        /* Setup for next iteration                                             */
        len--;
        p1++;
        p2++;
    }

    /* Reduce rc to 1 if rc is non-zero and 0 if rc is zero.                    */
    rc = (rc >> 4) | (rc & 0x0F);
    rc = (rc >> 2) | (rc & 0x03);
    rc = (rc >> 1) | (rc & 0x01);

    return (int) rc;
}
