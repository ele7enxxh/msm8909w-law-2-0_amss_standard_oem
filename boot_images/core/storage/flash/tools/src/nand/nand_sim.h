#ifndef __NAND_SIM_H__
#define __NAND_SIM_H__

/*=======================================================================
 * FILE:        nand_sim.c
 *
 * SERVICES:    Exports the simulated APIs for NAND programmer.
 *
 * GENERAL DESCRIPTION
 *   Exports the simulated APIs for NAND programmer.
 *
 * Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nand/nand_sim.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/10/10     bb      Fix for unaligned memcpy failure with RVCT 4.0
 * 08/12/10     nr      Replaced nand_log.h with flash_log.h 
 * 07/20/09     eo      Add die package type to OneNAND register info.
 * 02/27/09     jz      Updated to support SB2.0
 * 10/29/08     mm      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "miboot.h"

/* Allocate one extra block to surve as the main image area. This is required
 * for the the init routines that prep the image area by erasing the first good
 * block in it. */
#define NAND_RAM_DEV_SIZE (MI_BLOCK_LAST + 1)

/* Largest number of pages in a block */
#define MAX_BLOCK_SIZE 128

#ifndef MAX_PAGE_SIZE
  #error MAX_PAGE_SIZE must be defined!!
#else
  #define MAX_SPARE_SIZE  (MAX_PAGE_SIZE >> 5)
  #define TOTAL_PAGE_SIZE (MAX_PAGE_SIZE + MAX_SPARE_SIZE)
#endif

#define MAX_NAME_SIZE 20
#define SIZE_ONE_K 1024
#define MAX_FLAG_SIZE 20

#ifdef BUILD_FOR_X86
#include "DALStdDef.h"
#include "DALSys.h"

#ifdef FLASH_LOG_H
  #include FLASH_LOG_H
#else
  #define FLASHLOG(l, x)
#endif

typedef void  * DALDeviceHandle;

/** Flash DAL operation status */

/** Flash DAL operation status */
#define FLASH_DEVICE_DONE                            0  /**< Operation passed */
#define FLASH_DEVICE_FAIL                          (-1) /**< Operation failed */
#define FLASH_DEVICE_BUSY                          (-2) /**< Device currently busy */
#define FLASH_DEVICE_PARTITION_NOT_FOUND           (-3) /**< Partition/Drive not found */
#define FLASH_DEVICE_NO_PARTITION_TABLE            (-4) /**< Partition table not found */
#define FLASH_DEVICE_INVALID_PARTITION_NAME        (-5) /**< Not a Valid Partition Name */
#define FLASH_DEVICE_PARTITION_TABLE_EXISTS        (-6) /**< Partition table already exists */
#define FLASH_DEVICE_DUPLICATE_PARTITION           (-7) /**< Duplicate Partition detected */
#define FLASH_DEVICE_PARTITION_TABLE_VERSION_SKEW  (-8) /**< Partition table version skew detected */
#define FLASH_DEVICE_NOT_SUPPORTED                 (-9) /**< Device not supported */
#define FLASH_DEVICE_CP_READ_FAIL                 (-10) /**< Copy Page Read Failure */
#define FLASH_DEVICE_CP_WRITE_FAIL                (-11) /**< Copy Page Write Failure */
#define FLASH_DEVICE_FAIL_PAGE_ERASED             (-12) /**< Erased Page Detected */
#define FLASH_DEVICE_INVALID_PARAMETER            (-13) /**< API Parameters invalid */
#define FLASH_DEVICE_NO_MEM                       (-14) /**< Insufficient Memory */

/** Flash Device Technology */
enum flash_type
{
  FLASH_NOR,      /**< NOR Device */
  FLASH_NAND,     /**< NAND Device */
  FLASH_ONENAND   /**< OneNAND Device */
};

/** Bits Per Cell: SLC/MLC/Mirrorbit/etc */
enum flash_tech
{
  FLASH_SLC,       /**< Single Level Cell Device */
  FLASH_MLC,       /**< Multi Level Cell Device */
  FLASH_MirrorBit, /**< Mirrorbit based MLC Device */
  FLASH_FLEX       /**< Flex (SLC+MLC) Device */
};

/** Flash Device Width */
enum flash_width
{
  FLASH_x8  = 8,       /**<  x8 Interface Width Device */
  FLASH_x16 = 16       /**<  x16 Interface Width Device */
};

/** Definitions for ECC disabling/enabling. ECC can be enabled in two modes.
 * FLASH_ECC_MAIN_ENABLED - In this mode ECC is enabled only on the Main Area
 * FLASH_ECC_MAIN_SPARE_ENABLED - In this mode, ECC is enabled on the
 * Main and the spare area
 */
enum flash_ecc_state
{
  FLASH_ECC_NONE,               /**< No ECC Support */
  FLASH_ECC_DISABLED,           /**< ECC Disabled */
  FLASH_ECC_MAIN_ENABLED,       /**< ECC Enabled only on Main Area */
  FLASH_ECC_MAIN_SPARE_ENABLED  /**< ECC Enabled on Main and Spare Area */
};

/** Flash Block State */
enum flash_block_state
{
  FLASH_BLOCK_OK  = 0x1,       /**< Flash Block is Good */
  FLASH_BLOCK_BAD = 0x2,       /**< Flash Block is Bad */
  FLASH_BLOCK_RESERVED = 0x4   /**< Reserved State */
} ;

/** Flash Write style description of how write grouping happens, and what is necessary to
 * recover from partial and failed writes. */
enum flash_write_style
{
  FLASH_WRITES_SIMPLE, /**<  NOR devices where arbitrary bits can be set
    to zero. SLC NOR devices generally fall in this category. */
  FLASH_WRITES_PAGED,    /**<  NAND devices requiring entire pages to be
    written. Also it can only be written to once. These devices do not
    allow 'partial_write'. */
  FLASH_WRITES_PAIRED_BITS,  /**<  NOR devices where adjacent pairs of
    bits are written together. A pair can be written from '11' to a value,
    and from a value to '00'.  If recovery from failed writes is needed,
    the only two values that should be used are '11' and '00'.  After a
    failed write, it may not be possible to write the pair to any value
    other than '00'. */
  FLASH_WRITES_UNSUPPORTED = 0x1000,    /**<  Indicates a NOR device with
    an unsupported write style.  No assumptions can be made about how the
    data is written. Pages must be written in their entirety, and pages that
    have bad powerdowns cannot be used. */
};

#ifndef NOR_ERASE_REGION_MAX
#define NOR_ERASE_REGION_MAX            5    /**< Maximum number of erase block
                                                  regions in NOR device */
#endif

/** Flash information type */
enum flash_info_type
{
  FLASH_DEVICE_INFO,               /**< General flash device information */
  FLASH_ONENAND_REG_INFO,          /**< OneNAND register information */
  FLASH_NOR_ERASE_REGION_INFO      /**< NOR Erase region information */
};

/** Holds the new and previous MIBIB block number */
struct flash_mibib
{
  int new_mibib_block;             /**< Newest MIBIB block information */
  int old_mibib_block;             /**< Oldest MIBIB block information */
};

/** NOR erase block region information */
struct nor_region_info
{
  uint32 block_count;              /**< Number of blocks in this
                                        erase region */
  uint32 block_size_in_bytes;      /**< Size of each block in this
                                        erase region*/
};

/** NOR erase region specific information */
struct nor_erase_region_info
{
  uint32 erase_region_count;       /**< Number of erase block regions
                                        in this device */
  struct nor_region_info erase_region[NOR_ERASE_REGION_MAX]; /**< Erase
                                        block regions info */
};

/** OneNAND register information */
struct onenand_reg_info
{
  uint32 block_addr_reg1;          /**< Block acccess address register 1 */
  uint32 block_addr_reg2;          /**< Block acccess address register 2 */
  uint32 prot_reg_low;             /**< Low address for the protected
                                        region */
  uint32 prot_reg_high;            /**< High address for the protected
                                        region */
  uint32 cmd_reg;                  /**< Device address for the
                                        command register. */
  uint32 die_pkg;                  /**< Device single or dual die package. */
};

/** This holds the partition specific information for a flash client. */
struct partition_info
{
  enum flash_ecc_state ecc_state;  /**< Current ECC state for this partition */
  uint32   block_count;            /**< Total blocks in this partition  */
  uint32   pages_per_block;        /**< Number of pages in a block */
  uint32   page_size_bytes;        /**< Page size in bytes not including spare */
  uint32   total_page_size_bytes;  /**< Page size in bytes including spare */
  uint32   max_spare_udata_bytes;  /**< Max ECC covered user data bytes in spare */
  uint32   physical_start_block;   /**< Physical Block start of partition */
  uint32   attributes;             /**< Attributes of this partition. This
                                        field can be decoded by including
                                        mibib.h */
  uint32   reserved1;              /**< Reserved */
  uint32   reserved2;              /**< Reserved */
};

/** Flash client device and partition data */
struct flash_info
{
  enum flash_type  type;               /**< Device Technology Type */
  enum flash_tech  tech;               /**< Device Bits Per Cell Type */
  enum flash_width width;              /**< Device Interface Width */
  enum flash_write_style write_style;  /**< Write style specific to Flash */
  uint8  device_name[32];              /**< Device name string */
  uint32 maker_id;                     /**< Maker Identification */
  uint32 device_id;                    /**< Device Identification */
  uint32 version_id;                   /**< Device Version ID, if any */
  struct partition_info partition;     /**< Partition specific Data */
  struct flash_mibib  mibib_info;      /**< MIBIB information */
};

/** Block vector definition for block operations. This structure
    allows to operate on block_count contiguous blocks starting
    from start_block. The structure also has a field to store the
    status of each block operation */
struct flash_block_vector
{
  uint32 start_block;         /**< Logical start block number */
  uint32 block_count;         /**< Number of blocks to operate */
  int    *result_vector;      /**< Vector to store result of block operation */
};


/** I/O vector definition for flash page operations like read or write.
    This structure allows to operate on page_count contiguous pages starting
    from start_page.

    Note that no buffer information is embedded in this structure. All the
    buffers required for a flash operation must be passed through a
    DAL based buffer descriptor.
    */
struct flash_page_iovec
{
  uint32 start_page;       /**< Logical page used to start the operation */
  uint32 page_count;       /**< Number of pages to operate */
  uint32 main_size_bytes;  /**< Length of main data to read/write from a page in bytes */
  uint32 spare_size_bytes; /**< Length of spare data to read from a page in bytes */
};


/** Type of read to be used with flash read API */
enum page_read_opcode
{
  FLASH_READ_MAIN = 0,   /**< Read only main data. main_data is not NULL */
  FLASH_READ_SPARE,      /**< Read only spare data. spare_data is not NULL,
                              spare_size_bytes is set */
  FLASH_READ_MAIN_SPARE, /**< Read both main and spare data. main_data and
                              spare_data is not NULL, spare_size_bytes is set */
  FLASH_READ_RAW,        /**< Read all RAW bytes in page(No ECC done). main_data is not
                              NULL, main_size_bytes includes main+spare space */
  FLASH_READ_BYTES       /**< Read given number of bytes within page.
                              main_data is not NULL, main_size_bytes <= page size
                              bytes */
};

/** Type of write to be used with flash write API */
enum page_write_opcode
{
  FLASH_WRITE_MAIN = 0,  /**< Write only main data. main_data is not NULL */
  FLASH_WRITE_SPARE,     /**< Write only spare data. spare_data is not NULL */
  FLASH_WRITE_MAIN_SPARE,/**< Write both main and spare data. main_data and
                                 spare_data is not NULL, spare_size_bytes is set */
  FLASH_WRITE_RAW        /**< Write all RAW bytes in page(No ECC done). main_data is not
                                 NULL, main_size_bytes includes main+spare space */
};

/** I/O vector definition for flash copy page operation.
    This structure allows to copy copy_count pages sequentially
    from src_page to dest_page. */
struct flash_copy_iovec
{
  uint32 src_page;     /**< Logical source page to be operated on */
  uint32 dest_page;    /**< Logical destination page to be operated on */
  uint32 copy_count;   /**< Number of pages to copy */
};

/** Definition for flash partial page operation structure.This specifies to
    operate on write_size_bytes bytes starting from offset byte_offset in
    page 'page'. */
struct flash_partial_page_op
{
  uint32 page;                 /**< Logical page to be operated on */
  uint32 byte_offset;          /**< Offset within logical page */
  uint32 write_size_bytes;     /**< Bytes to write */
};

typedef struct flash_handle   flash_handle;
typedef flash_handle          *flash_handle_t;
#define dalsys_mem_desc_list  DALSysMemDescList

int flash_block_get_state(flash_handle_t handle,
  uint32 block, enum flash_block_state *block_state);

int flash_erase_blocks(flash_handle_t handle,
  struct flash_block_vector *block_vector, uint32 block_count);

int flash_get_info(flash_handle_t handle,
  enum flash_info_type info_type, void *flash_info);

int flash_open_partition(flash_handle_t handle,
  const unsigned char *partition_name);

/**  Wrapper for DAL_DeviceAttach. This registers the flash driver to DAL.
     and loads the driver */
#define flash_device_attach(dev_id,device_handle)\
        DAL_DeviceAttach(dev_id,(DalDeviceHandle **)device_handle)

/**  Wrapper for DalDevice_Close. Closes the opened device */
#define flash_device_close(device_handle)

/**  Wrapper for DAL_DeviceDetach. De-registers the driver from DAL */
#define flash_device_detach(device_handle) \
        DAL_DeviceDetach((DalDeviceHandle*)device_handle)

/*
 * Wrapper for memcpy API. Introduced as a
 * Workaround for unaligned memcpy failure with RVCT 4.0
 */
void* nand_memcpy(void *out, const void *in, uint32 n);

#define CHECK_FOR_DATA()

/* Allocate one extra block beyond what is required for essential MIBIB blocks
 * to represent rest of the device. This is required for the the init routines
 * that prep the main image area by erasing the first good block in it. */
extern unsigned char ram_blocks[NAND_RAM_DEV_SIZE][MAX_BLOCK_SIZE][TOTAL_PAGE_SIZE];

extern void flash_sim_strcpy(char *dir_name, char *filepath);
extern void flash_sim_makepath(char *dir_name, char *filepath, char *filename);
extern void main_c(void);

#endif /*BUILD_FOR_X86*/
#endif /*__NAND_SIM_H__*/
