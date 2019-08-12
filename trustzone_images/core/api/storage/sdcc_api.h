#ifndef __SDCC_API_H
#define __SDCC_API_H
/******************************************************************************
 * sdcc_api.h
 *
 * This file provides SDCC external definitions.
 *
 * Copyright (c) 2007-2010
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/storage/sdcc_api.h#1 $
  $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
2010-10-05   vj      Added force erase functionality
2010-06-11   vin     CLK types enumeration start at 0 and added comment
2009-11-05   vj      Added public API and structure for boot
2009-10-30   sc      Added feature to set active boot partition
2009-10-27   sc      Fixed the sdcc_sdio_handle_set Doxygen format
2009-10-16   sc      Added power-on write protection feature
2009-10-12   sc      Added write_protect_group_size_in_sectors
2009-09-11   rcc     Removed unnecessary legacy NXP WLAN type definitions
2009-08-21   sp      Added sdcc_sdio_set_max_speed API to support SDIO maxclk
2009-08-13   sc      Added is_embedded_device in sdcc_mem_info_type
2009-06-30   vin     Added SDIO clock definitions 
2009-06-15   rh      Adding common interface extraction tag
2009-06-11   rh      Adding the block erase support
2009-05-20   sc      Consolidated physical partition info support to get device info
2009-04-27   sc      Added support to get physical partition info
2009-03-27   vin     New SDIO API functions
2009-03-30   sc      Added handle support
2009-03-25   vin     Cleanup of SDIO unused and deprecated functions
2009-01-09   vj      OS Abstraction Layer
2009-01-09   rh      Cleanup of cust file dependencies
2008-12-04   vin     Added HAVE_SDCC_IOVEC_SUPPORT
2008-11-14   sc      Replaced reliable_write_support with
                     reliable_write_sector_count
2008-11-10   sc      Converted comments to Doxygen
2008-10-30   sc      Cleaned up file sdcc_api.h and removed sdcc_sfat.c
2008-10-23   vin     Support for MMC power collapse
2008-10-01   vin     Added sdcc_sdio_enable_int and sdcc_sdio_disable_int
2008-07-17   sc      Added scatter/gather support
2008-07-17   umr     Added BSP support & removed SDCC_WLAN_SLOT dependency.
2008-05-21   sc      Moved enum SDCC_STATUS to sdcc_errno.h
04/04/2008   vin     Fixed argument type sdcc_read_serial
02/27/2008   vin     Added function sdcc_slot_has_support
01/31/2008   umr     Removed SDCC_MAX_NUM_CONTROLLER in SDCC_INTF_TYPE
01/15/2008   vin     Moved FEATURE_SDCC_HAVE_CONFIGURATION_API from custsdcc.h
                     to sdcc_api.h
11/29/07     vin     Added support for four SD controllers
11/13/07     umr     Added pound define for drive numbers.
10/09/07     vin     Moved SDCC_CARD_TYPE and sdcc_find_card to sdcc_api.h
10/09/07     vin     Created sdio_pre_init & sdio_post_init
03/01/07     sc      Cleaned up the not referenced codes
01/25/07     sc      Ported CE-ATA feature to 7K target and did major
                     code clean up
01/09/07     dng     Added WINCE support
10/11/06     sep     CPRM support - expose read/write fifo functions
08/13/06     hwu     Merged in the support for WINCE.
06/19/06     hwu     Added enumeration for SDC1 GPIOs.
03/28/06     hwu     Added defines for SDC2 muxing.
12/21/05     hwu     Added dual SDCC support.
08/28/05     hwu     General cleanup. Move testing related to sdcc_priv.h
07/24/05     hwu     Moved the common includes from private API to
                     the public API.
05/26/05     hwu     Merge in changes from L4Linux.
02/15/05     hwu     Added more SDIO related defines.
06/15/04     hwu     Initial version.
=============================================================================*/

#include "sdcc_errno.h"
/* The following line #include "msm.h" will be removed once WLAN properly */
/* includes the needed header file. */
#include "msm.h"

/** \mainpage SD Card Controller API Overview
 *
 * SD Card Controller (SDCC) has several levels of public APIs for different
 * card types, different kinds of data request, and specific needs.\n
 *
 * For card types, SDCC provides two types of APIs - memory specific and SDIO
 * APIs.
 *
 * For data request, SDCC provides two types of APIs - legacy sequential code
 * path and I/O vector for a list of data transfers.
 *
 * SDCC also provides Raw Command APIs (see \ref sdcc_raw_cmd_api) to
 * encapsulate commands and data transfer.
 */

/**
 * @defgroup sdcc_driver SD Card Controller (SDCC)
 *
 * The SDCC driver provides block-oriented data transfer to or from the memory
 * device.
 */

/*@{*/

/*SDCC_EXTRACT_COMMON_BEGIN*/
/**
 * The drive numbers are of int16 type.  SDCC_DRIVENO_0 is the default card
 * slot.
 */
#define SDCC_DRIVENO_0  0
#define SDCC_DRIVENO_1  1
#define SDCC_DRIVENO_2  2
#define SDCC_DRIVENO_3  3

/**
 * This flag denotes that \ref sdcc_get_config_info API is available.
 */
#define FEATURE_SDCC_HAVE_CONFIGURATION_API

/**
 * This flag denotes that \ref sdcc_slot_has_support API exists.
 */
#define HAVE_SDCC_SLOT_HAS_SUPPORT

/**
 * This flag advertises existence of \ref sdcc_activity_timeout_event API.
 */
#define HAVE_SDCC_POWER_COLLAPSE

/**
 * This flag denotes that \ref sdcc_mem_get_device_info API is available.
 * This API retrieves card identification information including manufacturer
 * ID, product name, product serial number, etc.  Also, this API gets the
 * card type, card size (in sectors), speed class, and whether card supports
 * reliable write information.
 */
#define HAVE_SDCC_GET_MEMORY_DEVICE_INFO 

/**
 * This flag denotes presence of scatter-gather data transfer APIs
 */
#define HAVE_SDCC_IOVEC_SUPPORT

/**
 * This flag denotes SDCC Handle support is available.  All SDCC Handle APIs
 * take a pointer to a SDCC device except \ref sdcc_handle_open.
 */
#define HAVE_SDCC_HANDLE_SUPPORT

/**
 * A physical partition number denotes the entire memory device.  This value
 * is driver specific and it is used in \ref sdcc_handle_open if it is desired
 * to obtain a handle representing the whole memory device.
 */
#define SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE 0xFFFFFFFF

/**
 * Following are the card types that are supported by the SDCC driver.
 * Non-zero enum value indicates card is inserted in a card slot.
 */
typedef enum
{
   SDCC_CARD_UNKNOWN = 0, /**< Card cannot be detected */
   SDCC_CARD_SD      = 1, /**< SD card with card size <= 2GB */
   SDCC_CARD_MMC     = 2, /**< MMC card with card size <= 2GB */
   SDCC_CARD_COMB    = 3, /**< Comb card that supports both SDIO and memory */
   SDCC_CARD_SDIO    = 4, /**< SDIO card */
   SDCC_CARD_SDHC    = 5, /**< SD high capacity card with card size > 2GB */
   SDCC_CARD_MMCHC   = 6  /**< MMC high capacity card with card size > 2GB */
} SDCC_CARD_TYPE;
/*SDCC_EXTRACT_COMMON_END*/

/** \details
 * sdcc_slot_supports lists the kinds of devices that may be used in a slot.
 * This enum is used to associate a slot with the device type it supports.
 */
enum sdcc_slot_supports
{
   SDCC_SLOT_SDIO,
   SDCC_SLOT_MEMORY
};

/**
 * @ingroup sdcc_api
 * Supported bus widths
 */
typedef enum
{
   SDCC_BUSWIDTH_1BIT = 0,
   SDCC_BUSWIDTH_4BIT = 1,
   SDCC_BUSWIDTH_8BIT = 2,
   SDCC_BUSWIDTH_TYPE_COUNT /**< Used to check if a valid bus width value 
                                      is passed*/
} sdcc_buswidth_type;

/**
 * @ingroup sdio_api
 * SD/MMC clock definitions
 */
typedef enum  {
   SDCC_SD_DATA_TRANSFER_0_144_MHZ = 0,
   SDCC_SD_DATA_TRANSFER_0_400_MHZ,
   SDCC_SD_DATA_TRANSFER_20_MHZ,
   SDCC_SD_DATA_TRANSFER_25_MHZ,
   SDCC_SD_DATA_TRANSFER_49_152_MHZ
   /* While adding an enumeration, the sorted order must be maintained.*/
} sdcc_sd_data_transfer_clock_type;

/**
 * @ingroup sdcc_api
 * speed class for SD card
 * Refer to SD2.0 Part 1 Physical Layer Specification, Section 4.13.6 for
 * detailed speed class definition
 */
typedef enum
{
   SDCC_SD_SPEED_CLASS_0,
   SDCC_SD_SPEED_CLASS_2,
   SDCC_SD_SPEED_CLASS_4,
   SDCC_SD_SPEED_CLASS_6
} sdcc_sd_speed_class_type;

/**
 * @ingroup sdcc_api
 * speed class for MMC/eMMC card
 * Refer to JEDEC Standard No.\ 84-A43, Sections 7.8.1 and 8.4 for
 * detailed speed class definition
 */
typedef enum
{
   SDCC_MMC_SPEED_CLASS_LOWEST = 0x00, /**< For cards not reaching the 2.4MB/s
                                        * value */
   SDCC_MMC_SPEED_CLASS_A      = 0x08, /**< Lower bus category classes (26MHz
                                        * clock with 4bit data bus operation) */
   SDCC_MMC_SPEED_CLASS_B      = 0x0A,
   SDCC_MMC_SPEED_CLASS_C      = 0x0F,
   SDCC_MMC_SPEED_CLASS_D      = 0x14,
   SDCC_MMC_SPEED_CLASS_E      = 0x1E,
   SDCC_MMC_SPEED_CLASS_F      = 0x28, /**< Mid bus category classes (26MHz
                                        * clock with 8bit data bus or 52MHz
                                        * clock with 4bit data bus operation) */
   SDCC_MMC_SPEED_CLASS_G      = 0x32,
   SDCC_MMC_SPEED_CLASS_H      = 0x3C,
   SDCC_MMC_SPEED_CLASS_J      = 0x46,
   SDCC_MMC_SPEED_CLASS_K      = 0x50, /**< High bus category classes (52MHz
                                        * clock with 8bit data bus operation) */
   SDCC_MMC_SPEED_CLASS_M      = 0x64,
   SDCC_MMC_SPEED_CLASS_O      = 0x78,
   SDCC_MMC_SPEED_CLASS_R      = 0x8C,
   SDCC_MMC_SPEED_CLASS_T      = 0xA0
} sdcc_mmc_speed_class_type;

/**
 * @ingroup sdcc_iovec_api
 * The following enumeration type is to indicate the transfer type for the
 * scatter/gather data transfer.
 */
typedef enum
{
   SDCC_MEMORY_IOVEC_REGULAR_DATA_XFER = 0,
      /**< regular read or write data transfer */
   SDCC_MEMORY_IOVEC_RELIABLE_WRITE = 1
      /**< reliable write data transfer
       * refer to section 7.5.7 in JEDEC Standard No.\ 84-A43 */
} sdcc_memory_iovec_transfer_type;

/**
 * Interface type to index slot number in muxed controller
 */
typedef enum
{
   SDCC_SDC2_GPIO_B   = 0,
   SDCC_SDC2_GPIO_A   = 1,
   SDCC_SDC1_GPIO     = 2,
   SDCC_SDC3_GPIO     = 3,
   SDCC_SDC4_GPIO     = 4
} SDCC_INTF_TYPE;

/**
 * @ingroup sdio_api
 * SDIO definitions
 */
#define    SDIO_SET_FEATURE_TYPE            uint32
#define    SD_IO_FUNCTION_ENABLE            0
#define    SD_IO_FUNCTION_DISABLE           1
#define    SD_IO_FUNCTION_SET_BLOCK_SIZE    2
#define    SD_SET_DATA_TRANSFER_CLOCKS      3
#define    SD_SET_GPIO_INTERFACE            4
#define    SD_SET_BUS_WIDTH                 5
#define    SD_SET_OP_CODE                   6
#define    SD_SET_TYPE_COUNT                7
#define    SD_SET_BLOCK_MODE                8
#define    SD_IO_ENABLE_INTERRUPT           9
#define    SD_IO_DISABLE_INTERRUPT         10
#define    SD_IO_ENABLE_HIGHSPEED_MODE     11
#define    SD_IO_DISABLE_HIGHSPEED_MODE    12
#define    SD_IO_ATTACH_ISR                13
#define    SD_IO_DETACH_ISR                14

/**
 * @ingroup sdio_api
 * SDIO clock definitions
 */
typedef enum  {
   SDCC_SDIO_DATA_TRANSFER_0_144_MHZ,
   SDCC_SDIO_DATA_TRANSFER_5_MHZ,
   SDCC_SDIO_DATA_TRANSFER_20_MHZ,
   SDCC_SDIO_DATA_TRANSFER_25_MHZ,
   SDCC_SDIO_DATA_TRANSFER_38_4_MHZ,
   SDCC_SDIO_DATA_TRANSFER_49_152_MHZ
} sdcc_sdio_data_transfer_clock_type;

/**
 * @ingroup sdio_api
 */

/**
 * The following SDIO applications are supported. When a client wishes
 * to call \ref sdcc_sdio_handle_open(), it must specify the application type
 */
typedef enum
{
   SDCC_SDIO_APPLN_WLAN,
   SDCC_SDIO_APPLN_UBM,
   SDCC_SDIO_APPLN_CMMB
} sdcc_sdio_application_type;

/**
 * This enumeration is used to indicate direction of data transfer.
 */
enum sdcc_sdio_dir {
   SDCC_SDIO_READ_FROM_CARD  = 0, /**< card to controller */
   SDCC_SDIO_WRITE_TO_CARD = 1    /**< controller to card */
};

/**
 * These are possible IO transfer formats: byte mode and block mode
 */
enum sdcc_sdio_io_mode
{
   SDCC_SDIO_BYTE_MODE,
   SDCC_SDIO_BLOCK_MODE,
   SDCC_SDIO_IO_MODE_COUNT   /**< Used to check if a valid io_mode is passed*/
}; 

/**
 * Is used to populate CMD52 parameters
 */
typedef struct _sdcc_sdio_handle_cmd52_rw_arg
{
   uint32 fn_num;                 /**< SDIO function index */
   uint32 reg_address;            /**< location of read or write */
   enum sdcc_sdio_dir direction;  /**< direction of transfer */
   boolean read_after_write_flag; /**< if true, read at the same address
                                       after the write */
}sdcc_sdio_handle_cmd52_rw_arg_type;

/**
 * These are the addressing schemes possible for CMD53. Fixed
 * addressing and incremental addressing.
 */
enum sdcc_sdio_op_code
{
   SDCC_SDIO_FIXED_ADDR,       /**< Multibyte R/W the same address */
   SDCC_SDIO_INCREMENTAL_ADDR, /**< Multi-byte R/W to incrementing addresses */
   SDCC_SDIO_OP_CODE_COUNT     /**< Used to check if a valid opcode is passed*/
};

/**
 * Should be used to populate CMD53 parameters
 */
typedef struct _sdcc_sdio_xfer_arg
{
   enum sdcc_sdio_dir direction;    /**< direction of I/O operation */
   uint32 fn_num;                   /**< SDIO function index*/
   enum sdcc_sdio_io_mode blk_mode; /**< transfer format: bytes or blocks*/
   enum sdcc_sdio_op_code op_code;  /**< Fixed or incremental addressing*/
   uint32 reg_address;              /**< address on the device to R/W*/
}sdcc_sdio_xfer_arg_type;

/**
 * CMD53 data should be populated into this structure
 * to write to the device. For read, this structure will be
 * used to hold data read from the device.
 */
typedef struct _sdcc_sdio_xfer_data
{
   uint8 *buff;     /**< points to source / destination data buffer */
   uint32 units;    /**< number of units to transfer
                         units should be specified in bytes for byte mode
                         transfer & in blocks for block mode transfer */
}sdcc_sdio_xfer_data_type;

/**
 * The SDIO client interrupt handler is specified through this object type
 * See \ref sdcc_sdio_handle_set
 */
typedef struct
{
   void *isr;
   void *isr_param;
}sdcc_sdio_attach_intr_type;

/**
 * Supported bus widths
 */
typedef enum
{
   SDCC_SDIO_1BIT = 0,
   SDCC_SDIO_4BIT = 1,
   SDCC_SDIO_BUSWIDTH_TYPE_COUNT /**< Used to check if a valid bus width value 
                                      is passed*/
} sdcc_sdio_buswidth_type;


/**
 * Hardware/target specific information will be returned upon query via
 * \ref sdcc_get_config_info API
 */
typedef struct sdcc_config_info
{
   uint16   num_slots;          /**< total number of card slots */
   uint16   blk_size;           /**< number of bytes in a block supported by
                                 * SDCC controller */
   uint32   max_blks;           /**< maximum number of read/write blocks the
                                 * SDCC controller supports per each data
                                 * transaction */
} sdcc_config_info_type;

/**
 * @ingroup sdcc_api
 * The following legacy driver geometry descriptor structure is used in
 * \ref sdcc_read_serial API.  This structure will be removed once BREW/Hotplug
 * removed the invocation of sdcc_read_serial.
 */
typedef struct drv_geometry_desc
{
   uint32  totalLBA;           /**< Total drive logical blocks */
   uint16  dfltCyl;            /**< Number of cylinders        */
   uint16  dfltHd;             /**< Number of Heads            */
   uint16  dfltSct;            /**< Sectors per track          */
   uint16  dfltBytesPerSect;   /**< Bytes per sector           */
   uint8   serialNum[16];      /**< Drive serial number        */
   uint8   modelNum[32];       /**< Drive model number         */
   uint8   MID;                /**< Manufacturer ID (binary)   */
   uint8   OEMAppID[3];        /**< OEM/Application ID 2 ASCII string */
   uint8   revisionNum;        /**< Product revision number (binary)  */
   uint8   MDateM;             /**< Manufacturing date : month (binary) */
   uint8   MDateY;             /**< Manufacturing date : year (binary)  */
} DRV_GEOMETRY_DESC;

/**
 * @ingroup sdcc_api
 * The following device info structure is used in
 * \ref sdcc_boot_set_device_info API.  This structure provides device
 * specific information from Boot to avoid complete enumeration
 * of eMMC card
 */
typedef struct _sdcc_boot_device_info
{
   int16                            driveno;   /**< drive number */
   SDCC_CARD_TYPE                   card_type; /**< card type SD or MMC */
   uint16                           rca;       /**< relative card address */
   sdcc_buswidth_type               bus_width; /**< data width of SD or MMC */
   sdcc_sd_data_transfer_clock_type clock;     /**< data transfer clock */
   boolean                          is_raw_valid; /**< is below data valid */
   uint32                           raw_cid[4];/**< CID register */
   uint32                           raw_csd[4];/**< CSD register */
   byte                             raw_ext_csd[512];/**< EXT CSD register */
} sdcc_boot_device_info_type;

/**
 * @ingroup sdcc_api
 * memory card specific information will be returned upon query
 */
typedef struct _sdcc_mem_info
{
   SDCC_CARD_TYPE       card_type; /**< card type such as SD or MMC */
   uint32               card_size_in_sectors;
      /**< If the sdcc device handle represents the entire device, card size in
       * sectors denotes the size for the card.\  If the sdcc device handle
       * represents a physical partition, card size in sectors denotes the size
       * of the physical partition.
       * Each sector has 512 bytes */
   uint32               write_protect_group_size_in_sectors;
      /**< size of the write protect group in sectors
       * Each sector has 512 bytes.\  If this is 0, write protect is not
       * supported.\  This write protect group size can vary from 512 bytes to
       * 33292800 kilobytes.\  Refer to sections 8.3 and 8.4 in JEDEC Standard
       * No. 84-A44 for write protect group size calculation details. */
   uint32               block_len; /**< number of bytes per block */
   union {
      sdcc_sd_speed_class_type   sd_speed_class;
         /**< speed class for SD card */
      sdcc_mmc_speed_class_type  mmc_speed_class;
         /**< speed class for MMC card */
   } speed_class;
   uint32               reliable_write_sector_count;
      /**< For eMMC only: the number of reliable write sector count supported */
   boolean              is_embedded_device;
      /**< flag to indicate if device is embedded */
   uint32               num_phy_partition_created;
      /**< number of physical partition created */
   uint32               phy_partition_type;
      /**< type of physical partition to indicate if partition is the user
       * area, boot partition, general purpose partition, etc. */
   boolean              is_phy_partition_bootable;
      /**< flag to indicate if the physical partition is bootable */
   uint16               mfr_id;    /**< manufacturer ID */
   uint16               oem_id;    /**< OEM/application ID */
   byte                 prod_name[7];
      /**< product name with NULL terminator
       * For SD card, product name is 5 characters long
       * For MMC card, product name is 6 characters long */
   uint8                prod_rev;  /**< product revision */
   uint32               prod_serial_num; /**< product serial number */
   byte                 mfr_date[8];
      /**< manufacturing date according to SD Physical Layer Specification and
       * MMC/eMMC System Specification
       * Manufacturing date is in mm/yyyy format. */
} sdcc_mem_info_type;

/**
 * @ingroup sdcc_iovec_api
 * input/output vector structure to be populated by the SDCC client to pass
 * in as an argument in function call \ref sdcc_write_iovec and
 * \ref sdcc_read_iovec
 */
typedef struct _sdcc_memory_iovec_data_desc
{
   uint32 sector_address;
      /**< sector address to start the data transfer */
   uint32 *buffer;
      /**< pointer to data buffer for outgoing data to the memory device or
       * incoming data from the memory device */
   uint32 num_sector;
      /**< number of sectors requested for the data transfer
       * Each sector has 512 bytes. */
} sdcc_memory_iovec_data_desc_type;

/**
 * SDCC device is a private data structure.  This opaque structure type is
 * declared so the APIs can access the memory device via a pointer to this
 * sdcc_device structure.
 */
struct sdcc_device;

/** \details
 * This function gets the hardware/target specific information such as total
 * number of card slots, block size, and maximum number of blocks that the
 * target and SDCC controller support.
 *
 * @param[out] sdcc_pconfig_info
 *    pointer to the structure to save the hardware/target specific information
 *
 * @return SDCC_STATUS: SDCC_NO_ERROR or SDCC_ERR_UNKNOWN
 */
SDCC_STATUS sdcc_get_config_info( sdcc_config_info_type *sdcc_pconfig_info );

/** \details
 * This function checks if the specified drive number can be used for a certain
 * type of device as passed in parameter stype.
 *
 * @param[in] driveno
 *    drive number
 *
 * @param[in] stype
 *    type of slot (memory or IO)
 *
 * @return boolean TRUE if the driveno supports stype device type.
 *    FALSE otherwise
 */
boolean sdcc_slot_has_support( int16 driveno,
                               enum sdcc_slot_supports stype );

/** \details
 * This function tells SDCC that the slot specified by driveno has not been
 * accessed for a certain period of time.  If we have the ability to detect
 * card removal/re-insertion, we shutdown the slot, otherwise do nothing.
 *
 * @param[in] driveno
 *    drive number
 *
 * @return SDCC_NO_ERROR if successful, error code otherwise
 */
SDCC_STATUS sdcc_activity_timeout_event( int16 driveno );
/*@}*/ /* group sdcc_driver */

/**
 * @defgroup sdcc_api SDCC API Overview
 * @ingroup sdcc_driver
 *
 * The SDCC API provides a list of public functions to SDCC clients such as
 * Hotplug that perform data transfer to/from memory device such as SD or MMC
 * card.
 */
/*@{*/
/*SDCC_EXTRACT_COMMON_BEGIN*/
/** \details
 * This function initializes the SDCC controller.  It first turns on the clock,
 * configures the GPIOs and then turns on the power supply to the device.  It
 * also initializes the related software data structures.
 *
 * @param[in] driveno
 *    drive number
 *
 * @note
 *    This function will not be exposed after handle-based API is in place.
 *    This function remains in this header file because of backward
 *    compatibility.
 *
 * @return TRUE if successful.  FALSE if failed
 */
boolean sdcc_init( int16 driveno );
/*SDCC_EXTRACT_COMMON_END*/

/** \details
 * This function returns FALSE if there is no memory device attached.  If there
 * is a memory device, it will initialize the device and take it out of the
 * identification state.  It then sets up the device for data transfer.
 *
 * @param[in] driveno
 *    drive number
 *
 * @return TRUE if successful.  FALSE if failed.
 */
boolean sdcc_open( int16 driveno );

/** \details
 * This function de-selects the device, turn off the power supply to the device
 * and switches off the controller clock.
 *
 * @param[in] driveno
 *    drive number
 *
 * @return TRUE if successful.  FALSE if failed.
 */
boolean sdcc_close( int16 driveno );

/** \details
 * This function gets the memory card specific information.  If there is no
 * memory device attached or device is not initialized, all fields in the card
 * specific information are zero.
 *
 * @remarks
 *    \ref sdcc_open must be called before getting card info for the specified
 *    drive number.
 *
 * @param[in] driveno
 *    a valid drive number between 0 and the total number of card slots - 1
 *
 * @param[out] sdcc_pmem_info
 *    pointer to the structure to save the memory card specific information
 *
 * @return SDCC_NO_ERROR on success.  Error code otherwise
 */
SDCC_STATUS sdcc_mem_get_device_info( int16 driveno, 
                                      sdcc_mem_info_type *sdcc_pmem_info );

/** \details
 * This function is to read some drive information.
 *
 * @remarks
 *    \ref sdcc_open must be called before reading the drive information for
 *    the specified drive number.
 *
 * @param[in] driveno
 *    a valid drive number between 0 and the total number of card slots - 1
 *                        
 * @param[out] iDrvPtr
 *    pointer to the drive geometry descriptor that is to be filled
 *
 * @return TRUE if successful.  FALSE if failed.
 *
 * @note
 *    This function will be removed once BREW/Hotplug removed the invocation of
 *    this function.
 */
boolean sdcc_read_serial( int16 driveno,
                          DRV_GEOMETRY_DESC *iDrvPtr );

/** \details
 * This function reads \c num_blocks blocks of data from the \c data_address in
 * the device, storing them at the memory location given by \c buff.
 *
 * @param[in] driveno
 *    drive number
 *
 * @param[in] data_address
 *    start of the sector.  This is the sector unit address (512 bytes per
 *    sector).
 *
 * @param[out] buff
 *    pointer to buffer for the incoming data
 *
 * @param[in] num_blocks
 *    number of blocks to be read (512 bytes per block)
 *
 * @return TRUE if successful.  FALSE if failed.
 *
 * @note
 *    Multiblock transfer is used when more than 1 block data is transfered.
 */
boolean sdcc_read( int16    driveno,
                   uint32   data_address,
                   byte    *buff,
                   uint16   num_blocks );

/** \details
 * This function writes \c num_blocks blocks of data, obtained from the memory
 * location \c buff, to the \c data_address on the device.
 *
 * @param[in] driveno
 *    drive number
 *
 * @param[in] data_address
 *    start of the sector.  This is the sector unit address (512 bytes per
 *    sector).
 *
 * @param[in] buff
 *    pointer to buffer for the outgoing data
 *
 * @param[in] num_blocks
 *    number of blocks to be written (512 bytes per block)
 *
 * @return TRUE if successful.  FALSE if failed.
 *
 * \note
 *    Multiblock transfer is used when more than 1 block data is transfered.
 */
boolean sdcc_write( int16    driveno,
                    uint32   data_address,
                    byte    *buff,
                    uint16   num_blocks );

/** \details
 * This function sets \c device_info structure, obtained from boot
 * to be used for optimized device enumeration at boot.
 *
 * @param[in] device_info
 *    pointer to device information from boot
 */
void sdcc_boot_set_device_info( sdcc_boot_device_info_type *device_info );

/** \details
 * This function gets \c device_info structure, from driver
 * to be used for optimized device enumeration at boot.
 *
 * @param[in] device_info
 *    pointer to device information from boot
 */
void sdcc_boot_get_device_info( sdcc_boot_device_info_type *device_info );
/*@}*/ /* group sdcc_api */

/**
 * @defgroup sdcc_iovec_api SDCC I/O Vector API
 * @ingroup sdcc_api 
 *
 * The SDCC I/O Vector API allows the SDCC clients to request a list of data
 * transfers.
 */
/*@{*/
/** \details
 * This function reads data from the memory device indicated in the \c driveno
 * parameter.  Caller is required to populate the fields in structure variable
 * \c iovec which includes sector address and number of sectors.  Field buffer
 * must be big enough to hold the data read from the memory device.  It places
 * the data read from memory device into the \c iovcnt buffers specified by the
 * members of the \c iovec array: \c iovec[0], \c iovec[1], ...,
 * \c iovec[iovcnt-1].  Parameter \c iovec_xfer must be specified by the
 *  caller.
 *
 * @param[in] driveno
 *    a valid drive number between 0 and the total number of card slots - 1
 *                          
 * @param[in] iovec
 *    array of I/O vector data descriptors that contain pointer to buffer for
 *    incoming data from the memory device
 *
 * @param[in] iovcnt
 *    number of elements in the iovec array
 *
 * @param[in] iovec_xfer
 *    transfer type for the scatter gather data request
 *
 * @return SDCC_NO_ERROR on success.  Error code otherwise
 */
SDCC_STATUS sdcc_read_iovec(
   int16 driveno,
   const sdcc_memory_iovec_data_desc_type *iovec,
   uint32 iovcnt,
   sdcc_memory_iovec_transfer_type iovec_xfer);

/** \details
 * This function gathers data from the \c iovcnt buffers specified by the
 * members of the \c iovec array: \c iovec[0], \c iovec[1], ...,
 * \c iovec[iovcnt-1].  It writes data to the memory device indicated in the
 * \c driveno parameter.  Caller is required to populate the fields in
 * structure variable \c iovec which includes sector address, data buffer and
 * number of sectors.  Parameter \c iovec_xfer must be specified by the caller.
 *
 * @param[in] driveno
 *    a valid drive number between 0 and the total number of card slots - 1
 *
 * @param[in] iovec
 *    array of I/O vector data descriptors that contain the data to be written
 *    to the memory device
 *
 * @param[in] iovcnt
 *    number of elements in the iovec array
 *
 * @param[in] iovec_xfer
 *    transfer type for the scatter gather data request
 *
 * @return SDCC_NO_ERROR on success.  Error code otherwise
 */
SDCC_STATUS sdcc_write_iovec(
   int16 driveno,
   const sdcc_memory_iovec_data_desc_type *iovec,
   uint32 iovcnt,
   sdcc_memory_iovec_transfer_type iovec_xfer);
/*@}*/ /* group sdcc_iovec_api */

/**
 * @defgroup sdcc_handle_api SDCC Handle API
 * @ingroup sdcc_driver
 *
 * The SDCC Handle API provides a list of public functions to SDCC clients
 * such as Hotplug that perform data transfer to/from memory device such as
 * SD or MMC card.
 */
/*@{*/
/** \details
 * This function first initializes the SDCC controller such as turning on the
 * clock, configuring the GPIOs, and turning on the power supply to the device.
 * It returns NULL if there is no memory device attached.  If there
 * is a memory device, it will initialize the device and take it out of the
 * identification state.  It then sets up the device for data transfer.
 * A non-null pointer to the SDCC device is returned.
 *
 * @param[in] driveno
 *    a valid drive number between 0 and the total number of card slots - 1
 *
 * @param[in] phy_partition_num
 *    physical partition number.  Use
 *    \ref SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE to get a handle
 *    representing the entire memory device.  For eMMC v4.3, the valid boot
 *    partition numbers are 1 and 2.  User area partition number is 0.
 *
 * @return A pointer to the SDCC device if successful.  NULL if failed.
 *
 * @note
 *    sdcc_init is called in sdcc_handle_open
 */
struct sdcc_device *sdcc_handle_open( int16 driveno, uint32 phy_partition_num );

/** \details
 * This function de-selects the device, turn off the power supply to the device
 * and switches off the controller clock.
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @return SDCC_NO_ERROR if successful.  Error code if failed.
 */
SDCC_STATUS sdcc_handle_close( struct sdcc_device *handle );

/** \details
 * This function gets the memory card specific information.  If there is no
 * memory device attached or device is not initialized, all fields in the card
 * specific information are zero.
 *
 * @remarks
 *    \ref sdcc_handle_open must be called before getting card info for the
 *    specified SDCC handle.
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @param[out] sdcc_pmem_info
 *    pointer to the structure to save the memory card specific information
 *
 * @return SDCC_NO_ERROR on success.  Error code otherwise
 */
SDCC_STATUS
sdcc_handle_mem_get_device_info( struct sdcc_device *handle, 
                                 sdcc_mem_info_type *sdcc_pmem_info );

/** \details
 * This function gets the card status of the specified SDCC handle.
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @return TRUE if memory device is present.  FALSE otherwise.
 */
boolean sdcc_handle_is_mem_present( struct sdcc_device *handle );

/** \details
 * This function sets a bootable partition on the embedded device specified in
 * the SDCC \c handle.  Only one partition can be set as active bootable
 * partition -- boot partition 1, boot partition 2, or user area.
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @return SDCC_NO_ERROR on success.  Error code otherwise
 */
SDCC_STATUS
sdcc_handle_set_active_bootable_partition( struct sdcc_device *handle );

/** \details
 * This function sets the power-on write protection bit of the addressed group
 * provided the memory device specified in the SDCC \c handle is an embedded
 * device and write protect feature is supported on the device.  The power-on
 * write protection is set based on the write protect group.  The write protect
 * group size is memory device specific.  This group size information 
 * \ref write_protect_group_size_in_sectors can be queried via
 * \ref sdcc_handle_mem_get_device_info.  For boot area partitions, power-on
 * write protection would be applied to the entire partition and both boot
 * areas would be power-on write protected.
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @param[in] data_address
 *    start of the sector to get power-on write protected.  This is the sector
 *    unit address.  This is ignored for boot partition.
 *
 * @param[in] num_of_blocks
 *    number of blocks to be power-on write protected (512 bytes per block).
 *    For user area partition, this is based on the write protect group size.
 *    If num_of_blocks < write protect group size, the entire write protect
 *    group is write protected.
 *    This is ignored for boot partition.
 *
 * @return SDCC_NO_ERROR on success.  Error code otherwise
 *
 * @note
 *    After a power failure or hardware reset, the power-on write protection
 *    must be reapplied.
 */
SDCC_STATUS
sdcc_handle_set_power_on_write_protection( struct sdcc_device *handle,
                                           uint32 data_address,
                                           uint32 num_of_blocks );

/** \details
 * This function sets the SPS end-point for data transfer.  SDCC client that
 * uses SPS for data transfer is required to call this function before invoking
 * the read/write function.
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @param[in] end_point
 *    the SPS end-point to be configured for data transfer
 *
 * @return SDCC_NO_ERROR if successful.  Error code otherwise.
 */
SDCC_STATUS sdcc_handle_set_sps_end_point( struct sdcc_device *handle,
                                           void *end_point );

/** \details
 * This function gets the SPS end-point used for data transfer for the
 * specified client handle.
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @param[in] end_point
 *    the SPS end-point that is used for data transfer
 *
 * @return SDCC_NO_ERROR if successful.  Error code otherwise.
 */
SDCC_STATUS sdcc_handle_get_sps_end_point( struct sdcc_device *handle,
                                           void *end_point );

/** \details
 * This function reads \c num_blocks blocks of data from the \c data_address in
 * the device, storing them at the memory location given by \c buff.
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @param[in] data_address
 *    start of the sector.  This is the sector unit address (512 bytes per
 *    sector).
 *
 * @param[out] buff
 *    pointer to buffer for the incoming data
 *
 * @param[in] num_blocks
 *    number of blocks to be read (512 bytes per block)
 *
 * @return SDCC_NO_ERROR if successful.  Error code otherwise.
 *
 * @note
 *    Multiblock transfer is used when more than 1 block data is transfered.
 */
SDCC_STATUS sdcc_handle_read( struct sdcc_device *handle,
                              uint32              data_address,
                              byte               *buff,
                              uint16              num_blocks );

/** \details
 * This function writes \c num_blocks blocks of data, obtained from the memory
 * location \c buff, to the \c data_address on the device.
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @param[in] data_address
 *    start of the sector.  This is the sector unit address (512 bytes per
 *    sector).
 *
 * @param[in] buff
 *    pointer to buffer for the outgoing data
 *
 * @param[in] num_blocks
 *    number of blocks to be written (512 bytes per block)
 *
 * @return SDCC_NO_ERROR if successful.  Error code otherwise.
 *
 * \note
 *    Multiblock transfer is used when more than 1 block data is transfered.
 */
SDCC_STATUS sdcc_handle_write( struct sdcc_device *handle,
                               uint32              data_address,
                               byte               *buff,
                               uint16              num_blocks );

/** \details
 * This function reads data from the memory device indicated in the \c handle
 * parameter.  Caller is required to populate the fields in structure variable
 * \c iovec which includes sector address and number of sectors.  Field buffer
 * must be big enough to hold the data read from the memory device.  It places
 * the data read from memory device into the \c iovcnt buffers specified by the
 * members of the \c iovec array: \c iovec[0], \c iovec[1], ...,
 * \c iovec[iovcnt-1].  Parameter \c iovec_xfer must be specified by the
 * caller.
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *                          
 * @param[in] iovec
 *    array of I/O vector data descriptors that contain pointer to buffer for
 *    incoming data from the memory device
 *
 * @param[in] iovcnt
 *    number of elements in the iovec array
 *
 * @param[in] iovec_xfer
 *    transfer type for the scatter gather data request
 *
 * @return SDCC_NO_ERROR on success.  Error code otherwise
 */
SDCC_STATUS sdcc_handle_read_iovec(
   struct sdcc_device *handle,
   const sdcc_memory_iovec_data_desc_type *iovec,
   uint32 iovcnt,
   sdcc_memory_iovec_transfer_type iovec_xfer);

/** \details
 * This function gathers data from the \c iovcnt buffers specified by the
 * members of the \c iovec array: \c iovec[0], \c iovec[1], ...,
 * \c iovec[iovcnt-1].  It writes data to the memory device indicated in the
 * \c handle parameter.  Caller is required to populate the fields in structure
 * variable \c iovec which includes sector address, data buffer and number of
 * sectors.  Parameter \c iovec_xfer must be specified by the caller.
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @param[in] iovec
 *    array of I/O vector data descriptors that contain the data to be written
 *    to the memory device
 *
 * @param[in] iovcnt
 *    number of elements in the iovec array
 *
 * @param[in] iovec_xfer
 *    transfer type for the scatter gather data request
 *
 * @return SDCC_NO_ERROR on success.  Error code otherwise
 */
SDCC_STATUS sdcc_handle_write_iovec(
   struct sdcc_device *handle,
   const sdcc_memory_iovec_data_desc_type *iovec,
   uint32 iovcnt,
   sdcc_memory_iovec_transfer_type iovec_xfer);

/** \details
 * This function is to erase data on the device, the commands used
 * to perform the erase operation should only be used on eMMC cards
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @param[in] start_addr
 *    the address of the first sector to be erased
 *
 * @param[in] num_blocks
 *    number of blocks to be erased
 *
 * @return SDCC_NO_ERROR on success.  Error code otherwise
 */
SDCC_STATUS sdcc_handle_erase (struct sdcc_device *handle,
                               uint32 start_addr,
                               uint32 num_blocks );

/** \details
 * This function is to force erase data on the device, the commands used
 * to perform the erase operation should only be used on Locked cards
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @return SDCC_NO_ERROR on success.  Error code otherwise
 */
SDCC_STATUS sdcc_handle_force_erase (struct sdcc_device *handle);
/*@}*/ /* group sdcc_handle_api */



/**
 * @defgroup sdio_api SDIO API Overview
 * @ingroup sdcc_driver
 *
 * SDIO API provides a list of public functions to SDCC clients such as WLAN
 * that perform data transfer to/from the SDIO device.
 */
/*@{*/
/** \details
 * sdcc_sdio_set_max_speed is to configure the maxclk limit for a specific
 * SDIO client application.
 *
 * This is an optional API for SDIO clients who do not have the below issue:
 * SDIO device responds positively for High Speed request but it is NOT
 * supposed to (or) capable of supporting HS. In such case, it is expected
 * that such clients call this API before they open SDCC handle
 * via sdcc_sdio_handle_open and set the maxclk that device can support.
 * Based on the maxclk setting, driver will decide to switc to HS or not.
 * 
 * @param[in] sdio_appln
 *    a supported SDIO function of type sdcc_sdio_application_type
 *
 * @param[in] maxclk
 *    The maxclk SDIO device can support
 *
 * @return void
 */
void sdcc_sdio_set_max_speed(
   sdcc_sdio_application_type sdio_appln,
   sdcc_sdio_data_transfer_clock_type maxclk);

/** \details
 * sdcc_sdio_handle_open first initializes the SDCC controller HW parameters - turning
 * on the bus clock, configuring the GPIOs, and turning on the power supply. 
 * The slot to be used is determined by \c sdio_appln and statically
 * stored information in the SDCC BSP.
 * 
 * Optionally, if the calling client wishes to perform vendor specific 
 * initialization,the client should pass in a pointer to such a function through
 * \c appln_specific_init_fptr. This function is called after powering SDCC HW
 * but before card detection.
 * 
 * Note that the clients initialization is called synchronously, and its return
 * type is ignored.
 * 
 * Finally, sdcc_sdio_handle_open() detects the card type & if an SDIO card is detected,
 * it performs some basic SDIO initialization : reading the tuples, 
 * setting block size, setting clock speed, etc and returns a handle.
 * 
 * @param[in] sdio_appln
 *    a supported SDIO function of type sdcc_sdio_application_type
 *
 * @param[in] manfid
 *    The manufacturer ID of the device the client wishes to open.
 *    If the ID of the card does not match this, the card will be powered off & 
 *    NULL will be returned.
 *
 * @param[in] appln_specific_init_fptr
 *    pointer to a function to perform vendor specific initialization
 *
 * @param[in] appln_specific_init_param
 *    pointer to argument to be passed to appln_specific_init_fptr
 *
 * @return SDCC handle if SDIO card was detected. NULL otherwise.
 */
struct sdcc_device *sdcc_sdio_handle_open(
   sdcc_sdio_application_type sdio_appln,
   uint32 manfid,
   void (*appln_specific_init_fptr)(void *),
   void *appln_specific_init_param);

/** \details
 * Read or write one byte from/to the SDIO device.  It is
 * commonly used to program the device register map.
 *
 * @param[in] handle
 *    a pointer to the SDCC handle that was returned by sdcc_sdio_handle_open
 *
 * @param[in] cmd52_arg
 *    pointer to the CMD52 fields structure
 *
 * @param[in] pdata
 *    For write: Pointer to byte to be written. If the read_after_write
 *    flag in cmd52_arg is TRUE the byte read back will be stored at *pdata.
 *    For read: pointer to location where byte read will be stored.
 *
 * @return SDCC_NO_ERROR if successful, error code otherwise.
 */
SDCC_STATUS sdcc_sdio_handle_cmd52_rw(
   struct sdcc_device *handle,
   const sdcc_sdio_handle_cmd52_rw_arg_type *cmd52_arg,
   byte *pdata);

/** \details
 * Synchronous (Blocking) CMD53 data transfer from client to SDIO device
 * or from device to client as specified by the direction field in 
 * \c xfer_arg_type. To write to the device, the client should populate an array 
 * of sdcc_sdio_xfer_data_type structures and pass it as \c dptr. 
 * The number of elements in the array should be stored in \c num_elements.
 * 
 * @param[in] handle
 *    a pointer to the SDCC handle that was returned from sdcc_sdio_handle_open()
 *
 * @param[in] xfer_arg
 *    pointer to the CMD53 fields structure
 *
 * @param[in] dptr
 *    array of sdcc_sdio_xfer_data_type. 
 *
 * @param[in] num_elements
 *    number of elements in the data array
 *
 * @return status code
 */
SDCC_STATUS sdcc_sdio_handle_xfer_sync(
   struct sdcc_device *handle,
   const sdcc_sdio_xfer_arg_type *xfer_arg,
   sdcc_sdio_xfer_data_type *dptr,
   uint32 num_elements);

/** \details
 * This function is provided to set various SDIO device features.
 *
 * @param[in] handle
 *    a pointer to the SDCC handle that was returned from sdcc_sdio_handle_open()
 *
 * @param[in] fn_num
 *    device function number
 *
 * @param[in] set_feature
 *    feature to be set
 *
 * @param[in] pdata
 *    Pointer to data associated with the feature. The following table
 * describes what should be passed in thru pdata for each set feature
 * 
 * <table>
 * <tr>
 * <td><b>Feature to be set</b></td><td><b>Object to be passed in by reference</b></td>
 * </tr>
 * <tr>
 * <td>SD_IO_FUNCTION_SET_BLOCK_SIZE</td><td>uint32 specifying block size in bytes</td>
 * </tr>
 * <tr>
 * <td>SD_SET_BLOCK_MODE</td><td>enum sdcc_sdio_io_mode</td>
 * </tr>
 * <tr>
 * <td>SD_SET_OP_CODE</td><td>enum sdcc_sdio_op_code</td>
 * </tr>
 * <tr>
 * <td>SD_SET_BUS_WIDTH</td><td>sdcc_sdio_buswidth_type</td>
 * </tr>
 * <tr>
 * <td>SD_IO_ATTACH_ISR</td><td>sdcc_sdio_attach_intr_type</td>
 * </tr>
 * <tr>
 * <td>SD_SET_DATA_TRANSFER_CLOCKS</td><td>sdcc_sdio_data_transfer_clock_type</td>
 * </tr>
 * <tr>
 * <td>Others</td><td>NULL (pdata is ignored)</td>
 * </tr>
 * </table>
 * 
 * @return status code
 */
SDCC_STATUS sdcc_sdio_handle_set(
   struct sdcc_device *handle,
   uint8               fn_num,
   SDIO_SET_FEATURE_TYPE  set_feature,
   void               *pdata );

/* SDIO legacy functions */

/** \details
 * This function checks to see if we can exit early if the drive is in use.
 * If it is not in use, initialize the SDCC controller.
 *
 * @param[in] driveno
 *    drive number
 *
 * @param[in] dev_manfid
 *    SDIO manufacturer identification
 *
 * @return card type in use (SDCC_CARD_UNKNOWN, in case it is not in use)
 */
SDCC_CARD_TYPE sdio_pre_init( int16    driveno,
                              uint16   dev_manfid );

/** \details
 * This function looks for the specified device function implemented on the
 * device.  If the device is found as mentioned in "card type", it then will
 * configure the device and enable it.
 *
 * @param[in] driveno
 *    drive number
 *
 * @param[in] dev_manfid
 *    SDIO manufacturer identification
 *
 * @param[in] card_type
 *    card type from SDCC_CARD_TYPE
 *
 * @return error code
 */
SDCC_STATUS sdio_post_init( int16             driveno,
                            uint16            dev_manfid,
                            SDCC_CARD_TYPE    card_type );

/** \details
 * 
 * sdcc_sdio_init
 * 
 * OBSOLETE FUNCTION
 * 
 * Used formerly by NXP WLAN client and is preserved
 * only for compilation purposes.
 *
*/
SDCC_STATUS sdcc_sdio_init( int16    driveno,
                            uint16   dev_manfid );

/** \details
 * This function is to find out the type of the device.
 *
 * @param[in] driveno
 *    drive number
 *
 * @return card type
 */
SDCC_CARD_TYPE sdcc_find_card( int16 driveno );

/** \details
 * Read or write one byte to/from SDIO device function \c fn_num.  It is
 * commonly used to program the device register map.
 *
 * @param[in] driveno
 *    Controller ID
 *
 * @param[in] fn_num
 *    device function number
 *
 * @param[in] dir
 *    direction, SDCC_DATA_READ or SDCC_DATA_WRITE
 *
 * @param[in] reg_addr
 *    register address
 *
 * @param[in] pdata
 *    pointer to byte to be written.  Not used for read.
 *
 * @return SDCC_NO_ERROR if successful, error code else.
 */
SDCC_STATUS sdcc_sdio_cmd52( int16    driveno,
                             uint8    fn_num,
                             uint32   dir,
                             uint32   reg_addr,
                             uint8   *pdata );

/** \details
 * Write data to the SDIO device via CMD53
 *
 * @param[in] driveno
 *    Controller index
 *
 * @param[in] reg_addr
 *    register address on SDIO device
 *
 * @param[in] units
 *    number of blocks
 *
 * @param[in] buff
 *    pointer to outgoing data
 *
 * @return status code
 */
SDCC_STATUS sdcc_sdio_write( int16    driveno,
                             uint32   reg_addr,
                             uint16   units,
                             uint8   *buff );

/** \details
 * Read in data from the SDIO device via CMD53
 *
 * @param[in] driveno
 *    controller index
 *
 * @param[in] reg_addr
 *    register address on SDIO device
 *
 * @param[in] units
 *    number of blocks to be read
 *
 * @param[out] buff
 *    pointer to buffer for the incoming data 
 *
 * @return status code
 */
SDCC_STATUS sdcc_sdio_read( int16    driveno,
                            uint32   reg_addr,
                            uint16   units,
                            uint8   *buff );

/** \details
 * This function is provided to set various SDIO device features.  Currently
 * supported features are: enable/disable function, set block mode/block size
 * and set bus width.
 *
 * @param[in] driveno
 *    drive number
 *
 * @param[in] fn_num
 *    device function number
 *
 * @param[in] type
 *    features to be set
 *
 * @param[in] pdata
 *    data associated with the feature
 *
 * @return status code
 */
SDCC_STATUS sdcc_sdio_set( int16                   driveno,
                           uint8                   fn_num,
                           SDIO_SET_FEATURE_TYPE   type,
                           void                   *pdata );

/** \details
 * 
 * This function is OBSOLETE and is preserved only
 * to successfully compile
 *
 */
SDCC_STATUS sdcc_sdio_get( int16                driveno,
                           uint32               type,
                           void                *pdata,
                           uint32               size );

/** \details
 * This function enables the controller interrupt, writes to the device to
 * enable the interrupt for \c driveno and register an interrupt handle for the
 * device.
 *
 * @param[in] driveno
 *    drive number
 *
 * @param[in] isr
 *    pointer to the ISR function from OEM
 *
 * @param[in] isr_param
 *    params for the ISR
 */
void sdcc_sdio_connect_intr( int16 driveno,
                             void *isr,
                             void *isr_param );

/** \details
 * This function disconnects the SDIO interrupt.  This should be called at
 * clean up time if the interrupt connection is called earlier.
 *
 * @param[in] driveno
 *    drive number
 */
void sdcc_sdio_disconnect_intr( int16 driveno );

/** \details
 * This function disables SDIO interrupts.
 *
 * @param[in] driveno
 *    drive number
 */
void sdcc_sdio_disable_int( int16 driveno );

/** \details
 * This function enables SDIO interrupts.
 *
 * @param[in] driveno
 *    drive number
 */
void sdcc_sdio_enable_int( int16 driveno );
/*@}*/ /* group sdio_api */
#endif /* __SDCC_API_H */

