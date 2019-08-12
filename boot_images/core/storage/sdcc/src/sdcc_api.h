#ifndef __SDCC_API_H
#define __SDCC_API_H
/******************************************************************************
 * sdcc_api.h
 *
 * This file provides SDCC external definitions.
 *
 * Copyright (c) 2007-2014
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/sdcc_api.h#1 $
  $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
2014-03-11   bn      Added get_slot_type to query slot info using driveno
2011-08-30   rh      Added GPP partitioning support for eMMC
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
 * This flag indicates support for getting the slot type information 
 * (emmc/sd and internal/external)
 */
#define FEATURE_SDCC_GET_SLOT_TYPE_SUPPORTED

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
 * Following are the memory device slot types that are supported by the SDCC driver.
 */
typedef enum
{
   SDCC_SLOT_UNKNOWN = 0, /**< error. unknown slot type */
   SDCC_SLOT_EMMC    = 1, /**< eMMC slot */
   SDCC_SLOT_SD      = 2 /**< external SD card slot */
} sdcc_slot_mem_type;

/**
 * Following are the slot access (external or internal) configuations 
 * that are supported by the SDCC driver.
 */
typedef enum
{
   SDCC_SLOT_INVALID  = 0, /**< error. invalid slot */
   SDCC_SLOT_INTERNAL = 1, /**< Internal slot */
   SDCC_SLOT_EXTERNAL = 2  /**< External slot */
} sdcc_slot_access_type;

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
 */

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
 * @ingroup sdcc_api
 * The following structure is used to describe the configuration of the General
 * Purpose Partition and Enhanced User Data Area for eMMC memory devices.  The 
 * structure is expected to be populated with configuration information to be 
 * passed to the sdcc_prog_emmc_gpp_enh function to program the GPP and ENH 
 * configuration information.  Setting the size to 0 indicate the partition
 * is not used.
 */
typedef struct sdcc_emmc_gpp_enh_desc
{
   uint32  GPP_size[4];        /**< Size of the GPP partitions in sectors */
   uint32  ENH_size;           /**< Size of enhanced area in sectors      */
   uint32  ENH_start_addr;     /**< Start of the enhanced area in sectors  */
   uint8   GPP_enh_flag;       /**< Enhanced area flag         */
} sdcc_emmc_gpp_enh_desc_type;

#define SDCC_ENH_FLAG_USER_AREA     0x01   /**< Enhanced attribute for user area */
#define SDCC_ENH_FLAG_GPP_1         0x02   /**< Enhanced attribute for GPP 1 */
#define SDCC_ENH_FLAG_GPP_2         0x04   /**< Enhanced attribute for GPP 2 */
#define SDCC_ENH_FLAG_GPP_3         0x08   /**< Enhanced attribute for GPP 3 */
#define SDCC_ENH_FLAG_GPP_4         0x10   /**< Enhanced attribute for GPP 4 */

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
 * This function queries the SDCC slot information
 *
 * @param[in] driveno
 * @param[out] sdcc_slot_mem_type
 *      pointer to memory slot type. Slot is emmc or sd memory device
 * @param[out] sdcc_slot_access_type
 *      pointer to slot access info. Slot is for external or internal device
 *
 * @return SDCC_STATUS: SDCC_NO_ERROR or SDCC_ERR_UNKNOWN
 */
SDCC_STATUS sdcc_get_slot_type( int16 driveno, sdcc_slot_mem_type* slot_mem_type,
                                sdcc_slot_access_type* slot_access_type );
								
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

/** \details
 * This function sets \c device_info structure, obtained from boot
 * to be used for optimized device enumeration at boot.
 *
 * @param[in] device_info
 *    pointer to device information from boot
 */
void sdcc_boot_set_device_info( sdcc_boot_device_info_type *device_info );

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

/** \details
 * This function configures the General Purpose Partition and 
 * enhanced user data area information
 * Note: This routine can only be used once for the life of each
 * emmc chip!
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @param[in] gpp_enh_desc
 *    a pointer to the GPP/ENH description struction that contains partition info
 *
 * @return SDCC_NO_ERROR on success.  Error code otherwise
 */
SDCC_STATUS sdcc_config_emmc_gpp_enh (struct sdcc_device *handle,
                                      sdcc_emmc_gpp_enh_desc_type *gpp_enh_desc);
/*@}*/ /* group sdcc_handle_api */
#endif /* __SDCC_API_H */

