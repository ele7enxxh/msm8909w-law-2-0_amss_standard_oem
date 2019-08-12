/******************************************************************************
 * mem_share_client.h
 *
 * memory sharing interface functions
 * 
 *
 * Copyright (c) 2013
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/

#ifndef __MEM_SHARE_CLIENT_H__
#define __MEM_SHARE_CLIENT_H__

/* To avoid errors in new users due to type issues */
#include <stdint.h>
#include "dynamic_heap.h"
/**
  @file mem_share_client.h
  @brief Memshare interface functions used to share the memory with HLOS. This module keeps an internal
  pool data structure for the registered usecase.
*/


#ifdef __cplusplus
extern "C" {
#endif


/** Status of the memshare operations */
typedef enum _mem_share_status_t
{
   MEM_SHARE_SUCCESS      =  0,
   MEM_SHARE_FAILURE      = -1,
   MEM_SHARE_IPC_FAILURE  = -2,
   MEM_SHARE_INVALID_POOL = -3,
   MEM_SHARE_INVALID_CLIENT_ID =-4,
   MEM_SHARE_INVALID_ATTRIBUTE =-5,
   MEM_SHARE_INITIALIZING = -6,
   MEM_SHARE_NOT_AVAILABLE = -7,
}mem_share_status_t;

typedef struct mem_share_client_info_struct
{
	uint32 sequence_id;
	uint8  is_init;
}mem_share_client_info_type;

typedef struct mem_share_alloc_rsp_struct
{
	uint32_t phy_addr;
	uint32  phy_addr_length;
}mem_share_alloc_rsp_type;
/**
  This function establish a client connection with HLOS service layer, This function should be called 
  by the subsystem before any of the client functions getting called.
  
  @param[in] usr  user name
  @return
  mem_share_status_t, status of the client intialization

  @dependencies
  None.
*/
mem_share_status_t mem_share_client_init(void);



/**
  This function is to allocate the memory from the HLOS, once the blocks are added to the pool user should request for
  blocks by mem_share_block_alloc() function

  @param[in] pusr_handle      block pool handle
  @param[in] num_blocks       number of blocks to be allocated from HLOS
  @param[in] block_size       block size of allocated blocks, each block will be physically continous  
  @param[in] block_alignment  Each block alignment
  @param[in] num_blocks_ret   number of blocks returned from the HLOS
  @return
  mem_share_status_t, status of the allocation from the HLOS

  @dependencies
  None.
*/

mem_share_status_t mem_share_request_pool(
											 dynamic_mem_client_id client_id, 
											 dynamic_mem_attribute attr,
                                             uint32_t num_bytes, 
                                             uint32_t block_alignment,
                                             mem_share_alloc_rsp_type ** mem_share_alloc_rsp);


/**
  This is to trim the size of the pool, once the user released the usage of the block
  this pool should be trimed to return the memory to HLOS

  @param[in] pusr_handle      block pool handle
  @param[in] num_blocks       number of blocks to be freed to HLOS
  @param[in] block_size       block size of blocks to be freed
  @param[in] block_alignment  Each block alignment
  @param[in] num_trim_blocks  number of blocks freed.
  @return
  mem_share_status_t, status of the allocation from the HLOS

  @dependencies
  None.
*/
mem_share_status_t mem_share_release_pool(dynamic_mem_client_id client_id,uint32 handle,
                                                 uint32_t num_bytes);


/**
  This is to get the qmi service status for mem_share service

  @param[in] None. 
  @return 
  mem_share_status_t, qmi mem_share status

  @dependencies
  None. 
*/


mem_share_status_t mem_share_get_qmi_poll_status(void);


/**
  This is to query the size based on client id

  @param[in] client_id        client id
  @param[inout]size
  @return
  mem_share_status_t, status of the querysize from the HLOS

  @dependencies
  None.
*/
mem_share_status_t mem_share_query_size(dynamic_mem_client_id client_id,
                                                 uint32_t *size);


#ifdef __cplusplus
}
#endif

#endif /* __MEM_SHARE_CLIENT_H__ */
