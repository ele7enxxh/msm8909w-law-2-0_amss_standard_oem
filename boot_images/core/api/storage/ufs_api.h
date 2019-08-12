#ifndef __UFS_API_H
#define __UFS_API_H
/******************************************************************************
 * ufs_api.h
 *
 * This file provides UFS external API definitions.
 *
 * Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/storage/ufs_api.h#1 $
  $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ---------------------------------------------------
2013-09-09   rh      Adding a bitfield to indicate which LUN is enabled
2013-09-04   rh      Adding enhanced area capacity adjustment factor
2012-12-18   rh      Initial creation

=============================================================================*/

#include "stdint.h"

/** \mainpage UFS Card Controller API Overview
 *
 * Universal Flash Storage (UFS) provides a simple set of public APIs for 
 * reading and writing to the memory device.  A second group of APIs provides 
 * access to Enhanced card capabilities.
 *
 */

/**
 * @defgroup ufs_driver UFS Card Driver 
 *
 * The UFS driver provides block-oriented data transfer to or from the memory
 * device. 
 */

/*@{*/

/**
 * The drive id are of int32 type.  UFS_DRIVEID_0 is the default card slot.
 */
#define UFS_DRIVEID_0   0

/**
 * Define all the basic WLUN type
 */
#define UFS_WLUN_REPORT          0x81
#define UFS_WLUN_DEVICE          0xD0
#define UFS_WLUN_BOOT            0xB0
#define UFS_WLUN_RPMB            0xC4

/* Old define, keepting it for compatability */
#define UFS_UFS_DEVICE           0xD0

/**
 * Define all LUN that can be set as BOOTABLE
 */
#define UFS_LUN_BOOT_DISABLE     0x00
#define UFS_LUN_BOOT_A           0x01
#define UFS_LUN_BOOT_B           0x02

/**
 * Define the error code used for the public API
 */
#define UFS_EOK                  0
#define UFS_ENOENT               2
#define UFS_EIO                  5
#define UFS_EBUSY                16
#define UFS_ENODEV               19
#define UFS_EINVAL               22
#define UFS_EPROTO               100
#define UFS_ETIME                101

/**
 * UFS device handel is a private data structure.  This opaque structure type is
 * declared so the APIs can access the memory device via a pointer to this
 * ufs_handle structure.
 */
struct ufs_handle;

/**
 * ufs_info_type data stucture is used to return UFS device specific
 * information back to the caller of the ufs_get_device_info API. The
 * name of each enteries in this data structure is identical to the one
 * used in the UFS specification for easy cross reference.
 */
struct ufs_info_type {
   uint8_t     fInitialized;           /**< 1 for initialized hardware */
   uint8_t     bLUN;                   /**< Current LUN number, BOOT LU is translated to normal LU */
   uint8_t     bNumberLu;              /**< Total number of active LU */
   uint16_t    wManufacturerID;        /**< Manufacturer ID */
   uint8_t     bBootEnable;            /**< 1 for device with BOOT partition enabled */
   uint64_t    qTotalRawDeviceCapacity;/**< Total RAW capacity of the device in unit of 512 bytes */
   uint8_t     bMinAddrBlockSize;      /**< Minimum block size in unit of 512 bytes */
   uint32_t    dSegmentSize;           /**< Erase block size */
   uint8_t     bAllocationUnitSize;    /**< Value expressed in number of segments */
   uint32_t    bRPMB_ReadWriteSize;    /**< Maximum RPMB transfer for each command, unit of 256 bytes */
   uint32_t    dNumAllocUnits;         /**< Number of allocation unit for this LU */
   uint8_t     bLogicalBlockSize;      /**< See Section 13.2.3 of UFS standard */
   uint8_t     bProvisioningType;      /**< >0 for enabling of thin provisioning */
   uint8_t     bLUWriteProtect;        /**< 1 for power on WP, 2 for permanent WP */
   uint8_t     bBootLunID;             /**< Which boot LUN is this LU mapped to */
   uint8_t     bMemoryType;            /**< >0 for enhanced memory type */
   uint32_t    dLuTotalBlocks;         /**< Calculated from dNumAllocUnits, unit in blocks */
   uint16_t    wSupportedMemoryTypes;  /**< Types of memory supported by device */
   uint32_t    dEnhanced1MaxNAllocU;   /**< Max allocation for Enhanced memory type 1 */
   uint16_t    wEnhanced1CapAdjFac;    /**< Capacity adjustment factor, Enh Memory type 1 */
   uint32_t    dEnhanced2MaxNAllocU;   /**< Max allocation for Enhanced memory type 2 */
   uint16_t    wEnhanced2CapAdjFac;    /**< Capacity adjustment factor, Enh Memory type 2 */
   uint32_t    dLunEnabled;            /**< Bit mask of which LUN is enabled, LSB = LU0 */
};

/**
 * ufs_config_descr data structure reflects the UFS configuration descriptor
 * layout described by the UFS specification.  The structure contains
 * the user configurable field of the UFS configuration descriptor field
 * which is programmed once in the device's life to configure the LU layout
 */
struct ufs_unit_descr {
   uint8_t     bLUEnable;              /**< 1 for enabled LU */
   uint8_t     bBootLunID;             /**< >0 for using this LU for boot */
   uint8_t     bLUWriteProtect;        /**< 1 = power on WP, 2 = permanent WP */
   uint8_t     bMemoryType;            /**< >0 for enhanced memory type */
   uint32_t    dNumAllocUnits;         /**< Number of allocation unit for this LU */
   uint8_t     bDataReliability;       /**< >0 for reliable write support */
   uint8_t     bLogicalBlockSize;      /**< See section 13.2.3 of UFS standard */
   uint8_t     bProvisioningType;      /**< >0 for thin provisioning */
   uint16_t    wContextCapabilities;   /**< See Unit Descriptor Description of UFS standard */
};

struct ufs_config_descr {
   uint8_t     bNumberLU;              /**< Total number of active LUs */
   uint8_t     bBootEnable;            /**< 1 for enabling device for partial init */
   uint8_t     bDescrAccessEn;         /**< Can device descriptor be accessed during partial init */
   uint8_t     bInitPowerMode;         /**< Initial power mode of the device after reset */
   uint8_t     bHighPriorityLUN;       /**< LUN of the high priority LU */
   uint8_t     bSecureRemovalType;     /**< Erase configuration necessary for data removal */
   uint8_t     bInitActiveICCLevel;    /**< ICC level after reset */
   uint16_t    wPeriodicRTCUpdate;     /**< >0 to set a priodic RTC update rate */
   struct ufs_unit_descr unit[8];
};


/*@}*/ /* group ufs_driver */

/**
 * @defgroup ufs_api UFS API Overview
 * @ingroup ufs_driver
 *
 * The UFS API provides a list of public functions to UFS clients such as
 * Hotplug that perform data transfer to/from memory devices
 *
 */
/*@{*/

/** \details
 * This function first initializes the UFS controller.
 * It returns NULL if there is no memory device attached.  If there
 * is a memory device, it will initialize the device and take it out of the
 * partial initialized state.  It then sets up the device for data transfer.
 * A non-null pointer to the UFS handle is returned.
 *
 * @param[in] core_id
 *    Which hardware controller to use, in current design, only one UFS core
 *    is supported
 *
 * @param[in] lun
 *    Logic unit number.  The number is related to the LUN number
 *    available on a UFS card.  Each UFS device can have up to 128 LUNs and
 *    up to 128 well known LUNs
 *
 * @return A pointer to the UFS handle if successful.  NULL if failed.
 *
 */
struct ufs_handle *ufs_open( uint32_t core_id, uint32_t lun );

/** \details
 * This function free up the UFS handle. 
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @return 0 if successful.  Error code if failed.
 */
int32_t ufs_close( struct ufs_handle *handle );

/** \details
 * This function gets the memory card specific information.  If there is no
 * memory device attached or device is not initialized, all fields in the card
 * specific information are zero. 
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @param[out] info
 *    pointer to the structure to save the memory card specific information
 *
 * @return 0 on success.  Error code otherwise
 */
int32_t ufs_get_device_info( struct ufs_handle *handle, 
                             struct ufs_info_type *info );


/** \details
 * This function progam the one time programmable area of the UFS memory device
 * to configure the LU layout and configurations.  This function can only
 * be called for a blank UFS device that have not been configured before.
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @param[out] cfg
 *    pointer to the structure that contain the one time programmable UFS
 *    device descriptor
 *
 * @return 0 on success.  Error code otherwise
 */
int32_t ufs_configure_device( struct ufs_handle *handle, 
                              struct ufs_config_descr *cfg );


/** \details
 * This function sets active bootable flag to the LUN number indicated
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @return 0 on success.  Error code otherwise
 */
int32_t ufs_set_bootable( struct ufs_handle *handle, uint32_t lun );

/** \details
 * This function sets the power-on write protection bit of fPowerOnWPEn
 * Once the bit is set, all LUs marked as power on write protect will be
 * read only
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @return 0 on success.  Error code otherwise
 *
 * @note
 *    After a power failure or hardware reset, the power-on write protection
 *    must be reapplied.
 */
int32_t ufs_set_pon_write_protect( struct ufs_handle *handle);

/** \details
 * This function reads \c num_blocks blocks of data from the \c data_address from
 * the device, storing them at the memory location given by \c buff.
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @param[out] buff
 *    pointer to buffer for the incoming data, data buffer must be 32bit aligned
 *
 * @param[in] lba
 *    Logic Block Address of the starting block to read from. 
 *
 * @param[in] num_blocks
 *    number of blocks to be read (4096 bytes per block)
 *
 * @return 0 if successful.  Error code otherwise.
 */
int32_t ufs_read( struct ufs_handle *handle,
                  uint8_t           *buff,
                  uint64_t           lba,
                  uint32_t           num_blocks );

/** \details
 * This function writes \c num_blocks blocks of data, obtained from the memory
 * location \c buff, to the \c data_address on the device.
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @param[out] buff
 *    pointer to buffer for the incoming data, data buffer must be 32bit aligned
 *
 * @param[in] lba
 *    Logic Block Address of the starting block to write to.
 *
 * @param[in] num_blocks
 *    number of blocks to be read (4096 bytes per block)
 *
 * @return 0 if successful.  Error code otherwise.
 */
int32_t ufs_write( struct ufs_handle *handle,
                   uint8_t           *buff,
                   uint64_t           lba,
                   uint32_t           num_blocks );

/** \details
 * This function is to erase data on the device
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @param[in] lba
 *    the address of the first logic block to be erased
 *
 * @param[in] num_blocks
 *    number of blocks to be erased (4096 bytes per block)
 *
 * @return 0 on success.  Error code otherwise
 */
int32_t ufs_erase (struct ufs_handle *handle,
                   uint64_t           lba,
                   uint32_t           num_blocks );

/** \details
 * This function sets the reference clock value.  The setting is one time
 * programmable throughout the life of the device
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @param[in] freq
 *    reference clock frequency in hz, there are currently 4 possible value:
 *    19200000, 26000000, 38400000 and 52000000
 *    Use 0 for pulling the default device value base on BSP setting
 *
 * @return 0 on success.  Error code otherwise
 *
 * @note
 *    After a power failure or hardware reset, the power-on write protection
 *    must be reapplied.
 */
int32_t ufs_set_refclk_freq( struct ufs_handle *handle, 
                             uint32_t           freq);

/*@}*/ /* group ufs_driver */
#endif /* __UFS_API_H */

