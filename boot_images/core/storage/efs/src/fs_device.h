#ifndef __FS_DEVICE_H__
#define __FS_DEVICE_H__

/**********************************************************************
 * fs_device.h
 *
 * File System Device interface.
 *
 * This module implements a device-independent view of the low-level file
 * system devices.  This interface hides all of the implementation details
 * and device differences between the diverse device types.
 *
 * Copyright (C) 2002-2013, Qualcomm Technologies, Inc.
 *
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_device.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2013-01-17   wek     Remove reference to rex signal type.
2012-11-09    nr     Update rmts to use new api's.
2012-10-19   dks     Make superblock age run to uint32_max.
2012-08-17   wek     Move the rmts partition enum outside rmts feature.
2012-05-25    nr     Allow RAMFS and RMTS to have configurable page size.
2012-05-16    nr     Make MNAND cluster/page size configurable at runtime.
2012-04-09   dks     Support storing/reading cookies to/from emmc partitions.
2012-03-13   wek     Remove redundant header file inclusion fs_efs2_cust.
2012-01-19   dks     Fix remotefs client init sequence for QMI based transfer.
2011-08-17    rp     Add EFS OTP (One-Time-Programmable) support.
2011-05-13   dks     Add read_pointer buffer to device layer.
2011-03-25   dks     Support Golden-Copy in RAM for fusion.
2011-03-25    rp     Support RemoteFS layer for QMI changes.
2011-03-09   dks     Add device setup calls.
2010-11-30    vr     Remove redundant fs_flash.h
2010-11-24   wek     Remove dependency with page size macro.
2010-11-19    rp     Added support to read FSG from modem if possible.
2010-11-17    rp     Use enums to refer to FS1 and FS2 eMMC partitions.
2010-11-16    vr     Remove dependency on flash for non-flash based targets.
2010-10-12    vr     Change fs_iovec parameter from num_sector to size.
2010-09-30   wek     Remove device iovec interface.
2010-08-20    vr     Added logic to load efs partitions in EFS for rmts targets
2010-06-25   dks     Added counters for partial-write, begin and suspend-erase.
2010-05-13   wek     Make sync_no_wait non blocking.
2010-05-10   dks     Added change to support EFS on SPI-NOR devices.
2010-05-07    rp     Added Ram filesystem.
2010-03-03    rp     Fix crash in efs_sync_no_wait when called the second time.
2010-01-19    vr     Added efs_sync_no_wait and efs_sync_get_status APIs.
2009-05-08   ebb     Moved to block_device interface.
2009-01-26   ebb     Moved from flash.h => fs_flash.h
2008-12-15    rp     Remove reference to obsolete flash APIs.
2008-12-12    rp     Introducing i/o-vec and Managed Nand.
2008-11-03    rp     Introduce device abstraction.
2008-09-16   yog     Remove trailing commas from the enum declaration.
2008-05-23    rp     Use bencmark counters from fs_counters.h
2007-10-18    rp     Featurized QEFS2.
2007-02-23   dlb     Move read failures into device wrappers.
2006-11-09   dlb     Add dealloc counter.
2006-09-20   dlb     Lint fixes.
2006-09-12   dlb     Change some queries to unsigned values.
2006-09-11   dlb     Add wrappers for all flash functions.
2006-06-26    yg     Memory reduction effort
2006-01-27   dlb     Add wrappers for write styles on old drivers.
2005-10-21   dlb     Mark unsigned constants as such.
2004-10-15   dlb     Update copyright line.
2004-04-26   drh     Remove support for old non-uniform flash drivers
2004-02-20   drh     Add support for Uniform Flash Drivers
2003-06-15    gr     Added a function pointer for mark_block_bad.
2003-04-04   bgc     Added FS_DEVICE_BUSY and function pointers for
                     begin_erase, suspend, resume, and erase_status.
2002-09-05    gr     Changed signature of is_erased to accept a page id rather
                     than a block id.
2002-08-19   drh     Added is_erased to fs_device_data structure
2002-08-08   drh     Added total_page_size to fs_device_data structure
2002-06-13   dlb     Created

===========================================================================*/

#include "fs_config_i.h"

/* Until we upgrade all the custefs.h files to correctly define the device-type
   as NAND or MNAND or NOR, lets detect the device type the old-fashined way.*/
#if defined (FEATURE_EFS_EFS2_ON_NAND)    || \
    defined (FEATURE_EFS_EFS2_ON_NOR)     || \
    defined (FEATURE_EFS_EFS2_ON_MNAND)   || \
    defined (FEATURE_EFS_EFS2_ON_RMTS)    || \
    defined (FEATURE_EFS_EFS2_ON_RAMFS)   || \
    defined (FEATURE_EFS_EFS2_ON_SPI_NOR)
  /* Device type is configured, so proceed with this device ... */
#else
  /* The custefs.h is not upgraded yet, so detect NAND or NOR the old way */
  #ifdef FEATURE_EFS_PROBE_NAND
    #define FEATURE_EFS_EFS2_ON_NAND
  #else
    #define FEATURE_EFS_EFS2_ON_NOR
  #endif
#endif

/* Flash supported only if required */
#if defined (FEATURE_EFS_EFS2_ON_NAND)    || \
    defined (FEATURE_EFS_EFS2_ON_NOR)     || \
    defined (FEATURE_EFS_EFS2_ON_SPI_NOR)
  #include "fs_flash_dal_shim.h"
#endif

#include "comdef.h"
#include "fs_device_types.h"
#include "fs_counters.h"
#include "fs_iovec.h"
#include "fs_osal.h"
#include "fs_sys_types.h"
#include "fs_rmts.h"

#ifdef FEATURE_EFS_EFS2_ON_MNAND
  #include "fs_hotplug.h"
#endif

#ifdef FEATURE_EFS_EFS2_ON_RMTS
  #include "fs_rmts_config.h"
#endif
/* Encapsulate the get_write_style feature and types, so that the code will
 * continue to work with older drivers.  We assume that all older drivers
 * (for NOR) are simple devices.  The driver will define
 * HAVE_FLASH_WRITE_STYLE to indicate that the query is present, and this
 * driver might support other write styles. */

#ifdef HAVE_FLASH_WRITE_STYLE

  #define FS_GET_WRITE_STYLE(dev)  \
    ((dev)->get_write_style (dev))

#else /* not HAVE_FLASH_WRITE_STYLE */

  /* Enough of the types to allow things to build. */
  typedef word fs_device_write_style_t;

  /* And the definitions used in this file.  The only value that must match
   * is FS_DEVICE_WRITES_SIMPLE, because that is the one returned by the
   * constant "query".  The others are defined to vary different values to
   * make it clear if they ever show up. */
  #define FS_DEVICE_WRITES_SIMPLE 0
  #define FS_DEVICE_WRITES_PAIRED_BITS   0x18ea928b

  #define FS_GET_WRITE_STYLE(dev)   FS_DEVICE_WRITES_SIMPLE

#endif /* not HAVE_FLASH_WRITE_STYLE */

/* TODO: These are simple thin wrappers.  Eventually, we want to capture
 * which calls are allowed to fail, and die with a more meaningful message
 * if the others fail. */

/* These calls are wrappers around the device calls.  Most of the time,
 * device failures are fatal, but occasionally they are expected and are
 * handled.  The last argument to these calls indicates whether or not
 * these calls should fail. */
enum fs_flash_failure {
  FS_FLASH_FAIL_OK,   /* It is ok to return a failure. */
  FS_FLASH_MUST_WORK  /* This call should not fail, and a failure return
                         will result in a fatal error. */
};


/* wrapper around flash device structure to add information about number of
 * device pages that make up an EFS page */
#if defined (FEATURE_EFS_EFS2_ON_NAND) || \
    defined (FEATURE_EFS_EFS2_ON_NOR)  || \
    defined (FEATURE_EFS_EFS2_ON_SPI_NOR)
  struct fs_flash_device
  {
    fs_device_t flash_dev;

    /* Some devices like SPI-NOR may not have device page size smaller than
     * FS_MIN_PAGE_SIZE. Since our code has not been tested well for device
     * sizes less than FS_MIN_PAGE_SIZE we want to keep the page size constant
     * at a minimum of FS_MIN_PAGE_SIZE for the PM layer. To map the PM
     * operations to appropriate device level operation we calculate the number
     * of device pages that make one FS_MIN_PAGE_SIZE. SPI-NOR device has 256
     * byte page size, so a read_page on PM will map to read_mpages on device
     * layer to read (FS_MIN_PAGE_SIZE/256) pages at a time. */
    uint32 dev_pages_per_efs_page;
    uint32 dev_page_size;
  };
#endif

#ifdef FEATURE_EFS_EFS2_ON_MNAND
  struct fs_mnand_device
  {
    struct hotplug_device       *hdev;
    uint32                      dev_page_size;
    uint32                      sectors_per_page;
  };
#endif

/*-----------------------------------------------------------------------------
  EFS Mappings of RMTS Errors.
-----------------------------------------------------------------------------*/
  enum fs_rmts_device_errno
  {
    FS_RMTS_ERRNO_SUCCESS,          /* RMTS returned sucess. */
    FS_RMTS_ERRNO_NOT_SUPPORTED,    /* The RMTS API used is not supported */
    FS_RMTS_ERRNO_TIMEOUT,          /* RMTS api timedout. */
    FS_RMTS_ERRNO_INVALID_PARAMS,   /* RMTS returned invalid input params */
    FS_RMTS_ERRNO_WRITE_PROTECTED,  /* The partition is write protected. */
    FS_RMTS_ERRNO_API_FAILURE,      /* RMTS API failed. See the rmts errno */
    FS_RMTS_ERRNO_INVALID_OP        /* Operation not supported for the dev. */
  };

#ifdef FEATURE_EFS_EFS2_ON_RMTS

  /* Number of copies of Filesystem data maintained on storage device. It will
     be used to determine the number of times filesystem data needs to be
     synced to the backing store to keep data current and recoverable. */
  #define FS_RMTS_NUM_EFS_COPIES 2

  struct fs_device_efs_rmts_partition
  {
    uint32      start_sect;
    uint32      end_sect;
  };

  /* The structure holds the last (FS_RMTS_DEVICE_ERROR_QUEUE_SIZE + 1)  errors
   * returned by the rmts layer. It also holds a count of all the errors
   * that occured in rmts device since bootup.
   */
  struct fs_device_efs_rmts_dev_errors
  {
    int prev_errors[FS_RMTS_DEVICE_ERROR_QUEUE_SIZE];
    int current_error;
    uint32 prev_error_index;
    uint32 net_error_counter;
  };

  struct fs_device_efs_rmts
  {
    /* Handle to rmts client */
    void   *handle;
    enum fs_rmts_modem_type modem_type;

    fs_os_sigs_type sig;  /* Signal to use for QMI */
    uint32 timeout_in_ms; /* Timeout for the operation */

    /* EFS Partitions. */
    struct fs_device_efs_rmts_partition   fs1;
    struct fs_device_efs_rmts_partition   fs2;
    struct fs_device_efs_rmts_partition   fsg;
    struct fs_device_efs_rmts_partition   fsc;

    /* EFS Ram Addresses. */
    uint8   *modem_ram;
    uint8   *shared_ram;
    uint8   *golden_ram;
    uint32  ram_size;

    uint32  total_sectors;
    uint32  dev_page_size;

    struct fs_device_efs_rmts_dev_errors errors;

    uint32  is_dev_inited;
  };
#endif /* FEATURE_EFS_EFS2_ON_RMTS */



#ifdef FEATURE_EFS_EFS2_ON_RAMFS
  struct fs_ramfs_device
  {
    uint8       *ramfs_buffer;
    uint32      ramfs_size;
    uint32      dev_page_size;
  };
#endif


struct efs_device;
typedef struct efs_device *efs_device_t;
struct efs_device
{

  /* Query operations. */

  /* These will no longer be one each for each device, but
   * will be generic functions that extract data from the
   * private portions of the device structure and return
   * that data.  They will operate the same for all devices.*/
  char * (*device_name) (efs_device_t self);
  int (*device_maker_id) (efs_device_t self);
  int (*device_id) (efs_device_t self);
  uint32 (*block_count) (efs_device_t self);
  uint32 (*block_size) (efs_device_t self);
  uint32 (*page_size) (efs_device_t self);
  uint32 (*total_page_size) (efs_device_t self);
  uint32 (*device_type) (efs_device_t self);

  /* Bad block check for NAND devices */
  int (*bad_block_check) (efs_device_t self, fs_block_id block);

  /* Mark a block as bad for NAND device */
  int (*mark_block_bad) (efs_device_t self, fs_block_id block);

  /* Write operations. */
  int (*write_page) (efs_device_t self,
    fs_page_id page,
    void *data);

  /* A full erase.  Eventually, will implement NOR-style partial erasing. */
  int (*erase_block) (efs_device_t self,
    fs_block_id block);

  /* Get the address of read data.  Returns NULL if this device doesn't
   * support direct mapping of read data. */
  void *(*read_pointer) (efs_device_t self,
    fs_page_id page);

  /* Read the data into a buffer.  Can be used if the above returns NULL. */
  int (*read_page) (efs_device_t self,
    fs_page_id page,
    void *data);

  /* Determine erase status of page */
  int (*is_erased) (efs_device_t self, fs_page_id page);

  /* If this function pointer is non-null, then can be called to perform a
   * partial page write.  It is only valid to change FF's in flash into
   * values, and then to change values into 00's.  The write must be
   * contained entirely within a page, and is only guaranteed to work
   * on 32 bit boundaries. */
  int (*partial_write) (efs_device_t self, fs_page_id page,
    void *data, int offset, int length);

  /* A full erase that returns and leaves the erase pending */
  int (*begin_erase_block) (efs_device_t self,
    fs_block_id block);

  /* Suspend the current erase */
  int (*suspend_erase) (efs_device_t self);

  /* Resume suspended erase */
  int (*resume_erase) (efs_device_t self);

  /* Check if the erase block operation begun with begin_erase_block has
   * completed. */
  int (*erase_status) (efs_device_t self);

  /* Check if given block is erased */
  int (*is_block_erased) (efs_device_t self, fs_block_id block);

  /* Read len bytes from spare_offset in the spare area in given page
   * and copy into given data buffer. Data is read with ECC turned OFF */
  int (*read_spare_bytes) (efs_device_t self, fs_page_id page,
  uint32 spare_offset, uint32 len, void *data);

  /* Read num_pages from start_page and copy them into given data location/
   * address */
  int (*read_mpages) (efs_device_t self, fs_page_id start_page,
    fs_page_id num_pages, void *data, void (*wdog_kick_fn_ptr)(void));

  /* Copy data from source page and write it into dest_page */
  int (*copy_page) (efs_device_t self, fs_page_id src_page,
    fs_page_id dest_page);

  /* Support lock/unlock/locktight of blocks from start_page to end_page */
  int (*lock_ops) (efs_device_t self, fs_page_id start_page,
    fs_page_id end_page, uint32 opcode);

  /* Get corrected spare user data bytes available from controller */
  int (*get_spare_corrected_bytes) (efs_device_t self);

  /* Write page data and spare user data bytes to given page with any
     hardware/software ECC for spare user data bytes if needed */
  int (*write_page_and_udata) (efs_device_t self,
    fs_page_id page,
    void *page_data,
    void *spare_udata,
    uint32 spare_udata_blen);

  /* Read only the user data in spare location of given page with ECC
     correction/detection */
  int (*read_spare_udata) (efs_device_t self,
    fs_page_id page,
    void *spare_udata,
    uint32 spare_udata_blen);

 /* Read the data and Spare bytes into buffers. */
  int (*read_page_and_udata) (efs_device_t self,
    fs_page_id page,
    void *data,
    void *spare,
    uint32 spare_udata_blen);

  /* Open a particular partition.
   *     Caller passes device structure filled in during probe,
   *     this function call may modify it if successful. */
  int (*open_partition) (efs_device_t self, char *partition_name);

  /* Change the default state of ECC calculation
   *     This is used mainly when reading back a raw file system
   *     using flash tools. */
  void (*set_ecc_state) (efs_device_t self, int state);

  /* Get the device write style. */
  fs_device_write_style_t
    (*get_write_style) (efs_device_t self);

  /* Read/Write multiple sectors.. */
  int (*read_sectors) (efs_device_t self, uint32 sect, void *data,
                       uint32 num_sectors);
  int (*write_sectors) (efs_device_t self, uint32 sect, void *data,
                        uint32 num_sectors);
  /* Reliably write multiple sectors. */
  int (*reliable_write_sectors) (efs_device_t self, uint32 sect, void *data,
                                 uint32 num_sectors);

  /* initialize remotefs client. */
  enum fs_rmts_device_errno (*rmts_client_init) (efs_device_t self,
                                                 fs_os_sigs_type sig,
                                                 uint32 timeout_in_ms);

  /* Get the modem type of the rmts device */
  enum fs_rmts_modem_type (*rmts_get_rmts_modem_type) (efs_device_t self);

  /* Read iovec_cnt items onto the iovec array for rmts device. */
  enum fs_rmts_device_errno (*rmts_read_iovec) (efs_device_t self,
                             struct fs_iovec *iovec, uint32 iovec_cnt,
                             enum fs_rmts_partition_id which_partition);

  /* Write iovec_cnt items onto the iovec array for rmts device. */
  enum fs_rmts_device_errno (*rmts_write_iovec) (efs_device_t self,
                             struct fs_iovec *iovec, uint32 iovec_cnt,
                             enum fs_rmts_partition_id which_partition);

  /* Get the last errno returned by the rmts device */
  int (*rmts_get_last_errno) (efs_device_t self);

  enum fs_device_type             efs_device_type;

  union
  {
    #if defined (FEATURE_EFS_EFS2_ON_NAND) ||   \
        defined (FEATURE_EFS_EFS2_ON_NOR)  ||   \
        defined (FEATURE_EFS_EFS2_ON_SPI_NOR)
      struct fs_flash_device       flash;
    #endif

    #if defined (FEATURE_EFS_FTL)
      fs_device_t                  ftl_flash;
    #endif

    #ifdef FEATURE_EFS_EFS2_ON_MNAND
      struct fs_mnand_device        mnand_dev;
    #endif

    #ifdef FEATURE_EFS_EFS2_ON_RMTS
      struct fs_device_efs_rmts    rmts_dev;
    #endif

    #ifdef FEATURE_EFS_EFS2_ON_RAMFS
      struct fs_ramfs_device       ramfs_dev;
    #endif

    #ifdef FEATURE_EFS_OTP
      fs_device_t                  otp_flash;
    #endif

    int                           dummy; /* Fix potential empty union */
  }priv;

};

void fs_device_init_invalid_ops (efs_device_t self);

void fs_device_init (void);
efs_device_t fs_device_open (enum fs_device_type device_type,
                             uint32 which_device);
int fs_device_close (efs_device_t base_dev);
/* Return 1 if it is NAND device and 0 if it is not. */
int fs_device_is_nand_device (efs_device_t base_dev);

int fs_device_write_page (efs_device_t dev, fs_page_id page, void *data,
    enum fs_flashop_category category);
int fs_device_erase_block (efs_device_t dev, fs_block_id block,
    enum fs_flashop_category category);
int fs_device_read_page (efs_device_t dev, fs_page_id page, void *data,
    enum fs_flashop_category category,
    enum fs_flash_failure failure);

int fs_device_read_iovec (efs_device_t base_dev, struct fs_iovec *iovec,
                          uint32 iovec_cnt, enum fs_flashop_category category,
                          enum fs_flash_failure failure);
int fs_device_write_iovec (efs_device_t base_dev, struct fs_iovec *iovec,
                          uint32 iovec_cnt, enum fs_flashop_category category);
int fs_device_read_sectors (efs_device_t base_dev, uint32 sect, void *data,
                        uint32 num_sectors, enum fs_flashop_category category);
int fs_device_write_sectors (efs_device_t base_dev, uint32 sect, void *data,
                        uint32 num_sectors, enum fs_flashop_category category);
int fs_device_reliable_write_sectors (efs_device_t base_dev, uint32 sect,
                                  void *data, uint32 num_sectors,
                                  enum fs_flashop_category category);

uint32 fs_device_block_size (efs_device_t dev);
uint32 fs_device_page_size (efs_device_t dev);
uint32 fs_device_block_count (efs_device_t dev);
int fs_device_bad_block_check (efs_device_t dev, fs_block_id block);
int fs_device_is_erased (efs_device_t dev, fs_page_id page);
int fs_device_mark_block_bad (efs_device_t dev, fs_block_id block);

int fs_device_resume_erase (efs_device_t dev);
int fs_device_erase_status (efs_device_t dev);
int fs_device_suspend_erase (efs_device_t dev,
                             enum fs_flashop_category category);
int fs_device_begin_erase_block (efs_device_t dev, fs_block_id block,
                                 enum fs_flashop_category category);
int fs_device_partial_write (efs_device_t dev, fs_page_id page,
                             void *data, int offset, int length,
                             enum fs_flashop_category category);

void *fs_device_read_pointer (efs_device_t dev, fs_page_id page,
                              enum fs_flashop_category category);


#ifdef FEATURE_EFS_SETUP_DEVICE

void fs_device_setup_init (void);
void fs_device_setup_start (void);
void fs_device_setup_done (void);

#else

static inline void
fs_device_setup_init (void)
{
}

static inline void
fs_device_setup_start (void)
{
}

static inline void
fs_device_setup_done (void)
{
}

#endif /* FEATURE_EFS_SETUP_DEVICE */

#endif /* __FS_DEVICE_H__ */
