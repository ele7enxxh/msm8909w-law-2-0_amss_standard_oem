/*====================================================================
 *
 * FILE:        flash_bam_wrapper.c
 *
 * SERVICES:    BAM support for flash drivers
 *
 * DESCRIPTION: BAM for flash drivers
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/iface/flash_bam_wrapper.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 01/30/14     eo      Add bam_core_dump() API call for debug
 * 10/18/13     sb      Error fatal for unrecoverable errors
 * 03/25/13     eo      Add NAND_BAM_BASE_ADDR define
 * 12/04/12     sv      Remove exta bytes requested in MemRegion alloc
 * 07/27/12     sv      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include "flash_bam_wrapper.h"

bam_config_type flash_bam_cfg;
bam_handle flash_bam_handle;
bam_handle flash_bam_cmd_handle;
bam_handle flash_bam_read_handle;
bam_handle flash_bam_write_handle;
bam_pipe_config_type flash_bam_pipe_cfg;
bam_coredump_config_type flash_bam_dump_cfg = {0};
bam_result_type bam_poll_result;

void *desc_buf_handle = NULL;
void *dump_buffer = NULL;

uint32 dump_buf_size = 0;
uint32 flash_bam_poll_cnt = FLASH_BAM_MAX_POLL_COUNT;

/* BAM core dump debug buffer allocation */
int flash_bam_core_dump_mem_alloc(void)
{
  int result = DAL_SUCCESS;
  bam_info_type buf_info;
  uint32 bam_num_cnt, pipe_num_cnt, bam_status = 0;

  if (flash_bam_handle == NULL)
  {
    return FLASH_BAM_FAIL;
  }
  
  bam_status = bam_get_info(flash_bam_handle, &buf_info);
  if (bam_status != BAM_SUCCESS)
  {
    return FLASH_BAM_FAIL;
  }

  if (dump_buffer == NULL)
  {
    bam_num_cnt = FLASH_BAM_DEBUG_BAM_COUNT;
    pipe_num_cnt = FLASH_BAM_DEBUG_PIPE_COUNT;

    dump_buf_size = (bam_num_cnt * buf_info.bam_buffer_size) + (pipe_num_cnt * 
      buf_info.bam_pipe_buffer_size) + buf_info.bam_header_buffer_size;

    result = DALSYS_Malloc(dump_buf_size, (void **)&dump_buffer);
    if (result != DAL_SUCCESS)
    {
      return FLASH_BAM_FAIL;
    }
  }
  
  return FLASH_BAM_SUCCESS;
}
  
/* BAM core dump structures to help debug BAM */
int flash_bam_core_dump(void)
{
  uint32 pipe_mask, peer_pmask, bam_status = 0;

  if (flash_bam_handle == NULL)
  {
    return FLASH_BAM_FAIL;
  }
  
  /* Allocate BAM core dump buffer */
  if (flash_bam_core_dump_mem_alloc() != FLASH_BAM_SUCCESS)
  {
    return FLASH_BAM_FAIL;
  }

  /* 
   * System mode: pipe-3 data consumer, pipe-4 data producer, 
   * pipe-5 cmd consumer. 
   */
  pipe_mask = FLASH_BAM_DEBUG_PIPE_MASK;
  peer_pmask = FLASH_BAM_DEBUG_PEER_MASK;

  flash_bam_dump_cfg.user_buffer = dump_buffer;
  flash_bam_dump_cfg.buf_size    = dump_buf_size;
  flash_bam_dump_cfg.pipe_mask   = pipe_mask;
  flash_bam_dump_cfg.peer_pipe_mask = peer_pmask;
  flash_bam_dump_cfg.descfifo_buf  = NULL;
  flash_bam_dump_cfg.datafifo_buf  = NULL;
  flash_bam_dump_cfg.descfifo_size = 0;
  flash_bam_dump_cfg.datafifo_size = 0;

  bam_status = bam_core_dump(flash_bam_handle, &flash_bam_dump_cfg);
  if (bam_status != BAM_SUCCESS)
  {
    return FLASH_BAM_FAIL;
  }

  return FLASH_BAM_SUCCESS;
}
	 
/* Initializes a BAM PIPE */
bam_handle flash_bam_pipe_init(uint32 pipe_num, 
  bam_pipe_config_type *bam_pipe_cfg)
{
  bam_status_type bam_status;
  bam_handle bam_pipe_handle;

  /* Initialize the BAM PIPE */  
  bam_pipe_handle = 
    bam_pipe_init(flash_bam_handle, pipe_num, bam_pipe_cfg, NULL);
  
  if(bam_pipe_handle == NULL)
  {
    return NULL;
  }

  /* Disable the IRQ Mode to use polling mode for this pipe */
  bam_status = bam_pipe_setirqmode(bam_pipe_handle, 0, bam_pipe_cfg->options);
  if(bam_status != BAM_SUCCESS)
  {
    return NULL;
  }

  /* Enable the pipe to start using it */
  bam_status = bam_pipe_enable(bam_pipe_handle);
  if(bam_status != BAM_SUCCESS)
  {
    return NULL;
  }

  return bam_pipe_handle;
}

/* Initializes the BAM for NAND 
 *          Initialzes the BAM module
 *          Initializes the PIPES used for NAND*/

uint32 desc_fifo_size = 0;
DALSYSMemInfo desc_mem_info;
uint32 flash_bam_init_done = 0;

int flash_bam_init(void)
{
  int result = FLASH_BAM_SUCCESS;
  
  if(flash_bam_init_done)
  {
    return FLASH_BAM_SUCCESS;
  }
  
  flash_bam_init_done = 1;

  /* Adding 0x400 to force memory from DDR. This will be removed after the 
     DAL/SBL changes to allocate from DDR when nand driver does malloc */
  desc_fifo_size = FLASH_BAM_CMD_DESC_FIFO_SZ + FLASH_BAM_READ_DESC_FIFO_SZ +
    FLASH_BAM_WRITE_DESC_FIFO_SZ;

  /* Allocate a physically contiguous buffer for Descriptor FIFO - check 
     for 8 byte alignment */
  if ( DAL_SUCCESS != DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED |
    DALSYS_MEM_PROPS_PHYS_CONT, DALSYS_MEM_ADDR_NOT_SPECIFIED, 
    DALSYS_MEM_ADDR_NOT_SPECIFIED, desc_fifo_size, &desc_buf_handle, NULL))
  {
    desc_buf_handle = NULL;
    return FLASH_BAM_FAIL;
  }

  DALSYS_MemInfo(desc_buf_handle, &desc_mem_info);

  /* Mandatory fields to be supplied by the client */
  flash_bam_cfg.bam_pa =  NAND_BAM_BASE_PHY_ADDR; 
  flash_bam_cfg.bam_va = NAND_BAM_BASE_ADDR;   
  flash_bam_cfg.bam_irq = FLASH_BAM_IRQ;  /* Set this to 0 for polling mode */
  flash_bam_cfg.bam_irq_mask = BAM_IRQ_ERR_EN | BAM_IRQ_EMPTY_EN; 
  flash_bam_cfg.sum_thresh = 0x200; /* Desc Summing threshold - Not used */
  flash_bam_cfg.bam_mti_irq_pa = 0;  

  /* Initialize BAM module */
  flash_bam_handle =  bam_init(&flash_bam_cfg, NULL);
  
  if(flash_bam_handle == NULL)
  {
    result = FLASH_BAM_FAIL;
  }    

  /* Initialize common Pipe configuration structure values */
  flash_bam_pipe_cfg.options = BAM_O_DESC_DONE | BAM_O_OUT_OF_DESC | BAM_O_ERROR
    | BAM_O_EOT | BAM_O_POLL; 

  flash_bam_pipe_cfg.mode = BAM_MODE_SYSTEM;
  flash_bam_pipe_cfg.evt_thresh = 0x200; /* Not used - setting to 0x200 as per VI code */

  /* Security features */
  /* Set the lock group to be equal to the super-group. */
  flash_bam_pipe_cfg.lock_group = 1;

 /* NAND Command Pipe Initialization */ 
  flash_bam_pipe_cfg.desc_base_va = desc_mem_info.VirtualAddr;
  flash_bam_pipe_cfg.desc_base_pa = desc_mem_info.PhysicalAddr;
  flash_bam_pipe_cfg.desc_size = FLASH_BAM_CMD_DESC_FIFO_SZ;
  flash_bam_pipe_cfg.dir = BAM_DIR_CONSUMER;
  
  if (result == FLASH_BAM_SUCCESS)
  {
    flash_bam_cmd_handle = 
      flash_bam_pipe_init(NAND_BAM_CMD_PIPE, &flash_bam_pipe_cfg);
    if(flash_bam_cmd_handle == NULL)
    {
      result = FLASH_BAM_FAIL;
    }
  }

 /* NAND Write Pipe Initialization */ 
  flash_bam_pipe_cfg.desc_base_va += FLASH_BAM_CMD_DESC_FIFO_SZ;
  flash_bam_pipe_cfg.desc_base_pa += FLASH_BAM_CMD_DESC_FIFO_SZ;
  flash_bam_pipe_cfg.desc_size = FLASH_BAM_WRITE_DESC_FIFO_SZ;
  flash_bam_pipe_cfg.dir = BAM_DIR_CONSUMER;
  flash_bam_pipe_cfg.lock_group = 0;

  if (result == FLASH_BAM_SUCCESS)
  { 
    flash_bam_write_handle =
      flash_bam_pipe_init(NAND_BAM_WRITE_PIPE, &flash_bam_pipe_cfg);
    if(flash_bam_write_handle == NULL)
    {
      result = FLASH_BAM_FAIL;
    }
  }

  /* NAND Read Pipe Initialization */
  flash_bam_pipe_cfg.desc_base_va += FLASH_BAM_WRITE_DESC_FIFO_SZ;
  flash_bam_pipe_cfg.desc_base_pa += FLASH_BAM_WRITE_DESC_FIFO_SZ;
  flash_bam_pipe_cfg.desc_size = FLASH_BAM_READ_DESC_FIFO_SZ;
  flash_bam_pipe_cfg.dir = BAM_DIR_PRODUCER;
  
  if (result == FLASH_BAM_SUCCESS)
  { 
    flash_bam_read_handle =
      flash_bam_pipe_init(NAND_BAM_READ_PIPE, &flash_bam_pipe_cfg);
    if(flash_bam_read_handle == NULL)
    {
      result = FLASH_BAM_FAIL;
    }
  }

  /* Deinit BAM incase of any failure during initialization */
  if (result != FLASH_BAM_SUCCESS)
  {
    flash_bam_deinit();
  }
  
  return result;
}

/* Queue the descriptor to the desired BAM pipe */
int flash_bam_queue_desc(bam_handle bam_pipe_handle,
  struct flash_bam_xfer_desc_list *desc_list)
{
  bam_iovec_type *desc_ptr;
  bam_status_type bam_status;
  int i;

  desc_ptr = desc_list->desc_list_ptr;
  for(i = 1; i <= desc_list->desc_count; i++)
  {
    bam_status = bam_pipe_transfer(bam_pipe_handle, desc_ptr->buf_pa,
      desc_ptr->buf_size, desc_ptr->flags, (void *)i);
    if (bam_status != BAM_SUCCESS)
    {
      // Do error handling, resetting pipes etc.
      DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
        "Flash: bam transfer failed.!");
      return FLASH_BAM_FAIL;
    }
    desc_ptr++;
  }
  return FLASH_BAM_SUCCESS;
}

/* Poll the pipe for transfer completion */
int flash_bam_poll_pipe(bam_handle pipe_handle, void* user_data)
{
  bam_status_type bam_status;
  int result = FLASH_BAM_SUCCESS;

  memset( &bam_poll_result, 0, sizeof(bam_result_type));
  
  flash_bam_poll_cnt = FLASH_BAM_MAX_POLL_COUNT;
  
  while(flash_bam_poll_cnt)
  {      
    bam_status = bam_pipe_poll(pipe_handle, &bam_poll_result);
    if ((bam_status != BAM_SUCCESS) || (bam_poll_result.event == BAM_EVENT_ERROR) || (
      bam_poll_result.event == BAM_EVENT_OUT_OF_DESC))
    {
      // Do error handling, etc.
      if (flash_bam_core_dump() != FLASH_BAM_SUCCESS)
      {
        /* Error Fatal */
        DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
                        "Flash: bam error, bam core dump failed!");
      }
      else
      {
        DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
                        "Flash: bam pipe poll failed.!");
      }

      result = FLASH_BAM_FAIL;
      break;
    }
    if (((bam_poll_result.event == BAM_EVENT_DESC_DONE) || 
      (bam_poll_result.event == BAM_EVENT_EOT)) && 
      (bam_poll_result.data.xfer.xfer_cb_data == user_data))
    {
      break;
    }
    flash_bam_poll_cnt = flash_bam_poll_cnt - 1;
  }
  
  if(!flash_bam_poll_cnt)
  {      
    // Do error handling, etc.
    if (flash_bam_core_dump() != FLASH_BAM_SUCCESS)
    {
      /* Error Fatal */
      DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
                      "Flash: xfer fail, bam core dump failed!");
    }
    else
    {
      /* Error Fatal */
      DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
                      "Flash: bam transfer failed.!");
    }

    result = FLASH_BAM_FAIL;
  }
  return result;
}

int flash_bam_wait_for_xfer_completion(
  struct flash_bam_xfer_desc_list *cmd_desc_list, 
  struct flash_bam_xfer_desc_list *read_desc_list, 
  struct flash_bam_xfer_desc_list *write_desc_list)
{
  int result = FLASH_BAM_SUCCESS;
 
    result =  flash_bam_poll_pipe(flash_bam_cmd_handle, 
     (void *)cmd_desc_list->desc_count);
 
  if ((read_desc_list != NULL) && (read_desc_list->desc_count) && 
    (result == FLASH_BAM_SUCCESS))
  {
    result = flash_bam_poll_pipe(flash_bam_read_handle, 
      (void *)read_desc_list->desc_count);
  }

  if ((write_desc_list != NULL) && (write_desc_list->desc_count) &&
    (result == FLASH_BAM_SUCCESS))
  {
    result = flash_bam_poll_pipe(flash_bam_write_handle,
      (void *)write_desc_list->desc_count);
  }

  return result;
}

/* Transfer the command/data described by the descriptors to NAND using BAM */
int flash_bam_transfer(struct flash_bam_xfer_desc_list *cmd_desc_list, 
  struct flash_bam_xfer_desc_list *read_desc_list, 
  struct flash_bam_xfer_desc_list *write_desc_list)
{
  int result = FLASH_BAM_SUCCESS;

  /* Command descriptor should be present for any transfer */
  if((cmd_desc_list == NULL) || (cmd_desc_list->desc_count == 0))
  {
    DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
        "Flash: invalid bam input params!");
    return FLASH_BAM_FAIL;
  }

  /* Queue the command descriptors to the command pipe */
  result =  flash_bam_queue_desc(flash_bam_cmd_handle, cmd_desc_list);
  
  /* If read data decriptor is present then queue it to read pipe */
  if ((read_desc_list != NULL) && (read_desc_list->desc_count) && 
    (result == FLASH_BAM_SUCCESS))
  {
    result =  flash_bam_queue_desc(flash_bam_read_handle, read_desc_list);
  }

   /* If the write data descriptor is present then queue it to the write pipe */
  if ((write_desc_list != NULL) && (write_desc_list->desc_count) &&
    (result == FLASH_BAM_SUCCESS) )
  {
    result =  flash_bam_queue_desc(flash_bam_write_handle, write_desc_list);
  }

  /* Wait for the BAM transfer to complete */  
  if (result == FLASH_BAM_SUCCESS)
  {
    result = flash_bam_wait_for_xfer_completion (cmd_desc_list, read_desc_list,
      write_desc_list);
  }

  if(result != FLASH_BAM_SUCCESS)
  {
    //Handle resetting of BAM
    // Do we need to reset it or leave it to help debugging?
    DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
        "Flash: bam tx failed.!");
  }
  return result;
}

/* Deinitialize a BAM pipe */
int flash_bam_pipe_deinit(bam_handle bam_pipe_handle)
{
  int result = FLASH_BAM_SUCCESS;

  /* Disable the pipe */
  if (bam_pipe_disable(bam_pipe_handle) != BAM_SUCCESS)
  {
    result = FLASH_BAM_FAIL;
  }
  
  /* deinitialize a pipe */
  if (bam_pipe_deinit(bam_pipe_handle) != BAM_SUCCESS)
  {
    result = FLASH_BAM_FAIL;
  }
  
  return result;
}

/* Deinitialize the BAM */
int flash_bam_deinit(void)
{
  int ret_status = FLASH_BAM_SUCCESS;

  /* deinitialize command pipe */
  if (flash_bam_pipe_deinit(flash_bam_cmd_handle) != FLASH_BAM_SUCCESS)
  {
    ret_status = FLASH_BAM_FAIL;
  }
  
  /* deinitialize write pipe */
  if (flash_bam_pipe_deinit(flash_bam_write_handle) != FLASH_BAM_SUCCESS)
  {
    ret_status = FLASH_BAM_FAIL;
  }

  /* deinitialize read pipe */
  if (flash_bam_pipe_deinit(flash_bam_read_handle) != FLASH_BAM_SUCCESS)
  {
    ret_status = FLASH_BAM_FAIL;
  }

  /* deinitialize BAM */
  if (flash_bam_handle != NULL)
  {
    if (bam_deinit(flash_bam_handle, 1) != BAM_SUCCESS)
    {
      ret_status |= FLASH_BAM_FAIL;
    }
  }

  /* Free any memory allocated earlier */
  if(desc_buf_handle)
  {
    DALSYS_Free(desc_buf_handle);
    desc_buf_handle = NULL;
  }
  
  /* Reinitialize all the handles */
  flash_bam_cmd_handle = NULL;
  flash_bam_write_handle = NULL;
  flash_bam_read_handle = NULL;
  flash_bam_handle = NULL;
  
  return ret_status;
}
