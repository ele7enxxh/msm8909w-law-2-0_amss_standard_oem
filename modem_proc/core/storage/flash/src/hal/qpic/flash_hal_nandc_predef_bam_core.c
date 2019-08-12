/*====================================================================
 *
 * FILE:        flash_hal_nandc_predef_bam_core.c
 *
 * SERVICES:    Full version of QPIC nandc predefined APIs
 *
 * DESCRIPTION: The perdefined BAM support in flash HAL is to improve
 *              throughput with pre-allocated BAM lists.
 *
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2012 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/hal/qpic/flash_hal_nandc_predef_bam_core.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/04/12     sv      Initial Revision
 *===================================================================*/

#include "flashi_hal_nandc_predef_bam.h"
#include "flash_bam_wrapper.h"


/*
 * Initialize command pointers - reduced mode
 */
enum flash_hal_status hal_nandc_predef_cmd_ptrs_init_reduced(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data, 
  struct flash_hal_mem_data *buff_data, uint32 *used_buff_size)
{
  enum flash_hal_status status = FLASH_HAL_SUCCESS;
  uint32 total_mem_usage = 0, mem_used = 0;
  uint32 id = 0, vaddr = 0, paddr = 0, size = 0;
  nandc_bam_cmd_ptr_func_type default_cmd_ptr_fns[6];

  default_cmd_ptr_fns[0] = hal_nandc_bam_00_30_x1_init;       /* 00-30 single read */
  default_cmd_ptr_fns[1] = hal_nandc_bam_00_30_bb_marker_init;/* 00-30 Bad block read */
  default_cmd_ptr_fns[2] = hal_nandc_bam_FF_90_init;          /* FF-90 Reset read ID */
  default_cmd_ptr_fns[3] = hal_nandc_bam_90_onfi_id_init;     /* 90 ONFI ID read */
  default_cmd_ptr_fns[4] = hal_nandc_bam_00_EC_init;          /* 00-EC Parameter page read */
  default_cmd_ptr_fns[5] = NULL;   
  
  vaddr = buff_data->buff_vaddr;
  paddr = buff_data->buff_paddr;

  /* Non-multi command pointer list initialization */
  while (NULL != (default_cmd_ptr_fns[id]))
  {
    status = (default_cmd_ptr_fns[id]) (priv_data, init_data,
      buff_data, &mem_used);

    buff_data->buff_vaddr += mem_used;
    buff_data->buff_paddr += mem_used;
    buff_data->buff_size  -= mem_used;

    size += mem_used;

    if (FLASH_HAL_SUCCESS != status)
    {
      break;
    }

    id++;
  }

  hal_nandc_set_memory_region(NANDC_BAM_CMDDESC_BUFF_REGION,
    vaddr, paddr, size);

  total_mem_usage = size;

  *used_buff_size = total_mem_usage;

  return status;
}

/*
 * Initialize command pointers - full mode
 */
enum flash_hal_status hal_nandc_predef_cmd_ptrs_init_default(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data, 
  struct flash_hal_mem_data *buff_data, uint32 *used_buff_size)
{
  enum flash_hal_status status = FLASH_HAL_SUCCESS;
  uint32 total_mem_usage = 0, mem_used = 0;
  uint32 id = 0, vaddr = 0, paddr = 0, size = 0;
  nandc_bam_cmd_ptr_func_type default_cmd_ptr_fns[8];

  default_cmd_ptr_fns[0] = hal_nandc_bam_00_30_x1_init;       /* 00-30 single read */
  default_cmd_ptr_fns[1] = hal_nandc_bam_80_10_x1_init;       /* 10-80 single write */
  default_cmd_ptr_fns[2] = hal_nandc_bam_60_D0_init;          /* 60-D0 erase */
  default_cmd_ptr_fns[3] = hal_nandc_bam_00_30_bb_marker_init;/* 00-30 Bad block read */
  default_cmd_ptr_fns[4] = hal_nandc_bam_FF_90_init;          /* FF-90 Reset read ID */
  default_cmd_ptr_fns[5] = hal_nandc_bam_90_onfi_id_init;     /* 90 ONFI ID read */
  default_cmd_ptr_fns[6] = hal_nandc_bam_00_EC_init;          /* 00-EC Parameter page read */
  default_cmd_ptr_fns[7] = NULL;  
  
  vaddr = buff_data->buff_vaddr;
  paddr = buff_data->buff_paddr;

  /* Non-multi command pointer list initialization */
  while (NULL != (default_cmd_ptr_fns[id]))
  {
    status = (default_cmd_ptr_fns[id]) (priv_data, init_data,
      buff_data, &mem_used);

    buff_data->buff_vaddr += mem_used;
    buff_data->buff_paddr += mem_used;
    buff_data->buff_size  -= mem_used;

    size += mem_used;

    if (FLASH_HAL_SUCCESS != status)
    {
      break;
    }

    id++;
  }

  hal_nandc_set_memory_region(NANDC_BAM_CMDDESC_BUFF_REGION,
    vaddr, paddr, size);

  total_mem_usage = size;

  *used_buff_size = total_mem_usage;

  return status;
}

/*
 * Highlevel function for Initializing a pre-allocated sequence for BAM
 */
void hal_nandc_predef_bam_init (flash_hal_dev_info *dev_info,
  struct flash_hal_predef_init_data *init_data, uint32 *used_buff_size,
  enum flash_hal_status *status)
{
  enum flash_hal_status tmp_status;
  uint32 cw_count = 0, mem_used;
  struct flash_hal_mem_data buff_data;
  struct flash_hal_ctlr_data *priv_data;

  tmp_status = (init_data->num_cs == 1) ? FLASH_HAL_SUCCESS : FLASH_HAL_FAIL;
  *used_buff_size = 0;

  priv_data = &dev_info->config;

  if (FLASH_HAL_SUCCESS == tmp_status)
  {
    nandc_predef_bam_data.mem_info.predef_max_count =
      init_data->max_support;
    nandc_predef_bam_data.mem_info.predef_buffer_paddr =
      init_data->buff_data.buff_paddr;
    nandc_predef_bam_data.mem_info.predef_buffer_vaddr =
      init_data->buff_data.buff_vaddr;

    buff_data.buff_vaddr = init_data->buff_data.buff_vaddr;
    buff_data.buff_paddr = init_data->buff_data.buff_paddr;
    buff_data.buff_size = init_data->buff_data.buff_size;

    cw_count = priv_data->page_size_bytes / FLASH_NAND_UD_PER_CW;

    /* Allocate memory for register read buffers */
    tmp_status = hal_nand_bam_reg_read_buff_alloc(&buff_data,
      cw_count, priv_data->ecc_len_bits, &mem_used);

    *used_buff_size = mem_used;

    buff_data.buff_vaddr += mem_used;
    buff_data.buff_paddr += mem_used;
    buff_data.buff_size  -= mem_used;

    nandc_predef_bam_data.mem_info.reg_read_usage = mem_used;
  }

  /* Init command lists */
  if (FLASH_HAL_SUCCESS == tmp_status)
  {
    /* Initialize the command list(command blocks) */
    tmp_status = hal_nandc_predef_cmd_blk_buff_alloc(&buff_data,
      cw_count, &mem_used);

    *used_buff_size += mem_used;

    buff_data.buff_vaddr += mem_used;
    buff_data.buff_paddr += mem_used;
    buff_data.buff_size  -= mem_used;

    nandc_predef_bam_data.mem_info.cmd_list_usage = mem_used;
  }

    /* Init command lists */
  if (FLASH_HAL_SUCCESS == tmp_status)
  {
    /* Allocate memory for descriptor buffer */
    tmp_status = hal_nandc_predef_desc_buff_alloc(&buff_data,
      cw_count, &mem_used);

    *used_buff_size += mem_used;

    buff_data.buff_vaddr += mem_used;
    buff_data.buff_paddr += mem_used;
    buff_data.buff_size  -= mem_used;

    nandc_predef_bam_data.mem_info.desc_buff_usage = mem_used;
  }

  if (FLASH_HAL_SUCCESS == tmp_status)
  {
    hal_nandc_predef_cmd_list_init(cw_count, priv_data->ecc_len_bits);

    hal_nandc_predef_desc_list_init(cw_count, priv_data->ecc_len_bits);

    /* NOTE: THIS FUNCTION MODIFIES THE buff_data VARIABLE.
             PLEASE DO ACCRODINGLY IF YOU ARE USING THIS
             VARIABLE AFTER THIS FUNCTION CALL */
    tmp_status = hal_nandc_predef_cmd_ptrs_init(priv_data,
      init_data, &buff_data, &mem_used);

    *used_buff_size += mem_used;
  }

  nandc_predef_bam_data.mem_info.total_mem_usage = *used_buff_size;

  if (FLASH_HAL_SUCCESS == tmp_status)
  {
    priv_data->predef_init_flag = TRUE;
  }

  *status = tmp_status;
}

/*
 * Init pre-defined sequence function pointers
 */
void hal_nandc_fptrs_init_default(struct flash_hal_ctlr_ops  *ctlr_ops)
{
  ctlr_ops->flash_hal_predef_init = hal_nandc_predef_bam_init;
  ctlr_ops->flash_hal_predef_deinit = hal_nandc_predef_bam_deinit;

  ctlr_ops->flash_hal_read_id = hal_nandc_bam_FF_90_exec;
  ctlr_ops->flash_hal_read_onfi_id = hal_nandc_bam_90_onfi_id_exec;
  ctlr_ops->flash_hal_read_param_page = hal_nandc_bam_00_EC_exec;

  ctlr_ops->flash_hal_read_page = hal_nandc_bam_00_30_x1_exec;
  ctlr_ops->flash_hal_read_bb_marker = hal_nandc_bam_00_30_bb_marker_exec;

  ctlr_ops->flash_hal_write_page = hal_nandc_bam_80_10_x1_exec;
  ctlr_ops->flash_hal_erase_block = hal_nandc_bam_60_D0_exec;

  ctlr_ops->flash_hal_transfer = hal_nandc_transfer;
  ctlr_ops->flash_hal_begin_op = flash_hal_nand_begin_op;
  ctlr_ops->flash_hal_end_op = flash_hal_nand_end_op;
  
  ctlr_ops->flash_hal_post_process_op_list = hal_nandc_post_process_op_list;
}


/*
 * Init pre-defined sequence function pointers
 */
void hal_nandc_fptrs_init_reduced(struct flash_hal_ctlr_ops  *ctlr_ops)
{
  ctlr_ops->flash_hal_predef_init = hal_nandc_predef_bam_init;
  ctlr_ops->flash_hal_predef_deinit = hal_nandc_predef_bam_deinit;

  ctlr_ops->flash_hal_read_id = hal_nandc_bam_FF_90_exec;
  ctlr_ops->flash_hal_read_onfi_id = hal_nandc_bam_90_onfi_id_exec;
  ctlr_ops->flash_hal_read_param_page = hal_nandc_bam_00_EC_exec;

  ctlr_ops->flash_hal_read_page = hal_nandc_bam_00_30_x1_exec;
  ctlr_ops->flash_hal_read_bb_marker = hal_nandc_bam_00_30_bb_marker_exec;
  
  ctlr_ops->flash_hal_post_process_op_list = NULL;
}


/*
 * Initialize the Interface used to communicate with NAND
 */
int hal_nandc_iface_init(void)
{
  int result;

  result = flash_bam_init();
  
  if(result != FLASH_BAM_SUCCESS)
  {
    return FLASH_HAL_FAIL;
  }
  
  return FLASH_HAL_SUCCESS;
}

/*
 * De-Initialize the Interface used to communicate with NAND
 */
void hal_nandc_iface_deinit(void)
{
  /* Currently this is a stub. flash_bam_init() when called multiple
   * times will execute only once and return without executing any code
   * for the rest of the invocations. This can be modified later if 
   * required to do bam init and deinit by removing the check for multiple
   * invocation in flash_bam_init() and uncommenting the below code 
   */

  //(void)flash_bam_deinit(); 
}
