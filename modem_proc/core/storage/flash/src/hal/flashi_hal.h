#ifndef FLASHI_HAL_H
#define FLASHI_HAL_H

/*=======================================================================
 * FILE:        flashi_hal.h
 *
 * SERVICES:    Internal header file for flash HAL
 *
 * DESCRIPTION: Internal header file for flash HAL
 *
 * Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/hal/flashi_hal.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/05/12     sv      cleanup and optimization
 * 07/27/12     sv      Remove 512 byte page NAND related codes
 * 12/14/11     eo      Add optimized transfer steps values
 * 06/22/11     eo      Add erased cw detect cfg support
 * 05/20/11     bb      Flash hal optimization
 * 05/18/11     jz      Remove hard-coded address cycles and clock cycle gap
 * 02/15/11     sb      Adding addr_offset & ecc_check_size to flash_hal_ctlr_data
 * 01/13/11     sb      Adding 8bit ECC support
 * 10/21/10     sv      Add flash_hal_busywait API
 * 07/30/10     jz      Added 2X support and flash_hal_set_ctlr_mode()
 * 07/30/10     eo      Add write and erase operation error codes
 * 02/05/10     mcg     Support to get DMOV configs from DAL
 * 01/10/10     mm      Support for real two plane program
 * 09/25/09     mh      Add support for dual nand ctlrs
 * 08/11/09     bb      Support for HAL optimized copy-multi r/w API for NAND
 * 06/26/09     rk      512B page layout type updated
 * 07/27/09     jz      Fixed compiler warning
 * 07/29/09     mh      Add ctlr_info to flash_hal_ctlr_data
 * 07/21/09     sv      Add flash_hal_ api support for memory operations
 * 06/14/09     eo      Support for OneNAND HAL optimizations
 * 05/29/09     mh      Add nand_cfg and dev_id fields to flash_hal_ctlr_data
 * 04/01/09     mh      Add 4K page layout info
 * 03/12/09     eo      Add updates to post process list function
 * 03/30/08     BB      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_hal.h"
#include "flashi_hal_config.h"

/*-----------------------------------------------------------------------
 *           Definitions and Constants
 *----------------------------------------------------------------------*/

/* Macro to determine number of elements in an array */
#define NUM_ARRAY_ELEMENTS(array, type)  (sizeof(array)/sizeof(type))

/* Aligns an address to 32 byte */
#define FLASH_ALIGN_32(a) ((a)&0x1F)?((((a)+32)>>5)<<5):(a)

/* HAL Flash Error definitions */
#define FLASH_HAL_NO_ERROR                0x00000000  /* No Error reported   */
#define FLASH_HAL_OPLIST_BUFFER_OVERFLOW  0x00000001  /* Operation list
                                                         buffer overflow     */
#define FLASH_HAL_WS_BUFFER_OVERFLOW      0x00000002  /* Work space buffer
                                                         overflow            */
#define FLASH_HAL_PP_BUFFER_OVERFLOW      0x00000004  /* Post process list
                                                         buffer overflow     */
#define FLASH_HAL_OP_TIMEOUT              0x00000008  /* Operation timeout   */
#define FLASH_HAL_DEVICE_NOT_SUPPORTED    0x00000010  /* Device not supported*/
#define FLASH_HAL_UNKNOWN_DEVICE_ID       0x00000020  /* Uknown device id    */
#define FLASH_HAL_DEVICE_ID_OVERFLOW      0x00000040  /* Device id overflow  */
#define FLASH_HAL_OP_NOT_SUPPORTED_ERROR  0x00000080  /* Operation not
                                                         supported           */
#define FLASH_HAL_NON_ALIGNED_OP          0x00000100  /* Operation not
                                                         aligned             */
#define FLASH_HAL_PP_VERIFY_ERROR         0x00000200  /* Post process verify
                                                         error               */
#define FLASH_HAL_INVALID_ADDR_ERROR      0x00000400  /* Number of address
                                                         bytes exceeded max  */
#define FLASH_HAL_ERASED_PAGE_VALID       0x00000800  /* Erased page check
                                                         valid               */
#define FLASH_HAL_OP_FAILED               0x00001000  /* Flash operation
                                                         failed              */
#define FLASH_HAL_NO_BUFFER_ERROR         0x00002000  /* Error with buffer   */

#define FLASH_HAL_MPU_ERROR               0x00004000  /* Error with buffer   */

#define FLASH_HAL_WRITE_OP_ERROR          0x00008000  /* Error in write
                                                         operation error     */
#define FLASH_HAL_ERASE_OP_ERROR          0x00010000  /* Error in erase
                                                         operation           */
#define FLASH_HAL_UNKNOWN_ERROR           0x80000000  /* Unknown error       */

#define FLASH_HAL_NO_DEVID                0xFFFFFFFF  /* Invalid Device ID */

/*-----------------------------------------------------------------------
 *           Type Declarations
 *----------------------------------------------------------------------*/

/* HAL operation flags */
enum flash_hal_op_flag
{
  FLASH_HAL_NO_OP_FLAG = 0x0000, /* No operation */
  FLASH_HAL_LAST_CMD   = 0x0001, /* Last command */
  FLASH_HAL_LAST_SEQ   = 0x0002, /* Last command in sequence */
  FLASH_HAL_LAST_OP    = 0x1000  /* Last command in an operation */

};

/* Ctlr number */
enum flash_hal_ctlr_id
{
  NAND_CTLR0,              /* ID for First Ctlr (default) */
  NAND_CTLR1,              /* ID for Second Ctlr  */
  NAND_CTLR0_CTLR1         /* ID to addres Ctlr0 and Ctlr1  */
};

/* Data direction during an operation */
enum flash_hal_data_dir
{
  FLASH_HAL_DATA_NONE,              /* No data */
  FLASH_HAL_DATA_INTO_DEVICE,       /* Write to device */
  FLASH_HAL_DATA_OUT_FROM_DEVICE    /* Read from device */
};

/* HAL Page layout IDs
 *
 * - FLASH_HAL_LAYOUT_2K_516_0_X3_500_16_X1
 *     2K Page layout,
 *        516 main data + 0 spare bytes - Three 528 byte units
 *        500 main data + 16 spare bytes - One 528 byte units
 *
 * - FLASH_HAL_LAYOUT_2K_512_4_X4
 *     2K Page layout,
 *        512 main data + 4 spare bytes - Four 528 byte units
 *
 * - FLASH_HAL_LAYOUT_2K_528_0_X4
 *     2K Page layout,
 *        528 data (main and spare) - Four 528 byte units
 *
 * - 2K PAGE HW ECC ON SPARE ENABLED (FLASH_HAL_LAYOUT_2K_516_0_X3_500_16_X1)
 * - -----------------------------------------------------------------------
 * The following tables depicts how a 2K page data is organized when
 * hardware ECC enabled on main and spare.
 * - First three 528 byte units are depicted by layout (a).
 * - Fourth 528 byte unit is depicted by layout (b).
 *
 * Note: All numbers are in bytes
 *
 * (a)
 *
 * |Main (516)|Reserved (12)|
 *
 * (b)
 *
 * |Main (500)|Spare User data (16)|Reserved (12)|
 *
 * If we read the same NAND page with ECC disabled, (after having written
 * using the HW ECC on Main and Spare enabled), the layout of the page
 * is shown below.
 * - First three 528 byte units are depicted by layout (c).
 * - Fourth 528 byte unit is depicted by layout (d).
 *
 * (c)
 *
 * |Main (464)|Dummy BB (1/2)|Main (52)|Reserved (11/10)|
 *
 * (d)
 *
 * |Main (464)|Real BB(1/2)|Main(36)|Spare User data(16)|Reserved(11/10)|
 *
 *
 * - 2K PAGE HW ECC ONLY ON MAIN (FLASH_HAL_LAYOUT_2K_512_4_X4)
 * - --------------------------------------------------
 * The following tables depicts how a 2K page data is organized when
 * hardware ECC enabled only on main.
 * - All 4, 528 byte units are depicted by layout (e).
 *
 * (e)
 *
 * |Main(512)|Reserved(11)|Spare User data(4)|Reserved(1)|
 *
 * If we read the same NAND page with ECC disabled (after having written
 * using the HW ECC on Main), the layout of the page is shown below:
 *
 * - All 4 528 byte units are depicted by layout (f).
 *
 * (f)
 *
 * |Main (464)|Bad Block(1)|Main(52)|Reserved(12)|
 *
 *
 * - 2K PAGE RAW (FLASH_HAL_LAYOUT_2K_528_0_X4
 * - -------------------------------------
 * In this layout, flash HAL doesn't impose any restriction on the layout.
 * It all depends on the driver how to organize the main and spare data.
 *
 * - All 4, 528 byte units are depicted by layout (g).
 *
 * (g)
 *
 * |Main and Spare (528)|
 *
 */
enum flash_hal_page_layout_id
{
  FLASH_HAL_LAYOUT_2K_528_0_X4,                    /* 2K Page layout 1 */
  FLASH_HAL_LAYOUT_2K_516_0_X3_500_16_X1,          /* 2K Page layout 3 */
  FLASH_HAL_LAYOUT_8BIT_ECC_2K_532_0_X4,           /* 2K Page layout 1 for 8bit ecc */
  FLASH_HAL_LAYOUT_8BIT_ECC_2K_516_0_X3_500_16_X1, /* 2K Page layout 3 for 8bit ecc */
  FLASH_HAL_LAYOUT_4K_528_0_X8,                    /* 4K Page layout 1 */
  FLASH_HAL_LAYOUT_4K_516_0_X7_484_32_X1,          /* 4K Page layout 3 */
  FLASH_HAL_LAYOUT_8BIT_ECC_4K_532_0_X8,           /* 4K Page layout 1 for 8bit ecc */
  FLASH_HAL_LAYOUT_8BIT_ECC_4K_516_0_X7_484_32_X1, /* 4K Page layout 3 for 8bit ecc */
  FLASH_HAL_LAYOUT_NONE                            /* No Page layout   */
};

/* Transfer mechanism performed by controller */
enum flash_hal_ctlr_transfer
{
  FLASH_HAL_XFER_REGISTER_READ,    /* Perform a register read transfer */
  FLASH_HAL_XFER_REGISTER_WRITE,   /* Perform a register write transfer */
  FLASH_HAL_XFER_DATA_READ,        /* Perform a data read transfer */
  FLASH_HAL_XFER_DATA_WRITE,       /* Perform a data write transfer */
  FLASH_HAL_XFER_WAIT_INTH,        /* wait on INT high */
  FLASH_HAL_XFER_ACCESS_MODE       /* Set access mode */
};

/*-----------------------------------------------------------------------
 *           Structure Declarations
 *----------------------------------------------------------------------*/

/* Structure for specifying the flash details. Contents of this
 * structure is updated during probe, and driver can't modify the
 * values
 */
struct flash_hal_dev_capability
{
  uint32 max_addr_cycles;           /* Max. allowed address cycles */
  struct flash_hal_hw_ecc_info ecc_info;   /* ECC Information */
};

struct flash_hal_wait_info
{
  enum flash_hal_wait_t wait_type;  /* Wait type */
  uint32 timeout;                   /* Timeout */
};

/* Structure for specifying, the flash configuration details.
 * Driver modifies this structure through the HAL APIs
 */
struct flash_hal_ctlr_data
{
  enum flash_hal_dev_type dev_type;    /* NAND, oneNAND, ONFI etc. */
  enum flash_hal_chip_sel chip_sel;    /* Chip select */
  enum flash_hal_dev_width width;      /* 8/16 Bit */
  enum flash_hal_ecc_state ecc_state;  /* Confg for current ecc_state */
  enum flash_hal_ecc_length ecc_len_bits; /* Confg for current ECC Length */
  enum flash_hal_page_size page_size_bytes; /* page or sector size */
  enum flash_hal_spare_size spare_size_bytes; /* Max spare bytes available */
  enum flash_hal_predef_nand_ids write_type; /* Write type */
  uint32 addr0;                        /* Configuration Options 0 */
  uint32 addr1;                        /* Configuration Options 0 */
  uint32 row_count;                    /* Number of bytes in row addr */
  uint32 col_count;                    /* Number of bytes in column addr */
  uint32 cw_cnt;                       /* Code word count */
  uint32 cfg0;                         /* Configuration Options 0 */
  uint32 cfg1;                         /* Configuration Options 1 */
  uint32 ecc_cfg;                      /* Configuration for ecc */
  uint32 dev_vld;                      /* Valid device commands */
  uint32 ecc_buf;                      /* Bytes to cover by ECC logic */
  uint32 main_size_bytes;              /* Main data size to be configured */
  uint32 cw_size_bytes;                /* Code word size to be configured */
  uint32 ecc_check_size;               /* ECC check size to be configured */
  uint32 addr_offset;                  /* Address offset */
  uint32 seq_cnt;                      /* Total number of command in sequence*/
  int32  cur_seq_cnt;                  /* current command sequence number  */
  uint32 total_page_size;              /* Total page size */
  uint32 erased_cw_det_cfg;            /* Erased code word detect configuration */
  uint32 erased_cw_det_status;         /* Erased code word detect status */
  boolean config_is_needed;            /* Configuration needed flag */
  uint32 predef_init_flag;             /* Predef sequence init flag */
  uint32 block_size;                   /* # of pages per block */
};

/*
 * NOR Command structure
 */
struct flash_hal_nor_cmd
{
  uint32 op_flag;                         /* operation flag */
  uint32 xfer_mech;                       /* transfer mechanism */
  uint32 reg_addr;                        /* register address */
  uint16 wr_reg_val;                      /* register value */
  uint16 *rd_reg_val;                     /* register value pointer */
  uint32 reg_data_len_bytes;              /* register data length */
  void   *reg_data;                       /* register data pointer */
  struct flash_hal_wait_mask_info_16 wait_mask;  /* wait mechanism */
};

/* Buffer pointer for NANDC HAL */
struct flash_hal_nand_buffer
{
  uint32 cw_offset;                 /* Codeword offset */
  void   *buf_ptr;                  /* buffer pointer */
  uint32 buf_len;                   /* buffer length */
  uint32 buf_copy_len;              /* Number of bytes to copy */
};

/*
 * NAND Command structure
 */
struct flash_hal_nand_cmd
{
  uint32 op_flag;                              /* operation flag */
  uint32 num_addr_cycles;                      /* Number of address cycles */
  uint32 max_op_data_len;                      /* Operation data length */
  uint32 data_buff_cnt;                        /* data buffer count */
  struct flash_hal_nand_buffer data_buff[FLASH_HAL_NANDC_MAX_BUF];
                                               /* data buffer list */
  enum flash_hal_data_dir data_dir;            /* data direction */
  struct flash_hal_wait_info wait_mech;        /* wait mechanism */
  uint8  addr[FLASH_HAL_MAX_ADDR_CYCLES];      /* address bytes */
  uint8  cmd;                                  /* command */
};

/*
 * Structure to maintain the list of devices available in HAL.
 */
struct flash_hal_cmd_seq
{
  uint32 seq_id;                    /* sequence id */
  void   *cmd_seq;                  /* sequence pointer */
};

typedef struct flash_hal_device_info flash_hal_dev_info;

/* NAND Controller Operations */
struct flash_hal_ctlr_ops
{
  /* Flash operation transfer function */
  void (*flash_hal_transfer) (struct flash_hal_ctlr_data *priv_data,
    struct flash_hal_cmd_seq *cmd_seq, enum flash_hal_status *status);

  /* Controller specific flash close function */
  void (*flash_hal_close) (flash_hal_dev_info *dev_info);

  /* Controller specific flash configuration */
  void (*flash_hal_configure) (struct flash_hal_ctlr_data *priv_data,
    enum flash_hal_status *status );

  /* Sets the gap in number of clock cycles in flash HAL */
  void (*flash_hal_set_clk_cycle_gap) (uint32 gap);

  /* Indicates beginning of a flash HAL operation. */
  void (*flash_hal_begin_op) (struct flash_hal_ctlr_data *priv_data,
    void *ws_buffer, uint32 ws_buf_len_bytes);

  /* Indicates end of a flash HAL operation. */
  void (*flash_hal_end_op) (struct flash_hal_ctlr_data *priv_data);

  /* Returns page layout configured by flash HAL.  */
  void (*flash_hal_get_page_layout) (struct flash_hal_ctlr_data *priv_data,
    struct flash_hal_page_layout **layout);

  /* Returns predef memory usage by flash HAL.  */
  void (*flash_hal_get_predef_mem_usage) (struct flash_hal_ctlr_data *priv_data,
    uint32 no_of_pages, uint32 *mem_use);

  /* Post processes operation list after data mover processing. */
  void (*flash_hal_post_process_op_list) (struct flash_hal_ctlr_data *priv_data,
   struct flash_hal_op_list *op_list, enum flash_hal_status *status);

  /* Initialize pre-def sequences */
  void (*flash_hal_predef_init) (flash_hal_dev_info *dev_info,
    struct flash_hal_predef_init_data *init_data, uint32 *used_buff_size,
    enum flash_hal_status *status);

  /* De-initialize pre-def sequences */
  void (*flash_hal_predef_deinit) (flash_hal_dev_info *dev_info);

  /* Device Probe - Read IDs */
  void (*flash_hal_read_id) (flash_hal_dev_info *dev_info,
    struct flash_hal_buffer_desc *buff_desc, struct flash_hal_predef_op_list
    **op_list, enum flash_hal_status *status);

  /* Reading ONFI IDs from device */
  void (*flash_hal_read_onfi_id) (flash_hal_dev_info *dev_info, struct
    flash_hal_predef_onfi_data *probe_data, struct flash_hal_predef_op_list
    **op_list, enum flash_hal_status *status);

  /* Reading ONFI Parameter page */
  void (*flash_hal_read_param_page) (flash_hal_dev_info *dev_info,
    struct flash_hal_predef_onfi_data *page_data,
    struct flash_hal_predef_op_list **op_list, enum flash_hal_status *status);

  /* Reading single page */
  void (*flash_hal_read_page) (flash_hal_dev_info *dev_info, struct
    flash_hal_predef_page_data *page_data, struct flash_hal_predef_op_list
    **op_list, enum flash_hal_status *status);

  /* Reading bad block marker */
  void (*flash_hal_read_bb_marker) (flash_hal_dev_info *dev_info, struct
    flash_hal_predef_spare_data *page_data, struct flash_hal_predef_op_list
    **op_list, enum flash_hal_status *status);

  /* Writing a single page */
  void (*flash_hal_write_page) (flash_hal_dev_info *dev_info, struct
    flash_hal_predef_page_data *page_data, struct flash_hal_predef_op_list
    **op_list, enum flash_hal_status *status);


  /* Erasing a block */
  void (*flash_hal_erase_block) (flash_hal_dev_info *dev_info, struct
    flash_hal_predef_block_data *block_data, struct flash_hal_predef_op_list
    **op_list, enum flash_hal_status *status);

  /* Set transfer step configuration */
  void (*flash_hal_set_xfer_cfg) (flash_hal_dev_info *dev_info, struct 
    flash_hal_xfer_cfg *cfg_data, enum flash_hal_status *status);

};

/*
 * Structure to maintain the list of devices available in HAL.
 */
struct flash_hal_device_info
{
  uint32 dev_id;                            /* Device ID */
  struct flash_hal_dev_capability dev_cap;  /* Device capabilities */
  struct flash_hal_ctlr_data config;        /* Private configuration */
  struct flash_hal_ctlr_ops  ctlr_ops;      /* NAND Controller Operations */
};

/* Flash Devices */
extern flash_hal_dev_info  flash_hal_devices[FLASH_HAL_DEVICE_MAX];

void *flash_hal_memcpy(void *dest, const void* src, uint32 len);
void *flash_hal_memset(void *dest, int c, uint32 len);
void flash_hal_busywait(uint32 wait_time_us);
void flash_hal_cache_clean(uint32 addr, uint32 len);
void flash_hal_cache_inval(uint32 addr, uint32 len);

#endif  /* FLASHI_HAL_H */
