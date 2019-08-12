#ifndef SNS_BATCHBUFF_H
#define SNS_BATCHBUFF_H

/*============================================================================
  @file sns_batchbuff.h

  @brief
  This file contains definition for batch buffer, which allocated same-size
  memory blocks for clients to avoid memory fragmentation.

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/common/util/batchbuffer/inc/sns_batchbuff.h#1 $
  ============================================================================*/

/*=====================================================================
                       INCLUDES
=======================================================================*/
#include "sns_common.h"

/*maximum number of blocks can be allocated in uimage mode*/
#define SNS_BATCHBUFF_BLK_MAX_UIMG 26
/*maximum number of blocks can be allocated in big image mode*/
#define SNS_BATCHBUFF_BLK_MAX_BIMG 2048
/*maximum size for each memory block allocated*/
#define SNS_BATCHBUFF_MAX_BLK_SIZE 512
/*maximum 2 callback for uimg and 2 for big image*/
#define SNS_BATCHBUFF_MAX_CB_CNT 2
/*trigger callback when specified number of mem block left*/
#define SNS_BATCHBUFF_UIMG_CB_THRESHOLD1 (SNS_BATCHBUFF_BLK_MAX_UIMG - 7)
/*trigger callback when specified number of mem block left*/
#define SNS_BATCHBUFF_UIMG_CB_THRESHOLD2 1
/*trigger callback when specified number of mem block left*/
#define SNS_BATCHBUFF_BIMG_CB_THRESHOLD1 250
/*trigger callback when specified number of mem block left*/
#define SNS_BATCHBUFF_BIMG_CB_THRESHOLD2 200

/*=====================================================================
                    INTERNAL DEFINITIONS AND TYPES
=======================================================================*/

/* Client types batchbuff suports */
typedef enum{
  SNS_BATCHBUFF_CLIENT_SMGR,
  SNS_BATCHBUFF_CLIENT_SAM,
  SNS_BATCHBUFF_CLIENT_CNT,  /* Number of clients supported */
  SNS_BATCHBUFF_CLIENT_MAX = UINT32_MAX /* Force use of 32-bit unsigned enum */
}sns_batchbuff_client_e;

typedef enum{
  SNS_BATCHBUFF_UIMG_MEM,
  SNS_BATCHBUFF_BIMG_MEM,
  SNS_BATCHBUFF_MEM_CNT
}sns_batchbuff_mem_type_e;

/*=====================================================================
                          FUNCTIONS
=======================================================================*/

/*===========================================================================

  FUNCTION:   sns_batchbuff_threshold_cb

  ===========================================================================*/
/*!
  @brief callback function when a specified threshold is met

* @param[i] threshold: The threshold (number of available blocks) when this callback function is called
*/
/*=========================================================================*/
typedef void (*sns_batchbuff_threshold_cb)(sns_batchbuff_mem_type_e mem_type, uint32_t threshold);


/*===========================================================================

  FUNCTION:   sns_batchbuff_init

  ===========================================================================*/
/*!
  @brief Initializes batchbuff memory

  This function will initialize the queues used, it would be called when system bootup not in any client

  @return
  SNS_SUCCESS if the batch buffer module is initialized successfully
*/
/*=========================================================================*/
sns_err_code_e sns_batchbuff_init(void);

/*===========================================================================

  FUNCTION:   sns_batchbuff_cb_register

  ===========================================================================*/
/*!
  @brief register the callback functions
  @param[i] client: The client that the cb function is tied to
  @param[i] mem_type: The memory block type that the cb function is tied to
  @param[i] cb_function: The cb function
  @param[i] free_blk_cnt: The threshold value that the cb function would be called
  @return
  true if registered successfully
*/
/*=========================================================================*/
bool sns_batchbuff_cb_register(sns_batchbuff_client_e client,
  sns_batchbuff_mem_type_e mem_type,
  sns_batchbuff_threshold_cb cb_func,
  uint32_t free_blk_cnt);

/*===========================================================================

  FUNCTION:   sns_batchbuff_malloc

  ===========================================================================*/
/*!
  @brief allocate memory for samples, this function must be called in bimg if mem_type is BIMG
  @details if callback threshold is met, this function will call the callback function before it returns

  @param[i] client: The client that the block will be assigned for
  @param[i] mem_type: The memory block type that would be allocated
  @return
  address of allocated memory
*/
/*=========================================================================*/
void* sns_batchbuff_malloc(sns_batchbuff_client_e client, sns_batchbuff_mem_type_e mem_type);

/*===========================================================================

  FUNCTION:   sns_batchbuff_free

  ===========================================================================*/
/*!
  @brief free memory for samples, must be called in bimg if mem_type is BIMG

  @param[i] client: The client that the block belongs to
  @param[i] ptr: pointer to memory to be freed
  @return   true if memory is freed successfully
*/
/*=========================================================================*/
void sns_batchbuff_free(sns_batchbuff_client_e client, void* ptr);

/*===========================================================================

  FUNCTION:   sns_batchbuff_blocksize

  ===========================================================================*/
/*!
  @brief get memory block size for each batchbuff_malloc.

  @param[i]
  @return
  the size of memory block available
*/
/*=========================================================================*/
uint16_t sns_batchbuff_blocksize(void);

/*===========================================================================

  FUNCTION:   sns_batchbuff_available_block_cnt

  ===========================================================================*/
/*!
  @brief Get number of available mem blocks in specified mem type

  @param[i] mem_type: mem type the mem blocks belongs to
  @return     number of mem blocks available
*/
/*=========================================================================*/
uint16_t sns_batchbuff_available_block_cnt(sns_batchbuff_mem_type_e mem_type);

#endif /* SNS_BATCHBUFF_H */
