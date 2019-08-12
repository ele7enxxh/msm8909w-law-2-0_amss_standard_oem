#ifndef FLASH_HAL_NANDC_PREDEF_BAM_H
#define FLASH_HAL_NANDC_PREDEF_BAM_H

/*====================================================================
 *
 * FILE:        flash_hal_nandc_predef_bam.h
 *
 * SERVICES:    Common header file for predefined BAM support in QPIC nandc HAL
 *
 * DESCRIPTION: The perdefined BAM support in flash HAL is to improve
 *              through pre-allocated BAM lists.
 *
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/hal/qpic/flashi_hal_nandc_predef_bam.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 12/05/12     sv      Optimize NAND Read Page - cleanups
 * 10/02/12     sv      Add Transfer Steps config
 * 09/05/12     sv      Initial version
 *===================================================================*/

#include "flashi_hal_nand.h"
#include "flashi_hal_nandc.h"
#include "flashi_hal_common.h"

/* Following macros used as indexes to address different command lists. */

#define NANDC_BAM_CFG_CMD_LIST          0 /* CFG0/CFG1 */
#define NANDC_BAM_ECC_BCH_CFG_CMD_LIST  2 /* BCH ECC configuration */ 
#define NANDC_BAM_E_CW_DET_CFG_CMD_LIST 4 /* Erased CW detect Cfg */

#define NANDC_BAM_CMD_CMD_LIST          0 /* Command register */
#define NANDC_BAM_EXEC_CMD_LIST         1 /* Command execution */

#define NANDC_BAM_ADDR0_CMD_LIST        0 /* First ADDR register
                                               configuration command list */

#define NANDC_BAM_STATUS0_CMD_LIST      0 /* First status register
                                               command list */

#define NANDC_BAM_COPY0_CMD_LIST        0 /* First copy command list */


#define NANDC_BAM_MISC0_CMD_LIST        0 /* First miscellaneous
                                               command list */

#define NANDC_BAM_BUFF_CLEAR_CMD_LIST   0 /* Spare area clear command list */

#define NANDC_CLEAR_STATUS_CMD_LIST     0 /* Status register clear
                                               command list */
#define NANDC_XFER_STEPS_CFG_LIST       0 /* Transfer steps cfg
                                               command list */
											   
/* Maximum size for reading ECC bytes and spare udata
   in a codeword */
#define FLASH_BAM_ECC_BUFF_SIZE         FLASH_NAND_SA_PER_CW
#define FLASH_BAM_8BIT_ECC_BUFF_SIZE    FLASH_8BIT_ECC_NAND_SA_PER_CW

#define FLASH_BAM_PREDEF_MISC_MAX         16 /* Maximum size for miscellaneous
                                                buffer */

/* NANDC HAL predef memory region IDs
   Following macros used as indexes to address
   different memory regions in the nandc predefind code. */
enum nandc_predef_mem_region_ids
{
  NANDC_BAM_REG_READ_BUFF_REGION,  /* Buffer for Register read operations */

 
  NANDC_BAM_CMDBLOCK_BUFF_REGION, /* Buffer used for command block
                                       for single page operations */
  NANDC_BAM_CMDDESC_BUFF_REGION,  /* Buffer used for Command/Data Descriptor 
                                       - No need of cache handling */

  NANDC_BAM_MEM_REGION_MAX       /* Buffer used for command lists
                                      for single page operations */
};

/* IDs for supported predefined sequences in nandc HAL.
   NOTE: CHANGING THE ORDER IN THIS ENUM MAY IMPACT
         THE NANDC HAL LOGIC. CURRENTLY IT IMPACTS
         NANDC_BAM_pprocess_fns[] in the post process
         logic.
 */
enum nandc_predef_nand_seq_ids
{
  NANDC_00_x1_READ = 0x0,        /* 00 single read */
  NANDC_00_30_x1_READ,           /* 00-30 single read */
  NANDC_80_10_x1_WRITE,          /* 10-80 single write */
  NANDC_60_D0_ERASE,             /* 60-D0 erase */
  NANDC_00_30_BB_READ,           /* 00-30 Bad block read */
  NANDC_FF_90_PROBE,             /* FF-90 probe */
  NANDC_00_EC_READ,              /* 00-EC parameter page read */
  NANDC_90_ONFI_ID,              /* ONFI ID read */
  NANDC_XFER_STEPS_CFG,          /* Configure Transfer Steps */
  NANDC_OP_ID_MAX                /* Max supported IDs */
};

/* Structure holds the base address of buffers used */
struct nandc_predef_reg_read_buff
{
  uint32 base_virt_addr;
  uint32 base_phys_addr;

  /* These buffers are read buffers, that needs to be part of a
     32 byte aligned, size in multiple of 32 buffer. The read buffers
     needs be invlidated from the cache before the BAM operation */
  uint32 read_id_buffer;           /* Read ID buffer base */
  uint32 status_buffer;            /* Status buffer base */
  uint32 erased_cw_det_status_buffer; /* Erased CW detect status buffer base */
  uint32 dummy_buffer;

  /* Write Buffers */
  uint32 spare_fill_buffer;
};

/* Descriptor */
struct nandc_predef_desc
{
  struct flash_hal_op_desc *cfg_desc;         /* CS/CFG0/CFG1/ECC Buf command */
  struct flash_hal_op_desc *addr_desc;        /* Address command */
  struct flash_hal_op_desc *status_desc;      /* Status read command.*/
  struct flash_hal_op_desc *cmd_exec_desc;    /* Cmd/Exec register command */
  struct flash_hal_op_desc *copyid_desc;      /* Data copy command */
  struct flash_hal_op_desc *misc_desc;        /* Misc. read/write command */
  struct flash_hal_op_desc *buff_clear_desc;  /* Clear the spare area */

  struct flash_hal_op_desc *lock_desc;         /* Channel lock list command */
  struct flash_hal_op_desc *unlock_desc;       /* Channel unlock list command */

  struct flash_hal_op_desc *clear_status_desc;/* Clear ctlr Status command */

  struct flash_hal_op_desc *read_copy_desc;   /* Data copy command */
  struct flash_hal_op_desc *write_copy_desc;  /* Data copy command */

};

/* Command Block Lists */
struct nandc_predef_cmd_list
{
  struct flash_hal_op_cmd *cfg_list;           /* CS/CFG0/CFG1/ECC Buf cmd */
  struct flash_hal_op_cmd *addr_list;          /* Address command */
  struct flash_hal_op_cmd *status_list;        /* Status read command.*/
  struct flash_hal_op_cmd *cmd_exec_list;      /* Cmd/Exec register command */
  struct flash_hal_op_cmd *copyid_list;        /* Data copy command */
  struct flash_hal_op_cmd *misc_list;          /* Misc. read/write command */
  struct flash_hal_op_cmd *buff_clear_list;    /* Clear the spare area */

  struct flash_hal_op_cmd *lock_list;          /* Channel lock list command */
  struct flash_hal_op_cmd *unlock_list;        /* Channel unlock list command */

  struct flash_hal_op_cmd *clear_status_list;  /* Clear ctlr Status command */
  struct flash_hal_op_cmd *locate_list;        /* Clear ctlr Status command */
  
  struct flash_hal_op_cmd *xfer_steps_list;    /* Set Transfer Steps */
};


/* Mem used for command pointers (each command sequence) */
struct nandc_predef_cmdptr_mem_data
{
  enum nandc_predef_nand_seq_ids seq_id;
  struct flash_hal_mem_data mem_data;
};

/* Memory Usage Info
  - Mem used for command buffers
  - Mem used for command lists
  - Mem used for command pointers (each command sequence) */
struct nandc_predef_mem_info
{
  uint32 predef_max_count;           /* Max support count */
  uint32 predef_buffer_vaddr;        /* Virtual addr. of predef memory */
  uint32 predef_buffer_paddr;        /* Phys. addr. of predef memory */
  uint32 total_mem_usage;            /* Total memory usage */
  uint32 reg_read_usage;             /* Bytes used for command lists */
  uint32 cmd_list_usage;             /* Bytes used for command lists */
  uint32 desc_buff_usage;            /* Bytes used for command buffers */
  struct nandc_predef_cmdptr_mem_data cmd_ptr[NANDC_OP_ID_MAX]; /* Cmd. ptr
                                        mem info */
  struct flash_hal_mem_data region[NANDC_BAM_MEM_REGION_MAX]; /* Memory
                                        region information */
};

/* QPIC NANDC predef list info for BAM */
struct nandc_predef_list_info
{
  uint32 init_count;                       /* Pre-allocated intialize count */
  struct nandc_predef_cmd_list cmd_list; /* Pre-allocated command lists */
  struct nandc_predef_desc     desc; /* Pre-allocated command lists */
  struct nandc_predef_reg_read_buff reg_read_buff; /* Read Register buffer addresses */
  struct flash_hal_predef_op_list cmd_ptr_list[NANDC_OP_ID_MAX]; /* Cmd ptrs */
  struct nandc_predef_mem_info mem_info; /* Memory details */
};

/* Global variable which has all NANDC predef list info */
extern struct nandc_predef_list_info nandc_predef_bam_data;

/* Return the physical address from a given virtual address */
#define GET_PHYS_ADDR(vaddr)  ((nandc_predef_bam_data.mem_info.predef_buffer_paddr) + \
  ((uint32)(vaddr) - nandc_predef_bam_data.mem_info.predef_buffer_vaddr))

/* Add 'a' command list to command pointer list */
#define NANDC_CMD_PTR(a)  SET_CMD_PTR(GET_PHYS_ADDR((uint32)a))

/* Add 'a' command list to command pointer list and mark it as the last */
#define NANDC_LAST_CMD_PTR(a)  SET_LAST_CMD_PTR(GET_PHYS_ADDR((uint32)a))

/* Type for command pointer init function */
typedef enum flash_hal_status (*nandc_bam_cmd_ptr_func_type) (
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *mem_data, uint32 *used_buff_size);

/******************************************************************************
 *                 I N I T I A L I Z A T I O N   A P I S
 *****************************************************************************/
/* Update the memory region in the global table */
void hal_nandc_set_memory_region(enum nandc_predef_mem_region_ids id,
  uint32 vaddr, uint32 paddr, uint32 size);

  
enum flash_hal_status hal_nand_bam_reg_read_buff_alloc(
  struct flash_hal_mem_data *buff_data, uint32 cw_count,
  uint32 ecc_len, uint32 *mem_used);
  
/*
 * Allocate memory for pre-allocated command blocks
 */
enum flash_hal_status hal_nandc_predef_cmd_blk_buff_alloc(
  struct flash_hal_mem_data *buff_data, uint32 cw_count, uint32 *mem_used);

enum flash_hal_status hal_nandc_predef_desc_buff_alloc(
  struct flash_hal_mem_data *buff_data, uint32 cw_count, uint32 *mem_used);
  
void hal_nandc_predef_desc_list_init(uint32 cw_count, uint32 ecc_len);

void hal_nandc_predef_cmd_list_init(uint32 cw_count, uint32 ecc_len);  
  
/* Single read. */
enum flash_hal_status hal_nandc_bam_00_30_x1_init(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *mem_data, uint32 *used_buff_size);

/* Single write */
enum flash_hal_status hal_nandc_bam_80_10_x1_init(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *mem_data, uint32 *used_buff_size);

/* Single erase */
enum flash_hal_status hal_nandc_bam_60_D0_init(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *mem_data, uint32 *used_buff_size);

/* Single read bb marker */
enum flash_hal_status hal_nandc_bam_00_30_bb_marker_init(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *mem_data, uint32 *used_buff_size);

/* Probe. */
enum flash_hal_status hal_nandc_bam_FF_90_init(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *mem_data, uint32 *used_buff_size);

/* ONFI ID Read. */
enum flash_hal_status hal_nandc_bam_90_onfi_id_init(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *mem_data, uint32 *used_buff_size);

/* Parameter Page read */
enum flash_hal_status hal_nandc_bam_00_EC_init(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *mem_data, uint32 *used_buff_size);

/* Transfer Steps Configure */
enum flash_hal_status hal_nandc_bam_xfer_steps_init(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *mem_data, uint32 *used_buff_size);
  
/* De-Initializes a pre-allocated sequence */
void hal_nandc_predef_bam_deinit(flash_hal_dev_info *dev_info);

/******************************************************************************
 *                              E X E C   A P I S
 *****************************************************************************/

/* Prepare function for single read */
void hal_nandc_bam_00_30_x1_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_page_data *page_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status);

/* Prepare function for single write */
void hal_nandc_bam_80_10_x1_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_page_data *page_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status);

/* Prepare function for single erase */
void hal_nandc_bam_60_D0_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_block_data *block_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status);

/* Prepare function for reading bad block marker*/
void hal_nandc_bam_00_30_bb_marker_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_spare_data *page_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status);

/* Prepare the probe command pointer list */
void hal_nandc_bam_FF_90_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_buffer_desc *buff_desc,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status);

/* Prepare the read ONFI id command pointer list */
void hal_nandc_bam_90_onfi_id_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_onfi_data *probe_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status);

/* Prepare the parameter page read command pointer list */
void hal_nandc_bam_00_EC_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_onfi_data *page_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status);

/* Prepare the Transfer Steps config command pointer list */
void hal_nandc_set_xfer_cfg_exec(flash_hal_dev_info *dev_info, 
  struct flash_hal_xfer_cfg *cfg_data, enum flash_hal_status *status);
  
/******************************************************************************
 *                  P O S T   P R O C E S S   A P I S
 *****************************************************************************/

/* Does the post processing for single read. */
void hal_nandc_bam_00_30_x1_pprocess(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_op_list *op_list, enum flash_hal_status *status);

/* Does the post processing for single erase. */
void hal_nandc_bam_60_D0_pprocess(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_op_list *op_list, enum flash_hal_status  *status);

/* Does the post processing for single write. */
void hal_nandc_bam_80_10_x1_pprocess(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_op_list *op_list, enum flash_hal_status  *status);

/* Does the post processing for read bad block marker. */
void hal_nandc_bam_00_30_bb_marker_pprocess(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_op_list *op_list, enum flash_hal_status  *status);

/* Does the post processing for probe. */
void hal_nandc_bam_FF_90_pprocess(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_op_list *op_list, enum flash_hal_status  *status);

/* Does the post processing for ONFI ID read. */
void hal_nandc_bam_90_onfi_id_pprocess(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_op_list *op_list, enum flash_hal_status  *status);

/* Does the post processing for read parameter page. */
void hal_nandc_bam_00_EC_pprocess(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_op_list *op_list, enum flash_hal_status  *status);

/* Highlevel function for post processing. */
void hal_nandc_bam_pprocess(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_op_list *op_list, enum flash_hal_status  *status);

#endif /* FLASH_HAL_NANDC_PREDEF_BAM_H */
