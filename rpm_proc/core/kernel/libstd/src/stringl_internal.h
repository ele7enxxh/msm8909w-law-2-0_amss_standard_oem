/*===============================================================================================
 * FILE:        stringl_internal.h
 *
 * DESCRIPTION: Internal header file for libstd string lib implementation
 *
 *              Copyright (c) 1999 - 2010 Qualcomm Technologies Incorporated.
 *              All Rights Reserved. QUALCOMM Proprietary and Confidential.
 *===============================================================================================*/

/*===============================================================================================
 *
 *                            Edit History
 * $Header: //components/rel/rpm.bf/2.1.1/core/kernel/libstd/src/stringl_internal.h#1 $ 
 * $DateTime: 2015/02/25 02:53:43 $
 *
 *===============================================================================================*/

#ifndef __STRINGL_INTERNAL_H__
#define __STRINGL_INTERNAL_H__

#define stringl_to_lower(c)         (c|32)

#define stringl_is_hex_char(c)      (((c >= '0') && (c <= '9')) ||                         \
                                     ((stringl_to_lower(c) >= 'a') && (stringl_to_lower(c) <= 'f')))

#define stringl_is_decimal_char(c)  ((c >= '0') && (c <= '9'))

#define stringl_is_alpha_char(c)    ((stringl_to_lower(c) >= 'a') && (stringl_to_lower(c) <= 'z'))
    
#endif //__STRINGL_INTERNAL_H__
