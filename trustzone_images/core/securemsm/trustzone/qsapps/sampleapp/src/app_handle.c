/*
@file app_handle.c
@brief Contains test code for most of the QSEE APIs.

*/
/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/sampleapp/src/app_handle.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------

===========================================================================*/
#include "qsee_log.h"
#include "qsee_heap.h"
#include "qsee_comstr.h"
#include "qsee_core.h"
#include "qsee_hash.h"
#include "qsee_cipher.h"

/* Setting this at 20 because any higher causes a crash 
 * in tzbsp crypto code, they don't handle the failure correctly. 
 * This is due to tzbsp_malloc() returning NULL when mallocing
 * the context correctly. Filing a CR for that now. 10/29/13. */
 /* Reduced to 10 on 3/17/2014 for 8084*/
#define MAX_HANDLES 10 

int free_contexts(int i);
int run_hash_handle_test(void);
int run_cipher_handle_test(void);
uint32 my_handles[MAX_HANDLES+1];

extern int tz_app_hash_func(void);
typedef struct aes_vector_type
{
  QSEE_CIPHER_ALGO_ET  alg;
  QSEE_CIPHER_MODE_ET  mode;
  uint8*               pt;
  uint32               pt_len;
  uint8*               key;
  uint32               key_len;
  uint8*               iv;
  uint8*               ct;
} __attribute__ ((packed)) aes_vector_type_t;
extern int tz_app_crypto_aes_func(QSEE_CIPHER_ALGO_ET alg,
                           QSEE_CIPHER_MODE_ET mode,
                           uint8 *pt,
                           uint32 pt_len,
                           uint8 *key,
                           uint32 key_len,
                           uint8 *iv,
                           uint8 *ct,
                 uint8 verify);
extern aes_vector_type_t aes_test_vectors[];


int run_handle_test()
{
  int ret = 0;
  ret = run_hash_handle_test();
  if(ret)
    return ret;

  ret = run_cipher_handle_test();
  if(ret)
    return ret;

  QSEE_LOG(QSEE_LOG_MSG_ERROR, "  Qsee_handle test PASS: ALL handle tests PASSED");  
  return 0;
}

int run_cipher_handle_test()
{
  int ret = 0;

  /* Standard use case - 
   * Does qsee_cipher_init, qsee_cipher_set_param, qsee_cipher_encrypt, qsee_cipher_get_param, qsee_cipher_free_ctx */
  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "  Qsee_handle test, running standard crypto cipher test.");
  ret = tz_app_crypto_aes_func(aes_test_vectors[0].alg,
                              aes_test_vectors[0].mode,
                              aes_test_vectors[0].pt,
                              aes_test_vectors[0].pt_len,
                              aes_test_vectors[0].key,
                              aes_test_vectors[0].key_len,
                              aes_test_vectors[0].iv,
                              aes_test_vectors[0].ct, 1);
  if(ret)
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "    Qsee_handle test FAILURE: tz_app_crypto_aes_func() failed, with error (%x)", ret);
    return ret;
  }

  return 0;
}

int run_hash_handle_test()
{
  int ret = 0;
  int i=0;
  char * message_buffer = "This is a message. It is more than 20 chars long.";
  int message_len = 22;
  uint8 result_digest[QSEE_SHA1_HASH_SZ];
  uint32 digest_len = QSEE_SHA1_HASH_SZ;

  /* Start with standard use case - 
   * crypto test code which uses qsee_hash_init, qsee_hash_update_, etc 
   * expecting sucess*/
  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "  Qsee_handle test, running standard crypto hash test.");
  ret = tz_app_hash_func();
  if(ret)
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "    Qsee_handle test FAILURE: tz_app_hash_func() failed, with error (%x)", ret);
    return ret;
  }

  /* Failure cases - Let's pass in a bad context pointers, 
   * expecting to see error codes from the APIs, no crashes or aborts*/
  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "  Qsee_handle test, running failure scenarios.");
  ret = qsee_hash_update( 
    (qsee_hash_ctx*)0xABC,
    (uint8*)message_buffer,
    message_len);
  if(ret == 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "    FAILURE: qsee_hash_update gave sucess when using a bad ctx");
    return -1;
  }
  else
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "    pass: qsee_hash_update gave error using a bad ctx");
  }

  /*Failure case - Pass null context*/
  ret = qsee_hash_update( 
    (qsee_hash_ctx*)0x0,
    (uint8*)message_buffer,
    message_len);
  if(ret == 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "    FAILURE: qsee_hash_update gave sucess when using a null ctx");
    return -1;
  }
  else
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "    pass: qsee_hash_update gave error using a null ctx");
  }

  /*Stress test - Let's allocate a bunch of contexts*/
  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "  Qsee_handle test, running stess test.");
  for(i=0; i<MAX_HANDLES; i++)
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, " Stress iteration: %d", i);
    ret = qsee_hash_init(QSEE_HASH_SHA1, (qsee_hash_ctx**) &my_handles[i] );
    if(ret)
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "    Qsee_handle test FAILURE: qsee_hash_init error (%x) allocing ctx %d." , ret, i);
      free_contexts(i);
      return -1;
    }

  }

  /*Try one update operation. Expecting sucess.*/
  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "  Qsee_handle test, continuing stess test.");
  ret = qsee_hash_update( 
    (qsee_hash_ctx*)my_handles[0],
    (uint8*)message_buffer,
    message_len);
  if(ret)
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  Qsee_handle test FAILURE:  qsee_hash_update gave error during stress test: (%x)", ret);
    return -1;
  }
  
  /*Try one hash_final operation. Expecting sucess.*/
  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "  Qsee_handle test, continuing stess test.");
  ret = qsee_hash_final( 
    (qsee_hash_ctx*)my_handles[0],
    (uint8*)result_digest,
    digest_len);
  if(ret)
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  Qsee_handle test FAILURE:  qsee_hash_final gave error during stress test: (%x)", ret);
    return -1;
  }

  /*Allocate one more context. Expecting failure - all contexts have been allocated.*/
/*
  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "  Qsee_handle test, running failure scenario - allocating too many ctx's.");
  ret = qsee_hash_init(QSEE_HASH_SHA1, (qsee_hash_ctx**)&my_handles[MAX_HANDLES]);
  if(ret==0)
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  Qsee_handle test FAILURE:  qsee_hash_init gave sucess when allocating too many ctx's.  Failure was expected here.");
  }
  if(ret==0)
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  Qsee_handle test FAILURE:  qsee_hash_init gave sucess when allocating too many ctx's.  Failure was expected here.");
  }
*/

  /*Free contexts from stress test*/
  if(free_contexts(i) != 0)
  {
    return -1;
  }

  /*Failure case - Double free a context*/
  ret = qsee_hash_free_ctx( (qsee_hash_ctx*)my_handles[0] );
  if(ret == 0)
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "    FAILURE: qsee_hash_free_ctx gave sucess when double freeing a ctx");
    return -1;
  }
  else
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "    pass: qsee_hash_free_ctx gave error double freeing a ctx");
  }

  QSEE_LOG(QSEE_LOG_MSG_ERROR, "  Qsee_handle test - all hash tests passed.");
  return 0;
}

int free_contexts(int i)
{
  int a = 0;
  /*Free all the contexts*/
  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "  Qsee_handle test, freeing hash contexts.");
  for(a=0; a<i; a++)
  {
    if(qsee_hash_free_ctx((qsee_hash_ctx*)my_handles[a]))
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "    Qsee_handle test FAILURE:  qsee_hash_free_ctx gave freeing ctx # %d.", a);
      return -1;
    }
  }
  return 0;
}

