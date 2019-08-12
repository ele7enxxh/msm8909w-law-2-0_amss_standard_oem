/*===============================================================================================
 * FILE:        secure_memset.c
 *
 * DESCRIPTION: Implementation of standard string API secure_memset.
 *
 *              Copyright (c) 2013 Qualcomm Technologies Incorporated.
 *              All Rights Reserved. Qualcomm Confidential and Proprietary.
 *===============================================================================================*/
 
/*===============================================================================================
 *
 *                            Edit History
 * $Header: //components/rel/rpm.bf/2.1.1/core/kernel/libstd/src/secure_memset.c#1 $ 
 * $DateTime: 2015/02/25 02:53:43 $
 *
 *===============================================================================================*/

/* Example implementation of a secure_memset() function
   This file MUST always be separate from all other files
*/

#include <stdint.h>
#include <stringl/stringl.h>

/* This function MUST NEVER be included in a file with any other functions    */
/* Security relies on the fact that the linker cannot optimize function calls */
/* between separate files and so cannot strip this functionality from the     */
/* execution flow.                                                            */

void* secure_memset(void* ptr, int value, size_t len)
{
    uint8_t ivalue;
    uint8_t* iptr;

    ivalue = (uint8_t) value;
    iptr = (uint8_t*) ptr;

    while(len>0)
    {
        *iptr = ivalue;
        iptr++;
        len--;
    }

    return ptr;
}
