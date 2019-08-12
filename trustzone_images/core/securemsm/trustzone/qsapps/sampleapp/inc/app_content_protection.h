#ifndef APP_CONTENT_PROTECTION_H
#define APP_CONTENT_PROTECTION_H

/** @file app_content_protection.h
 * @brief
 * This file contains the definitions of the constants, data structures and
 * and functions for Content Protection Copy implementation
 */
/*===========================================================================
  Copyright (c) 2011-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/sampleapp/inc/app_content_protection.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/20/13   wt      Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/** @addtogroup Content_Protection
  @} */

#include "stdlib.h"
#include "IxErrno.h"
#include "qsee_log.h"
#include "qsee_heap.h"
#include "qsee_core.h"
#include "qsee_services.h"
#include "stringl.h"


//globals
#define TZ_SC_AES_128_LEN 16
#define TZ_OUT_BUF_MAX    512

typedef enum SampleClientResult {
  SAMPLE_CLIENT_SUCCESS = 0,
  SAMPLE_CLIENT_ERROR_COPY_FAILED,
  SAMPLE_CLIENT_ERROR_INIT_FAILED,
  SAMPLE_CLIENT_ERROR_TERMINATE_FAILED,
  SAMPLE_CLIENT_ERROR_ION_MALLOC_FAILED,
  SAMPLE_CLIENT_ERROR_ION_FREE_FAILED,
  SAMPLE_CLIENT_ERROR_NSS_COPY_FAILED,
  SAMPLE_CLIENT_ERROR_SNS_COPY_FAILED,
  SAMPLE_CLIENT_ERROR_MEM_SEG_COPY_FAILED,
  SAMPLE_CLIENT_ERROR_INVALID_PARAMS,
  SAMPLE_CLIENT_ERROR_FEATURE_NOT_SUPPORTED,
  SAMPLE_CLIENT_FAILURE = 0x7FFFFFFF
} SampleClientResult;

/* Structure to hold the pointers to multiple buffer space*/
struct tz_mem_array_seg
{
  uint32 _address;
  uint32 _size;
}__attribute__ ((packed));


/* Structure to hold the pointers to multiple buffer space*/
typedef struct tz_mem_array_s
{
  struct tz_mem_array_seg _tz_mem_seg[TZ_OUT_BUF_MAX];
  uint32 _seg_num;
}__attribute__ ((packed)) tz_mem_array_s_t;

//TZ send command structures
typedef struct tz_sc_copy_req_s
{
  /** First 4 bytes should always be command id */
  uint32                          cmd_id;
  uint32                          copyDir;
  uint32                          nonSecBuffer;
  uint32                          nonSecBufferLength;
  tz_mem_array_s_t                secBufferHandle;
  uint32                          secBufferOffset;
} __attribute__ ((packed)) tz_sc_copy_req_t;

typedef struct tz_sc_copy_rsp_s
{
  /** First 4 bytes should always be command id */
  uint32                          cmd_id;
  uint32                          secBufferLength;
  long                            ret;
} __attribute__ ((packed)) tz_sc_copy_rsp_t;


/**
  This API copies the size bytes from the source buffer to the destination buffer.

  @return
  SAMPLE_CLIENT_SUCCESS               - Success.
  SAMPLE_CLIENT_ERROR_COPY_FAILED     - Failed decryption

  @param[out]	      *outBuff        buffer to receive data from inBuff
  @param[in]	       outBuffLen     size of outBuff
  @param[in]	      *inBuff         source buffer to copy from
  @param[in]           inBuffLen      in: size to copy from inBuff

  @dependencies
  None

  @sideeffects
  None
*/
SampleClientResult mem_seg_copy(uint8* outBuff, uint32 outBuffLen, uint8* inBuff, uint32 inBuffLen);

/**
  This API copies the payload in the non-secure buffer referenced by
  the *input parameter into the secure buffer referenced by the output parameter.
  If inputLength is not a multiple of 16 bytes, use CTS.

  @return
  SAMPLE_CLIENT_SUCCESS               - Success.
  SAMPLE_CLIENT_ERROR_COPY_FAILED     - Failed decryption

  @param[in]	*nonSecBuffer        non-secure buffer containing the video data
  @param[in]	 nonSecBufferLength  number of bytes in the input payload
  @param[in]    *secBufferHandle     reference to the secure buffer which will receive the
                                     decrypted data
  @param[in]     secBufferOffset     offset from the beginning of the secure buffer where the
                                     data will be written
  @param[out]   *secBufferLength     number of bytes written into the secure buffer

  @dependencies
  None

  @sideeffects
  None
*/
SampleClientResult Content_Protection_Copy_NonsecureToSecure
(
  const uint8       *nonSecBuffer,
  const uint32       nonSecBufferLength,
  tz_mem_array_s_t  *secBufferHandle,
  uint32             secBufferOffset,
  uint32            *secBufferLength
);

/**
  This API Copies data from secure to non-secure buffer.  This API is meant for
  internal testing only and works only on internal builds.

  @return
  SAMPLE_CLIENT_SUCCESS             - Success.
  SAMPLE_CLIENT_ERROR_COPY_FAILED   - Failed to copy

  @param[in]     *nonSecBuffer           non-secure buffer to receive the data
  @param[in]      nonSecBufferLength     size of non-secure buffer
  @param[in]     *secBufferHandle        reference to the secure buffer with the data
  @param[in]      secBufferOffset        offset from the beginning of the secure buffer where the data is written
  @param[in/out] *secBufferLength        in:  size of secure buffer
                                         out: number of bytes written into the non-secure buffer

  @dependencies
  None

  @sideeffects
  None
*/
SampleClientResult Content_Protection_Copy_SecureToNonsecure
(
  const uint8       *nonSecBuffer,
  const uint32       nonSecBufferLength,
  tz_mem_array_s_t  *secBufferHandle,
  uint32             secBufferOffset,
  uint32            *secBufferLength
);

#endif //APP_CONTENT_PROTECTION_H
