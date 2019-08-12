/*====================================================================
 *
 * FILE:        flash_hal_nandc_common.c
 *
 * SERVICES:    APIs for QPIC NAND Controller
 *
 * DESCRIPTION: APIs for QPIC NAND Controller, which will be used by
 *              HAL common Layer
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2011-2012,2014 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/hal/qpic/flash_hal_nandc_common.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 06/13/14     sb      Update to support 9x45
 * 10/02/12     sv      Move XFR steps cfg to iface specific code
 * 09/05/12     sv      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flashi_hal_common.h"
#include "flashi_hal_nand.h"
#include "flashi_hal_nandc.h"

/*====================================================================
 *
 *          S T A T I C   V A R I A B L E   D E F I N I T I O N S
 *
 ====================================================================*/

/* Page layout for 2K page, when ECC enabled on main and spare */
static const struct flash_hal_page_unit_layout page_2k_516_0_x3_500_16_x1[] =
         {
            {3,  0,   516,  528,   0},
            {1,  0,   500,  500, 16}
         };

/* Page layout for 2K page, when ECC disabled */
static const struct flash_hal_page_unit_layout page_2k_528_0_x4[] =
         {
            {4,  0,      528,  528,    0},
         };

/* Page layout for 2K page, when ECC enabled on main and spare */
static const struct flash_hal_page_unit_layout page_8bit_ecc_2k_516_0_x3_500_16_x1[] =
         {
            {3, 0,    516, 532,   0},
            {1, 0,    500, 500, 16}
         };

/* Page layout for 2K page, when ECC disabled */
static const struct flash_hal_page_unit_layout page_8bit_ecc_2k_532_0_x4[] =
         {
            {4, 0,    532,  532, 0},
         };

/* Page layout for 4K page, when ECC enabled on main and spare */
static const struct flash_hal_page_unit_layout page_4k_516_0_x7_484_32_x1[] =
         {
            {7, 0,    516,  528,   0},
            {1, 0,   484,  484, 32}
         };

/* Page layout for 4K page, when ECC disabled */
static const struct flash_hal_page_unit_layout page_4k_528_0_x8[] =
         {
            {8, 0,      528,  528,    0},
         };

/* Page layout for 4K page, when ECC enabled on main and spare */
static const struct flash_hal_page_unit_layout page_8bit_ecc_4k_516_0_x7_484_32_x1[] =
         {
            {7, 0,    516, 532,   0},
            {1, 0, 484, 484, 32}
         };

/* Page layout for 4K page, when ECC disabled */
static const struct flash_hal_page_unit_layout page_8bit_ecc_4k_532_0_x8[] =
         {
            {8, 0,    532,  532, 0},
         };

/* Page layouts for QPIC NANDC - Please make sure this is in sync
   with flash_hal_page_layout_id */
static struct flash_hal_page_layout nandc_page_layout[] =
         {
            {1, page_2k_528_0_x4},
            {2, page_2k_516_0_x3_500_16_x1},
            {1, page_8bit_ecc_2k_532_0_x4},
            {2, page_8bit_ecc_2k_516_0_x3_500_16_x1},
            {1, page_4k_528_0_x8},
            {2, page_4k_516_0_x7_484_32_x1},
            {1, page_8bit_ecc_4k_532_0_x8},
            {2, page_8bit_ecc_4k_516_0_x7_484_32_x1}
         };

static uint32 default_cfg0 = NAND_DEVn_CFG0_BASE;
static uint32 default_cfg1 = NAND_DEVn_CFG1_BASE;
static uint32 default_ecc_cfg = NAND_DEVn_ECC_CFG_BASE;
static uint32 default_erased_cw_det_cfg = NAND_ERASED_CW_DETECT_CFG_BASE;

/* Return mask for post processing */
uint32 get_mask(uint32 num, uint32 clear_flag)
{
  uint32 mask = 0;

  /* TBD: See the possibility of using
      mask = 0xFFFFFFFF >> (32 - (num << 3)); instead of
      the below switch case */
  mask = 0xFFFFFFFF >> (32 - (num << 3)); 

  if (clear_flag)
  {
    mask = ~mask;
  }

  return mask;
}

/* ====================================================================
 *
 *        F L A S H   H A L   C O N F I G U R A T I O N   A P I S
 *
 * ==================================================================*/

/*
 * Returns Memory required by flash HAL
 */
void hal_nandc_get_memory_usage(struct flash_hal_ctlr_data *priv_data,
  uint32 no_of_pages, uint32 *mem_use)
{
  *mem_use = 0;

  switch (priv_data->page_size_bytes)
  {
    case FLASH_HAL_2048B_PAGE:
      *mem_use = FLASH_HAL_MEM_USAGE_2048PAGE(no_of_pages);
      break;
    case FLASH_HAL_4096B_PAGE:
      *mem_use = FLASH_HAL_MEM_USAGE_4096PAGE(no_of_pages);
      break;
    default:
      break;
  }
}

/*
 * Returns page layout configured by flash HAL.
 */
void hal_nandc_get_page_layout(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_page_layout **layout)
{
  uint32 layout_id = FLASH_HAL_LAYOUT_NONE;

  *layout = NULL;

  /* If the HW ECC is not used, the return NULL as HAL doesn't impose
     any page layouts in that mode */
  if (FLASH_HAL_HW_ECC_NONE == priv_data->ecc_state)
  {
    return;
  }

  /* For 2K page NAND */
  if (2048 == priv_data->page_size_bytes)
  {
    if (FLASH_HAL_HW_ECC_8BIT == priv_data->ecc_len_bits)
    {
      switch (priv_data->ecc_state)
      {
        case FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE :
          layout_id = FLASH_HAL_LAYOUT_8BIT_ECC_2K_516_0_X3_500_16_X1; 
          break;
        default:
          layout_id = FLASH_HAL_LAYOUT_8BIT_ECC_2K_532_0_X4; 
          break;
      }
    }
    else
    {
      switch (priv_data->ecc_state)
      {
        case FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE :
          layout_id = FLASH_HAL_LAYOUT_2K_516_0_X3_500_16_X1;
          break;
        default:
          layout_id = FLASH_HAL_LAYOUT_2K_528_0_X4;
          break;
      }
    }
  }
  /* For 4K page NAND */
  else if (4096 == priv_data->page_size_bytes)
  {
    if (FLASH_HAL_HW_ECC_8BIT == priv_data->ecc_len_bits)
    {
      switch (priv_data->ecc_state)
      {
        case FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE :
          layout_id = FLASH_HAL_LAYOUT_8BIT_ECC_4K_516_0_X7_484_32_X1; 
          break;
        default:
          layout_id = FLASH_HAL_LAYOUT_8BIT_ECC_4K_532_0_X8; 
          break;
      }
    }
    else
    {
      switch (priv_data->ecc_state)
      {
        case FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE :
          layout_id = FLASH_HAL_LAYOUT_4K_516_0_X7_484_32_X1; 
          break;
        default:
          layout_id = FLASH_HAL_LAYOUT_4K_528_0_X8; 
          break;
      }
    }
  }

  /* Return the layout if a valid layout ID found */
  if (layout_id != FLASH_HAL_LAYOUT_NONE)
  {
    *layout = (struct flash_hal_page_layout *) &nandc_page_layout[layout_id];
  }
}

/*
 * Sets the gap in number of clock cycles in flash HAL
 */
static void hal_nandc_set_clock_cycle_gap(uint32 gap)
{
  /* Program NAND_DEVn_CFG1[22:17]: WR_RD_BSY_GAP */
  default_cfg1 &= ~HWIO_FMSK(NAND_DEVn_CFG1, WR_RD_BSY_GAP);       \
  default_cfg1 |= gap << HWIO_SHFT(NAND_DEVn_CFG1, WR_RD_BSY_GAP); \
}

/*
 * HAL configure for QPIC NAND Controller.  This function won't write to the
 * registers, but it updates the devices private data structure.
 */
static void hal_nandc_config_ctlr_data_4bit_ecc(struct flash_hal_ctlr_data *priv_data,
  enum flash_hal_status *status)
{
  uint32 spare_data_len = 0;

  *status = FLASH_HAL_SUCCESS;

  priv_data->cw_size_bytes = FLASH_NAND_BYTES_PER_CW;

  priv_data->addr_offset = ((FLASH_HAL_8_BIT_WIDTH == priv_data->width) ?
    priv_data->cw_size_bytes : (priv_data->cw_size_bytes >> 1));

  priv_data->total_page_size = priv_data->page_size_bytes +
                                priv_data->spare_size_bytes;
 
  /* Mode = 0: 4 Bit ECC BCH */
  FLASH_BCH_ECC_MODE(priv_data, 0); 

  FLASH_SELECT_BCH_ECC(priv_data);

  /* ECC byte size */
  if (FLASH_HAL_8_BIT_WIDTH == priv_data->width)
  {
    FLASH_BCH_ECC_SIZE(priv_data, NAND_4BIT_ECC_SIZE_8);
    spare_data_len = NAND_4BIT_ECC_SPARE_SIZE_8;
  }
  else
  {
    FLASH_BCH_ECC_SIZE(priv_data, NAND_4BIT_ECC_SIZE_16);
    spare_data_len = NAND_4BIT_ECC_SPARE_SIZE_16;
  }

  /* If ECC enabled on spare */
  if (FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE == priv_data->ecc_state)
  {
    spare_data_len = FLASH_HAL_SPARE_UDATA_LEN_PER_CW;

    /* Spare bytes is 4 when ECC is enabled on Spare */
    FLASH_SPARE_SIZE_BYTES(priv_data, 0x4);

    /* UD bytes = main + spare - 1 - for ECC on spare */
    FLASH_UD_SIZE(priv_data, (FLASH_NAND_UD_PER_CW + spare_data_len));

    FLASH_BCH_ECC_DATA_BYTES(priv_data, (FLASH_NAND_UD_PER_CW + spare_data_len));

    priv_data->ecc_check_size = ((FLASH_HAL_8_BIT_WIDTH == priv_data->width) ?
      FLASH_ERASE_4BIT_ECC_CHECK_SIZE2_8 : FLASH_ERASE_4BIT_ECC_CHECK_SIZE2_16);    
  }
  else if (FLASH_HAL_HW_ECC_ON_MAIN_ONLY == priv_data->ecc_state)
  {
    spare_data_len = ((FLASH_HAL_8_BIT_WIDTH == priv_data->width) ?
      NAND_4BIT_ECC_SPARE_SIZE_8 : NAND_4BIT_ECC_SPARE_SIZE_16);

    FLASH_SPARE_SIZE_BYTES(priv_data, spare_data_len);

    priv_data->ecc_check_size = ((FLASH_HAL_8_BIT_WIDTH == priv_data->width) ?
        FLASH_ERASE_4BIT_ECC_CHECK_SIZE1_8 : FLASH_ERASE_4BIT_ECC_CHECK_SIZE1_16);    
  }

  /* When ECC disabled */
  if (FLASH_HAL_HW_ECC_DISABLE == priv_data->ecc_state)
  {
    /* Disable ECC */
    FLASH_DISABLE_BCH_ECC(priv_data);

    /* Bad block in spare */
    FLASH_BAD_BLOCK_IN_SPARE_AREA(priv_data);

    /* Bad block byte number.*/
    FLASH_BAD_BLOCK_BYTE_NUM(priv_data, NAND_BAD_BLOCK_BYTE_NUM);
    
    /* Spare size */
    if (FLASH_HAL_8_BIT_WIDTH == priv_data->width)
    {
      FLASH_SPARE_SIZE_BYTES(priv_data, NAND_4BIT_ECC_SPARE_BYTES_8);
    }
    else
    {
      FLASH_SPARE_SIZE_BYTES(priv_data, NAND_4BIT_ECC_SPARE_BYTES_16);
    }
  }
  else
  {
    FLASH_ENABLE_BCH_ECC(priv_data);

    /* Bad block byte number */
    FLASH_BAD_BLOCK_BYTE_NUM(priv_data,
       (priv_data->page_size_bytes - 
       (FLASH_NAND_BYTES_PER_CW * (priv_data->cw_cnt - 1)) + 1));
  }
}

/*
 * HAL configure for QPIC NAND Controller to support 8bit ecc.  This function 
 * won't write to the registers, but it updates the devices private data structure.
 */
static void hal_nandc_config_ctlr_data_8bit_ecc(struct flash_hal_ctlr_data *priv_data,
  enum flash_hal_status *status)
{
  uint32 max_udata_len = 0;

  *status = FLASH_HAL_SUCCESS;

  priv_data->cw_size_bytes = FLASH_8BIT_ECC_NAND_BYTES_PER_CW;

  priv_data->addr_offset =  ((FLASH_HAL_8_BIT_WIDTH == priv_data->width) ?
             priv_data->cw_size_bytes : (priv_data->cw_size_bytes >> 1));
  
  priv_data->total_page_size = priv_data->page_size_bytes + 
      (FLASH_8BIT_ECC_NAND_SA_PER_CW *  priv_data->cw_cnt);
  
  /* Mode = 1: 8 Bit ECC BCH */
  FLASH_BCH_ECC_MODE(priv_data, 1); 

  FLASH_SELECT_BCH_ECC(priv_data);

  /* Wide flash */
  if (FLASH_HAL_16_BIT_WIDTH == priv_data->width)
  {
    FLASH_BCH_ECC_SIZE(priv_data, NAND_8BIT_ECC_SIZE_16);
  }

  /* If ECC enabled on spare */
  if (FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE == priv_data->ecc_state)
  {
    max_udata_len = FLASH_HAL_SPARE_UDATA_LEN_PER_CW;

    /* Spare bytes is 2 when ECC is enabled on Spare */
    FLASH_SPARE_SIZE_BYTES(priv_data, 0x2);

    /* UD bytes = main + spare - for ECC on spare */
    FLASH_UD_SIZE(priv_data, (FLASH_NAND_UD_PER_CW + max_udata_len));

    FLASH_BCH_ECC_DATA_BYTES(priv_data, (FLASH_NAND_UD_PER_CW + max_udata_len));

    priv_data->ecc_check_size = ((FLASH_HAL_8_BIT_WIDTH == priv_data->width) ?
        FLASH_ERASE_8BIT_ECC_CHECK_SIZE2_8 : FLASH_ERASE_8BIT_ECC_CHECK_SIZE2_16);
  }
  else if (FLASH_HAL_HW_ECC_ON_MAIN_ONLY == priv_data->ecc_state)
  {
    max_udata_len = ((FLASH_HAL_8_BIT_WIDTH == priv_data->width) ?
                     NAND_8BIT_ECC_SPARE_SIZE_8 : NAND_8BIT_ECC_SPARE_SIZE_16);

    FLASH_SPARE_SIZE_BYTES(priv_data, max_udata_len);

    priv_data->ecc_check_size = ((FLASH_HAL_8_BIT_WIDTH == priv_data->width) ?
        FLASH_ERASE_8BIT_ECC_CHECK_SIZE1_8 : FLASH_ERASE_8BIT_ECC_CHECK_SIZE1_16);
  }

  /* When ECC disabled */
  if (FLASH_HAL_HW_ECC_DISABLE == priv_data->ecc_state)
  {
    /* Disable ECC */
    FLASH_DISABLE_BCH_ECC(priv_data);
 
    /* Bad block in spare */
    FLASH_BAD_BLOCK_IN_SPARE_AREA(priv_data);

    /* Bad block byte number */
    FLASH_BAD_BLOCK_BYTE_NUM(priv_data, NAND_8BIT_ECC_BAD_BLOCK_BYTE_NUM);

    /* Spare size */
    if (FLASH_HAL_8_BIT_WIDTH == priv_data->width)
    {
      FLASH_SPARE_SIZE_BYTES(priv_data, NAND_8BIT_ECC_SPARE_BYTES_8);
    }
    else
    {
      FLASH_SPARE_SIZE_BYTES(priv_data, NAND_8BIT_ECC_SPARE_BYTES_16);
    }
  }
  else
  {
    /* Bad block byte number */
    FLASH_BAD_BLOCK_BYTE_NUM(priv_data,
       (priv_data->page_size_bytes - 
       (FLASH_8BIT_ECC_NAND_BYTES_PER_CW * (priv_data->cw_cnt - 1)) + 1));

    FLASH_ENABLE_BCH_ECC(priv_data);
  }
}
/*
 * HAL configure for QPIC NAND Controller.  This function won't write to the
 * registers, but it updates the devices private data structure.
 */
static void hal_nandc_configure(struct flash_hal_ctlr_data *priv_data,
  enum flash_hal_status *status)
{
  *status = FLASH_HAL_SUCCESS;

  /* Write CFG shadows with base that is defined */
  priv_data->addr0 = 0;
  priv_data->addr1 = 0;

  priv_data->cfg0 = default_cfg0;
  priv_data->cfg1 = default_cfg1;
  priv_data->ecc_cfg = default_ecc_cfg;
  priv_data->erased_cw_det_cfg = default_erased_cw_det_cfg;

  priv_data->cw_cnt = priv_data->page_size_bytes / FLASH_NAND_UD_PER_CW;

  /* Initialize supported device commands as VALID to NANDC */
  priv_data->dev_vld = NAND_FLASH_DEV_CMD_VLD_BASE;

  /* Bad Block in spare area */
  priv_data->cfg1 |= NAND_OPT_OFF <<
    HWIO_SHFT(NAND_DEVn_CFG1, BAD_BLOCK_IN_SPARE_AREA);

  /* Wide flash */
  if (FLASH_HAL_16_BIT_WIDTH == priv_data->width)
  {
    priv_data->cfg1 |= NAND_OPT_ON << 
      HWIO_SHFT(NAND_DEVn_CFG1, WIDE_FLASH);
  }
  
  if(FLASH_HAL_HW_ECC_8BIT == priv_data->ecc_len_bits)
  {
    hal_nandc_config_ctlr_data_8bit_ecc(priv_data, status);
  }
  else
  {
    hal_nandc_config_ctlr_data_4bit_ecc(priv_data, status);
  }

  FLASH_NUM_ADDR_CYCLES(priv_data, (priv_data->row_count+priv_data->col_count));

  priv_data->config_is_needed = FALSE;
}

/*
 *  Deinitializes the data structures for qpic nandc HAL.
 */
void hal_nandc_close(flash_hal_dev_info *dev_info)
{
  /* Invoke predef deinit, if any */
  if (dev_info->ctlr_ops.flash_hal_predef_deinit)
  {
    dev_info->ctlr_ops.flash_hal_predef_deinit(dev_info);
  }
}

/*
 *  HAL initialization for QPIC NAND Controller.
 */
void hal_nandc_init(const struct flash_hal_dev_config *dev_cfg,
  flash_hal_dev_info *dev_info, enum flash_hal_status *status)
{
  struct flash_hal_ctlr_data *priv_data;
  struct flash_hal_ctlr_ops  *ctlr_ops;

  *status = FLASH_HAL_FAIL;

  priv_data = &dev_info->config;
  ctlr_ops = &dev_info->ctlr_ops;

  /* Configure device into 8-bit mode for probing */
  priv_data->dev_type = dev_cfg->dev_type;
  priv_data->chip_sel = dev_cfg->chip_sel;
  priv_data->width = FLASH_HAL_8_BIT_WIDTH;
   /* Write CFG shadows with base that is defined */
  priv_data->addr0 = 0;
  priv_data->addr1 = 0;
  priv_data->cfg0 = default_cfg0;
  priv_data->cfg1 = default_cfg1;
  priv_data->dev_vld = NAND_FLASH_DEV_CMD_VLD_BASE;
  priv_data->seq_cnt = 0;
  priv_data->cur_seq_cnt = -1;
  priv_data->page_size_bytes = FLASH_HAL_2048B_PAGE;
  priv_data->cw_size_bytes = FLASH_NAND_BYTES_PER_CW;
  priv_data->addr_offset = 0;
  priv_data->ecc_state = FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE;
  priv_data->config_is_needed = TRUE;
  priv_data->predef_init_flag = FALSE;
  priv_data->block_size = FLASH_NAND_BLOCK_SIZE_64;
   /* Init NAND controllers function pointers */
  ctlr_ops->flash_hal_configure = hal_nandc_configure;
  ctlr_ops->flash_hal_close = hal_nandc_close;
  ctlr_ops->flash_hal_get_page_layout = hal_nandc_get_page_layout;
  ctlr_ops->flash_hal_get_predef_mem_usage = hal_nandc_get_memory_usage;
  ctlr_ops->flash_hal_set_clk_cycle_gap = hal_nandc_set_clock_cycle_gap;
  
  hal_nandc_fptrs_init(ctlr_ops);

  *status = (enum flash_hal_status)hal_nandc_iface_init();
}

/*
 *  HAL de-initialization for QPIC NAND Controller.
 */
void hal_nandc_deinit(flash_hal_dev_info *dev_info)
{
  hal_nandc_iface_deinit();
}
