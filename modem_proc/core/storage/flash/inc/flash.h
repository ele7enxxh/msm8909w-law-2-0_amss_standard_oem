#ifndef FLASH_H
#define FLASH_H

/**********************************************************************
 * flash.h
 *
 * Public interface declaration for the Flash Drivers.
 *
 * This file describes the Flash DAL external interface, common to all
 * targets.
 *
 * Copyright (c) 2008-2010, 2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *
 **********************************************************************/
/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/inc/flash.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 2012-03-19   sv      Add SPI NOR power management support
 * 2012-03-19   eo      Support for erase blocks by size
 * 2010-10-01   bn      Added support for NOR I2C
 * 2010-06-23   bb      Fixing all enum sizes to be 4 bytes
 * 2010-05-26   eo      Fix read_pointer api error.
 * 2010-05-11   eo      Added support for additional NOR based APIs
 * 2010-04-29   sb      Fix for limitation from Q6 C++ compiler on 9K
 * 2010-01-16   mm      Support for 2x Error codes
 * 2009-12-28   sc      Added 2x support in DAL
 * 2009-11-24   sc      Added structure flash_2x_page_iovec
 * 2009-07-18   eo      Add dual die package type to onenand_reg_info
 * 2009-07-28   mm      Documentation updates for ISOD generation
 * 2009-04-28   bb      Header file case correction for Linux builds
 * 2009-03-09   bb      Removed iovec count from read/write APIs
 * 2008-07-15   dp      Initial version
 *=======================================================================*/

/**  \mainpage Flash DAL Functional Overview
 *
 * The Flash driver module consists of three main components :
 * - Flash device adaptation layer common to all OSs (Flash DAL)
 * - Flash hardware abstraction layer (Flash Hardware Abstraction Layer (HAL))
 * - Flash target specific configuration data
 *
 * \n
 *
 * The Flash driver provides a high-level interface for clients, such
 * as bootloaders, file systems, and tools to interact with the flash devices. It
 * handles the Flash device specifics and abstracts the client from the device-
 * specific algorithms, interfaces, and OS adaptations. The Flash HAL layer
 * also contains the HAL interface and its implementation. The HAL provides a
 * standard API to the Flash DAL, independent of the underlying Mobile Station
 * Modem&tm; &nbsp;(MSM&tm;) ASIC hardware and Flash device. \n
 *
 * \n
 *
 */

#include "DALStdDef.h"
#include "DalDevice.h"
#include "DALSys.h"

#define FLASH_USES_DAL  /**< Indicates that DAL-based flash driver is in use */

#define FLASH_HAVE_ERASE_BLK_BY_SIZE /**< Indicates block erase by size API is 
                                          supported */

/** Flash DAL operation status */
#define FLASH_DEVICE_DONE                 0  /**< Operation passed */
#define FLASH_DEVICE_FAIL               (-1) /**< Operation failed */
#define FLASH_DEVICE_NOT_SUPPORTED      (-2) /**< Device/operation not supported */
#define FLASH_DEVICE_CP_READ_FAIL       (-3) /**< Copy page read failure */
#define FLASH_DEVICE_CP_WRITE_FAIL      (-4) /**< Copy page write failure */
#define FLASH_DEVICE_FAIL_PAGE_ERASED   (-5) /**< Erased page detected */
#define FLASH_DEVICE_INVALID_PARAMETER  (-6) /**< API parameters invalid */
#define FLASH_DEVICE_NO_MEM             (-7) /**< Insufficient memory */
#define FLASH_DEVICE_OP_INPROGRESS      (-8) /**< Operation in progress */
#define FLASH_DEVICE_PRGM_FAIL_PLANE1   (-9) /**< Operation failed in Plane 1 */
#define FLASH_DEVICE_PRGM_FAIL_PLANE2   (-10) /**< Operation failed in Plane 1 */
#define FLASH_DEVICE_PRGM_FAIL_BOTH_PLANES (-11) /**< Operation failed in both planes */
#define DALFLASH_INTERFACE_VERSION DALINTERFACE_VERSION(1,0) /**< DAL interface
                                                  version */

#define FLASH_NOR_ERASE_REGION_MAX       5   /**< Maximum number of erase block
                                                  regions in NOR device */
#define FLASH_NOR_BLOCK_ERASE_CONFIG_MAX 5   /**< Maximum number of erase block
                                                  configuration info in NOR device */

/** Flash Device Technology */
enum flash_type
{
  FLASH_NOR,      /**< NOR Device */
  FLASH_NOR_SPI,  /**< SPI NOR Device */
  FLASH_NAND,     /**< NAND Device */
  FLASH_ONENAND,  /**< OneNAND Device */
  FLASH_NOR_I2C,  /**< I2C EEPROM Device */
  FLASH_UNKNOWN = 0x7FFFFFFF /* Force 32-bit enum */
};

/** Bits per cell: SLC/MLC */
enum flash_tech
{
  FLASH_SLC,        /**< Single Level Cell Device */
  FLASH_MLC,        /**< Multi Level Cell Device */
  FLASH_TECH_UNKNOWN = 0x7FFFFFFF /* Force 32-bit enum */
};

/** Flash device width */
enum flash_width
{
  FLASH_x8  = 8,    /**< x8 Interface Width Device */
  FLASH_x16 = 16,   /**< x16 Interface Width Device */
  FLASH_WIDTH_UNKNOWN = 0x7FFFFFFF /* Force 32-bit enum */
};

/** Definitions for Error-Correcting Code (ECC) disabling/enabling. ECC can be enabled in two modes.
 * FLASH_ECC_MAIN_ENABLED - In this mode ECC is enabled only on the Main Area
 * FLASH_ECC_MAIN_SPARE_ENABLED - In this mode, ECC is enabled on the
 * Main and the spare area
 */
enum flash_ecc_state
{
  FLASH_ECC_NONE,               /**< No ECC Support */
  FLASH_ECC_DISABLED,           /**< ECC Disabled */
  FLASH_ECC_MAIN_ENABLED,       /**< ECC Enabled only on Main Area */
  FLASH_ECC_MAIN_SPARE_ENABLED, /**< ECC Enabled on Main and Spare Area */
  FLASH_ECC_UNKNOWN = 0x7FFFFFFF /* Force 32-bit enum */
};

/** Flash block state. This indicates the state of a block. */
enum flash_block_state
{
  FLASH_BLOCK_OK  = 0x1,       /**< Flash Block is Good */
  FLASH_BLOCK_BAD = 0x2,       /**< Flash Block is Bad */
  FLASH_BLOCK_RESERVED = 0x4,  /**< Reserved State */
  FLASH_BLOCK_STATE_UNKNOWN = 0x7FFFFFFF /* Force 32-bit enum */
} ;

/** Flash write style description of how write grouping happens, and what is necessary to
 * recover from partial and failed writes. */
enum flash_write_style
{
  FLASH_WRITES_SIMPLE,    /**< NOR devices where arbitrary bits can be set
    to zero. SLC NOR devices generally fall in this category. */
  FLASH_WRITES_PAGED,     /**< NAND devices requiring entire pages to be
    written. Also it can only be written to once. These devices do not
    allow 'partial_write'. */
  FLASH_WRITES_PAIRED_BITS, /**< NOR devices where adjacent pairs of
    bits are written together. A pair can be written from '11' to a value,
    and from a value to '00'. If recovery from failed writes is needed,
    the only two values that should be used are '11' and '00'.  After a
    failed write, it may not be possible to write the pair to any value
    other than '00'. */
  FLASH_WRITES_UNSUPPORTED = 0x1000, /**<  Indicates a NOR device with
    an unsupported write style.  No assumptions can be made about how the
    data is written. Pages must be written in their entirety, and pages that
    have bad powerdowns cannot be used. */
  FLASH_WRITES_STYLE_UNKNOWN = 0x7FFFFFFF /* Force 32-bit enum */
};

/** Flash information type */
enum flash_info_type
{
  FLASH_DEVICE_INFO,               /**< General flash device information */
  FLASH_ONENAND_REG_INFO,          /**< OneNAND register information */
  FLASH_NOR_ERASE_REGION_INFO,     /**< NOR Erase region information */
  FLASH_NOR_BLOCK_ERASE_CFG_INFO   /**< NOR Block configuration information */
};

/** Holds the new and previous Multi-Image Boot Information Block (MIBIB)
    block number. This structure is used to keep track of current and previous
  MIBIB block numbers. \n
    @note This is primarily intended to use in flash tools during
    software download. \n
 */
struct flash_mibib
{
  int new_mibib_block;             /**< New MIBIB block number */
  int old_mibib_block;             /**< Old MIBIB block number */
};

/** Different NOR block erase configurations */
struct flash_nor_block_erase_info
{
  uint32   block_count;            /**< Total blocks in this partition  */
  uint32   pages_per_block;        /**< Number of pages in a block */
};

/** Different NOR block erase configurations */
struct flash_nor_block_erase_cfg_info
{
  uint32 erase_cfg_count;       /**< Number of erase block configurations */
  struct flash_nor_block_erase_info block_cfg[FLASH_NOR_BLOCK_ERASE_CONFIG_MAX];
                                /**< Erase block configuration */
};


/** NOR erase block region information */
struct flash_nor_region_info
{
  uint32 block_count;              /**< Number of blocks in this
                                        erase region */
  uint32 block_size_in_bytes;      /**< Size of each block in this
                                        erase region*/
};

/** NOR erase region specific information */
struct flash_nor_erase_region_info
{
  uint32 erase_region_count;       /**< Number of erase block regions
                                        in this device */
  struct flash_nor_region_info erase_region[FLASH_NOR_ERASE_REGION_MAX];
                                   /**< Erase block regions info */
};

/** OneNAND register information */
struct onenand_reg_info
{
  uint32 block_addr_reg1;          /**< Block access address register 1 */
  uint32 block_addr_reg2;          /**< Block access address register 2 */
  uint32 prot_reg_low;             /**< Low address for the protected
                                        region */
  uint32 prot_reg_high;            /**< High address for the protected
                                        region */
  uint32 cmd_reg;                  /**< Device address for the
                                        command register. */
  uint32 die_pkg;                  /**< Device single or dual die package. */
};

/** This holds the partition specific information for a Flash client. */
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
  uint32 maker_id;                     /**< Manufacturer Identification */
  uint32 device_id;                    /**< Device Identification */
  uint32 version_id;                   /**< Device Version ID, if any */
  uint32 block_count;                  /**< Number of total blocks in device */
  struct partition_info partition;     /**< Partition specific Data */
  struct flash_mibib  mibib_info;      /**< MIBIB information */
};

/** Block vector definition for block operations. This structure
    allows operations on block_count contiguous blocks starting
    from the start_block. This structure also has a field to store the
    status of each block operation */
struct flash_block_vector
{
  uint32 start_block;         /**< Logical start block number */
  uint32 block_count;         /**< Number of blocks to operate */
  int    *result_vector;      /**< Vector to store result of block operation */
};

/** I/O vector definition for flash page operations like read or write.
    This structure allows flash page operations to operate on page_count
  contiguous pages starting from start_page.

    @note Buffer information is embedded in this structure. All the
    buffers required for a flash operation must be passed through a
    DAL based buffer descriptor.
    */
struct flash_page_iovec
{
  uint32 start_page;       /**< Logical page used to start the operation */
  uint32 total_page_count;       /**< Number of pages to operate */
  uint32 main_size_bytes;  /**< Length of main data to read/write from a page in bytes */
  uint32 spare_size_bytes; /**< Length of spare data to read from a page in bytes */
};

/** I/O vector definition for flash 2X page operations like read or write.
    This structure allows flash page operations to operate on page_count
    contiguous pages starting from start_page.

    @note No buffer information is embedded in this structure. All the
    buffers required for a flash operation must be passed through a
    DAL based buffer descriptor.
    */
struct flash_2x_page_iovec
{
  uint32 start_page_index; /**< Page index within the block */
  uint32 total_page_count; /**< Number of pages to operate */
  uint32 main_size_bytes;  /**< Length of main data to read/write from a page in bytes */
  uint32 spare_size_bytes; /**< Length of spare data to read from a page in bytes */
  uint32 block_x; /**< Block number on which the first page of the 2X pages resides */
  uint32 block_y; /**< Block number on which the second page of the 2X pages resides */
};

/** Type of read to be used with flash read API */
enum page_read_opcode
{
  FLASH_READ_MAIN = 0,   /**< Read only main data. main_data is not NULL */
  FLASH_READ_SPARE,      /**< Read only spare data. spare_data is not NULL,
                              spare_size_bytes is set */
  FLASH_READ_MAIN_SPARE, /**< Read both main and spare data. main_data and
                              spare_data is not NULL, spare_size_bytes is set */
  FLASH_READ_RAW,        /**< Read all RAW bytes in page(with ECC disabled). main_data is not
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
  FLASH_WRITE_RAW        /**< Write all RAW bytes in page(with ECC disabled). main_data is not
                              NULL, main_size_bytes includes main+spare space */
};

/** Type of block size to be used with flash nor erase block API */
enum block_size_opcode
{
  BLOCK_SIZE_4KB = 4,     /**< Erase block size unit of 4KB. */
  BLOCK_SIZE_32KB = 32,   /**< Erase block size unit of 32KB. */
  BLOCK_SIZE_64KB = 64,   /**< Erase block size unit of 64KB. */
  BLOCK_SIZE_UNKNOWN = 0x7FFFFFFF /* Force 32-bit enum */
};

/** I/O vector definition for flash copy page operation.
    This structure allows the flash copy page operation to copy copy_count pages sequentially
    from src_page to dest_page. */
struct flash_copy_iovec
{
  uint32 src_page;     /**< Logical source page to be operated on */
  uint32 dest_page;    /**< Logical destination page to be operated on */
  uint32 copy_count;   /**< Number of pages to copy */
};

/** Definition for flash partial page operation structure. This specifies to
    operate on write_size_bytes bytes starting from offset byte_offset in
    page 'page'. */
struct flash_partial_page_op
{
  uint32 page;                 /**< Logical page to be operated on */
  uint32 byte_offset;          /**< Offset within logical page */
  uint32 write_size_bytes;     /**< Bytes to write */
};

/** Flash power mode */
enum flash_power_mode
{
  FLASH_POWER_LOW_PWR_MODE, /**< Low power mode */
  FLASH_POWER_NORMAL_MODE,  /**< Normal(Default) mode */
  FLASH_POWER_UNKNOWN_MODE = 0x7FFFFFFF /* Force 32-bit enum */
};

typedef struct flash_handle   flash_handle;
typedef flash_handle          *flash_handle_t;

/**\cond */

#define dalsys_mem_desc_list  DALSysMemDescList

#define GET_FLASH_HANDLE_VTBL(handle) \
    (*((flash_handle_vtbl **)((void *)((uint32 *) handle + 1))))

#define INHERIT_FLASH_VTBL(type) \
  int (*open_partition)(type *handle, const unsigned \
    char *partition_name); \
  int (*get_info)(type *handle, enum flash_info_type info_type, \
    void *flash_info); \
  int (*get_status)(type *handle); \
  \
  int (*erase_blocks)(type *handle, struct flash_block_vector *block_vectors, \
    uint32 block_count); \
  int (*begin_erase_block)(type *handle, uint32 block); \
  int (*erase_suspend)(type *handle); \
  int (*erase_resume)(type *handle); \
  \
  int (*block_set_state)(type *handle, uint32 block, \
    enum flash_block_state block_state); \
  int (*block_get_state)(type *handle, uint32 block, \
    enum flash_block_state *block_state); \
  \
  int (*write_pages)(type* handle, enum page_write_opcode write_type, \
    dalsys_mem_desc_list *buffer_desc, struct flash_page_iovec *page_iovec); \
  \
  int (*write_partial_page)(type* handle, \
    dalsys_mem_desc_list *buffer_desc, \
    const struct flash_partial_page_op *partial_page_op); \
  \
  int (*write_2x_pages)(type* handle, enum page_write_opcode write_type, \
    dalsys_mem_desc_list *buffer_desc, \
    struct flash_2x_page_iovec *page_2x_data_iovec); \
  \
  int (*read_pages)(type* handle, enum page_read_opcode read_type, \
    dalsys_mem_desc_list *buffer_desc, struct flash_page_iovec *page_iovec); \
  \
  int (*read_pointer)(type* handle, void **read_addr, uint32 page); \
  \
  int (*copy_pages)(type* handle, struct flash_copy_iovec *copy_op, \
    uint32 iovec_count); \
  \
  int (*get_uuid)(type *handle, void *uuid_buf, uint32 uuid_buf_len, \
    uint32 *uuid_len); \
  \
  int (*erase_blocks_by_size)(type *handle, \
    enum block_size_opcode block_size_type, \
    struct flash_block_vector *block_vectors, uint32 block_count); \
  \
  int (*set_power_mode)(type *handle, enum flash_power_mode power_mode)

/*  Required structure as per requirements of the DAL framework */
typedef struct
{
  struct DalDevice DalDevice;       /**< Holds the DAL framework required APIs */
  uint32 (*AddRef)(flash_handle*);  /**< Holds the number of clients of the driver*/
  INHERIT_FLASH_VTBL(flash_handle); /**< Holds flash API for the client */
}flash_handle_vtbl;

/**  Wrapper for DAL_DeviceAttach. This registers the flash driver to DAL.
     and loads the driver */
#define flash_device_attach(dev_id,device_handle)\
        DAL_DeviceAttach(dev_id,(DalDeviceHandle **)device_handle)

/**  Wrapper for DalDevice_Close. Closes the opened device */
#define flash_device_close(device_handle) \
        DalDevice_Close((DalDeviceHandle*)device_handle)

/**  Wrapper for DAL_DeviceDetach. De-registers the driver from DAL */
#define flash_device_detach(device_handle) \
        DAL_DeviceDetach((DalDeviceHandle*)device_handle)

/**\endcond */

/**
 * Opens a given partition on Flash
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @param partition_name [IN]
 *   Name of the partition to open.
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
static __inline int flash_open_partition(flash_handle_t handle,
  const unsigned char *partition_name)
{
  return GET_FLASH_HANDLE_VTBL(handle)->open_partition(
         (flash_handle *) handle, partition_name);
}

/**
 * Gets flash device information. Flash driver assumes that the memory for returned
 * information is allocated by the driver client. It is
 * client's responsibility to typecast the void* parameter
 * to the appropriate type. \n
 * \n
 *  Info Type                  \t  -  Data Structure Associated\n \n
 *
 *  FLASH_DEVICE_INFO            -  struct flash_info \n
 *  FLASH_ONENAND_REG_INFO       -  struct onenand_reg_info \n
 *  FLASH_NOR_ERASE_REGION_INFO  -  struct nor_erase_region_info \n
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @param info_type [IN]
 *   Type of information needed.
 *
 * @param flash_info [OUT]
 *   Pointer to the client's flash device info data.
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
static __inline int flash_get_info(flash_handle_t handle,
  enum flash_info_type info_type, void *flash_info)
{
  return GET_FLASH_HANDLE_VTBL(handle)->get_info(
     (flash_handle *) handle, info_type, flash_info);
}

/**
 * Get current flash device status. This API returns,
 *
 *   FLASH_DEVICE_DONE - If the device in operation in progress
 *   is successfully completed \n
 *   FLASH_DEVICE_FAIL - If the operation in progress fails \n
 *   FLASH_DEVICE_OP_INPROGRESS - If any operation is in progress\n
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @return int [OUT]
 *   Current Flash Device Status.
 *
 */
static __inline int flash_get_status(flash_handle_t handle)
{
  return GET_FLASH_HANDLE_VTBL(handle)->get_status((flash_handle *) handle);
}

/**
 * Erases given flash blocks
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @param block_vector [IN-OUT]
 *   Vector for block operation. Each element contains a starting block with
 *   the number of blocks to erase and a result vector to hold each block operation
 *   status
 *
 * @param block_count [IN]
 *   Number of elements in block_vector
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
static __inline int flash_erase_blocks(flash_handle_t handle,
  struct flash_block_vector *block_vector, uint32 block_count)
{
  return GET_FLASH_HANDLE_VTBL(handle)->erase_blocks(
      (flash_handle *) handle, block_vector, block_count);
}

/**
 * Begin erase a given flash block
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @param block [IN-OUT]
 *   The block to be begin erase operation.
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
static __inline int flash_begin_erase_block(flash_handle_t handle,
   uint32 block)
{
  return GET_FLASH_HANDLE_VTBL(handle)->begin_erase_block((flash_handle *) handle,
    block);
}

/**
 * Suspend ongoing erase operation. This API does nothing
 * and returns FLASH_DEVICE_DONE if there is no erase in progress.
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
static __inline int flash_erase_suspend(flash_handle_t handle)
{
  return GET_FLASH_HANDLE_VTBL(handle)->erase_suspend((flash_handle *) handle);
}

/**
 * Resume the last suspended erase operation.  This API does nothing
 * and returns FLASH_DEVICE_DONE if there is no erase in progress.
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
static __inline int flash_erase_resume(flash_handle_t handle)
{
  return GET_FLASH_HANDLE_VTBL(handle)->erase_resume((flash_handle *) handle);
}

/**
 * Sets state to a given block.
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @param block [IN]
 *   The logical block number
 *
 * @param block_state [IN]
 *   The block state to be set
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
static __inline int flash_block_set_state(flash_handle_t handle,
  uint32 block, enum flash_block_state block_state)
{
  return GET_FLASH_HANDLE_VTBL(handle)->block_set_state(
       (flash_handle *) handle, block, block_state);
}

/**
 * Gets the current state of a given block.
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @param block [IN]
 *   The logical block number
 *
 * @param block_state [OUT]
 *   Pointer to the block state buffer
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
static __inline int flash_block_get_state(flash_handle_t handle,
  uint32 block, enum flash_block_state *block_state)
{
  return GET_FLASH_HANDLE_VTBL(handle)->block_get_state(
      (flash_handle *) handle, block, block_state);
}

/**
 * Flash write page API. This API allows writing single/multiple pages
 * within a block.
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @param write_type [IN]
 *   Flash page write type
 *
 * @param buffer_desc [OUT]
 *   Buffers for flash read operation. This buffer descriptor holds
 *   main and spare data buffers for the flash page read operation.
 *   The 'buffer_desc' structure has 4 fields,
 *         - VirtualAddr
 *         - PhysicalAddr
 *         - size
 *         - user
 */

/**
 * This argument points to a set of buffer descriptors. The flash_write_pages
 * API assumes that there are at least 2 buffer descriptors. A client can
 * have up to 128 buffer descriptors in an API call. @n
 * @par
 * The main buffer descriptor points to the main data buffer and the spare buffer
 * descriptor points to the spare user data buffer. Clients can have one buffer
 * descriptor for more than one page. The 'user' field in the buffer descriptor
 * needs to be initialized with the number of pages corresponding to the buffer. @n
 * @par
 * For Example: If the main buffer descriptor points to a 10K buffer, the client
 * can set the user field in the buffer descriptor to 5 (in the case of a 2K page
 * device). Remember to initialize the main and spare user fields with the
 * same value. @n
 * @par
 * For optimum performance, pass DMA friendly (physically contiguous,
 * non-cached) buffer. If the passed memory is not DMA friendly,
 * the DAL will allocate DMA friendly memory and may result in
 * low performance. @n
*/

/**
 * @param page_iovec [IN]
 *   I/O vector for flash write. Each element in the vector
 *   contains size of main and spare data to operate,
 *   and logical start page with number of pages to write.
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 * @note
 *   IMPORTANT: The 'page_iovec' allows clients to specify number of
 *   pages to write from a given start page. Ensure that the given
 *   range falls in one block. It is NOT allowed to write pages that span
 *   across multiple blocks in a single API call.
 *
 */
static __inline int flash_write_pages(flash_handle_t handle,
  enum page_write_opcode write_type, dalsys_mem_desc_list *buffer_desc,
  struct flash_page_iovec *page_iovec)
{
  return GET_FLASH_HANDLE_VTBL(handle)->write_pages((flash_handle *) handle,
    write_type, buffer_desc, page_iovec);
}

/**
 * Write partial data to specified page
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @param buffer_desc [OUT]
 *   Buffers for flash partial write operation. This buffer descriptor holds
 *   page data for the partial flash page write operation.
 *   Note: For optimum performance, pass DMA friendly (physically
 *         contiguous, non-cached) buffer.
 *
 * @param partial_page_op [IN]
 *   Pointer to the flash partial page type that describes the
 *     logical page details to be written.
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
static __inline int flash_write_partial_page(flash_handle_t handle,
  dalsys_mem_desc_list *buffer_desc,
  const struct flash_partial_page_op *partial_page_op)
{
  return GET_FLASH_HANDLE_VTBL(handle)->write_partial_page(
       (flash_handle *) handle, buffer_desc, partial_page_op);
}

/**
 * Flash 2x write page API. This API allows writing 2 pages at a time
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @param write_type [IN]
 *   Flash page write type
 *
 * @param buffer_desc [OUT]
 *   Buffers for flash read operation. This buffer descriptor holds
 *   main and spare data buffers for the flash page read operation.
 *   The 'buffer_desc' structure has 4 fields,
 *         - VirtualAddr
 *         - PhysicalAddr
 *         - size
 *         - user
 */

/**
 * This argument points to a set of buffer descriptors. The flash_2x_write_pages
 * API assumes that there are at least 2 buffer descriptors. A client can
 * have up to 128 buffer descriptors in an API call. @n
 * @par
 * The main buffer descriptor points to the main data buffer and the spare buffer
 * descriptor points to the spare user data buffer. Clients can have one buffer
 * descriptor for more than one page. The 'user' field in the buffer descriptor
 * needs to be initialized with the number of pages corresponding to the buffer. @n
 * @par
 * For Example: If the main buffer descriptor points to a 10K buffer, the client
 * can set the user field in the buffer descriptor to 5 (in the case of a 2K page
 * device). Remember to initialize the main and spare user fields with the
 * same value. @n
 * @par
 * For optimum performance, pass DMA friendly (physically contiguous,
 * non-cached) buffer. If the passed memory is not DMA friendly,
 * the DAL will allocate DMA friendly memory and may result in
 * low performance. @n
*/

/**
 * @param page_iovec [IN]
 *   I/O vector for flash write. Each element in the vector
 *   contains size of main and spare data to operate,
 *   and logical start page with number of pages to write.
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 * @note
 *   IMPORTANT: The 'page_2x_data_iovec' allows clients to specify number of
 *   pages to write from a given start page. Ensure that the given
 *   range falls in one block.
 *
 */
/**
 * @param page_2x_data_iovec [IN]
 *   I/O vector for flash 2X write. Each element in the vector
 *   contains size of main and spare data to operate,
 *   and logical start page with number of pages to write.
 *
 * @return int [OUT]
 *   Result of the operation.
 */
static __inline int flash_write_2x_pages(flash_handle_t handle,
  enum page_write_opcode write_type, dalsys_mem_desc_list *buffer_desc,
  struct flash_2x_page_iovec *page_2x_data_iovec)
{
  return GET_FLASH_HANDLE_VTBL(handle)->write_2x_pages((flash_handle *) handle,
    write_type, buffer_desc, page_2x_data_iovec);
}

/**
 * Flash read page API. This API allows to read single/multiple pages
 * within a block.
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @param read_type [IN]
 *   Flash page read type
 *
 * @param buffer_desc [OUT]
 *   Buffers for flash read operation. This buffer descriptor holds
 *   main and spare data buffers for the flash page read operation.
 *   The 'buffer_desc' structure has 4 fields,
 *         - VirtualAddr
 *         - PhysicalAddr
 *         - size
 *         - user
*/

/**
 * This argument points to a set of buffer descriptors. The flash_read_pages
 * API assume that there are at least 2 buffer descriptors. A client can
 * have up to 128 buffer descriptors in an API call.
 * @par
 * The main buffer descriptor points to the main data buffer and the spare buffer
 * descriptor points to the spare user data buffer. Clients can have one buffer
 * descriptor for more than one page. The 'user' field in the buffer descriptor
 * need to be initialized with the number of pages corresponding to the buffer.
 * @par
 * For example: If the main buffer descriptor points to a 10K buffer, then the client
 * can set the user field in the buffer descriptor to 5 (in the case of a 2K page
 * device). Remember to initialize the main and spare user fields with
 * same value.
 * @par
 * For optimum performance, pass DMA friendly (physically contiguous,
 * non-cached) buffer. If the passed memory is not DMA friendly, then
 * the DAL will allocate DMA friendly memory and may result in
 * low performance.
*/

/**
 * @param page_iovec [IN]
 *   I/O vector for flash write. Each element in the vector
 *   contains size of main and spare data to operate,
 *   and logical start page with number of pages to write.
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 * @note
 *   IMPORTANT: The 'page_iovec' allows clients to specify number of
 *   pages read from a given start page. Ensure that the given
 *   range falls in one block. It is NOT allowed to read pages that span
 *   across multiple blocks in a single API call.
 *
 */
static __inline int flash_read_pages(flash_handle_t handle,
  enum page_read_opcode read_type, dalsys_mem_desc_list *buffer_desc,
  struct flash_page_iovec *page_iovec)
{
  return GET_FLASH_HANDLE_VTBL(handle)->read_pages((flash_handle *) handle,
    read_type, buffer_desc, page_iovec);
}

/**
 * Flash read pointer API. This API gets the data address of specified page
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @read_addr [OUT]
 *   Pointer to a read data address. This function is applicable for NOR flash
 *   devices only. This parameter returns NULL if direct mapping of read data
 *   address is not supported.
 *
 * @page [IN]
 *   Logical read page number.
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
static __inline int flash_read_pointer(flash_handle_t handle,
  void **read_addr, uint32 page)
{
  return GET_FLASH_HANDLE_VTBL(handle)->read_pointer((flash_handle *) handle,
    read_addr, page);
}

/**
 * Copy all specified sequential Page(s) from source page(s) to destination
 * page(s)
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @param copy_vec [IN-OUT]
 *   flash_copy_iovec vector for flash copy operation.
 *   Each element in the vector contains logical source page and
 *   logical destination page along with number of pages to copy.
 *
 * @param vec_count [IN]
 *   Number of elements in copy_vec
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
static __inline int flash_copy_pages(flash_handle_t handle,
  struct flash_copy_iovec *copy_vec, uint32 vec_count)
{
  return GET_FLASH_HANDLE_VTBL(handle)->copy_pages((flash_handle *)handle,
    copy_vec, vec_count);
}

/**
 * Retrieve Unique ID from the flash device
 *
 * @param handle [IN]
 *   Pointer to DAL I/F handle
 *
 *
 * @param uuid_data [OUT]
 *   The Unique ID from the OTP Page
 *
 * @param uuid_buf_len [IN]
 *   Size of the buffer in bytes
 *
 * @param uuid_len [OUT]
 *   Number of bytes of the Unique ID copied into the buffer
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */

static __inline int flash_get_uuid(flash_handle_t handle, void *uuid_data,
  uint32 uuid_buf_len, uint32 *uuid_len)
{
  return GET_FLASH_HANDLE_VTBL(handle)->get_uuid((flash_handle *)handle,
    uuid_data, uuid_buf_len, uuid_len);

}

/**
 * Erases given flash blocks of specified block size
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @param handle [IN]
 *   block size type specified
 *
 * @param block_vector [IN-OUT]
 *   Vector for block operation. Each element contains a starting block 
 *   with the number of blocks to erase and a result vector to hold each 
 *   block operation status.
 *
 * @param block_count [IN]
 *   Number of elements in block_vector
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
static __inline int flash_erase_blocks_by_size(flash_handle_t handle,
  enum block_size_opcode block_size_type, 
  struct flash_block_vector *block_vector, uint32 block_count)
{
  return GET_FLASH_HANDLE_VTBL(handle)->erase_blocks_by_size(
      (flash_handle *) handle, block_size_type, block_vector, block_count);
}

/**
 * Set the Power mode for the flash device that supports power mode 
 * selection
 *
 * @param handle [IN]
 *   Pointer to DAL I/F handle
 *
 * @param power_mode [IN]
 *   The power mode to which the flash hardware needs to be set
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */

static __inline int flash_set_power_mode(flash_handle_t handle, 
  enum flash_power_mode power_mode)
{
  return GET_FLASH_HANDLE_VTBL(handle)->set_power_mode((flash_handle *)handle,
    power_mode);
}

#endif /* #ifndef FLASH_H */
