#ifndef FLASHI_HAL_COMMON_H
#define FLASHI_HAL_COMMON_H
/*=======================================================================
 * FILE:        flashi_hal_common.h
 *
 * SERVICES:    Header for Common APIs used across all controllers
 *
 * DESCRIPTION: Header for Common APIs used across all controllers
 *
 * Copyright (c) 2008-2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 04/30/11     bb      Revised the allocation for optimization
 * 03/30/08     bb      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flashi_hal.h"

/*-----------------------------------------------------------------------
 *           Definitions and Constants
 *----------------------------------------------------------------------*/

/* Skip to the last command */
#define SKIP_TO_LAST_CMD(cmd) \
  while ((cmd->op_flag & FLASH_HAL_LAST_CMD) != FLASH_HAL_LAST_CMD) \
  { ++cmd; }

#define FLASH_HAL_DEVICE_FREE    0     /* Device free status */
#define FLASH_HAL_DEVICE_IN_USE  1     /* Device in use status */

/* The below % allocations are made by trial and error method. Tested with 
   multiple command sequences, and optimized for memory usage. */

/* Post process buffer. The 532 (worst case size - 8bit ECC enabled) is to 
   offset the memory allocated for codeword buffer in flash_hal_nand_begin_op() */
#define GET_MAX_PP_LENGTH(len) ((uint32)(((len-532) * 10 )/100))

/* Operation list buffer. The 532 (worst case size - 8bit ECC enabled) is to 
   offset the memory allocated for codeword buffer in flash_hal_nand_begin_op() */
#define GET_MAX_LI_LENGTH(len) ((uint32)(((len-532) * 60)/100))

/* Workspace buffer. The 532 (worst case size - 8bit ECC enabled) is to 
   offset the memory allocated for codeword buffer in flash_hal_nand_begin_op() */
#define GET_MAX_WS_LENGTH(len) ((uint32)((((len-532) * 29)/100) + 528))

typedef struct
{
   /* Buffer info for DM List */
   struct flash_hal_op_cmd *op_list;
   uint32 op_list_cnt;
   uint32 op_list_max;

   /* Workspace Buffer info */
   uint32 *ws_buf;
   uint32 ws_cnt;
   uint32 ws_max;

   /* Buffer info for post process List */
   struct flash_hal_post_process_cmd *post_list;
   uint32 post_list_cnt;
   uint32 post_list_max;

} flash_hal_ws_buffer_info;

/* Flash HAL buffer information */
extern flash_hal_ws_buffer_info ws_buffer_info;

/* Flash HAL DM state flag */
extern uint8 hal_dma_state;

/* Flash HAL Status */
extern uint32 hal_status;

/*---------------------------------------------------------------------
 *             Function Declarations
 * ------------------------------------------------------------------*/

/* Returns an unused device ID */
void flash_hal_get_dev_id(uint32 *dev_id);

/* Releases a device ID */
void flash_hal_release_dev_id(uint32 dev_id);

/* Returns the usage status of a device ID  */
boolean flash_hal_dev_id_in_use(uint32 dev_id);

/* Used for debugging only. Prints work space buffer information. */
void flash_hal_print_ws_buffer_info(void);

/* Returns next available work space buffer address */
void flash_hal_get_next_ws_addr(uint32 *addr, uint32 num_of_dwords);

/* Returns next available post processing list address. */
void flash_hal_get_next_pp_addr(struct flash_hal_post_process_cmd **pp_list);

/* Resets all pointers used for the HAL work space buffer. */
void flash_hal_init_ws_buffer(uint8 *buf, uint32 len);

#endif /* FLASHI_HAL_COMMON_H */
