#ifndef GPTYPES_H
#define GPTYPES_H

/**
@file   gpTypes.h
@brief  Type definitions defined by GP standard.
*/

/*===========================================================================
   Copyright (c) 2013-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/libs/storage/inc/gpTypes.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
10/15/13    dp      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include <stdint.h>

#define TEE_Malloc(size, x) qsee_malloc(size)
#define TEE_Free(ptr)       qsee_free(ptr)

/*----------------------------------------------------------------------------
 * TEE data types
 * -------------------------------------------------------------------------*/

/* TEE core framework data types */
typedef unsigned int bool;
typedef unsigned int size_t;

#define true  0x1
#define false 0x0

// Return type
typedef uint32_t TEE_Result;

/*----------------------------------------------------------------------------
 * TEE Core framework constants
 * -------------------------------------------------------------------------*/

/* TEE Error codes */

#define TEE_SUCCESS                           0x00000000    /* Operation success */
#define TEE_ERROR_GENERIC                     0xFFFF0000    /* Non-specific error */
#define TEE_ERROR_ACCESS_DENIED               0xFFFF0001    /* Access priveleges are not sufficient */
#define TEE_ERROR_CANCEL                      0xFFFF0002    /* Operation was cancelled */
#define TEE_ERROR_ACCESS_CONFLICT             0xFFFF0003    /* Concurrent accesses caused conflict */
#define TEE_ERROR_EXCESS_DATA                 0xFFFF0004    /* Too much data for the requested operation was provided */
#define TEE_ERROR_BAD_FORMAT                  0xFFFF0005    /* The input data was of invalid format */
#define TEE_ERROR_BAD_PARAMETERS              0xFFFF0006    /* The input parameters were invalid */
#define TEE_ERROR_BAD_STATE                   0xFFFF0007    /* The operation is not valid in current state */
#define TEE_ERROR_ITEM_NOT_FOUND              0xFFFF0008    /* The requested data could not be found */
#define TEE_ERROR_NOT_IMPLEMENTED             0xFFFF0009    /* The operation should be implemented, but does not currently exist */
#define TEE_ERROR_NOT_SUPPORTED               0xFFFF000A    /* The operation is valid, but not supported */
#define TEE_ERROR_NO_DATA                     0xFFFF000B    /* Expected data was missing */
#define TEE_ERROR_OUT_OF_MEMORY               0xFFFF000C    /* System ran out of resources */
#define TEE_ERROR_BUSY                        0xFFFF000D    /* System is busy with something else */
#define TEE_ERROR_COMMUNICATION               0xFFFF000E    /* Communication with a remote party failed */
#define TEE_ERROR_SECURITY                    0xFFFF000F    /* A security fault was detected */
#define TEE_ERROR_SHORT_BUFFER                0xFFFF0010    /* The buffer is NULL or is too small */
#define TEE_PENDING                           0xFFFF2000    /* The operation is pending action */
#define TEE_ERROR_TIMEOUT                     0xFFFF3001    /* The operation has timed out and could not complete */
#define TEE_ERROR_OVERFLOW                    0xFFFF300F    /* An integer overflow has occurred */
#define TEE_ERROR_TARGET_DEAD                 0xFFFF3024    /* TA has shut down */
#define TEE_ERROR_STORAGE_NO_SPACE            0xFFFF3041    /* Insufficient space available to create persistent object */
#define TEE_ERROR_MAC_INVALID                 0xFFFF3071    /* The computed MAC does not correspond with passed-in param */
#define TEE_ERROR_SIGNATURE_INVALID           0xFFFF3072    /* The signature is invalid */
#define TEE_ERROR_TIME_NOT_SET                0xFFFF5000    /* Persistent object is not currently set */
#define TEE_ERROR_TIME_NEEDS_RESET            0xFFFF5001    /* Persistent object is set, but may be corrupted and can no longer be trusted */

/* Implementation defined errors */
#define TEE_ERROR_MAX_SESSIONS_EXCEEDED       0x0000FFF1

#define TEE_OBJECT_ID_MAX_LEN                 64

#endif /* GPTYPES_H */
