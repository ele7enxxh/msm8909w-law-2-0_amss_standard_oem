/*===============================================================================================
 * FILE:        libstd_std_scanul.c
 *
 * DESCRIPTION: Implementation of string API std_scanul
 *
 *              Copyright (c) 1999 - 2010 Qualcomm Technologies Incorporated.
 *              All Rights Reserved. QUALCOMM Proprietary and Confidential.
 *===============================================================================================*/
 
/*===============================================================================================
 *
 *                            Edit History
 * $Header: //components/rel/rpm.bf/2.1.1/core/kernel/libstd/src/libstd_std_scanul.c#1 $ 
 * $DateTime: 2015/02/25 02:53:43 $
 *
 *===============================================================================================*/
#include <limits.h>
#include <stringl/stringl.h>
#include "stringl_internal.h"

unsigned int std_scanul( const char *  pchBuf,
                   int           nRadix,
                   const char ** ppchEnd,
                   int *         pnError )
{
    int nError = 0;   // SUCCESS
    const unsigned char *pch = (const unsigned char*)pchBuf;
    long long llVal = 0;
    unsigned long ulVal;
    unsigned char c;

    if (nRadix < 0 || nRadix > 36 || nRadix == 1) {
        nError = STD_BADPARAM;
        goto done;
    }

    // Skip whitespace   
    while ( (c = *pch) == ' ' || c == '\t' ) {
        ++pch;
    }

    // Scan sign
    if (c == '-') {
        nError = STD_NEGATIVE;
        ++pch;
    } else if (c == '+') {
        ++pch;
    }

    // Read optional prefix
    if ((0 == nRadix || 16 == nRadix) &&
       '0' == pch[0] && 'x' == stringl_to_lower(pch[1]) && 
       stringl_is_hex_char(pch[2])) {
        pch += 2;
        nRadix = 16;
    }
   
    if (0 == nRadix && '0' == pch[0]) {
        nRadix = 8;
    }

    if (0 == nRadix) {
        nRadix = 10;
    }

    // Read digits
    {
        const unsigned char* pchStartOfDigits = pch;

        for (;;) 
        {
            int nDigit;

            c = *pch;

            if (stringl_is_decimal_char(c)) {
                nDigit = c - '0';
            } else if (stringl_is_alpha_char(c)) {
                nDigit = (stringl_to_lower(c) - 'a') + 10;
            } else {
                break;
            }

            if (nDigit >= nRadix) {
                break;
            }

            ++pch;

            llVal = llVal * nRadix + nDigit;
            if (llVal > UINT_MAX) {
                llVal = UINT_MAX;
                nError = STD_OVERFLOW;
                break;
            }
        }

        if (pch == pchStartOfDigits) {
            pch = (const unsigned char*)pchBuf;
            nError = STD_NODIGITS;
        }
    }

    done:
        ulVal = (unsigned long) llVal;
        if (nError == STD_NEGATIVE) {
            ulVal = (unsigned int) (-1 * (signed int)ulVal);
            if (ulVal == 0) {
                nError = 0; // SUCCESS
            }
        }

        if (ppchEnd) {
            *ppchEnd = (const char*)pch;
        }
        if (pnError) {
            *pnError = nError;
        }
    
    return ulVal;
}

/*
unsigned long strtoul( const char *pch, const char **ppchEnd, int nRadix)
{
  unsigned long ul;
  
  int err = std_scanul(pch, nRadix, ppchEnd, &ul);
  
    if (err == LIBSTD_ERR_NEGATIVE && ((int)ul) < 0) {
        //
        // strtoul() range limits are ill-defined for signed values
        //
        err = 0;
    }
  
    if (err == STD_NEGATIVE || err == STD_OVERFLOW) {
        ul = ULONG_MAX;
        errno = ERANGE;
    } else if (err == LIBSTD_ERR_BADPARAM) {
        ul = 0;
        errno = EINVAL;
    } else {
        // http://www.opengroup.org/onlinepubs/007908799/xsh/strtoul.html
        // "Because 0 and ULONG_MAX are returned on error and are also valid
        //  returns on success, an application wishing to check for error
        //  situations should set errno to 0, then call strtoul(), then check
        //  errno."
    }
    return ul;
}
*/
