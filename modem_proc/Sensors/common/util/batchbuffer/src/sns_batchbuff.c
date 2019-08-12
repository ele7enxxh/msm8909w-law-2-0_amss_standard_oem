/*=============================================================================
  @file sns_batchbuff.c

  @brief
  Contains implementation batching buffer utilities for SAM and SMGR.

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include "sns_batchbuff.h"
#include "sns_common.h"
#include "sns_uimg_utils.h"
#include "sns_init.h"
#include "sns_debug_str.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/

#ifdef SNS_USES_ISLAND
#define SNS_BATCHBUFF_UIMAGE_CODE __attribute__((section (".text.BATCHBUFF")))
#define SNS_BATCHBUFF_UIMAGE_DATA __attribute__((section (".data.BATCHBUFF")))
#define SNS_BATCHBUFF_UIMAGE_BSS __attribute__((section (".bss.BATCHBUFF")))
#else
#define SNS_BATCHBUFF_UIMAGE_CODE
#define SNS_BATCHBUFF_UIMAGE_DATA
#define SNS_BATCHBUFF_UIMAGE_BSS
#endif /* USES_ISLAND */

#ifdef SNS_BATCHBUFF_DEBUG
STATIC SNS_BATCHBUFF_UIMAGE_CODE
void sns_batchbuff_print( void )
{
  uint16_t i = 0;
  sns_batchbuff_blk_s *blk_ptr = NULL;
  // PEND: Rewrite/design this code

  sns_os_mutex_pend( sns_batchbuff_mutex, 0, NULL );
  blk_ptr = sns_q_check( &sns_batchbuff_uimg_used_q );
  for( i = 0; i < sns_q_cnt( &sns_batchbuff_uimg_used_q ); i++ )
  {
    SNS_PRINTF_STRING_LOW_2( SNS_DBG_MOD_DSPS_BATCHBUFF,
        "uimg used addr %u %x", i, blk_ptr->ptr );
    blk_ptr = sns_q_next( &sns_batchbuff_uimg_used_q, &blk_ptr->q_link );
  }

  blk_ptr = sns_q_check( &sns_batchbuff_uimg_free_q );
  for( i = 0; i < sns_q_cnt( &sns_batchbuff_uimg_free_q ); i++ )
  {
    SNS_PRINTF_STRING_LOW_2( SNS_DBG_MOD_DSPS_BATCHBUFF,
        "uimg free addr %u %x", i, blk_ptr->ptr );
    blk_ptr = sns_q_next( &sns_batchbuff_uimg_free_q, &blk_ptr->q_link );
  }
  sns_os_mutex_post( sns_batchbuff_mutex );
}
#else
STATIC SNS_BATCHBUFF_UIMAGE_CODE void sns_batchbuff_print( void ){}
#endif //SNS_BATCHBUFF_DEBUG

/*============================================================================
  Data Types
  ===========================================================================*/

typedef struct sns_batchbuff_blk
{
  /* Data block, as given to the client */
  uint8_t data[ SNS_BATCHBUFF_MAX_BLK_SIZE ];
  /* Client to whom this block has been allocated */
  sns_batchbuff_client_e in_use_by;
  /* The next item in the free_q; allows O(1) malloc/free */
  struct sns_batchbuff_blk *next;
} sns_batchbuff_blk;

typedef struct
{
  sns_batchbuff_threshold_cb threshold_cb;
  uint32_t free_blk_cnt;
} sns_batchbuff_threshold;

/* Heap-specific information.  the following variables are not strictly
 * required, but they improve the efficiency of common operations. */
typedef struct
{
  /* Pointer to the associated heap, allocated below */
  sns_batchbuff_blk *heap;
  /* Client-registered low memory callback functions and thresholds */
  sns_batchbuff_threshold client_threshold[ SNS_BATCHBUFF_CLIENT_CNT ][ SNS_BATCHBUFF_MAX_CB_CNT ];
  /* Remaining unallocated blocks from this heap */
  sns_batchbuff_blk *free_q;
  /* Count of unallocated blocks */
  uint32_t free_blk_cnt;
  /* Total block count of this heap */
  uint32_t max_blk_cnt;
} sns_batchbuff_heap_info;

/*============================================================================
  Static Data
  ===========================================================================*/

/* Array of allocated blocks */
SNS_BATCHBUFF_UIMAGE_DATA STATIC sns_batchbuff_blk sns_batchbuff_uimg[ SNS_BATCHBUFF_BLK_MAX_UIMG ];
STATIC sns_batchbuff_blk sns_batchbuff_bimg[ SNS_BATCHBUFF_BLK_MAX_BIMG ];

/* Heap-specific information */
SNS_BATCHBUFF_UIMAGE_DATA STATIC sns_batchbuff_heap_info sns_batchbuff_info[ SNS_BATCHBUFF_MEM_CNT ];

/* Mutex protecting all batchbuff queues */
SNS_BATCHBUFF_UIMAGE_DATA STATIC OS_EVENT *sns_batchbuff_mutex = NULL;

/*============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Determine the memory type of the given pointer.
 *
 * @param[i] ptr Given pointer to be checked
 *
 * @return Memory type of the pointer; SNS_BATCHBUFF_MEM_CNT if invalid
 */
STATIC SNS_BATCHBUFF_UIMAGE_CODE sns_batchbuff_mem_type_e
sns_batchbuff_get_mem_type( void *ptr )
{
  if( (uintptr_t)ptr >= (uintptr_t)sns_batchbuff_uimg &&
      (uintptr_t)ptr < (uintptr_t)sns_batchbuff_uimg + sizeof(sns_batchbuff_uimg) )
  {
    return SNS_BATCHBUFF_UIMG_MEM;
  }
  else if( (uintptr_t)ptr >= (uintptr_t)sns_batchbuff_bimg &&
           (uintptr_t)ptr < (uintptr_t)sns_batchbuff_bimg + sizeof(sns_batchbuff_bimg) )
  {
    return SNS_BATCHBUFF_BIMG_MEM;
  }

  return SNS_BATCHBUFF_MEM_CNT;
}

/**
 * Check whether the new free count prompts us to call any registered low
 * memory callbacks.
 *
 * @param[i] mem_type Memory type of the requested allocation
 * @param[i] free_cnt Current free count after making the allocation
 */
STATIC SNS_BATCHBUFF_UIMAGE_CODE void
sns_batchbuff_check_callbacks( sns_batchbuff_mem_type_e mem_type, uint32_t free_cnt )
{
  sns_batchbuff_threshold (*client_thresh)[ SNS_BATCHBUFF_MAX_CB_CNT ]=
    sns_batchbuff_info[ mem_type ].client_threshold;
  int i, j;

  for( i = 0; i < SNS_BATCHBUFF_CLIENT_CNT; i++ )
  {
    for( j = 0; j < SNS_BATCHBUFF_MAX_CB_CNT; j++ )
    {
      if( NULL != client_thresh[ i ][ j ].threshold_cb &&
          client_thresh[ i ][ j ].free_blk_cnt == free_cnt )
      {
        bool is_island = SNS_OS_IS_ISLAND_FUNC( client_thresh[i][j].threshold_cb );

        if( is_island )
        {
          client_thresh[ i ][ j ].threshold_cb( mem_type, client_thresh[ i ][ j ].free_blk_cnt );
        }
        else
        {
          sns_utils_place_uimge_hold( SNS_UIMG_BB );
          client_thresh[ i ][ j ].threshold_cb( mem_type, client_thresh[ i ][ j ].free_blk_cnt );
          sns_utils_remove_uimage_hold( SNS_UIMG_BB );
        }
      }
    }
  }
}

/*============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_err_code_e
sns_batchbuff_init( void )
{
  uint8_t err;
  int i, j, k;

  SNS_PRINTF_STRING_LOW_0( SNS_DBG_MOD_DSPS_BATCHBUFF, "sns_batchbuff_init" );

  sns_batchbuff_mutex =
    sns_os_mutex_create_uimg( SNS_BATCHBUFF_QUE_MUTEX, &err );
  SNS_ASSERT( OS_ERR_NONE == err );

  for( k = 0; k < ARR_SIZE(sns_batchbuff_info); k++ )
  {
    for( i = 0; i < ARR_SIZE(sns_batchbuff_info[ k ].client_threshold); i++ )
    {
      for( j = 0; j < ARR_SIZE(sns_batchbuff_info[ k ].client_threshold[ i ]); j++ )
      {
        sns_batchbuff_info[ k ].client_threshold[ i ][ j ].threshold_cb = NULL;
        sns_batchbuff_info[ k ].client_threshold[ i ][ j ].free_blk_cnt = 0;
      }
    }
  }

  sns_batchbuff_info[ SNS_BATCHBUFF_UIMG_MEM ].heap = sns_batchbuff_uimg;
  sns_batchbuff_info[ SNS_BATCHBUFF_BIMG_MEM ].heap = sns_batchbuff_bimg;
  SNS_OS_MEMZERO( sns_batchbuff_uimg, sizeof(sns_batchbuff_uimg) );
  SNS_OS_MEMZERO( sns_batchbuff_bimg, sizeof(sns_batchbuff_bimg) );
  sns_batchbuff_info[ SNS_BATCHBUFF_UIMG_MEM ].max_blk_cnt = ARR_SIZE(sns_batchbuff_uimg);
  sns_batchbuff_info[ SNS_BATCHBUFF_BIMG_MEM ].max_blk_cnt = ARR_SIZE(sns_batchbuff_bimg);

  for( i = 0; i < ARR_SIZE(sns_batchbuff_info); i++ )
  {
    sns_batchbuff_info[ i ].free_blk_cnt = sns_batchbuff_info[ i ].max_blk_cnt;
    sns_batchbuff_info[ i ].free_q = NULL;

    for( j = 0; j < sns_batchbuff_info[ i ].max_blk_cnt; j++ )
    {
      sns_batchbuff_info[ i ].heap[ j ].in_use_by = SNS_BATCHBUFF_CLIENT_CNT;
      sns_batchbuff_info[ i ].heap[ j ].next = sns_batchbuff_info[ i ].free_q;
      sns_batchbuff_info[ i ].free_q = &sns_batchbuff_info[ i ].heap[ j ];
    }
  }

  sns_init_done();

  return SNS_SUCCESS;
}

bool
sns_batchbuff_cb_register( sns_batchbuff_client_e client,
  sns_batchbuff_mem_type_e mem_type,
  sns_batchbuff_threshold_cb cb_func,
  uint32_t free_blk_cnt )
{
  int i;
  sns_batchbuff_threshold *client_thresh =
    sns_batchbuff_info[ mem_type ].client_threshold[ client ];

  for( i = 0; i < SNS_BATCHBUFF_MAX_CB_CNT; i++ )
  {
    if( NULL == client_thresh[ i ].threshold_cb )
    {
      client_thresh[ i ].threshold_cb = cb_func;
      client_thresh[ i ].free_blk_cnt = free_blk_cnt;
      return true;
    }
  }
  return false;
}

SNS_BATCHBUFF_UIMAGE_CODE void*
sns_batchbuff_malloc( sns_batchbuff_client_e client,
    sns_batchbuff_mem_type_e mem_type )
{
  sns_batchbuff_blk *block = NULL;
  uint32_t free_cnt = 0;
  uint8_t err;

  sns_os_mutex_pend( sns_batchbuff_mutex, 0, &err );
  if( 0 != sns_batchbuff_info[ mem_type ].free_blk_cnt )
  {
    block = sns_batchbuff_info[ mem_type ].free_q;
    sns_batchbuff_info[ mem_type ].free_q = block->next;

    free_cnt = --sns_batchbuff_info[ mem_type ].free_blk_cnt;
    block->in_use_by = client;
  }
  sns_os_mutex_post( sns_batchbuff_mutex );

  if( NULL != block )
  {
    SNS_PRINTF_STRING_LOW_3( SNS_DBG_MOD_DSPS_BATCHBUFF,
      "batchbuff malloc type: %u, %u available, %x",
      mem_type, free_cnt, block );

    SNS_OS_MEMZERO( block->data, sizeof(block->data) );
    sns_batchbuff_check_callbacks( mem_type, free_cnt );
    return (void*)block->data;
  }
  else
  {
    SNS_PRINTF_STRING_LOW_1( SNS_DBG_MOD_DSPS_BATCHBUFF,
        "batchbuff malloc mem depleted mode: %u", mem_type );
    return NULL;
  }
}

SNS_BATCHBUFF_UIMAGE_CODE void
sns_batchbuff_free( sns_batchbuff_client_e client, void *ptr )
{
  UNREFERENCED_PARAMETER(client);
  sns_batchbuff_blk *block = (sns_batchbuff_blk*)ptr;
  uint8_t err;
  bool aligned = false;
  sns_batchbuff_mem_type_e mem_type = sns_batchbuff_get_mem_type( ptr );

  sns_os_mutex_pend( sns_batchbuff_mutex, 0, &err );
  if( SNS_BATCHBUFF_MEM_CNT == mem_type )
  {
    SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_DSPS_BATCHBUFF,
      "Attempt to free an invalid address %x", ptr );
    SNS_ASSERT( false );
  }
  else
  {
    sns_batchbuff_info[ mem_type ].free_blk_cnt++;
    aligned = 0 == ((uintptr_t)block - (uintptr_t)sns_batchbuff_info[ mem_type ].heap ) % sizeof(sns_batchbuff_blk);
  }

  if( !aligned )
  {
    SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_DSPS_BATCHBUFF,
      "Attempt to free an unaligned block %x", ptr );
    SNS_ASSERT( false );
  }

  block->in_use_by = SNS_BATCHBUFF_CLIENT_CNT;
  block->next = sns_batchbuff_info[ mem_type ].free_q;
  sns_batchbuff_info[ mem_type ].free_q = block;
  sns_batchbuff_print();
  sns_os_mutex_post( sns_batchbuff_mutex );
}

SNS_BATCHBUFF_UIMAGE_CODE
uint16_t sns_batchbuff_blocksize( void )
{
  return SNS_BATCHBUFF_MAX_BLK_SIZE;
}

SNS_BATCHBUFF_UIMAGE_CODE
uint16_t sns_batchbuff_available_block_cnt( sns_batchbuff_mem_type_e mem_type )
{
  uint32_t cnt = 0;
  uint8_t err;

  sns_os_mutex_pend( sns_batchbuff_mutex, 0, &err );
  cnt = sns_batchbuff_info[ mem_type ].free_blk_cnt;
  sns_os_mutex_post( sns_batchbuff_mutex );

  return cnt;
}
