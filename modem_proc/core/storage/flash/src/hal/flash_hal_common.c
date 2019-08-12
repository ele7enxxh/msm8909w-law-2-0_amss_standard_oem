/*====================================================================
 *
 * FILE: flash_hal_common.c
 *
 * SERVICES: Functions used across all flash controller HALs
 *
 * DESCRIPTION: Functions used across all flash controller HALs
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2008-2009, 2011-2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/hal/flash_hal_common.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/05/12     sv      Code Optimization
 * 05/19/11     bb      Flash hal Optimization
 * 02/19/09     mh      Clean-up remove unused API
 * 03/30/08     BB      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flashi_hal_common.h"

/*====================================================================
 *
 *          S T A T I C   V A R I A B L E   D E F I N I T I O N S
 *
 ====================================================================*/


/* HAL workspace buffer information */
flash_hal_ws_buffer_info ws_buffer_info;

/* Data mover state */
uint8  hal_dma_state = FLASH_HAL_DMA_DISABLE;

/* HAL status. Most of the HAL APIs wont return status,
   So HAL maintains status of each critical operation
   in hal_status variable. This variable can be checked
   for any HAL error status. Maintaining this variable
   correctly will ease debugging any issues with flash HAL */
uint32 hal_status = FLASH_HAL_NO_ERROR;

/*====================================================================
 *
 *                     F U N C T I O N S
 *
 ====================================================================*/

/*
 * Initializes all pointers used for the HAL work space buffer
 */
void flash_hal_init_ws_buffer(uint8 *buffer, uint32 len_bytes)
{
  uint32 pp_max;
  uint32 ws_max;
  uint32 li_max;

  /* Currently the HAL uses the buffer given by driver for
     3 purposes,
      1) To create register command list corresponding to a sequence
      2) Used for workspace buffer inside HAL
      3) To create post processing list

      The GET_MAX_xxx macros divides a given buffer size into 3
      buffers what HAL wants.
   */

  if (len_bytes)
  {
    li_max = GET_MAX_LI_LENGTH(len_bytes);
    ws_max = GET_MAX_WS_LENGTH(len_bytes);
    pp_max = GET_MAX_PP_LENGTH(len_bytes);

    /* Align to 4 byte boundary
     *   The buffer needs to be aligned to 4 byte boundary as the
     *   NAND controller registers are 32 bits.
     */
    li_max += 4 - (li_max % 4);
    ws_max += 4 - (ws_max % 4);
    pp_max += 4 - (pp_max % 4);

    /* Operation list init */
    ws_buffer_info.op_list = (struct flash_hal_op_cmd *) buffer;
    ws_buffer_info.op_list_cnt = 0;
    ws_buffer_info.op_list_max = li_max / (sizeof(struct flash_hal_op_cmd));

    /* Workspace buffer init */
    ws_buffer_info.ws_buf = (uint32 *) (buffer + li_max);
    ws_buffer_info.ws_cnt = 0;
    ws_buffer_info.ws_max = ws_max / (sizeof(uint32));

    /* Postprocess buffer init */
    ws_buffer_info.post_list =
      (struct flash_hal_post_process_cmd *) (buffer + ws_max + li_max);
    ws_buffer_info.post_list_cnt = 0;
    ws_buffer_info.post_list_max = pp_max /
      (sizeof(struct flash_hal_post_process_cmd));
  }
}

/*
 * Returns next available work space buffer address
 */
void flash_hal_get_next_ws_addr(uint32 *addr, uint32 num_of_dwords)
{
  *addr = 0;

  /* Return num_of_dwords buffer from the HAL workspace buffer */
  if ((ws_buffer_info.ws_cnt + num_of_dwords) < ws_buffer_info.ws_max)
  {
    *addr = (uint32) &ws_buffer_info.ws_buf[ws_buffer_info.ws_cnt];
    ws_buffer_info.ws_cnt += num_of_dwords;
  }
  else
  {
    /* Set the workspace buffer overflow bit in hal status */
    hal_status |= FLASH_HAL_WS_BUFFER_OVERFLOW;
  }
}

/*
 * Returns next available post processing list address
 */
void flash_hal_get_next_pp_addr(struct flash_hal_post_process_cmd **pp_list)
{
  /* Return next available post process list buffer from
   * HAL post process buffer */
  if (ws_buffer_info.post_list_cnt < ws_buffer_info.post_list_max)
  {
    *pp_list = &ws_buffer_info.post_list[ws_buffer_info.post_list_cnt];
    ws_buffer_info.post_list_cnt++;
  }
  else
  {
    /* Set the post-process buffer overflow bit in hal status */
    hal_status |= FLASH_HAL_PP_BUFFER_OVERFLOW;
  }
}

