#ifndef FLASH_NAND_CONFIG_H
#define FLASH_NAND_CONFIG_H

/*=======================================================================
 * FILE:        flash_nand_config.h
 *
 * SERVICES:    Common header for NAND Flash Configurations. NAND includes
 *              all NAND like devices (NAND, OneNAND, FlexOneNAND, etc)
 *
 * DESCRIPTION: 
 *
 * Copyright (c) 2008-2009, 2011-2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=====================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/dal/flash_nand_config.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/05/12     sv      cleanup
 * 12/14/11     eo      Add optimized transfer steps values
 * 05/20/11     bb      Reduce Max BB entries, add max_wb_ns in nand_params
 * 06/03/09     mh      Add "next" field to nand_params
 * 03/30/08     bb      Initial Revision
 *==================================================================*/

/*-----------------------------------------------------------------------
 *            Include Files
 *----------------------------------------------------------------------*/
#include "flash_hal.h"
#include "flash_otp_config.h"

/*-----------------------------------------------------------------------
 *           Definitions and Constants
 *----------------------------------------------------------------------*/

#define FLASH_MAX_BB_ENTRIES  2       /**< Maximum bad block entries */
#define FLASH_MAX_DEVICE_NAME 20      /**< Maximum device name */
#define FLASH_MAX_DEVICE_IDS  8       /**< Maximum device ids */ 
#define FLASH_MAX_XFER_STEPS  7       /**< Maximum transfer step entries */
#define FLASH_EBI2_64MHZ      0       /**< EBI2 clock setting for 64Mhz */

/*-----------------------------------------------------------------------
 *           Type Declarations
 *----------------------------------------------------------------------*/

/** Different Multi Copy-page/copyback features */
enum nand_multi_cp_options
{
  FLASH_DEFAULT_READ_WRITE_CP = 0,        /**< Do complete read followed by write*/
  NAND_0x00_0x35_0x85_0x10_ODD_ODD_CP,    /**< Optimized copy page using 
                                              0x00-0x35 and 0x85-0x10 with 
                                              restriction to do it only across 
                                              same plane
                                              (odd to odd or even to even pages). 
                                              Odd to even copy page is done
                                              using default copypage option */
  NAND_0x00_0x35_0x85_0x10_ODD_EVEN_CP,  /**< Optimized copy pge using
                                             0x00-0x35 and 0x85-0x10 with no 
                                             plane restrictions */
  ONENAND_CP                             /**< Optimized OneNAND copy */
};                                       /**<   back command support */

/** Different Read multiple sequential pages, within same block 
   features */
enum nand_multi_read_options
{
  FLASH_DEFAULT_READ_MPAGES = 0,          /**< Do page by page reads  */
  NAND_0x00_0x30_0x31n_0x3F_READ_MPAGES,  /**< Optimized reading with 0x00-0x30
                                              for first page. All succedding 
                                              pages (except lastpage) using 0x31. 
                                              Last page using 0x3F command */
  ONENAND_PINGPONG_READ_MPAGES,           /**< Use OneNAND dual buffer */
  ONENAND_CACHE_READ_MPAGES               /**< Use OneNAND cache read algorithm */
};

/** Different Write multiple sequential pages, within same block 
   features */
enum nand_multi_write_options
{
  FLASH_DEFAULT_WRITE_MPAGES = 0,         /**< Do page by page writes */
  NAND_0x80_0x15_0x80_0x10_WRITE_MPAGES,  /**< Optimized writing with 0x80-0x15 for
                                             all pages except last page and 
                                             0x80-0x10 for last page*/
  ONENAND_PINGPONG_WRITE_MPAGES           /**< Use OneNAND dual buffer */
};

/** Different Erase multiple blocks features */
enum nand_multi_erase_options
{
  FLASH_DEFAULT_ERASE_MBLOCKS = 0,        /**< Do block by block erase */
  ONENAND_ERASE_MBLOCKS                   /**< Use OneNAND multi erase algo */
};

/** HW ECC requirement of device */
enum nand_ecc_requirement
{
  NAND_NO_HW_ECC   = 0,                    /**< Unsupported HW ECC by Ctlr */
  NAND_INBUILT_ECC = 0,                    /**< Device has its own ECC inbuilt */
  NAND_1BIT_HW_ECC = 1,                    /**< Device needs Ctlr HW 1-bit ECC */
  NAND_2BIT_HW_ECC = 2,                    /**< Device needs Ctlr HW 2-bit ECC */
  NAND_4BIT_HW_ECC = 4,                    /**< Device needs Ctlr HW 4-bit ECC */
  NAND_8BIT_HW_ECC = 8,                    /**< Device needs Ctlr HW 8-bit ECC */
  NAND_MAX_SUPPORTED_HW_ECC = 16           /**< Device needs Ctlr HW 16-bit ECC */
};

/** Page pairing considertation for powerdown scenarios */
enum nand_pairing_options
{
  FLASH_NO_PAGE_PAIRING_RESTRICTIONS = 0, /**< No pairing of page consideration
                                             for sudden powerdown during writes */
  NAND_PAIRING_4_6_1                      /**< Pairing algorithm : Min pair 
                                             distance_Max pair_distance_index 
                                             Sample page pairing: 
                                             {(0,4), (1,5), (2,8), (3,9), 
                                              (6,12), (7,13), (10,16), (11,17), 
                                              (14,20), (15, 21)} */
};

/** Unlock block features */
enum nand_unlock_options
{
   FLASH_DEFAULT_BLOCK_UNLOCK = 0,       /**< No block/unlock feature supported */
   ONENAND_BLOCK_UNLOCK,                 /**< Unlock blocks one at a time */  
   ONENAND_RANGE_UNLOCK                  /**< Unlock blocks w/specified range */
};

/** Transfer steps configuration */
enum nand_xfer_steps_cfg
{
  NAND_2K_8_BIT_XFER_CFG  = 0,           /**< For x8, 2kb page devices */
  NAND_2K_16_BIT_XFER_CFG = 1,           /**< For x16, 2kb page devices */
  NAND_4K_8_BIT_XFER_CFG  = 2,           /**< For x8, 4kb page devices */
  NAND_4K_16_BIT_XFER_CFG = 3,           /**< For x16, 4kb page devices */
  NAND_MAX_XFER_CFG
};

/* Set of transfer step values */
struct nand_xfer_steps_val
{
  uint32 xfer_count;                      /**< Transfer steps count */
  uint32 xfer_data[FLASH_MAX_XFER_STEPS]; /**< Transfer steps data */
};

/* Table of transfer steps based on EBI2 clock frequency */
struct flash_nand_ebi2_cfg
{
  const struct nand_xfer_steps_val *xfer_steps[NAND_MAX_XFER_CFG]; /**< xfer config table */
};

/** Device specific supported feature flags */
struct nand_feature_flags1
{
  enum nand_ecc_requirement ecc:5;         /**< NAND device ECC requirement */
  enum nand_multi_read_options mreads:3;   /**< Multi read page options */
  enum nand_multi_write_options mwrites:4; /**< Multi write page options */
  enum nand_multi_cp_options mcopy:4;      /**< Multi copy page options */
  enum nand_multi_erase_options merases:2; /**< Multi erase block options */
  enum nand_pairing_options pagepairing:2; /**< Page Pairing matrix, if any */
  enum nand_unlock_options unlock:2;       /**< OneNAND Range/Block unlock options */
  uint32 dp_read_supported:1;              /**< Dual Plane read supported */
  uint32 dp_write_supported:1;             /**< Dual Plane write supported */
  uint32 dp_erase_supported:1;             /**< Dual Plane erase supported */
  uint32 reserved:7;                       /**< Reserved */
};

/** Device specific supported feature flags */
struct nand_feature_flags2
{
  uint32 reserved:32;                    /**< Reserved */
};

/** Structure definition for bad block byte layout in a block */
struct flash_bad_block_layout
{
  uint32 page;                           /**< Page to check for bad block */
  uint32 byte_offset;                    /**< Badblock byte start offset */
  uint32 byte_length;                    /**< Badblock length in bytes */
};

/** Structure definition for spare bytes info */
struct flash_spare_info
{
  struct flash_bad_block_layout bad_block_info[FLASH_MAX_BB_ENTRIES]; 
                                         /**<  Bad block page/byte layout info */
  uint32 udata_max;                      /**< Total user bytes in spare */
  uint32 udata_ecc_max;                  /**< Total ECC for spare user bytes */
  uint32 max_corrected_udata_bytes;      /**< Correctable bytes by ECC */

  /**< Encodes given spare_udate_bytes using ECC algorithm used and returns
     encoded data along with its ECC */
  void *(*sw_encode_data)(struct flash_spare_info *spare_info,
    void *data, uint32 bytes_len);

  /**< Decodes given spare_udata_ecc_bytes to check for any error and returns
     status as to no error, correctable error or uncorrectable error */
  int (*sw_decode_data)(struct flash_spare_info *spare_info, void *data);
};

/** NAND device characteristics */
struct flash_nand_params
{
  enum flash_hal_dev_width   dev_width;        /**< Device width */
  enum nand_xfer_steps_cfg   dev_xfer_steps_cfg; /**< Device transfer steps */
  enum nand_otp_config_type  otp_sequence_cfg; /**< OTP config type */
  struct flash_spare_info    *dev_spare_info;  /**< Device spare info */

  char   device_name[FLASH_MAX_DEVICE_NAME];  /**< Device name */
  uint8  dev_ids[FLASH_MAX_DEVICE_IDS];       /**< Device ID */
  uint32 block_count;                         /**< Block count */
  uint32 pages_per_block;                     /**< Pages per block */
  uint32 page_size;                           /**< Page size */
  uint32 spare_size;                          /**< Spare size */
  uint32 max_wb_ns;                           /**< Max tWB in ns (from datasheet) */
  uint32 op_status_mask;   /**< Operation status mask */
  uint32 op_status_value;  /**< Operation success status value */
  struct nand_feature_flags1 feature_flags1;  /**< Device Feature flags 1 */
  struct nand_feature_flags2 feature_flags2;  /**< Device Feature flags 1 */
};

/* Flash NAND device parameter data */
typedef struct flash_nand_params flash_nand_params_type;

/* Nand Bad Block Information */
#define NAND_BB_INFO(params) params->dev_spare_info->bad_block_info

/* Structure for NAND configuration parameters */
struct flash_nand_cfg_data
{
  enum flash_hal_dev_type    dev_type;        /**< Device type */
  enum flash_hal_chip_sel    chip_sel;        /**< Chip select number */
  uint32                     num_devices;     /**< Number of devices */ 
  struct flash_nand_params   *dev_params;     /**< Device param */
  struct flash_nand_ebi2_cfg *ebi2_cfg;       /**< EBI2 xfer cfg data*/
};


/*-----------------------------------------------------------------------
 *           NAND Configuration APIs
 *----------------------------------------------------------------------*/

/*
 * Gets the configuration data associated with all supported NAND devices
 */
void flash_nand_get_configs (struct flash_nand_cfg_data **cfg_data);

#endif  /*ifndef FLASH_NAND_CONFIG_H */
