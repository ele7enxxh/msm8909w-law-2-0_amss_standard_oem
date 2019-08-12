#ifndef FLASH_NAND_H
#define FLASH_NAND_H

/*==========================================================================
 *
 * FILE:      flash_nand.h
 *
 * DESCRIPTION: Function and data structure declarations for NAND
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2008-2012, 2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 * This section contains comments describing changes made to the
 * module. Notice that changes are listed in reverse chronological
 * order.
 *
 * $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/dal/flash_nand.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 10/08/15     sb      Clean-up
 * 09/05/12     sv      cleanup, Add core/init function declarations
 * 05/20/11     bb      Flash driver Optimization
 * 02/08/10     bb      Clean up and moved common code to nand entry layer
 * 08/13/09     bb      Added support for optimized copy - multi r/w APIs
 * 08/06/09     jz      Added nand_probe() function prototype
 * 03/12/09     bb      Relocate nand_dm_wrapper to common module
 * 02/19/09     mh      Clean-up
 * 08/28/08     dp      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include "flash_hal.h"
#include "flash_nand_entry.h"

#define FLASH_OP_TIMEOUT 5000  /* TBD - where should this go ? */

/* Macro to set x8 or x16 column address */
#define NAND_SET_COL_ADDR(width, addr) \
  ((width == FLASH_HAL_8_BIT_WIDTH) ? (uint32)addr : ((uint32)addr >> 1))

/* Max address cycles */
#define NAND_MAX_ADDR_CYCLES    5

/* Max page unit size in NAND HAL */
#define NAND_MAX_PAGE_UNIT_SIZE  528

/* Logical page to Physical page translation
   1. If translation table is not initialized, use direct logical to
      physical page mapping
   2. else use the translation table to calculate the physical page
      within the block and add to start page of that block
 */
#define NAND_LP_TO_PP(client_ctxt, lp)                                      \
  ((NULL == ((struct nand_dev_pdata *)                                      \
   (client_ctxt->flash_dev_ctxt->dev_info.common.dev_specific_data))->lp_to_pp_lookup) ?\
  /* physical page = logical page + start page of partition */              \
  (lp + (client_ctxt->client_data.partition_block_start <<                  \
   client_ctxt->client_data.block_size_shift)) :                            \
  /* physical page = start_page_of_block(lp) + trans_table(lp) */           \
  ((((lp >> client_ctxt->client_data.block_size_shift) +                    \
   client_ctxt->client_data.partition_block_start) <<                       \
   (client_ctxt->client_data.block_size_shift + 1)) + (((byte *)((struct nand_dev_pdata *)\
   (client_ctxt->flash_dev_ctxt->dev_info.common.dev_specific_data))->lp_to_pp_lookup)[(lp &\
   (client_ctxt->client_data.pages_per_block - 1))])))

/* Logical start page of given block to physical start page of given block
   within the partition
   1. If translation table is not initialized, use direct logical to
      physical page mapping
   2. else use twice the number of pages per block when calculating logical
      to physical page mapping
 */
#define NAND_SLP_TO_SPP(client_ctxt, lb)                                    \
   ((NULL == ((struct nand_dev_pdata *)                                     \
    (client_ctxt->flash_dev_ctxt->dev_info.common.dev_specific_data))->lp_to_pp_lookup) ?\
   /* phy. start page = start_page_of_blk(lb + partition start blk)*/       \
   ((lb + client_ctxt->client_data.partition_block_start) <<                \
    client_ctxt->client_data.block_size_shift) :                            \
   /* Same as above but use twice the number of pages per block */          \
   ((lb + client_ctxt->client_data.partition_block_start) <<                \
    (client_ctxt->client_data.block_size_shift + 1)))


/* NAND specific/prviate/probe data that is shared across clients and is
   malloced once-only at probe time */
struct nand_dev_pdata
{
  uint8 row_count;
  uint8 col_count;
  uint8 total_address_cycles;
  uint8 *lp_to_pp_lookup;
  uint8 *raw_page_buffer;
  uint32 *drv_ws_buffer;
  uint32 op_status_mask;   /* Operation status mask */
  uint32 op_status_value;  /* Operation success status value */
  enum flash_hal_dma_state dma_state;
  struct flash_nand_params *config_data;
  struct nand_flash_meminfo *flash_mem_info;
};

struct nand_client_specific_data
{
  struct nand_entry_client_data nand_client_data;
};

/* This data structure is used as a temperory save point of buffer descriptors
   in the core driver */
struct nand_buff_desc_save_data
{
  /* To save and restore buffer descriptors during multi
     page read/write operations. The idea is to pass the
     client passed buffer descriptors down to the HAL to
     avoid any extra memory allocation and data copy.
     The 'nand_buff_desc_save' allows to save a pair of
     main and spare buffer descriptors temporarily. Please
     make sure that the client buffer descriptors retains
     its original value when it returns from the driver.
     The 'buff_desc_saved' is a flag to indicate the buff_desc
     got saved or not.*/
  struct flash_hal_buffer_desc buff_desc[2];

  /* Flag to indicate client passed buff_desc needs restoration */
  uint32 buff_desc_saved;
};

#define GET_PDATA(client_ctxt)   ((struct nand_dev_pdata *) \
  client_ctxt->flash_dev_ctxt->dev_info.common.dev_specific_data)

#define GET_MEM_INFO(client_ctxt) (GET_PDATA(client_ctxt))->flash_mem_info

#define CHECK_DEVICE_STATUS(pdata, status) ((status & \
  pdata->op_status_mask) == pdata->op_status_value)

/*
 * Probe the NAND device and do all one time initialization for NAND driver
 */
int nand_probe(flash_client_ctxt *);

/*
 * Read the Nand ID from the device
 */
int nand_read_id(uint32 hal_id, struct flash_hal_buffer_desc *buff_desc);

int nand_decode_ids(flash_client_ctxt *client_ctxt, struct
  flash_nand_params **device_params, uint8 *ids);

int nand_onfi_probe(flash_client_ctxt *client_ctxt,
  struct flash_nand_params **, uint8 *);

int nand_power_event(flash_client_ctxt *client_ctxt, DalPowerCmd power_cmd,
  DalPowerDomain power_domain);

int nand_close(flash_client_ctxt *client_ctxt);

int nand_deinit (flash_client_ctxt *client_ctxt);

int nand_open_partition(flash_client_ctxt *client_ctxt,
  const unsigned char *partition_name);

int nand_erase_blocks(flash_client_ctxt *client_ctxt,
  struct flash_block_vector *block_vector, uint32 iovec_count);

int nand_set_block_state(flash_client_ctxt *client_ctxt,
  uint32 block, enum flash_block_state block_state);

int nand_get_block_state(flash_client_ctxt *client_ctxt, uint32 block,
   enum flash_block_state *block_state);

int nand_write_pages(flash_client_ctxt *client_ctxt,
  enum page_write_opcode write_type, dalsys_mem_desc_list *buffer_desc,
  struct flash_page_iovec *page_data_iovec);

int nand_read_pages(flash_client_ctxt *client_ctxt,
  enum page_read_opcode read_type, dalsys_mem_desc_list *buffer_desc,
  struct flash_page_iovec *page_data_iovec);

int nand_copy_pages(flash_client_ctxt *client_ctxt,
  struct flash_copy_iovec *copy_op, uint32 iovec_count);

int nand_read_otp_page(flash_client_ctxt *client_ctxt, struct
  flash_read_otp_page_op *read_otp_page);

int nand_get_uuid(flash_client_ctxt *client_ctxt, void *uuid_buf,
  uint32 uuid_buf_len, uint32 *uuid_len);

#endif /* FLASH_NAND_H */
