/*====================================================================
 *
 * FILE:        flash_hal_nandc_predef_bam_init.c
 *
 * SERVICES:    Initialization for predefined BAM support in nandc HAL
 *
 * DESCRIPTION: The perdefined BAM support in flash HAL is to improve
 *              throughput with pre-allocated BAM lists.
 *
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2012-2015 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/hal/qpic/flash_hal_nandc_predef_bam_init.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 08/06/15     sb      Enhance ONFI param page read to read multiple copies
 * 11/28/14     sb      Use 0x35 cmd for ONFI parameter page read
 * 03/07/13     eo      Fix pointer access to virtual address
 * 09/03/13     eo      Include target specific HWIO register mapping
 * 12/05/12     sv      Optimize Read Page
 * 12/04/12     sv      Add 4K Page Micron NAND Support
 * 09/28/12     sv      Add NWD flag in nand read id sequence
 * 09/04/12     sv      Initial Revision

 *===================================================================*/

#include "flashi_hal_nandc_predef_bam.h"
#include "flash_bam_wrapper.h"

/******************************************************************************
 *
 *        QPIC NANDC HAL Memory usage layout for
 *        predefined sequence support. Flash DAL provides the memory to HAL
 *
 *        +------------------------------------+ ---+
 *        |                                    |    |
 *        | Region - 1                         |    |
 *        |                                    |    |
 *        | Register Read Buffers used to      |    |
 *        | store values read from nandc       |    |
 *        | registers.                         |    |
 *        |                                    |    |
 *        | (status, read id etc.)             |    |
 *        |                                    |    |
 *        |------------------------------------|    |
 *        |                                    |    |
 *        | Region - 2                         |    |
 *        |                                    |    |
 *        | Command Block Region that has the  |    |
 *        | list of command elements that will |    |
 *        | be processed by BAM                |    |
 *        |                                    |    |
 *        | (config list, address list etc)    |    |
 *        |                                    |    |
 *        |------------------------------------|    |
 *        |                                    |    |
 *        | Region - 3                         |    |
 *        |                                    |    |
 *        | Descriptor Region - has the list   |    |
 *        | of BAM command descriptors.This is |    |
 *        | internal to the driver and no cache|    |
 *        | maintenance is required.           |    |
 *        | (read page descriptor list, write  |    |
 *        |  page descriptor list etc.)        |    |
 *        |                                    |    |
 *        +------------------------------------+ ---+
 *
 *
 * Cache maintenance Information
 *
 *    - For all single page/block operations
 *        - Cache inavlidate region - 1
 *        - Cache clean region - 2
 *
 *****************************************************************************/

/* Parent structure for BAM predefined data */
struct nandc_predef_list_info nandc_predef_bam_data;

/* Update the memory region in the global table */
void hal_nandc_set_memory_region(enum nandc_predef_mem_region_ids id,
  uint32 vaddr, uint32 paddr, uint32 size)
{
  struct flash_hal_mem_data *region;

  region = &nandc_predef_bam_data.mem_info.region[id];

  region->buff_vaddr = vaddr;
  region->buff_paddr = paddr;
  region->buff_size  = size;
}

/*
 * Initialize buffers for pre-allocated command lists
 */
enum flash_hal_status hal_nand_bam_reg_read_buff_alloc(
  struct flash_hal_mem_data *buff_data, uint32 cw_count,
  uint32 ecc_len, uint32 *mem_used)
{
  struct nandc_predef_reg_read_buff *reg_read_buff;
  uint32 temp_addr = 0;
  uint32 read_buff_start = 0, read_buff_size = 0;
  enum flash_hal_status result = FLASH_HAL_SUCCESS;

  reg_read_buff = &nandc_predef_bam_data.reg_read_buff;
  temp_addr = buff_data->buff_vaddr;

    /* Buffer properties */
  reg_read_buff->base_virt_addr = buff_data->buff_vaddr;
  reg_read_buff->base_phys_addr = buff_data->buff_paddr;

    /*--------------- READ BUFFER ALLOCATION --------------------*/

  /* Align to 32 byte boundary */
  temp_addr = FLASH_ALIGN_32(temp_addr);  // 8 byte boundary sufficient?

  read_buff_start = temp_addr;

  /* Allocate memory for status register read.
     Needs 'cw_count' + 1 dwords. The
     extra one is used in write spare */
  reg_read_buff->status_buffer = temp_addr;
  temp_addr += sizeof(uint32) * (cw_count + 1);

  /* Allocate memory for erased cw detect status register read.
     Needs 'cw_count' dwords. We read
     a dword from this register for each codeword to check erased page */
  reg_read_buff->erased_cw_det_status_buffer = temp_addr;
  temp_addr += sizeof(uint32) * cw_count;

  /* Allocate memory for erased cw detect status register read.
     Needs 'cw_count' dwords. We read
     a dword from this register for each codeword to check erased page */
  reg_read_buff->read_id_buffer = temp_addr;
  temp_addr += sizeof(uint32);
 
  /* Read buffers for default APIs end here. We need to make sure that the
     write buffers doesn't start in read buffer's cache
     line. So align the buffer on a 32 byte memory
     to make sure that. This will help us doing
     cache maintanence optimally. */

  read_buff_size = temp_addr - read_buff_start;

  /* Add the read buffer for single block/page operations
     into memory region table */
  hal_nandc_set_memory_region(NANDC_BAM_REG_READ_BUFF_REGION,
    read_buff_start, GET_PHYS_ADDR(read_buff_start), read_buff_size);

  /* Total memory used */
  temp_addr = FLASH_ALIGN_32(temp_addr);

  reg_read_buff->spare_fill_buffer = temp_addr;
  temp_addr += FLASH_HAL_SPARE_UDATA_LEN_MAX;

  *mem_used = temp_addr - buff_data->buff_vaddr;

  if (*mem_used > buff_data->buff_size)
  {
    result = FLASH_HAL_NO_MEM_ERROR;
  }

  return result;
}

/*
 * Allocate memory for pre-allocated command blocks
 */
enum flash_hal_status hal_nandc_predef_cmd_blk_buff_alloc(
  struct flash_hal_mem_data *buff_data, uint32 cw_count, uint32 *mem_used)
{
  uint32 temp_addr = 0, write_buff_start = 0;
  struct nandc_predef_cmd_list *cmd_list;
  enum flash_hal_status result = FLASH_HAL_SUCCESS;

  /***********************************************************
   *                                                         *
   *  IMPORTANT NOTE: IF YOU ARE MAKING CHANGES TO           *
   *  NUMBER OF COMMAND LISTS ALLOCATED BELOW, PLEASE        *
   *  MAKE SURE THAT YOU ARE ALLOCATING ENOUGH               *
   *  BUFFERS WITH THE hal_nandc_predef_buffers_init() AND *
   *  TO INITIALIZE THE COMMAND LISTS CORRECTLY WITH THE     *
   *  hal_nandc_predef_cmd_list_init()                     *
   *                                                         *
   ***********************************************************/

  temp_addr = buff_data->buff_vaddr;
  /* Align the the buffer to a 32 byte boundary */
  temp_addr = FLASH_ALIGN_32(temp_addr);

  write_buff_start = temp_addr;

  cmd_list = &nandc_predef_bam_data.cmd_list;

  /* Allocate memory for configuration command list.
     Need only one command list with 6 commands.
     One for CFGs, one for ECC BCH CFG register, one for ECC BUF register,
	 one for chip select and one for erased CW detect CFG
	 (one to clear and one to set) */
  cmd_list->cfg_list = (struct flash_hal_op_cmd *) temp_addr;
  temp_addr += sizeof(struct flash_hal_op_cmd) * 6;

  /* Allocate memory for address command list.
     Needs 2 address commands to write to NAND_ADDR0 and NAND_ADDR1 
	 registers. */
  cmd_list->addr_list = (struct flash_hal_op_cmd *) temp_addr;
  temp_addr += sizeof(struct flash_hal_op_cmd) * 2;

  /* Allocate memory for status register read command list.
     This would read the NAND_FLASH_STATUS and NAND_ERASED_CW_DETECT_STATUS
	 registers. Need cw_count * 2 commands as we need to
     verify nand status and erased codeword status for every codewords in 
	 every page */
  cmd_list->status_list = (struct flash_hal_op_cmd *) temp_addr;
  temp_addr += sizeof(struct flash_hal_op_cmd) * (cw_count * 2);

  /* Allocate memory for copying the device id.
     Need 2 commands to read both the ID registers */
  cmd_list->copyid_list = (struct flash_hal_op_cmd *) temp_addr;
  temp_addr += sizeof(struct flash_hal_op_cmd) * 2;

  /* Allocate memory for command and execution command list.
     Need two commands.
       - Command register
       - Execution register */
  cmd_list->cmd_exec_list = (struct flash_hal_op_cmd *) temp_addr;
  temp_addr += sizeof(struct flash_hal_op_cmd) * 2;

  /* Allocate memory for miscellaneous register list.
     This can be used for any register read/write commands operation */
  cmd_list->misc_list = (struct flash_hal_op_cmd *) temp_addr;
  temp_addr += sizeof(struct flash_hal_op_cmd) * FLASH_BAM_PREDEF_MISC_MAX;

  /* Allocate memory for general purpose register list.
     This can be used to clear the nandc buffer */
  cmd_list->buff_clear_list = (struct flash_hal_op_cmd *) temp_addr;
  temp_addr += sizeof(struct flash_hal_op_cmd);

  /* Allocate memory for Clear Ctlr Status list.
     This is used to clear the controller status registers */
  cmd_list->clear_status_list = (struct flash_hal_op_cmd *) temp_addr;
  temp_addr += sizeof(struct flash_hal_op_cmd) * 2;

  /* Allocate memory for Clear Ctlr Status list.
     This is used to clear the controller status registers */
  cmd_list->locate_list = (struct flash_hal_op_cmd *) temp_addr;
  temp_addr += sizeof(struct flash_hal_op_cmd) * 2;

  /* Add the command list buffer for single page operations details
     into memory region table */
  hal_nandc_set_memory_region(NANDC_BAM_CMDBLOCK_BUFF_REGION,
    write_buff_start, GET_PHYS_ADDR(write_buff_start),
    (temp_addr - write_buff_start));

  /* Total memory used */
  temp_addr = FLASH_ALIGN_32(temp_addr);
  *mem_used = temp_addr - buff_data->buff_vaddr;

  if (*mem_used > buff_data->buff_size)
  {
    result = FLASH_HAL_NO_MEM_ERROR;
  }

  return result;
}

enum flash_hal_status hal_nandc_predef_desc_buff_alloc(
  struct flash_hal_mem_data *buff_data, uint32 cw_count, uint32 *mem_used)
{
  uint32 temp_addr = 0;
  struct nandc_predef_desc *nand_desc;
  enum flash_hal_status result = FLASH_HAL_SUCCESS;

//Note:  Add other descriptors
  /***********************************************************
   *                                                         *
   *  IMPORTANT NOTE: IF YOU ARE MAKING CHANGES TO           *
   *  NUMBER OF COMMAND LISTS ALLOCATED BELOW, PLEASE        *
   *  MAKE SURE THAT YOU ARE ALLOCATING ENOUGH               *
   *  BUFFERS WITH THE hal_nandc_predef_buffers_init() AND *
   *  TO INITIALIZE THE COMMAND LISTS CORRECTLY WITH THE     *
   *  hal_nandc_predef_cmd_list_init()                     *
   *                                                         *
   ***********************************************************/

  temp_addr = buff_data->buff_vaddr;
  /* Align the the buffer to a 32 byte boundary */
  temp_addr = FLASH_ALIGN_32(temp_addr);

  nand_desc = &nandc_predef_bam_data.desc;
  /* Allocate memory for read descriptor list.
     Need 'cw_count * 2' command lists, for
     main and spare for every codewords in every page */
  nand_desc->read_copy_desc = (struct flash_hal_op_desc *) temp_addr;
  temp_addr += sizeof(struct flash_hal_op_desc) * cw_count * 2;
  *mem_used = temp_addr - buff_data->buff_vaddr;

  /* Allocate memory for read descriptor list.
     Need 'cw_count * 2' command lists, for
     main and spare for every codewords in every page */
  nand_desc->write_copy_desc = (struct flash_hal_op_desc *) temp_addr;
  temp_addr += sizeof(struct flash_hal_op_desc) * cw_count * 2;
  *mem_used = temp_addr - buff_data->buff_vaddr;

  if (*mem_used > buff_data->buff_size)
  {
    result = FLASH_HAL_NO_MEM_ERROR;
  }

  return result;
}

/*
 * Initialize the data desc list for QPIC nandc
 * The command list is common to Read and Write Page operations.
 */
void hal_nandc_predef_desc_list_init(uint32 cw_count, uint32 ecc_len)
{
  uint32 i = 0;
  struct flash_hal_op_desc *read_copy_desc, *write_copy_desc;
  uint32 *spare_fill_buff = NULL;

  spare_fill_buff = 
    (uint32 *)nandc_predef_bam_data.reg_read_buff.spare_fill_buffer;

  read_copy_desc = &nandc_predef_bam_data.desc.read_copy_desc[0];
  write_copy_desc = &nandc_predef_bam_data.desc.write_copy_desc[0];

  // not required as we are setting it everytime in exec
  for(i = 0; i < cw_count * 2; i++)
  {
    read_copy_desc->buf_pa = NULL;
    read_copy_desc->buf_size = 0;
    read_copy_desc->flags = FLASH_BAM_DESC_FLAG_INT;
    write_copy_desc->buf_pa = NULL;
    write_copy_desc->buf_size = 0;
    write_copy_desc->flags = FLASH_BAM_DESC_FLAG_EOT;
    read_copy_desc++;
    write_copy_desc++;
  }

  for(i = 0; i < (FLASH_HAL_SPARE_UDATA_LEN_MAX >> 2); i++)
  {
    *spare_fill_buff++ = 0xFFFFFFFF;
  }
}

/*
 * Initialize the command list for qpic nandc
 *   The command list is common to all the flash operations.
 */
void hal_nandc_predef_cmd_list_init(uint32 cw_count, uint32 ecc_len)
{
  uint32 i = 0, status_reg_buf_pa, erased_cw_status_reg_buf_pa;
  struct flash_hal_op_cmd *op_cmd;
  struct nandc_predef_cmd_list *cmd_list;
  struct nandc_predef_reg_read_buff *reg_read_buff;

  cmd_list = &nandc_predef_bam_data.cmd_list;
  reg_read_buff = &nandc_predef_bam_data.reg_read_buff;

  /* Initialize config command list.*/
  op_cmd = &cmd_list->cfg_list[NANDC_BAM_CFG_CMD_LIST];

  op_cmd->command = FLASH_BAM_CE_WRITE_OP;
  op_cmd->data = 0;
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_DEV0_CFG0_PADDR;
  op_cmd->mask = FLASH_BAM_CE_DEFAULT_MASK;

  op_cmd++;
  op_cmd->command = FLASH_BAM_CE_WRITE_OP;
  op_cmd->data = 0;
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_DEVn_CFG1_PADDR(0);
  op_cmd->mask = FLASH_BAM_CE_DEFAULT_MASK;

    /* Initialize BCH ECC config command list.*/
  op_cmd = &cmd_list->cfg_list[NANDC_BAM_ECC_BCH_CFG_CMD_LIST];

  // test - to reset the ECC engine */
  op_cmd->command = FLASH_BAM_CE_WRITE_OP;
  op_cmd->data = 0;
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_DEV0_ECC_CFG_PADDR;
  op_cmd->mask = FLASH_BAM_CE_DEFAULT_MASK;

  op_cmd++;
  op_cmd->command = FLASH_BAM_CE_WRITE_OP;
  op_cmd->data = 0;
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_DEV0_ECC_CFG_PADDR;
  op_cmd->mask = FLASH_BAM_CE_DEFAULT_MASK;

  /* Initialize Erased CW detect config command list.*/
  op_cmd = &cmd_list->cfg_list[NANDC_BAM_E_CW_DET_CFG_CMD_LIST];

  /* Configure erased cw det cfg register */
  op_cmd->command = FLASH_BAM_CE_WRITE_OP;
  op_cmd->data = 3;  // Default Value
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_ERASED_CW_DETECT_CFG_PADDR;
  op_cmd->mask = FLASH_BAM_CE_DEFAULT_MASK;

  /* Configure erased cw det cfg register */
  op_cmd++;
  op_cmd->command = FLASH_BAM_CE_WRITE_OP;
  op_cmd->data = 2;  // Default Value
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_ERASED_CW_DETECT_CFG_PADDR;
  op_cmd->mask = FLASH_BAM_CE_DEFAULT_MASK;

  op_cmd++;

  /* Initialize Address command list.*/
  i = 0;
  op_cmd = &(cmd_list->addr_list[NANDC_BAM_ADDR0_CMD_LIST]);

  op_cmd->command = FLASH_BAM_CE_WRITE_OP;
  op_cmd->data = 0;
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_ADDR0_PADDR;
  op_cmd->mask = FLASH_BAM_CE_DEFAULT_MASK;
  op_cmd++;
  op_cmd->command = FLASH_BAM_CE_WRITE_OP;
  op_cmd->data = 0;
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_ADDR1_PADDR;
  op_cmd->mask = FLASH_BAM_CE_DEFAULT_MASK;
  op_cmd++;

  /* Initialize status command list.*/
  i = 0;
  op_cmd = &(cmd_list->status_list[NANDC_BAM_STATUS0_CMD_LIST]);
  erased_cw_status_reg_buf_pa = 
    GET_PHYS_ADDR(reg_read_buff->erased_cw_det_status_buffer);
  status_reg_buf_pa = GET_PHYS_ADDR(reg_read_buff->status_buffer);
	
  while (i < cw_count)
  {
    op_cmd->command = FLASH_BAM_CE_READ_OP;
    op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_STATUS_PADDR;
	
    op_cmd->data = status_reg_buf_pa;
    op_cmd->mask = FLASH_BAM_CE_DEFAULT_MASK;
    op_cmd++;
    status_reg_buf_pa += sizeof(uint32);
	
    op_cmd->command = FLASH_BAM_CE_READ_OP;
    op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_ERASED_CW_DETECT_STATUS_PADDR;
    op_cmd->data = erased_cw_status_reg_buf_pa;
    op_cmd->mask = FLASH_BAM_CE_DEFAULT_MASK;
    op_cmd++;
    erased_cw_status_reg_buf_pa += sizeof(uint32);
	
    i++;
  }

  /* Initialize Copy ID list */
  op_cmd = &(cmd_list->copyid_list[0]);

  op_cmd->command = FLASH_BAM_CE_READ_OP;
  op_cmd->data = 0;
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_READ_ID_PADDR;
  op_cmd->mask = FLASH_BAM_CE_DEFAULT_MASK;

  /* Initialize Command register commands */
  op_cmd = &(cmd_list->cmd_exec_list[NANDC_BAM_CMD_CMD_LIST]);

  op_cmd->command = FLASH_BAM_CE_WRITE_OP;
  op_cmd->data = 0;
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_CMD_PADDR;
  op_cmd->mask = FLASH_BAM_CE_DEFAULT_MASK;

  /* Initialize Exec register commands */
  op_cmd = &(cmd_list->cmd_exec_list[NANDC_BAM_EXEC_CMD_LIST]);

  op_cmd->command = FLASH_BAM_CE_WRITE_OP;
  op_cmd->data = 1; /* EXEC cmd */
  op_cmd->reg_addr = HAL_QPIC_NANDC_NANDC_EXEC_CMD_PADDR;
  op_cmd->mask = FLASH_BAM_CE_DEFAULT_MASK;

  //Check if required
  /* Miscellaneous command list */
  i = 0;
  op_cmd = &(cmd_list->misc_list[NANDC_BAM_MISC0_CMD_LIST]);

  while (i < FLASH_BAM_PREDEF_MISC_MAX)
  {
    op_cmd->command = FLASH_BAM_CE_WRITE_OP;
    op_cmd->data = 0;
    op_cmd->reg_addr = 0;
    op_cmd->mask = FLASH_BAM_CE_DEFAULT_MASK;

    op_cmd++;
    i++;
  }

  /* Status Clear command list */
  op_cmd = &(cmd_list->clear_status_list[NANDC_CLEAR_STATUS_CMD_LIST]);

  /* Read Status register clear commands */
  op_cmd->command = FLASH_BAM_CE_WRITE_OP;
  op_cmd->data = 0;
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_READ_STATUS_PADDR;
  op_cmd->mask = FLASH_BAM_CE_DEFAULT_MASK;

  /* Status register clear commands */
  op_cmd++;

  /* Status register clear commands */
  op_cmd->command = FLASH_BAM_CE_WRITE_OP;
  op_cmd->data = 0;
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_STATUS_PADDR;
  op_cmd->mask = FLASH_BAM_CE_DEFAULT_MASK;

  /* Config Locate Register for Data Read */
  op_cmd = &(cmd_list->locate_list[0]);

  /* Config Locate Register */
  op_cmd->command = FLASH_BAM_CE_WRITE_OP;
  op_cmd->data = 0x82040000;  // Default value
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_READ_LOCATION_PADDR;
  op_cmd->mask = FLASH_BAM_CE_DEFAULT_MASK;
  op_cmd++;

  /* Config Locate Register - last CW */
  op_cmd->command = FLASH_BAM_CE_WRITE_OP;
  op_cmd->data = 0x82040000;  // Default value
  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_READ_LOCATION_PADDR;
  op_cmd->mask = FLASH_BAM_CE_DEFAULT_MASK;
}

/* Probe.
 *
 * Note: Please make sure that you pass enough memory to this
 *       API. We are determining the memory used only at the
 *       end of this function. This may lead to memory corruption.
 */
enum flash_hal_status hal_nandc_bam_FF_90_init(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *mem_data, uint32 *used_buff_size)
{
  int32  buff_vaddr = 0;
  uint32 misc_count = 0;
  uint32 buff_paddr = 0;
  struct flash_hal_op_desc  *cmd_desc_vaddr_ptr;
  struct nandc_predef_cmd_list *cmd_list;
  struct nandc_predef_cmdptr_mem_data *cmdptr_mem_data;
  struct flash_hal_predef_op_list *op_list;

  cmd_list = &nandc_predef_bam_data.cmd_list;
  cmdptr_mem_data =
    &nandc_predef_bam_data.mem_info.cmd_ptr[NANDC_FF_90_PROBE];
  op_list = &nandc_predef_bam_data.cmd_ptr_list[NANDC_FF_90_PROBE];

  /* Flash HAL creates a list which is BAM friendly. ie,
     All the buffers and addresses in the pre-allocated list
     will be with physical addresses. And all the command
     pointer list and command list are aligned to 32 byte
     boundary. Please note that the command pointer list
     is virtual. */

  /* Get a 32 byte aligned buffer for the command pointer list */
  buff_paddr = mem_data->buff_paddr;
  buff_paddr = FLASH_ALIGN_32(buff_paddr);
  buff_vaddr = mem_data->buff_vaddr;
  buff_vaddr = FLASH_ALIGN_32(buff_vaddr);

  cmdptr_mem_data->mem_data.buff_vaddr = buff_vaddr;
  cmdptr_mem_data->mem_data.buff_paddr = buff_paddr;
  cmdptr_mem_data->seq_id = NANDC_FF_90_PROBE;

  cmd_desc_vaddr_ptr = (struct flash_hal_op_desc *)buff_vaddr;

  /*--------------------------------------------------------------
   |  The command pointer list for read is have the following
   |  format,
   |
   |  - OCB
   |  - Config
   |  - Issue reset command
   |  - Wait for status
   |  - Issue read ID command
   |  - Status
   |  - Read the ID
   |  - OCU
   *-------------------------------------------------------------*/

  /* Lock the Pipe and Set config registers */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->cfg_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 6; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_LCK;
  cmd_desc_vaddr_ptr++;

  /* Set address registers */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->addr_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 2; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
  cmd_desc_vaddr_ptr++;

  /* Set command registers */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->cmd_exec_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 2; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_NWD;
  cmd_desc_vaddr_ptr++;

  /* Read Status register */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->status_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd); 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
  cmd_desc_vaddr_ptr++;

  /* Change the command register */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->misc_list[misc_count++]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd); 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
  cmd_desc_vaddr_ptr++;

  /* Exec register */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->misc_list[misc_count++]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd); 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_NWD;
  cmd_desc_vaddr_ptr++;

  /* Read Status register */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->status_list[2]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd); 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
  cmd_desc_vaddr_ptr++;
  
  /* Copy ID from NANDC ID register to buffer and Unlock Pipe */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->copyid_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd); 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | 
    FLASH_BAM_DESC_FLAG_UNLCK | FLASH_BAM_DESC_FLAG_INT;
  cmd_desc_vaddr_ptr++;

  /* Get the memory use and store it */
  *used_buff_size = (uint32)cmd_desc_vaddr_ptr - mem_data->buff_vaddr;
  cmdptr_mem_data->mem_data.buff_size = *used_buff_size;

  /* Initialize the BAM list for read */

  op_list->op_list = (uint32 *) cmdptr_mem_data->mem_data.buff_vaddr;
  op_list->op_list_count = ((uint32)cmd_desc_vaddr_ptr - buff_vaddr) / 
    sizeof(struct flash_hal_op_desc);
  op_list->seq_id = NANDC_FF_90_PROBE;
  op_list->prev_op_count = 1;

  /* If memory used is more than given, return failure. */
  return ((*used_buff_size > mem_data->buff_size) ?
    FLASH_HAL_NO_MEM_ERROR : FLASH_HAL_SUCCESS);
}

/* ONFI ID Read.
 *
 * Note: Please make sure that you pass enough memory to this
 *       API. We are determining the memory used only at the
 *       end of this function. This may lead to memory corruption.
 */
enum flash_hal_status hal_nandc_bam_90_onfi_id_init(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *mem_data, uint32 *used_buff_size)
{
  int32  buff_vaddr = 0;
  uint32 buff_paddr = 0;
  struct nandc_predef_cmd_list *cmd_list;
  struct nandc_predef_cmdptr_mem_data *cmdptr_mem_data;
  struct flash_hal_predef_op_list *op_list;
  struct flash_hal_op_desc  *cmd_desc_vaddr_ptr;

  cmd_list = &nandc_predef_bam_data.cmd_list;
  cmdptr_mem_data =
    &nandc_predef_bam_data.mem_info.cmd_ptr[NANDC_90_ONFI_ID];
  op_list = &nandc_predef_bam_data.cmd_ptr_list[NANDC_90_ONFI_ID];

  /* Flash HAL creates a list which is BAM friendly. ie,
     All the buffers and addresses in the pre-allocated list
     will be with physical addresses. And all the command
     pointer list and command list are aligned to 32 byte
     boundary. Please note that the command pointer list
     is virtual. */

  /* Get a 32 byte aligned buffer for the command pointer list */

  buff_paddr = mem_data->buff_paddr;
  buff_paddr = FLASH_ALIGN_32(buff_paddr);

  buff_vaddr = mem_data->buff_vaddr;
  buff_vaddr = FLASH_ALIGN_32(buff_vaddr);

  cmdptr_mem_data->mem_data.buff_vaddr = buff_vaddr;
  cmdptr_mem_data->mem_data.buff_paddr = buff_paddr;
  cmdptr_mem_data->seq_id = NANDC_90_ONFI_ID;

  cmd_desc_vaddr_ptr = (struct flash_hal_op_desc *)buff_vaddr;

  /*--------------------------------------------------------------
   |  The command pointer list for read is have the following
   |  format,
   |
   |  - OCB
   |  - Config
   |  - Issue reset command
   |  - Wait for status
   |  - Issue read ID command
   |  - Status
   |  - Read the ID
   |  - OCU
   *-------------------------------------------------------------*/

  /* Lock Pipe and Set config registers */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->cfg_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 6; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_LCK;
  cmd_desc_vaddr_ptr++;

  /* Set address registers */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->addr_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 2; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
  cmd_desc_vaddr_ptr++;

  /* Set command registers */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->cmd_exec_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 2; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_NWD;
  cmd_desc_vaddr_ptr++;

    /* Read Status register */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->status_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd); 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
  cmd_desc_vaddr_ptr++;

  /* Copy ID from NANDC ID register to buffer and Unlock Pipe */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->copyid_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd); 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | 
    FLASH_BAM_DESC_FLAG_UNLCK | FLASH_BAM_DESC_FLAG_INT;
  cmd_desc_vaddr_ptr++;

  /* Get the memory use and store it */
  *used_buff_size = (uint32)cmd_desc_vaddr_ptr - mem_data->buff_vaddr;
  cmdptr_mem_data->mem_data.buff_size = *used_buff_size;

  /* Initialize the BAM list for read */
  op_list->op_list = (uint32 *) cmdptr_mem_data->mem_data.buff_vaddr;
  op_list->op_list_count = ((uint32)cmd_desc_vaddr_ptr - buff_vaddr) /
    sizeof(struct flash_hal_op_desc);
  op_list->seq_id = NANDC_90_ONFI_ID;
  op_list->prev_op_count = 1;

  /* If memory used is more than given, return failure. */
  return ((*used_buff_size > mem_data->buff_size) ?
    FLASH_HAL_NO_MEM_ERROR : FLASH_HAL_SUCCESS);
}

/* Parameter Page read
 *
 * Note: Please make sure that you pass enough memory to this
 *       API. We are determining the memory used only at the
 *       end of this function. This may lead to memory corruption.
 */
enum flash_hal_status hal_nandc_bam_00_EC_init(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *mem_data, uint32 *used_buff_size)
{
  uint32 buff_paddr = 0;
  int32  cw_count = 0, tmp_cw_count = 0, buff_vaddr = 0;
  struct nandc_predef_cmd_list *cmd_list;
  struct nandc_predef_cmdptr_mem_data *cmdptr_mem_data;
  struct flash_hal_predef_op_list *op_list;
  struct flash_hal_op_desc  *cmd_desc_vaddr_ptr;

  cmd_list = &nandc_predef_bam_data.cmd_list;
  cmdptr_mem_data =
    &nandc_predef_bam_data.mem_info.cmd_ptr[NANDC_00_EC_READ];
  op_list = &nandc_predef_bam_data.cmd_ptr_list[NANDC_00_EC_READ];
  cw_count = priv_data->page_size_bytes / FLASH_NAND_UD_PER_CW;

  /* Flash HAL creates a list which is BAM friendly. ie,
     All the buffers and addresses in the pre-allocated list
     will be with physical addresses. And all the command
     pointer list and command list are aligned to 32 byte
     boundary. Please note that the command pointer list
     is virtual. */

  /* Get a 32 byte aligned buffer for the command pointer list */

  buff_paddr = mem_data->buff_paddr;
  buff_paddr = FLASH_ALIGN_32(buff_paddr);

  buff_vaddr = mem_data->buff_vaddr;
  buff_vaddr = FLASH_ALIGN_32(buff_vaddr);

  cmdptr_mem_data->mem_data.buff_vaddr = buff_vaddr;
  cmdptr_mem_data->mem_data.buff_paddr = buff_paddr;
  cmdptr_mem_data->seq_id = NANDC_00_EC_READ;

  cmd_desc_vaddr_ptr = (struct flash_hal_op_desc *)buff_vaddr;

  /*--------------------------------------------------------------
   |  The command pointer list for read is have the following
   |  format,
   |
   |  - config (cfg0/cfg1/ecc buf/cs) + Lock Pipe
   |  - Address
   |  - Change read command
   |  - Exec
   |  - Status
   |  - Copy the codeword
   |  - Restore original command + Unlock Pipe
   *-------------------------------------------------------------*/

  /* Set config registers and lock pipe*/
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->cfg_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 6; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_LCK;
  cmd_desc_vaddr_ptr++;

  /* Set address registers */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->addr_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 2; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
  cmd_desc_vaddr_ptr++;

  /* Configure Locate Register for Data Read */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->locate_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd); 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
  cmd_desc_vaddr_ptr++;

  while (tmp_cw_count < cw_count)
  {
    /* Set command - exec registers */
    cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->cmd_exec_list[0]);
    cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 2; 
    cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_NWD;
    cmd_desc_vaddr_ptr++;

    /* Read Status register */
    cmd_desc_vaddr_ptr->buf_pa = 
      GET_PHYS_ADDR(&cmd_list->status_list[tmp_cw_count * 2]);
    cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 2; 
    cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;

    if (tmp_cw_count == (cw_count - 1))
    {
      cmd_desc_vaddr_ptr->flags = 
        FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_UNLCK | 
        FLASH_BAM_DESC_FLAG_INT;
    }
    else 
    {
      cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
    }
    cmd_desc_vaddr_ptr++;
    tmp_cw_count++;
  }

  /* Get the memory use and store it */
  *used_buff_size = (uint32)cmd_desc_vaddr_ptr - mem_data->buff_vaddr;
  cmdptr_mem_data->mem_data.buff_size = *used_buff_size;

  /* Initialize the BAM list for read */
  op_list->op_list = (uint32 *) cmdptr_mem_data->mem_data.buff_vaddr;
  op_list->op_list_count = ((uint32)cmd_desc_vaddr_ptr - buff_vaddr) / 
    sizeof(struct flash_hal_op_desc);
  op_list->seq_id = NANDC_00_EC_READ;
  op_list->prev_op_count = 1;

  /* If memory used is more than given, return failure. */
  return ((*used_buff_size > mem_data->buff_size) ?
    FLASH_HAL_NO_MEM_ERROR : FLASH_HAL_SUCCESS);
}

/* Single read.
 *
 * Note: Please make sure that you pass enough memory to this
 *       API. We are determining the memory used only at the
 *       end of this function. This may lead to memory corruption.
 */
enum flash_hal_status hal_nandc_bam_00_30_x1_init(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *mem_data, uint32 *used_buff_size)
{
  int32  cw_count = 0, tmp_cw_count = 0, buff_vaddr = 0;
  uint32 buff_paddr = 0;
  struct nandc_predef_cmd_list *cmd_list;
  struct nandc_predef_cmdptr_mem_data *cmdptr_mem_data;
  struct flash_hal_predef_op_list *op_list;
  struct flash_hal_op_desc  *cmd_desc_vaddr_ptr;

  cmd_list = &nandc_predef_bam_data.cmd_list;
  cmdptr_mem_data =
    &nandc_predef_bam_data.mem_info.cmd_ptr[NANDC_00_30_x1_READ];
  op_list = &nandc_predef_bam_data.cmd_ptr_list[NANDC_00_30_x1_READ];
  cw_count = priv_data->page_size_bytes / FLASH_NAND_UD_PER_CW;

  /* Flash HAL creates a list which is BAM friendly. ie,
     All the buffers and addresses in the pre-allocated list
     will be with physical addresses. And all the command
     pointer list and command list are aligned to 32 byte
     boundary. Please note that the command pointer list
     is virtual. */

  /* Get a 32 byte aligned buffer for the command pointer list */

  buff_paddr = mem_data->buff_paddr;
  buff_paddr = FLASH_ALIGN_32(buff_paddr);

  buff_vaddr = mem_data->buff_vaddr;
  buff_vaddr = FLASH_ALIGN_32(buff_vaddr);

  cmdptr_mem_data->mem_data.buff_vaddr = buff_vaddr;
  cmdptr_mem_data->mem_data.buff_paddr = buff_paddr;
  cmdptr_mem_data->seq_id = NANDC_00_30_x1_READ;

  cmd_desc_vaddr_ptr = (struct flash_hal_op_desc *)buff_vaddr;

  /*--------------------------------------------------------------
   |  The command pointer list for read is have the following
   |  format,
   |
   |  - config (cfg0/cfg1/ecc buf/cs/erased cw det cfg) + Lock Pipe
   |  - Address
   |  - Reset command register
   |
   |    for each codeword,
   |      - Command Register
   |      - Exec
   |      - Status
   |      - Copy Main Bytes
   |      - Copy Spare Bytes
   |      - Copy ECC Bytes and Erased CW det status bytes
   |
   |  - OCU
   *-------------------------------------------------------------*/

  /* Lock Pipe and Set config registers */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->cfg_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 6; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_LCK;
  cmd_desc_vaddr_ptr++;

  /* Set address registers */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->addr_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 2; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
  cmd_desc_vaddr_ptr++;

  
  /* Configure Locate Register for Data Read */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->locate_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd); 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
  cmd_desc_vaddr_ptr++;

  while (tmp_cw_count < cw_count)
  {
    if (tmp_cw_count == (cw_count - 1))
    {
      /* Configure Locate Register for Data Read */
      cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->locate_list[1]);
      cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd); 
      cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
      cmd_desc_vaddr_ptr++;
    }
    
    /* Set command - exec registers */
    cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->cmd_exec_list[0]);
    cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 2; 
    cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_NWD;
    cmd_desc_vaddr_ptr++;

    /* Read Status register */
    cmd_desc_vaddr_ptr->buf_pa = 
      GET_PHYS_ADDR(&cmd_list->status_list[tmp_cw_count * 2]);
    cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 2; 
    cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;

    if (tmp_cw_count == (cw_count - 1))
    {
      cmd_desc_vaddr_ptr->flags = 
	    FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_UNLCK | 
        FLASH_BAM_DESC_FLAG_INT;
	}
	else 
    {
      cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
	}
    cmd_desc_vaddr_ptr++;

    tmp_cw_count++;
  }

  /* Get the memory use and store it */
  *used_buff_size = (uint32)cmd_desc_vaddr_ptr - mem_data->buff_vaddr;
  cmdptr_mem_data->mem_data.buff_size = *used_buff_size;

  /* Initialize the BAM list for read */
  op_list->op_list = (uint32 *) cmdptr_mem_data->mem_data.buff_vaddr;
  op_list->op_list_count = ((uint32)cmd_desc_vaddr_ptr - buff_vaddr) /
    sizeof(struct flash_hal_op_desc);
  op_list->seq_id = NANDC_00_30_x1_READ;
  op_list->prev_op_count = 1;

  /* If memory used is more than given, return failure. */
  return ((*used_buff_size > mem_data->buff_size) ?
    FLASH_HAL_NO_MEM_ERROR : FLASH_HAL_SUCCESS);
}

/* Single write
 *
 * Note: Please make sure that you pass enough memory to this
 *       API. We are determining the memory used only at the
 *       end of this function. This may lead to memory corruption.
*/
enum flash_hal_status hal_nandc_bam_80_10_x1_init(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *mem_data, uint32 *used_buff_size)
{
  int32  cw_count = 0, tmp_cw_count = 0, buff_paddr = 0;
  uint32 buff_vaddr = 0;
  struct nandc_predef_cmd_list *cmd_list;
  struct nandc_predef_cmdptr_mem_data *cmdptr_mem_data;
  struct flash_hal_predef_op_list *op_list;
  struct flash_hal_op_desc  *cmd_desc_vaddr_ptr;
    
  cmd_list = &nandc_predef_bam_data.cmd_list;
  cmdptr_mem_data =
    &nandc_predef_bam_data.mem_info.cmd_ptr[NANDC_80_10_x1_WRITE];
  op_list = &nandc_predef_bam_data.cmd_ptr_list[NANDC_80_10_x1_WRITE];
  cw_count = priv_data->page_size_bytes / FLASH_NAND_UD_PER_CW;

  /* Flash HAL creates a list which is BAM friendly. ie,
     All the buffers and addresses in the pre-allocated list
     will be with physical addresses. And all the command
     pointer list and command list are aligned to 32 byte
     boundary. Please note that the command pointer list
     is virtual. */

  /* Get a 32 byte aligned buffer for the command pointer list */

  buff_paddr = mem_data->buff_paddr;
  buff_paddr = FLASH_ALIGN_32(buff_paddr);

  buff_vaddr = mem_data->buff_vaddr;
  buff_vaddr = FLASH_ALIGN_32(buff_vaddr);

  cmdptr_mem_data->mem_data.buff_vaddr = buff_vaddr;
  cmdptr_mem_data->mem_data.buff_paddr = buff_paddr;
  cmdptr_mem_data->seq_id = NANDC_80_10_x1_WRITE;

  cmd_desc_vaddr_ptr = (struct flash_hal_op_desc *) buff_vaddr;

  /*--------------------------------------------------------------
   |  The command pointer list for write is have the following
   |  format,
   |
   |  - config (cfg0/cfg1/ecc buf/cs)
   |  - Address
   |
   |    for each codeword,
   |      - Clear the spare area with 0xFF
   |      - Copy Main Bytes
   |      - Copy Spare Bytes
   |      - Command Register
   |      - Exec
   |      - Read Status
   |      - Clear Status
   *-------------------------------------------------------------*/

  /* Lock Pipe Set config registers */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->cfg_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 4; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_LCK;
  cmd_desc_vaddr_ptr++;

  /* Set address registers */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->addr_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 2; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
  cmd_desc_vaddr_ptr++;


  while (tmp_cw_count < cw_count)
  {
    /* Set command - exec registers */
    cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->cmd_exec_list[0]);
    cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 2; 
    cmd_desc_vaddr_ptr->flags =
      FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_NWD;
    cmd_desc_vaddr_ptr++;

    /* Read Status register */
    cmd_desc_vaddr_ptr->buf_pa =
      GET_PHYS_ADDR(&cmd_list->status_list[tmp_cw_count * 2]);
    cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd); 
    cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
    cmd_desc_vaddr_ptr++;

    tmp_cw_count++;
  }

  /* Clear Status register and unlock pipe */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->clear_status_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 2; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | 
    FLASH_BAM_DESC_FLAG_UNLCK | FLASH_BAM_DESC_FLAG_INT;
  cmd_desc_vaddr_ptr++;

  /* Get the memory use and store it */
  *used_buff_size = (uint32)cmd_desc_vaddr_ptr - mem_data->buff_vaddr;
  cmdptr_mem_data->mem_data.buff_size = *used_buff_size;

  /* Initialize the bam list for read */
  op_list->op_list = (uint32 *) cmdptr_mem_data->mem_data.buff_vaddr;
  op_list->op_list_count = ((uint32)cmd_desc_vaddr_ptr - buff_vaddr) /
    sizeof(struct flash_hal_op_desc);
  op_list->seq_id = NANDC_80_10_x1_WRITE;
  op_list->prev_op_count = 1;

  /* If memory used is more than given, return failure. */
  return ((*used_buff_size > mem_data->buff_size) ?
    FLASH_HAL_NO_MEM_ERROR : FLASH_HAL_SUCCESS);
}

/* Single erase
 *
 * Note: Please make sure that you pass enough memory to this
 *       API. We are determining the memory used only at the
 *       end of this function. This may lead to memory corruption.
*/
enum flash_hal_status hal_nandc_bam_60_D0_init(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *mem_data, uint32 *used_buff_size)
{
  uint32 buff_vaddr = 0, buff_paddr = 0;
  struct nandc_predef_cmd_list *cmd_list;
  struct nandc_predef_cmdptr_mem_data *cmdptr_mem_data;
  struct flash_hal_predef_op_list *op_list;
  struct flash_hal_op_desc  *cmd_desc_vaddr_ptr;

  cmd_list = &nandc_predef_bam_data.cmd_list;
  cmdptr_mem_data =
    &nandc_predef_bam_data.mem_info.cmd_ptr[NANDC_60_D0_ERASE];
  op_list = &nandc_predef_bam_data.cmd_ptr_list[NANDC_60_D0_ERASE];

  /* Flash HAL creates a list which is BAM friendly. ie,
     All the buffers and addresses in the pre-allocated list
     will be with physical addresses. And all the command
     pointer list and command list are aligned to 32 byte
     boundary. Please note that the command pointer list
     is virtual. */

  /* Get a 32 byte aligned buffer for the command pointer list */

  buff_paddr = mem_data->buff_paddr;
  buff_paddr = FLASH_ALIGN_32(buff_paddr);

  buff_vaddr = mem_data->buff_vaddr;
  buff_vaddr = FLASH_ALIGN_32(buff_vaddr);

  cmdptr_mem_data->mem_data.buff_vaddr = buff_vaddr;
  cmdptr_mem_data->mem_data.buff_paddr = buff_paddr;
  cmdptr_mem_data->seq_id = NANDC_60_D0_ERASE;

  cmd_desc_vaddr_ptr = (struct flash_hal_op_desc *) buff_vaddr;

  /*--------------------------------------------------------------
   |  The command pointer list for write is have the following
   |  format,
   |
   |  - config (cfg0/cfg1/ecc buf/cs)
   |  - Address
   |  - Command Register
   |  - Exec
   |  - Read Status
   |  - Clear Status
   |
   *-------------------------------------------------------------*/
  /* Set config registers */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->cfg_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 4; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_LCK;
  cmd_desc_vaddr_ptr++;

  /* Set address registers */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->addr_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 2; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
  cmd_desc_vaddr_ptr++;

  /* Set command - exec registers */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->cmd_exec_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 2; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_NWD;
  cmd_desc_vaddr_ptr++;

  /* Read Status register */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->status_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd); 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
  cmd_desc_vaddr_ptr++;

  /* Clear Status register and  Unlock Pipe */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->clear_status_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 2; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD |
    FLASH_BAM_DESC_FLAG_UNLCK | FLASH_BAM_DESC_FLAG_INT;
  cmd_desc_vaddr_ptr++;

  /* Get the memory use and store it */
  *used_buff_size = (uint32)cmd_desc_vaddr_ptr - mem_data->buff_vaddr;
  cmdptr_mem_data->mem_data.buff_size = *used_buff_size;

  /* Initialize the bam list for read */
  op_list->op_list = (uint32 *) cmdptr_mem_data->mem_data.buff_vaddr;
  op_list->op_list_count = ((uint32)cmd_desc_vaddr_ptr - buff_vaddr) /
    sizeof(struct flash_hal_op_desc);
  op_list->seq_id = NANDC_60_D0_ERASE;
  op_list->prev_op_count = 1;

  /* If memory used is more than given, return failure. */
  return ((*used_buff_size > mem_data->buff_size) ?
    FLASH_HAL_NO_MEM_ERROR : FLASH_HAL_SUCCESS);
}

/* Single read bb marker
 *
 * Note: Please make sure that you pass enough memory to this
 *       API. We are determining the memory used only at the
 *       end of this function. This may lead to memory corruption.
*/
enum flash_hal_status hal_nandc_bam_00_30_bb_marker_init(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *mem_data, uint32 *used_buff_size)
{
  uint32 buff_vaddr = 0, buff_paddr = 0;
  struct nandc_predef_cmd_list *cmd_list;
  struct nandc_predef_cmdptr_mem_data *cmdptr_mem_data;
  struct flash_hal_predef_op_list *op_list;
  struct flash_hal_op_desc  *cmd_desc_vaddr_ptr;

  cmd_list = &nandc_predef_bam_data.cmd_list;
  cmdptr_mem_data =
    &nandc_predef_bam_data.mem_info.cmd_ptr[NANDC_00_30_BB_READ];
  op_list = &nandc_predef_bam_data.cmd_ptr_list[NANDC_00_30_BB_READ];

  /* Flash HAL creates a list which is BAM friendly. ie,
     All the buffers and addresses in the pre-allocated list
     will be with physical addresses. And all the command
     pointer list and command list are aligned to 32 byte
     boundary. Please note that the command pointer list
     is virtual. */

  /* Get a 32 byte aligned buffer for the command pointer list */
  buff_paddr = mem_data->buff_paddr;
  buff_paddr = FLASH_ALIGN_32(buff_paddr);

  buff_vaddr = mem_data->buff_vaddr;
  buff_vaddr = FLASH_ALIGN_32(buff_vaddr);

  cmdptr_mem_data->mem_data.buff_vaddr = buff_vaddr;
  cmdptr_mem_data->mem_data.buff_paddr = buff_paddr;
  cmdptr_mem_data->seq_id = NANDC_00_30_BB_READ;

  cmd_desc_vaddr_ptr = (struct flash_hal_op_desc *) buff_vaddr;

  /*--------------------------------------------------------------
   | Reading bad block marker byte has the below steps
   |
   |  - ECC buffer config
   |  - Config
   |  - Address
   |  - Command Register
   |  - Exec
   |  - Status
   |  - Copy Spare Bytes
   *-------------------------------------------------------------*/

  /* Lock Pipe and Set config registers */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->cfg_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 6; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_LCK;
  cmd_desc_vaddr_ptr++;

  /* Set address registers */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->addr_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 2; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
  cmd_desc_vaddr_ptr++;

  /* Configure Locate Register for Data Read */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->locate_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd); 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD;
  cmd_desc_vaddr_ptr++;

  /* Set command registers */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->cmd_exec_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd) * 2; 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD | FLASH_BAM_DESC_FLAG_NWD;
  cmd_desc_vaddr_ptr++;

  /* Read Status register and unlock pipe */
  cmd_desc_vaddr_ptr->buf_pa = GET_PHYS_ADDR(&cmd_list->status_list[0]);
  cmd_desc_vaddr_ptr->buf_size = sizeof(struct flash_hal_op_cmd); 
  cmd_desc_vaddr_ptr->flags = FLASH_BAM_DESC_FLAG_CMD |
    FLASH_BAM_DESC_FLAG_UNLCK | FLASH_BAM_DESC_FLAG_INT;
  cmd_desc_vaddr_ptr++;

  /* Get the memory use and store it */
  *used_buff_size = (uint32)cmd_desc_vaddr_ptr - mem_data->buff_vaddr;
  cmdptr_mem_data->mem_data.buff_size = *used_buff_size;

  /* Initialize the bam list for read */
  op_list->op_list = (uint32 *) cmdptr_mem_data->mem_data.buff_vaddr;
  op_list->op_list_count = ((uint32)cmd_desc_vaddr_ptr - buff_vaddr) /
    sizeof(struct flash_hal_op_desc);
  op_list->seq_id = NANDC_00_30_BB_READ;
  op_list->prev_op_count = 1;

  /* If memory used is more than given, return failure. */
  return ((*used_buff_size > mem_data->buff_size) ?
    FLASH_HAL_NO_MEM_ERROR : FLASH_HAL_SUCCESS);
}

/*
 * De-Initializes a pre-allocated sequence
 */
void hal_nandc_predef_bam_deinit(flash_hal_dev_info *dev_info)
{
}
