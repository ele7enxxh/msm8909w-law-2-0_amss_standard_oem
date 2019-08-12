/*====================================================================
 *
 * FILE:        flash_hal_nandc_predef_bam_pprocess.c
 *
 * SERVICES:    Postprocess APIs for predefined bam support in QPIC
 *              nandc HAL
 *
 * DESCRIPTION: The perdefined BAM support in flash HAL is to improve
 *              throughput with pre-allocated BAM lists.
 *
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2012-2015 QUALCOMM Technologies Incorporated.
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
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/hal/qpic/flash_hal_nandc_predef_bam_pprocess.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 11/12/15     sb      ECC bit flip check support
 * 08/06/15     sb      Enhance ONFI param page read to read multiple copies
 * 10/18/13     sb      Return error codes for WP and CW mismatch
 * 11/06/12     sv      Update Flash Status check
 * 09/04/12     sv      Initial Revision
 *===================================================================*/

#include "flashi_hal_nandc_predef_bam.h"

/*
 * Routine to check common errors in status buffer
 */
static void hal_nandc_bam_check_status_error(uint32 *status_buffer,
  uint32 num_status, enum flash_hal_status *status)
{
  uint32 mask = FLASH_NANDC_STATUS_MASK;

  *status = FLASH_HAL_SUCCESS;

  while ((num_status) && (FLASH_HAL_SUCCESS == *status))
  {
    /* Check for status bytes */
    if ((*status_buffer & mask) != FLASH_NAND_STATUS_OP_DONE)
    {
      if (FLASH_NAND_STATUS_MPU_ERROR_MASK ==
        (*status_buffer & (FLASH_NAND_STATUS_MPU_ERROR_MASK & mask)))
      {
        *status = FLASH_HAL_FAIL_MPU_ERR;
      }
      else
      {
        *status = FLASH_HAL_FAIL;
      }

      break;
    }

    num_status--;
    status_buffer++;
  }
}

/*
 * This API does the post processing check for an erased page.
 */
static void hal_nandc_bam_erased_page_pprocess(
  struct flash_hal_ctlr_data *priv_data, uint32 *ecc_read_buff,
  uint32 *erased_cw_det_status_buff, enum flash_hal_status *status)
{
  *status = FLASH_HAL_FAIL;

  if ((*erased_cw_det_status_buff & 
       FLASH_NANDC_ERASED_CW_DET_STATUS_MASK) ==
       FLASH_ERASED_CW_DETECT_STATUS)
  {
    *status = FLASH_HAL_ERASED_PAGE;
  }
  else
  {
    *status = FLASH_HAL_CHECK_ECC;
  }
}

/*
 * Does the post processing for single read.
 */
void hal_nandc_bam_00_30_x1_pprocess(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_op_list *op_list, enum flash_hal_status *status)
{
  int32 cw_count, erase_cw_count = 0;
  uint32 status_val = 0, mask = FLASH_NANDC_STATUS_MASK;
  uint32 *status_buffer, *erased_cw_det_status_buffer;
  struct nandc_predef_reg_read_buff *reg_read_buff;
  enum flash_hal_status tmp_status = FLASH_HAL_SUCCESS;

  *status = FLASH_HAL_SUCCESS;

  reg_read_buff = &nandc_predef_bam_data.reg_read_buff;

  status_buffer = (uint32 *) reg_read_buff->status_buffer;
  erased_cw_det_status_buffer = (uint32 *)reg_read_buff->erased_cw_det_status_buffer;

  /* For each page,
       - Check for status bytes
       - Check for Erase page
          - Check for ECC bytes
          - Check for Spare Udata bytes (if applicable)
          - Check for Erased CW detect status register
  */
  cw_count = priv_data->cw_cnt - 1;

  while (cw_count >= 0)
  {
    /* Check for status bytes */
    status_val = (cw_count << FLASH_NAND_STATUS_CW_CNTR_SHFT) |
      FLASH_NAND_STATUS_OP_DONE;

    /* If there is an error, look for erased page */
    if ((*status_buffer & mask) != status_val)
    {
      if (FLASH_NAND_STATUS_MPU_ERROR_MASK ==
        (*status_buffer & (FLASH_NAND_STATUS_MPU_ERROR_MASK & mask)))
      {
        *status = FLASH_HAL_FAIL_MPU_ERR;
        break;
      }
      else if(((*status_buffer & mask) >> FLASH_NAND_STATUS_CW_CNTR_SHFT) != cw_count)
      {
        *status = FLASH_HAL_CW_MISMATCH_ERR;
		break;
      }

      /* Look for Erased page, if it is not an erased page then flag it as a real failure 
       * currently we are checking for each CW. Update to use page erased status bit 
       */
      hal_nandc_bam_erased_page_pprocess(priv_data, NULL,
        erased_cw_det_status_buffer, &tmp_status);

      if (FLASH_HAL_ERASED_PAGE == tmp_status)
      {
        erase_cw_count++;
      }
      else if (FLASH_HAL_CHECK_ECC == tmp_status)
      {
        *status = FLASH_HAL_CHECK_ECC;
        break;
      }
      else if (FLASH_HAL_FAIL == tmp_status)
      {
        *status = FLASH_HAL_FAIL;
        break;
      }
    }

    status_buffer++;
    erased_cw_det_status_buffer++;
    cw_count--;
  }
  if (erase_cw_count == priv_data->cw_cnt)
  {
    *status = FLASH_HAL_ERASED_PAGE;
  }
}


/*
 * Does the post processing for single erase.
 */
void hal_nandc_bam_60_D0_pprocess(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_op_list *op_list, enum flash_hal_status  *status)
{
  uint32 *status_buffer,  block_count = 0;
  struct nandc_predef_reg_read_buff *reg_read_buff;
  uint32  mask = FLASH_NANDC_STATUS_MASK;

  reg_read_buff = &nandc_predef_bam_data.reg_read_buff;
  status_buffer = (uint32 *) reg_read_buff->status_buffer;
  block_count = op_list->prev_op_count;

  while (block_count) 
  {
    /* Check for status bytes */
    if ((*status_buffer & mask) != FLASH_NAND_STATUS_OP_DONE)
    {
      if (FLASH_NAND_STATUS_MPU_ERROR_MASK ==
        (*status_buffer & (FLASH_NAND_STATUS_MPU_ERROR_MASK & mask)))
      {
        *status = FLASH_HAL_FAIL_MPU_ERR;
      }
      else
      {
        *status = FLASH_HAL_FAIL;
      }

      break;
    }

    block_count--;
    status_buffer++;
  }
}

/*
 * Does the post processing for single write.
 */
void hal_nandc_bam_80_10_x1_pprocess(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_op_list *op_list, enum flash_hal_status  *status)
{
  int32 cw_count = 0;
  uint32 status_val = 0;
  uint32 *status_buffer;
  uint32  mask = FLASH_NANDC_STATUS_MASK;
  struct nandc_predef_reg_read_buff *reg_read_buff;

  *status = FLASH_HAL_SUCCESS;

  reg_read_buff = &nandc_predef_bam_data.reg_read_buff;

  status_buffer = (uint32 *) reg_read_buff->status_buffer;

  /* For each page, Check for status bytes */
  cw_count = priv_data->cw_cnt - 1;

  while (cw_count >= 0)
  {
    /* Check for status bytes */
    status_val = (cw_count << FLASH_NAND_STATUS_CW_CNTR_SHFT) |
      FLASH_NAND_STATUS_OP_DONE;

    /* If there is an error, look for MPU error */
    if ((*status_buffer & mask) != status_val)
    {
      if (FLASH_NAND_STATUS_MPU_ERROR_MASK ==
        (*status_buffer & (FLASH_NAND_STATUS_MPU_ERROR_MASK & mask)))
      {
        *status = FLASH_HAL_FAIL_MPU_ERR;
      }
      else if(((*status_buffer & mask) >> FLASH_NAND_STATUS_CW_CNTR_SHFT) != cw_count)
      {
        *status = FLASH_HAL_CW_MISMATCH_ERR;
      }
      else
      {
        *status = FLASH_HAL_FAIL;
      }

      break;
    }
    else if ((cw_count == 0) && 
      ((*status_buffer & FLASH_NAND_DEV_STATUS_WP_MASK) ==
      FLASH_NAND_DEV_STATUS_WP_ENABLED))
    {
      /* The check for WP bits after last Code Word write shows that 
       * Write Protect is enabled. The check is made only on the 
       * last code word because the device status is read only 
       * after the last code word write */
      *status = FLASH_HAL_WP_ENABLED;
    }
    status_buffer++;
    cw_count--;
  }
}

/*
 * Does the post processing for read bad block marker.
 */
void hal_nandc_bam_00_30_bb_marker_pprocess(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_op_list *op_list, enum flash_hal_status  *status)
{
  uint32 *status_buffer;
  struct nandc_predef_reg_read_buff *reg_read_buff;

  reg_read_buff = &nandc_predef_bam_data.reg_read_buff;
  status_buffer = (uint32 *) reg_read_buff->status_buffer;

  hal_nandc_bam_check_status_error(status_buffer, 1, status);

}

/*
 * Does the post processing for probe.
 */
void hal_nandc_bam_FF_90_pprocess(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_op_list *op_list, enum flash_hal_status  *status)
{
  uint32 *tmp_buffer;
  struct nandc_predef_reg_read_buff *reg_read_buff;
 
  reg_read_buff = &nandc_predef_bam_data.reg_read_buff;
  tmp_buffer = (uint32 *) reg_read_buff->status_buffer;

  hal_nandc_bam_check_status_error(tmp_buffer, 2, status);
}

/*
 * Does the post processing for ONFI ID read.
 */
void hal_nandc_bam_90_onfi_id_pprocess(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_op_list *op_list, enum flash_hal_status  *status)
{
  uint32 *tmp_buffer;
  struct nandc_predef_reg_read_buff *reg_read_buff;

  reg_read_buff = &nandc_predef_bam_data.reg_read_buff;
  tmp_buffer = (uint32 *) reg_read_buff->status_buffer;

  hal_nandc_bam_check_status_error(tmp_buffer, 1, status);

}

/*
 * Does the post processing for read parameter page.
 */
void hal_nandc_bam_00_EC_pprocess(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_op_list *op_list, enum flash_hal_status  *status)
{
  int32 cw_count = 0;
  uint32 status_val = 0;
  uint32 *status_buffer;
  uint32  mask = FLASH_NANDC_STATUS_MASK;
  struct nandc_predef_reg_read_buff *reg_read_buff;

  reg_read_buff = &nandc_predef_bam_data.reg_read_buff;
  status_buffer = (uint32 *) reg_read_buff->status_buffer;

  cw_count = priv_data->cw_cnt - 1;

  while (cw_count >= 0)
  {
    /* Check for status bytes */
    status_val = (cw_count << FLASH_NAND_STATUS_CW_CNTR_SHFT) |
      FLASH_NAND_STATUS_OP_DONE;

    if ((*status_buffer & mask) != status_val)
    {
      if (FLASH_NAND_STATUS_MPU_ERROR_MASK ==
        (*status_buffer & (FLASH_NAND_STATUS_MPU_ERROR_MASK & mask)))
      {
        *status = FLASH_HAL_FAIL_MPU_ERR;
      }
      else
      {
        *status = FLASH_HAL_FAIL;
      }
      break;
    }
    status_buffer++;
    cw_count--;
  }
}

