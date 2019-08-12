#ifndef __FS_FLASH_DAL_SHIM_H__
#define __FS_FLASH_DAL_SHIM_H__

/**********************************************************************
 * fs_flash_dal_shim.h
 *
 * Flash device public interface.
 *
 * This module implements a device-independent view of the low-level
 * flash system devices.  This interface hides the implementation details
 * and device peculiarities of a diverse range of devices supported by
 * this module.
 *
 * Copyright (c) 2008-2010,2013 QUALCOMM Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_flash_dal_shim.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2013-08-08   wek     Init flash dal shim layer at run time.
2013-04-29   wek     Fix compile erros in flash dal shim for boot.
2010-11-30   vr      Remove redundant fs_flash.h.
2010-11-16   vr      Remove dependency on flash for non-flash based targets.
2010-05-10   dks     Return codes changed to match FLASH return codes.
2010-03-05   vr      Add support for QEFS2 in BOOT
2009-03-20   ebb     Update from flash drivers.
2009-01-19   ebb     Inherited from flash drivers.
2008-11-10   mh      created

===========================================================================*/

#include "flash.h"

/* This whole file depends on DAL. Should be defined in flash.h if available */
#ifdef FLASH_USES_DAL

/* Enable support for devices with different write styles - eg MLC NOR */
#define HAVE_FLASH_WRITE_STYLE

/* Support copy_page API */
#define HAVE_FLASH_COPY_PAGE

/* Support read_multiple_pages API */
#define HAVE_FLASH_READ_MPAGES

/* Support a generic read_spare_bytes API */
#define HAVE_FLASH_READ_SPARE_BYTES

/* Support the API for reading/writing user bytes in spare area */
#define HAVE_FLASH_RW_SPARE_UDATA_BYTES

/* Support a generic 'flash_open' API that probes for all (NOR, NAND, OneNAND)
 * available flash devices, searches for the partition name provided by the
 * caller and opens it if one exists */
#define HAVE_FLASH_OPEN_AND_CLOSE


/* Types used by flash driver routines */
typedef uint32 page_id;
typedef uint32 block_id;
typedef uint32 cluster_id;
/* Please note that the type 'cluster_id' is not used by flash driver code
 * anymore.  It has been maintained in the API for backward compatibility.*/

/* Possible result codes returned by flash drive routines */
#define FS_NO_DEVICE                              ((fs_device_t) 0)
#define FS_DEVICE_OK                              FLASH_DEVICE_DONE
#define FS_DEVICE_DONE                            FLASH_DEVICE_DONE
#define FS_DEVICE_FAIL                            FLASH_DEVICE_FAIL
#define FS_DEVICE_NOT_SUPPORTED                   FLASH_DEVICE_NOT_SUPPORTED
#define FS_DEVICE_CP_READ_FAIL                    FLASH_DEVICE_CP_READ_FAIL
#define FS_DEVICE_CP_WRITE_FAIL                   FLASH_DEVICE_CP_WRITE_FAIL
#define FS_DEVICE_FAIL_PAGE_ERASED                FLASH_DEVICE_FAIL_PAGE_ERASED
#define FS_DEVICE_BUSY                            FLASH_DEVICE_OP_INPROGRESS
#define FS_DEVICE_BAD_BLOCK                       (-12)

/* Flash Lock/Unlock operations */
#define FLASH_UNLOCK_OP                           0x1
#define FLASH_LOCK_OP                             0x2
#define FLASH_LOCKTIGHT_OP                        0x3


/* Definitions for ECC disabling/enabling. ECC can be enabled in two modes.
 * FLASH_NAND_ECC_ENABLED - In this mode ECC is enabled only on the Main Area
 * FLASH_NAND_MAIN_AND_SPARE_ECC_ENABLED - In this mode, ECC is enabled on the
 * Main and the spare area
 */
#define FLASH_NAND_ECC_DISABLED                   0x0
#define FLASH_NAND_ECC_DEFAULT_DISABLED           FLASH_NAND_ECC_DISABLED
#define FLASH_NAND_ECC_ENABLED                    0x1
#define FLASH_NAND_MAIN_AND_SPARE_ECC_ENABLED     0x2

/* Definitions to allow NAND width detection */
#define FLASH_NAND_8BIT                           0x01
#define FLASH_NAND_16BIT                          0x02

/* Description of how write grouping happens, and what is necessary to
 * recover from partial and failed writes. */
typedef enum {

  /* NOR devices where arbitrary bits can be set to zero.  SLC NOR devices
   * generally fall in this category. */
  FS_DEVICE_WRITES_SIMPLE,

  /* NAND devices requiring entire pages to be written, and can only be
   * written to once.  These devices do not allow 'partial_write'. */
  FS_DEVICE_WRITES_PAGED,

  /* NOR devices where adjacent pairs of bits are written together.  A pair
   * can be written from '11' to a value, and from a value to '00'.  If
   * recovery from failed writes is needed, the only two values that should
   * be used are '11' and '00'.  After a failed write, it may not be
   * possible to write the pair to any value other than '00'. */
  FS_DEVICE_WRITES_PAIRED_BITS,

  /* Indicates a NOR device with an unsupported write style.  No
   * assumptions can be made about how the data is written.  Pages must be
   * written in their entirety, and pages that have bad powerdowns cannot
   * be used. */
  FS_DEVICE_WRITES_UNSUPPORTED = 0x1000,
} fs_device_write_style_t;

/*------------------------------------------------------------
 *  Flash Device Definitions
 * ---------------------------------------------------------*/

/* Device structure */
struct fs_device_data;

/* Device descriptor which is a pointer to the device structure */
typedef struct fs_device_data *fs_device_t;

/* Public interface to the flash device */
struct fs_device_data {

 flash_handle_t         handle_flash;
 struct flash_info             info;
 uint8 * page_buf;

 char * (*device_name) (fs_device_t self);
 int (*device_maker_id) (fs_device_t self);
 int (*device_id) (fs_device_t self);
 uint32 (*block_count) (fs_device_t self);
 uint32 (*block_size) (fs_device_t self);
 uint32 (*page_size) (fs_device_t self);
 uint32 (*total_page_size) (fs_device_t self);
 uint32 (*device_type) (fs_device_t self);
 uint32 (*device_width) (fs_device_t self);
 int (*bad_block_check) (fs_device_t self, block_id block);
 int (*mark_block_bad) (fs_device_t self, block_id block);
 int (*write_page) (fs_device_t self, page_id page, void *data);
 int (*erase_block) (fs_device_t self, block_id block);
 void *(*read_pointer) (fs_device_t self, page_id page);
 int (*read_page) (fs_device_t self, page_id page, void *data);
 int (*is_erased) (fs_device_t self, page_id page);
 int (*partial_write) (fs_device_t self, page_id page, void *data,
                        int offset, int length);
 int (*begin_erase_block) (fs_device_t self, block_id block);
 int (*suspend_erase) (fs_device_t self);
 int (*resume_erase) (fs_device_t self);
 int (*erase_status) (fs_device_t self);
 int (*is_block_erased) (fs_device_t self, block_id block);
 int (*read_spare_bytes) (fs_device_t self, page_id page, uint32 spare_offset,
                           uint32 len, void *data);
 int (*read_mpages) (fs_device_t self, page_id start_page, page_id num_pages,
                      void *data, void (*wdog_kick_fn_ptr)(void));
 int (*copy_page) (fs_device_t self, page_id src_page, page_id dest_page);
  int (*lock_ops) (fs_device_t self, page_id start_page, page_id end_page,
                   uint32 opcode);
  int (*get_spare_corrected_bytes) (fs_device_t self);
  int (*write_page_and_udata) (fs_device_t self, page_id page, void *page_data,
                               void *spare_udata, uint32 spare_udata_blen);
  int (*read_spare_udata) (fs_device_t self, page_id page, void *spare_udata,
                           uint32 spare_udata_blen);
  int (*read_page_and_udata) (fs_device_t self, page_id page, void *data,
                              void *spare_udata, uint32 spare_udata_blen);
  int (*read_otp_page_and_spare) (fs_device_t self, page_id page,  void *data,
                                  void *spare_udata, uint32 spare_udata_len,
                                  void *dev_otp_params);
  int (*open_partition) (fs_device_t self, char *partition_name);
  int (*get_partition_phy_limits) (fs_device_t self, uint32 *start,
                                   uint32 *limit, void *rsrvd);
  int (*get_partition_attribs) (fs_device_t self, uint32 *attribs);
  int (*set_ecc_state) (fs_device_t self, int state);
  fs_device_write_style_t (*get_write_style) (fs_device_t self);

}; /* End of public interface to the flash device */

/* Initialize the data structures used by the dal shim layer. */
void fs_flash_dal_shim_init (void);

/*===========================================================================
  Probe for the flash and open the requested partition if a partition table
  exists.
===========================================================================*/
fs_device_t flash_shim_open (char *partition_name, int whichflash);

/*===========================================================================
DESCRIPTION
  Close the flash device. This API will free the flash allocated
  device structure.
===========================================================================*/
int flash_shim_close (fs_device_t flash_device);

#endif /* FLASH_USES_DAL */

#endif /* __FS_FLASH_DAL_SHIM_H__ */
