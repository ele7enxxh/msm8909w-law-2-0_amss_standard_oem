/*
@file app_crypto.c
@brief This file is a sample code that show how QSEE crypto API's can be used.

*/
/*===========================================================================
   Copyright (c) 2011 - 2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/sampleapp/src/app_gpPersistObj.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------

===========================================================================*/
#include "qsee_log.h"
#include "qsee_heap.h"
#include "qsee_timer.h"
#include "gpPersistObjCrypto.h"
#include "gpPersistObjFileIO.h"
#include "gpPersistObj.h"
//#include "qsee_clk.h"
#include "qsee_fs.h"

#define TEST_OBJ_SIZE   (12*1024) //((300 * 1024))
#define TEST_OBJ_NAME   "/persist/data/obj1"

typedef struct
{
  uint8 data[TEST_OBJ_SIZE];
  uint8 iv[GPCRYPTO_IV_SIZE];
  uint8 hmac[GPCRYPTO_HMAC_SIZE];
} dataObj;

#define UNIT_OBJ_NAME "/persist/data/objUnit"

typedef struct
{
  uint8 *data;
  uint8 iv[GPCRYPTO_IV_SIZE];
  uint8 hmac[GPCRYPTO_HMAC_SIZE];
  uint8 hash[GPCRYPTO_HASH_SIZE];
} unitDataObj;

#if 0
static char CE_CLK_NAME[]       = "CE_TEST";
static QSEE_RESOURCE CE_CLK     = CE1;
static QSEE_CLKLEV CE_FREQ_ON   = HIGH;
static QSEE_CLKLEV CE_FREQ_OFF  = INACTIVE;
#endif

int gpPersistObj_FileIO_Unit_Tests(void);
int gpPersistObj_Crypto_Unit_Tests(void);
int gpPersistObj_Performance_Tests(void);
int tz_app_gpReadWrite_test(void);

int tz_app_gpPersistObj(void)
{
  TEE_Result res = TEE_SUCCESS;
  int ret        = 0;
  uint32_t count   = 0;
  dataObj* obj   = NULL;
    
  do
  {
    //if (0 != (ret = tz_app_gpReadWrite_test()))
    //{
    //  GPLOG_ERROR(ret);
    //  break;
    //}

    /* First Run Unit Tests */
    if (0 != (ret = gpPersistObj_FileIO_Unit_Tests()))
    {
      GPLOG_ERROR(ret);
      break;
    }

    if (0 != (ret = gpPersistObj_Crypto_Unit_Tests()))
    {
      GPLOG_ERROR(ret);
      break;
    }

    if (0 != (ret = gpPersistObj_Performance_Tests()))
    {
      GPLOG_ERROR(ret);
      break;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- gpPersistObj TESTS ---------------------------------");

    /* Run Typical Application Use Case */
    if (NULL == (obj = qsee_malloc(sizeof(dataObj))))
    {
      GPLOG_ERROR(sizeof(dataObj));
      ret = -1;
      break;
    }

#if 0
    if (0 > (ret = qsee_set_bandwidth(CE_CLK_NAME, (sizeof(CE_CLK_NAME) / sizeof(CE_CLK_NAME[0])) - 1, 
                                      (uint32)CE_CLK, (uint32)CE_FREQ_ON, 0)))
    {
      GPLOG_ERROR(ret = -1);
      break;
    }
#endif

    GPLOG_MARKER_START();

    /* Perform Encrypt and Protect */
    if (TEE_SUCCESS != (res = gpCrypto_Encrypt(NULL, obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }
    
    if (TEE_SUCCESS != (res = gpCrypto_Integrity_Protect(NULL, obj->data, sizeof(obj->data)+sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }

    /* Store Data */
    if (TEE_SUCCESS != (res = gpFileIO_Write_File(NULL, TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }

    GPLOG_MARKER_STOP();
    GPLOG_MARKER_START();

    /* Read Data */
    if (TEE_SUCCESS != (res = gpFileIO_Read_File(NULL, TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }

    /* Verify and Decrypt */
    if (TEE_SUCCESS != (res = gpCrypto_Integrity_Verify(NULL, obj->data, sizeof(obj->data) + sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }

    if (TEE_SUCCESS != (res = gpCrypto_Decrypt(NULL, obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }

    GPLOG_MARKER_STOP();
    GPLOG_MARKER_START();

    /* Remove the Object */
    if (TEE_SUCCESS != (res = gpFileIO_Remove_File(NULL, TEST_OBJ_NAME)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }

    GPLOG_MARKER_STOP();

#if 0
    if (0 > (ret = qsee_set_bandwidth(CE_CLK_NAME, (sizeof(CE_CLK_NAME) / sizeof(CE_CLK_NAME[0])) - 1,
                                       (uint32)CE_CLK, (uint32)CE_FREQ_OFF, 0)))
    {
      GPLOG_ERROR(ret);
      break;
    }
#endif

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- gpPersistObj TESTS PASSED");

  } while (0);

  QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- gpPersistObj TESTS ---------------------------------");

  if (NULL != obj) qsee_free(obj);

  return ret;
}

#define ITERS 100

int gpPersistObj_Performance_Tests()
{
  TEE_Result res = TEE_SUCCESS;
  int ret        = 0;
  uint32_t count   = 0;
  dataObj* obj   = NULL;
  unsigned long long start = 0;
  unsigned long long stop  = 0;
  unsigned long long total = 0;
  int i = 0;

  QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- gpPersistObj Perf TESTS ---------------------------------");

  do
  {

    if (NULL == (obj = qsee_malloc(sizeof(dataObj))))
    {
      GPLOG_ERROR(sizeof(dataObj));
      ret = -1;
      goto Exit;
    }

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      /* Perform Encrypt */
      if (TEE_SUCCESS != (res = gpCrypto_Encrypt(NULL, obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      stop = qsee_get_uptime();
      total += stop - start;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  --> Encrypt (Without Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      /* Perform Protect */
      if (TEE_SUCCESS != (res = gpCrypto_Integrity_Protect(NULL, obj->data, sizeof(obj->data) + sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      stop = qsee_get_uptime();
      total += stop - start;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  --> Protect (Without Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      /* Perform Write */
      if (TEE_SUCCESS != (res = gpFileIO_Write_File(NULL, TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      stop = qsee_get_uptime();
      total += stop - start;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  --> Write (Without Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      /* Perform Read */
      if (TEE_SUCCESS != (res = gpFileIO_Read_File(NULL, TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
      {
        GPLOG_ERROR(res);
        ret = -1;
        break;
      }

      stop = qsee_get_uptime();
      total += stop - start;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  --> Read (Without Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      /* Perform Verify */
      if (TEE_SUCCESS != (res = gpCrypto_Integrity_Verify(NULL, obj->data, sizeof(obj->data) + sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        break;
      }

      stop = qsee_get_uptime();
      total += stop - start;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  --> Verify (Without Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      /* Perform Decrypt */
      if (TEE_SUCCESS != (res = gpCrypto_Decrypt(NULL, obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        break;
      }

      stop = qsee_get_uptime();
      total += stop - start;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  --> Decrypt (Without Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      /* Perform Encrypt and Protect */
      if (TEE_SUCCESS != (res = gpCrypto_Encrypt(NULL, obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      if (TEE_SUCCESS != (res = gpCrypto_Integrity_Protect(NULL, obj->data, sizeof(obj->data) + sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      /* Store Data */
      if (TEE_SUCCESS != (res = gpFileIO_Write_File(NULL, TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      stop = qsee_get_uptime();
      total += stop - start;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  --> Encrypt, Protect, Write (Without Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      /* Read Data */
      if (TEE_SUCCESS != (res = gpFileIO_Read_File(NULL, TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
      {
        GPLOG_ERROR(res);
        ret = -1;
        break;
      }

      /* Verify and Decrypt */
      if (TEE_SUCCESS != (res = gpCrypto_Integrity_Verify(NULL, obj->data, sizeof(obj->data) + sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        break;
      }

      if (TEE_SUCCESS != (res = gpCrypto_Decrypt(NULL, obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        break;
      }

      stop = qsee_get_uptime();
      total += stop - start;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  --> Read, Verify, Decrypt (Without Clocks): avg=%llu ms", total / ITERS);
    total = 0;

#if 0
    if (0 > (ret = qsee_set_bandwidth(CE_CLK_NAME, (sizeof(CE_CLK_NAME) / sizeof(CE_CLK_NAME[0])) - 1,
      (uint32)CE_CLK, (uint32)CE_FREQ_ON, 0)))
    {
      GPLOG_ERROR(ret = -1);
      goto Exit;
    }
#endif

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      /* Perform Encrypt */
      if (TEE_SUCCESS != (res = gpCrypto_Encrypt(NULL, obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      stop = qsee_get_uptime();
      total += stop - start;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  --> Encrypt (With Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      /* Perform Protect */
      if (TEE_SUCCESS != (res = gpCrypto_Integrity_Protect(NULL, obj->data, sizeof(obj->data) + sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      stop = qsee_get_uptime();
      total += stop - start;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  --> Protect (With Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      /* Perform Write */
      if (TEE_SUCCESS != (res = gpFileIO_Write_File(NULL, TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      stop = qsee_get_uptime();
      total += stop - start;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  --> Write (With Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      /* Perform Read */
      if (TEE_SUCCESS != (res = gpFileIO_Read_File(NULL, TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
      {
        GPLOG_ERROR(res);
        ret = -1;
        break;
      }

      stop = qsee_get_uptime();
      total += stop - start;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  --> Read (With Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      /* Perform Verify */
      if (TEE_SUCCESS != (res = gpCrypto_Integrity_Verify(NULL, obj->data, sizeof(obj->data) + sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        break;
      }

      stop = qsee_get_uptime();
      total += stop - start;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  --> Verify (With Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      /* Perform Decrypt */
      if (TEE_SUCCESS != (res = gpCrypto_Decrypt(NULL, obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        break;
      }

      stop = qsee_get_uptime();
      total += stop - start;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  --> Decrypt (With Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      /* Perform Encrypt and Protect */
      if (TEE_SUCCESS != (res = gpCrypto_Encrypt(NULL, obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      if (TEE_SUCCESS != (res = gpCrypto_Integrity_Protect(NULL, obj->data, sizeof(obj->data) + sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      /* Store Data */
      if (TEE_SUCCESS != (res = gpFileIO_Write_File(NULL, TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      stop = qsee_get_uptime();
      total += stop - start;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  --> Encrypt, Protect, Write (With Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      /* Read Data */
      if (TEE_SUCCESS != (res = gpFileIO_Read_File(NULL, TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
      {
        GPLOG_ERROR(res);
        ret = -1;
        break;
      }

      /* Verify and Decrypt */
      if (TEE_SUCCESS != (res = gpCrypto_Integrity_Verify(NULL, obj->data, sizeof(obj->data) + sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        break;
      }

      if (TEE_SUCCESS != (res = gpCrypto_Decrypt(NULL, obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        break;
      }

      stop = qsee_get_uptime();
      total += stop - start;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  --> Read, Verify, Decrypt (With Clocks): avg=%llu ms", total / ITERS);
    total = 0;

#if 0
    if (0 > (ret = qsee_set_bandwidth(CE_CLK_NAME, (sizeof(CE_CLK_NAME) / sizeof(CE_CLK_NAME[0])) - 1,
      (uint32)CE_CLK, (uint32)CE_FREQ_OFF, 0)))
    {
      GPLOG_ERROR(ret);
      goto Exit;
    }
#endif

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- gpPersistObj Perf PASSED ---------------------------------");

  }while (0);
Exit:

  if (NULL != obj) qsee_free(obj);

  return ret;
}

#define BASE (AES_BLOCK_SZ*50) //(AES_BLOCK_SZ*100)
#define LONG_NAME "ksdfkajasl djhfasdfia shenbcihai sldjhfljah sdflkjhasd \
                   lkjfhalksj dfhlkasjdh fkljashdf lkjashdfl kjashasdfa   \
                   sdfasdfasd fasdfasdfa sdfasdfasdf asdfasdfasdf asdfasd"

int gpPersistObj_FileIO_Unit_Tests(void)
{
  TEE_Result res = TEE_SUCCESS;
  int i = 0, j = 0, k = 0, ret = 0;
  uint8* data = NULL;
  uint32_t count = 0;

  for (i = 1; i < 10; i++)
  {
    int len = BASE * i;
    if (NULL == (data = qsee_malloc(len)))
    {
      GPLOG_ERROR(len);
      ret = -1;
      goto Exit;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- gpPersistObjFileIO TESTS ---------------------------------");

    /* Negative Test Cases */
    if (TEE_SUCCESS ==  gpFileIO_Write_File((gpHandle*)data, UNIT_OBJ_NAME, data, len, 0, &count))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpFileIO_Write_File(NULL, NULL, data, len, 0, &count))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpFileIO_Write_File(NULL, LONG_NAME, data, len, 0, &count))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpFileIO_Write_File(NULL, UNIT_OBJ_NAME, NULL, len, 0, &count))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpFileIO_Write_File(NULL, UNIT_OBJ_NAME, data, 0, 0, NULL))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    if (TEE_SUCCESS ==  gpFileIO_Read_File((gpHandle*)data, UNIT_OBJ_NAME, data, len, 0, &count))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpFileIO_Read_File(NULL, NULL, data, len, 0, &count))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpFileIO_Read_File(NULL, LONG_NAME, data, len, 0, &count))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpFileIO_Read_File(NULL, UNIT_OBJ_NAME, NULL, len, 0, &count))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpFileIO_Read_File(NULL, UNIT_OBJ_NAME, data, 0, 0, NULL))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    if (TEE_SUCCESS ==  gpFileIO_Remove_File((gpHandle*)data, UNIT_OBJ_NAME))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpFileIO_Remove_File(NULL, LONG_NAME))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpFileIO_Remove_File(NULL, NULL))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- Negative Tests PASSED ---------------------------------");

    /* Typical Test Cases */

    /* Different Sizes */
    for (j = 1; j <= len; j *= 7)
    {
      memset(data, 'T', len/j);

      if (TEE_SUCCESS != (res = gpFileIO_Write_File(NULL, UNIT_OBJ_NAME, data, len/j, 0, &count)))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      if (len/j != count)
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      memset(data, 0, len/j);

      if (TEE_SUCCESS != (res = gpFileIO_Read_File(NULL, UNIT_OBJ_NAME, data, len/j, 0, &count)))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      if (len/j != count)
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      for (k = 0; k < len/j; k++)
      {
        if (data[k] != 'T')
        {
          GPLOG_ERROR(res);
          ret = -1;
          goto Exit;
        }
      }

      if (TEE_SUCCESS != (res = gpFileIO_Remove_File(NULL, UNIT_OBJ_NAME)))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- Typical Tests PASSED ---------------------------------");

    /* No file to Read */
    if (TEE_SUCCESS == (res = gpFileIO_Read_File(NULL, UNIT_OBJ_NAME, data, len, 0, &count)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    /* Read more than whats in file */
    memset(data, 'G', len);

    if (TEE_SUCCESS != (res = gpFileIO_Write_File(NULL, UNIT_OBJ_NAME, data, len, 0, &count)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    if (len != count)
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    memset(data, 0, len);

    if (TEE_SUCCESS != (res = gpFileIO_Read_File(NULL, UNIT_OBJ_NAME, data, len + 10, 0, &count)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    if (len != count)
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    for (k = 0; k < len; k++)
    {
      if (data[k] != 'G')
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }
    }

    if (TEE_SUCCESS != (res = gpFileIO_Remove_File(NULL, UNIT_OBJ_NAME)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- Edge Case Tests PASSED ---------------------------------");

    qsee_free(data);
    data = NULL;

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- gpPersistObjFileIO PASSED ---------------------------------");
  }
Exit:

  if (NULL != data) qsee_free(data);

  return ret;
}

int gpPersistObj_Crypto_Unit_Tests(void)
{
  TEE_Result res = TEE_SUCCESS;
  unitDataObj obj_t = { 0 };
  unitDataObj *obj  = &obj_t;
  int i = 0, j = 0, ret = 0;

  for (i = 1; i < 10; i++)
  {
    int len = BASE * i;
    if (NULL == (obj->data = qsee_malloc(len)))
    {
      GPLOG_ERROR(len);
      ret = -1;
      goto Exit;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- gpPersistObjCrypto TESTS ---------------------------------");

    /* Negative Test Cases */
    if (TEE_SUCCESS == gpCrypto_Encrypt((gpHandle*)obj, obj->data, len, obj->iv, sizeof(obj->iv)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS == gpCrypto_Encrypt(NULL, NULL, len, obj->iv, sizeof(obj->iv)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS == gpCrypto_Encrypt(NULL, obj->data, 0, obj->iv, sizeof(obj->iv)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Encrypt(NULL, obj->data, 0, obj->iv, sizeof(obj->iv)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Encrypt(NULL, obj->data, len, NULL, sizeof(obj->iv)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Encrypt(NULL, obj->data, len, obj->iv, GPCRYPTO_IV_SIZE - 1))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Encrypt(NULL, obj->data, len, obj->iv, 0))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    if (TEE_SUCCESS ==  gpCrypto_Decrypt((gpHandle*)obj, obj->data, len, obj->iv, sizeof(obj->iv)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Decrypt(NULL, NULL, len, obj->iv, sizeof(obj->iv)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Decrypt(NULL, obj->data, 0, obj->iv, sizeof(obj->iv)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Decrypt(NULL, obj->data, 0, obj->iv, sizeof(obj->iv)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Decrypt(NULL, obj->data, len, NULL, sizeof(obj->iv)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Decrypt(NULL, obj->data, len, obj->iv, GPCRYPTO_IV_SIZE - 1))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Decrypt(NULL, obj->data, len, obj->iv, 0))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    if (TEE_SUCCESS ==  gpCrypto_Integrity_Protect((gpHandle*)obj, obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Integrity_Protect(NULL, NULL, len, NULL, 0, obj->hmac, sizeof(obj->hmac)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Integrity_Protect(NULL, obj->data, 0, NULL, 0, obj->hmac, sizeof(obj->hmac)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Integrity_Protect(NULL, obj->data, len, obj->hash, 0, obj->hmac, sizeof(obj->hmac)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Integrity_Protect(NULL, obj->data, len, NULL, 0, obj->hmac, 0))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Integrity_Protect(NULL, obj->data, len, obj->hash, 0, obj->hmac, 0))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Integrity_Protect(NULL, obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac) - 1))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Integrity_Protect(NULL, obj->data, len, obj->hash, sizeof(obj->hash) - 1, obj->hmac, sizeof(obj->hmac)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    if (TEE_SUCCESS ==  gpCrypto_Integrity_Verify((gpHandle*)obj, obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Integrity_Verify(NULL, NULL, len, NULL, 0, obj->hmac, sizeof(obj->hmac)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Integrity_Verify(NULL, obj->data, 0, NULL, 0, obj->hmac, sizeof(obj->hmac)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Integrity_Verify(NULL, obj->data, len, obj->hash, 0, obj->hmac, sizeof(obj->hmac)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Integrity_Verify(NULL, obj->data, len, NULL, 0, obj->hmac, 0))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Integrity_Verify(NULL, obj->data, len, obj->hash, 0, obj->hmac, 0))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Integrity_Verify(NULL, obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac) - 1))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }
    if (TEE_SUCCESS ==  gpCrypto_Integrity_Verify(NULL, obj->data, len, obj->hash, sizeof(obj->hash) - 1, obj->hmac, sizeof(obj->hmac)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- Negative Tests PASSED ---------------------------------");

    /* Typical Cases*/
    memset(obj->data, 'T', len);

    if (TEE_SUCCESS != (res = gpCrypto_Encrypt(NULL, obj->data, len, obj->iv, sizeof(obj->iv))))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    if (TEE_SUCCESS != (res = gpCrypto_Decrypt(NULL, obj->data, len, obj->iv, sizeof(obj->iv))))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    for (j = 0; j < len; j++)
    {
      if ((obj->data)[j] != 'T')
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }
    }

    if (TEE_SUCCESS != (res = gpCrypto_Integrity_Protect(NULL, obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac))))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    if (TEE_SUCCESS != (res = gpCrypto_Integrity_Verify(NULL, obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac))))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    if (TEE_SUCCESS != (res = gpCrypto_Integrity_Protect(NULL, obj->data, len, obj->hash, sizeof(obj->hash), NULL, 0)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    if (TEE_SUCCESS != (res = gpCrypto_Integrity_Verify(NULL, obj->data, len, obj->hash, sizeof(obj->hash), NULL, 0)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    /* Different Sizes */
    for (j = 1; j <= len; j*=7)
    {
      if (TEE_SUCCESS != (res = gpCrypto_Encrypt(NULL, obj->data, len/j, obj->iv, sizeof(obj->iv))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      if (TEE_SUCCESS != (res = gpCrypto_Decrypt(NULL, obj->data, len/j, obj->iv, sizeof(obj->iv))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      if (TEE_SUCCESS != (res = gpCrypto_Integrity_Protect(NULL, obj->data, len/j, NULL, 0, obj->hmac, sizeof(obj->hmac))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      if (TEE_SUCCESS != (res = gpCrypto_Integrity_Verify(NULL, obj->data, len/j, NULL, 0, obj->hmac, sizeof(obj->hmac))))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      if (TEE_SUCCESS != (res = gpCrypto_Integrity_Protect(NULL, obj->data, len / j, obj->hash, sizeof(obj->hash), NULL, 0)))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }

      if (TEE_SUCCESS != (res = gpCrypto_Integrity_Verify(NULL, obj->data, len / j, obj->hash, sizeof(obj->hash), NULL, 0)))
      {
        GPLOG_ERROR(res);
        ret = -1;
        goto Exit;
      }
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- Typical Tests PASSED ---------------------------------");

    /* Corruption Case */
    if (TEE_SUCCESS != (res = gpCrypto_Integrity_Protect(NULL, obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac))))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    memset(obj->data, 'Y', len / 2);

    if (TEE_SUCCESS == (res = gpCrypto_Integrity_Verify(NULL, obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac))))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    if (TEE_SUCCESS != (res = gpCrypto_Integrity_Protect(NULL, obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac))))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    memset(obj->data + len - 1, 1, 1);

    if (TEE_SUCCESS == (res = gpCrypto_Integrity_Verify(NULL, obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac))))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    if (TEE_SUCCESS != (res = gpCrypto_Integrity_Protect(NULL, obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac))))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    memset(obj->hmac, 2, 1);

    if (TEE_SUCCESS == (res = gpCrypto_Integrity_Verify(NULL, obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac))))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    if (TEE_SUCCESS != (res = gpCrypto_Integrity_Protect(NULL, obj->data, len, obj->hash, sizeof(obj->hash), NULL, 0)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    memset(obj->data, 'W', len / 2);

    if (TEE_SUCCESS == (res = gpCrypto_Integrity_Verify(NULL, obj->data, len, obj->hash, sizeof(obj->hash), NULL, 0)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    if (TEE_SUCCESS != (res = gpCrypto_Integrity_Protect(NULL, obj->data, len, obj->hash, sizeof(obj->hash), NULL, 0)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    memset(obj->data + len - 1, 3, 1);

    if (TEE_SUCCESS == (res = gpCrypto_Integrity_Verify(NULL, obj->data, len, obj->hash, sizeof(obj->hash), NULL, 0)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    if (TEE_SUCCESS != (res = gpCrypto_Integrity_Protect(NULL, obj->data, len, obj->hash, sizeof(obj->hash), NULL, 0)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    memset(obj->hash, 4, 1);

    if (TEE_SUCCESS == (res = gpCrypto_Integrity_Verify(NULL, obj->data, len, obj->hash, sizeof(obj->hash), NULL, 0)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      goto Exit;
    }

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- Corruption Tests PASSED ---------------------------------");

    qsee_free(obj->data);
    obj->data = NULL;

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- gpPersistObjCrypto PASSED ---------------------------------");
  }
Exit:

  if (NULL != obj->data) qsee_free(obj->data);

  return ret;
}

/* TestCases
// Test1:File does not exist. Should create new file. No error
// Test2:Write 300k to file 'Bytes A'
// Test3:Read 300k from file 'Bytes A'
// Test4:Read more than size of file i.e 302k Only size of file should be read. No error
// Test5:Go to offset 100k, write 100k - 'Bytes B'
// Test6:Go to offset 100k, read 100k - 'Bytes B'
// Test7:Go to last 8 bytes of file, and read 16 bytes from file - i.e more bytes than are present. No error, 8 bytes should be returned
// Test8:Append 8 bytes to the file, offset is set 'Bytes C'
// Test9:Read last 8 bytes from the file, offset is set 'Bytes C'
// Test10:Read 400k from file. Count should return 300k+8 'Bytes A,B,C'
// Test11:Go to past end of file, and read 5 bytes. No error, count should be 0
// Test13:Remove file
// Test14:Read file that does not exist
// Test15:Remove file that does not exist
// Test16a and Test16b:Read/write 300k file using FS listener, to measure time difference
// TODO:Test17:Send command that does not exist. Should fail gracefully, i.e error response returned
*/
int tz_app_gpReadWrite_test(void)
{
  TEE_Result res = TEE_SUCCESS;
  int ret = 0;
  uint32_t count = 0;
  uint32 offset = 0;
  uint8*   testbuf = NULL;
  uint32   test300klen = 307200;
  int i = 0;
  int fd = -1;

  QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- gpfslistener UNIT TESTS ---------------------------------");

  do
  {
    if (NULL == (testbuf = qsee_malloc(409600)))
    {
      GPLOG_ERROR(ret = -1);
      break;
    }
    memset(testbuf, '0', 409600);

    // Remove file (start with clean slate in case it wasn't removed in last run
    // Ignore any error, as file may not be there
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Removing file so we start on clean slate");
    res = gpFileIO_Remove_File(NULL, "/persist/data/sampleapp/gptestfile300k");

#if 0
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test1 :Write 300k of (Z) to a new file with new path");
    // Test1:File does not exist. Should create new file. No error
    memset(testbuf, 'Z', test300klen);
    offset = 0;
    count = test300klen;
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Start)", qsee_get_uptime(), __func__, __LINE__);
    if (TEE_SUCCESS != (res = gpFileIO_Write_File(NULL, "/persist/data/newpath/gptestfile300k", testbuf, count, offset, &count)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Stop)", qsee_get_uptime(), __func__, __LINE__);

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Removing file");
    res = gpFileIO_Remove_File(NULL, "/persist/data/newpath/gptestfile300k");
#endif
    // Test2:Write 300k file
    memset(testbuf, 'A', test300klen);
    offset = 0;
    count = test300klen;
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Start)", qsee_get_uptime(), __func__, __LINE__);
    if (TEE_SUCCESS != (res = gpFileIO_Write_File(NULL, "/persist/data/sampleapp/gptestfile300k", testbuf, count, offset, &count)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Stop)", qsee_get_uptime(), __func__, __LINE__);

    // Test3:Read 300k file
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test3:Read 300k from file. Verify all bytes are (A)'");
    memset(testbuf, 0, test300klen);
    offset = 0;
    count = test300klen;
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Start)", qsee_get_uptime(), __func__, __LINE__);
    if (TEE_SUCCESS != (res = gpFileIO_Read_File(NULL, "/persist/data/sampleapp/gptestfile300k", testbuf, count, offset, &count)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Stop)", qsee_get_uptime(), __func__, __LINE__);
    if (count != test300klen)
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "Incorrect count returned %d", count);
      GPLOG_ERROR(ret = -1);
      break;
    }
    //Check the bytes read were what was written
    for (i = 0; i < count; i++)
    {
      if (testbuf[i] != 'A')
        GPLOG_ERROR(res);
    }

    // Test4:Read more than size of file. Only size of file should be read. No error
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test4:Read 301k from file. Should return 300k of (A)");
    memset(testbuf, 0, test300klen);
    offset = 0;
    count = test300klen + 1024;
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Start)", qsee_get_uptime(), __func__, __LINE__);
    if (TEE_SUCCESS != (res = gpFileIO_Read_File(NULL, "/persist/data/sampleapp/gptestfile300k", testbuf, count, offset, &count)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Stop)", qsee_get_uptime(), __func__, __LINE__);
    if (count > test300klen)
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "Incorrect count returned:%d. Count should have been set to size of file", count);
      GPLOG_ERROR(ret = -1);
      break;
    }
    //Check the bytes read were what was written
    for (i = 0; i < test300klen; i++)
    {
      if (testbuf[i] != 'A')
        GPLOG_ERROR(res);
    }

    // Test5:Go to offset 100k, write 100k - 'Bytes B'
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test5:Go to offset 100k, and write 100k of (B)");
    memset(testbuf, 'B', test300klen);
    offset = 100 * 1024;
    count = 100 * 1024;  //Trying to write 100k
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Start)", qsee_get_uptime(), __func__, __LINE__);
    if (TEE_SUCCESS != (res = gpFileIO_Write_File(NULL, "/persist/data/sampleapp/gptestfile300k", testbuf, count, offset, &count)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Stop)", qsee_get_uptime(), __func__, __LINE__);

    // Test6:Go to offset 100k, read 100k - 'Bytes B'
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test6:Go to offset 100k, and write 100k of (B)");
    memset(testbuf, 0, test300klen);
    offset = 100 * 1024;
    count = 100 * 1024;  //Trying to read 100k
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Start)", qsee_get_uptime(), __func__, __LINE__);
    if (TEE_SUCCESS != (res = gpFileIO_Read_File(NULL, "/persist/data/sampleapp/gptestfile300k", testbuf, count, offset, &count)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Stop)", qsee_get_uptime(), __func__, __LINE__);
    if (count != 100 * 1024)
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "Incorrect count returned %d", count);
      GPLOG_ERROR(ret = -1);
      break;
    }
    //Check the bytes read were what was written
    for (i = 0; i < count; i++)
    {
      if (testbuf[i] != 'B')
        GPLOG_ERROR(res);
    }

    // Test7:Go to last 8 bytes of file, and read 16 bytes from file - i.e more bytes than are present. No error, 8 bytes should be returned
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test7:Go to last 8 bytes of file and try to read 16 bytes from file. Only 8 bytes of (A) should be returned");
    memset(testbuf, 0, test300klen);
    offset = test300klen - 8;
    count = 16; //Trying to read 8 more bytes from end of file
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Start)", qsee_get_uptime(), __func__, __LINE__);
    if (TEE_SUCCESS != (res = gpFileIO_Read_File(NULL, "/persist/data/sampleapp/gptestfile300k", testbuf, count, offset, &count)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Stop)", qsee_get_uptime(), __func__, __LINE__);
    if (count != 8)
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "Incorrect count returned %d", count);
      GPLOG_ERROR(ret = -1);
      break;
    }
    //Check the bytes read were what was written
    for (i = 0; i < count; i++)
    {
      if (testbuf[i] != 'A')
        GPLOG_ERROR(res);
    }

    // Test8:Append 8 bytes to the file, offset is set 'Bytes C'
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test8:Append 8 bytes of (C) to file");
    memset(testbuf, 'C', test300klen);
    offset = test300klen; //Go to end of file
    count = 8;  //Write 8 bytes
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Start)", qsee_get_uptime(), __func__, __LINE__);
    if (TEE_SUCCESS != (res = gpFileIO_Write_File(NULL, "/persist/data/sampleapp/gptestfile300k", testbuf, count, offset, &count)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Stop)", qsee_get_uptime(), __func__, __LINE__);

    // Test9:Read last 8 bytes from the file, offset is set 'Bytes C'
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test9:Read last 8 bytes of (C) from file");
    memset(testbuf, 0, test300klen);
    offset = test300klen;
    count = 8; //Trying to read 8 more bytes from end of file
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Start)", qsee_get_uptime(), __func__, __LINE__);
    if (TEE_SUCCESS != (res = gpFileIO_Read_File(NULL, "/persist/data/sampleapp/gptestfile300k", testbuf, count, offset, &count)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Stop)", qsee_get_uptime(), __func__, __LINE__);
    if (count != 8)
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "Incorrect count returned %d", count);
      GPLOG_ERROR(ret = -1);
      break;
    }
    //Check the bytes read were what was written
    for (i = 0; i < count; i++)
    {
      if (testbuf[i] != 'C')
        GPLOG_ERROR(res);
    }

    // Test10:Read 400k from file. Count should return 300k+8 'Bytes A,B,C'
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test10:Read 400k from file. Should read 300k+8 100(A)100(B)100(A)8(C)");
    memset(testbuf, 0, test300klen);
    offset = 0;
    count = 409600;
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Start)", qsee_get_uptime(), __func__, __LINE__);
    if (TEE_SUCCESS != (res = gpFileIO_Read_File(NULL, "/persist/data/sampleapp/gptestfile300k", testbuf, count, offset, &count)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Stop)", qsee_get_uptime(), __func__, __LINE__);
    if (count != test300klen + 8)
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "Incorrect count returned %d", count);
      GPLOG_ERROR(ret = -1);
      break;
    }
    //Check the bytes read were what was written
    for (i = 0; i < (100 * 1024 - 1); i++)
    {
      if (testbuf[i] != 'A')
        GPLOG_ERROR(res);
    }
    for (i = 100 * 1024; i < (200 * 1024 - 1); i++)
    {
      if (testbuf[i] != 'B')
        GPLOG_ERROR(res);
    }
    for (i = 200 * 1024; i < (300 * 1024 - 1); i++)
    {
      if (testbuf[i] != 'A')
        GPLOG_ERROR(res);
    }
    for (i = 300 * 1024; i < 8; i++)
    {
      if (testbuf[i] != 'C')
        GPLOG_ERROR(res);
    }

    // Test11:Go to past end of file, and read 5 bytes. No error, count should be 0
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test11:Go to past end of file, and read 5 bytes. No error, count should be 0");
    memset(testbuf, 0, test300klen);
    offset = test300klen + 9;
    count = 5;
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Start)", qsee_get_uptime(), __func__, __LINE__);
    if (TEE_SUCCESS != (res = gpFileIO_Read_File(NULL, "/persist/data/sampleapp/gptestfile300k", testbuf, count, offset, &count)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Stop)", qsee_get_uptime(), __func__, __LINE__);
    if (count != 0)
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "Incorrect count returned %d", count);
      GPLOG_ERROR(ret = -1);
      break;
    }

    // Test13:Remove file
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test13:Remove file");
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Start)", qsee_get_uptime(), __func__, __LINE__);
    if (TEE_SUCCESS != (res = gpFileIO_Remove_File(NULL, "/persist/data/sampleapp/gptestfile300k")))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Stop)", qsee_get_uptime(), __func__, __LINE__);

    // Test14:Read file that does not exist
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test14:Read file that does not exist");
    memset(testbuf, 0, test300klen);
    count = test300klen;
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Start)", qsee_get_uptime(), __func__, __LINE__);
    if (TEE_SUCCESS == (res = gpFileIO_Read_File(NULL, "/persist/data/sampleapp/gptestfile300k", testbuf, count, offset, &count)))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Stop)", qsee_get_uptime(), __func__, __LINE__);

    // Test15:Remove file that does not exist
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test15:Remove file that does not exist");
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Start)", qsee_get_uptime(), __func__, __LINE__);
    if (TEE_SUCCESS == (res = gpFileIO_Remove_File(NULL, "/persist/data/sampleapp/gptestfile300k")))
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Stop)", qsee_get_uptime(), __func__, __LINE__);

    // Test16:Open, write, read, close 300k file using FS Listener
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test16a:Open file using FS Listener with flags O_RDWR | O_CREAT | O_SYNC");
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Start)", qsee_get_uptime(), __func__, __LINE__);
    fd = open("/persist/data/sampleapp/fstestfile300k", O_RDWR | O_CREAT | O_SYNC);
    if (fd < 0)
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Stop)", qsee_get_uptime(), __func__, __LINE__);
    count = test300klen;
    memset(testbuf, 'D', count);
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test16b:Write 300k to file using FS Listener");
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Start)", qsee_get_uptime(), __func__, __LINE__);
    if (write(fd, (void *) testbuf, count) < 0)
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Stop)", qsee_get_uptime(), __func__, __LINE__);

    memset(testbuf, '0', count);
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test16C:Read 300k from file using FS Listener");
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Start)", qsee_get_uptime(), __func__, __LINE__);
    if (lseek(fd, 0, SEEK_SET) < 0)
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }

    if (read(fd, (void *) testbuf, count) < 0)
    {
      GPLOG_ERROR(res);
      ret = -1;
      break;
    }
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Stop)", qsee_get_uptime(), __func__, __LINE__);
    for (i = 0; i < test300klen; i++)
    {
      if (testbuf[i] != 'D')
        GPLOG_ERROR(res);
      
    }
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test16d:Close file using FS Listener");
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Start)", qsee_get_uptime(), __func__, __LINE__);
    close(fd);
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Stop)", qsee_get_uptime(), __func__, __LINE__);
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test16e:Removing file using FS Listener");
    remove("/persist/data/sampleapp/fstestfile300k");

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "tz_app_gpReadWrite_test() PASS");
  } while (0);

  QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- gpPersistObj UNIT TESTS ---------------------------------");

  if (NULL != testbuf) qsee_free(testbuf);

  return ret;
}
