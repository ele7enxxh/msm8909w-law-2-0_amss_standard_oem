/***********************************************************************
 * fs_device_efs_rmts.c
 *
 * EFS Remote Storage Device module.
 * Copyright (C) 2009-2013 QUALCOMM Technologies, Inc.
 *
 * Device layer abstration for EFS on Remote MMC device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_device_efs_rmts.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-04-17   nr    Make the fs_rmts_devices and fs_rmts_efs_size non static.
2013-01-18   wek   Allow multiple devices to be defined in one build.
2013-01-17   wek   Remove reference to rex signal type.
2013-01-14   nr    Use new get buffer api that accepts os_params.
2012-11-09   nr    Update rmts to use new api's.
2012-09-20   dks   Update rmts layout to support write-cache.
2012-09-03   dks   Cleanup externing Rmts Device RAM address.
2012-06-12   dks   Cleanup IMEM cookies and cookie APIs.
2012-05-25   nr    Allow RAMFS and RMTS to have configurable page size.
2012-05-07   dks   Support storing/reading cookies to/from emmc partitions.
2012-05-04   nr    Modify the check on shared ram size.
2012-01-19   dks   Fix remotefs client init sequence for QMI based transfer.
2011-11-29   nr    Fix same age superblock in FS1 and FS2 when FSG is present.
2011-10-13   rp    Remove inclusion of Hotplug header file.
2011-05-27   rp    Handle write_iovec failure when remote server is down.
2011-05-09   dks   Remove dev_hookup_ram function.
2011-03-25   dks   Support Golden-Copy in RAM for fusion.
2011-03-25   rp    Support RemoteFS layer for QMI changes.
2011-01-12   vr    Restore partition paths to Fix compilation issue.
2010-12-23   vr    Remove hardcoded EFS partition paths.
2010-12-23   vr    Use handle in updated remotefs_get_shared_ram_address().
2010-11-29   rp    Don't open FSG on older targets that don't support FSG.
2010-11-19   rp    Added support to read FSG from modem if possible.
2010-11-17   rp    Use enums to refer to FS1 and FS2 eMMC partitions.
2010-11-17   rp    Avoid reset to try FSG on first ever boot.
2010-11-15   rp    Replace Boot cookies with EFS cookies.
2010-10-15   wek   Add page size ops for all efs devices.
2010-10-12   vr    Change fs_iovec parameter from num_sector to size.
2010-08-27   rp    Fix compilation error when SHARED_EFS_RAM_BASE is undef.
2010-08-20   vr    Added logic to load efs partitions in EFS for rmts targets.
2010-05-13   wek   Make sync_no_wait non blocking.
2010-05-06   wek   Implement golden copy for remote storage page manager.
2010-03-03   rp    Fix crash in efs_sync_no_wait when called the second time.
2010-02-24   rp    Added hotplug_open_device_by_partition_type API.
2010-01-19   vr    Added efs_sync_no_wait and efs_sync_get_status APIs.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_rmts_config.h"

#ifdef FEATURE_EFS_EFS2_ON_RMTS

#include "fs_device_efs_rmts.h"
#include <string.h>
#include "assert.h"
#include "fs_device.h"
#include "fs_err.h"
#include "fs_osal.h"
#include "fs_osal.h"
#include "fs_rmts_guid.h"

#include "rmts_api.h"

#include "task.h"


#ifdef FS_UNIT_TEST
  #include "utdevice.h"
  #include "rmts_client.h"
#endif


#define FS_DEVICE_EFS_RMTS_MAX_DEVICES          (1)

uint32 fs_rmts_efs_size = SCL_MODEM_EFS_RAM_SIZE;

struct efs_device fs_rmts_devices[FS_DEVICE_EFS_RMTS_MAX_DEVICES];

/* If not configured in build, declare the private modem copy. */
#ifndef SCL_MODEM_EFS_RAM_BASE
  static uint8 fs_rmts_efs_ram_fs1_inst[SCL_MODEM_EFS_RAM_SIZE];
#endif

static uint8* fs_device_efs_rmts_get_modem_ram_addr
                                  (struct fs_device_efs_rmts *rmts_dev);
static uint8* fs_device_efs_rmts_get_shared_ram_addr
                                  (struct fs_device_efs_rmts *rmts_dev);
static uint8* fs_device_efs_rmts_get_golden_ram_addr
                                  (struct fs_device_efs_rmts *rmts_dev);

void
fs_device_efs_rmts_init (void)
{
  memset (fs_rmts_devices, 0, sizeof (fs_rmts_devices));
}

static uint8*
fs_device_efs_rmts_get_modem_ram_addr (struct fs_device_efs_rmts *rmts_dev)
{
  uint8 *modem_ram_addr = NULL;
  (void) rmts_dev;

#ifdef SCL_MODEM_EFS_RAM_BASE
  modem_ram_addr = (uint8 *)SCL_MODEM_EFS_RAM_BASE;
#else
  modem_ram_addr = (uint8 *)fs_rmts_efs_ram_fs1_inst;
#endif

  if (modem_ram_addr == NULL)
  {
    FS_ERR_FATAL ("Invalid modem ram address", 0, 0, 0);
  }

  return modem_ram_addr;
}

static uint8*
fs_device_efs_rmts_get_shared_ram_addr (struct fs_device_efs_rmts *rmts_dev)
{
  uint8 *shared_ram_addr = NULL;
  uint32 shared_ram_size = rmts_dev->ram_size;
  enum rmts_status status;
  rmts_os_params_t os_params;

  ASSERT (rmts_dev->handle != NULL);

  os_params.sig = FS_OP_COMPLETE_SIG;
  os_params.timeout_in_ms = 0; /* Blocking wait (Infinite) as we do not expect
                                * this call to fail or timeout.
                                */
  /*
   * This function either gives us a local ram address (in case of fusion) or
   * gets the shared adress from the server. In case the function returns local
   * address it is likely the SCL_SHARED_EFS_RAM_BASE address.
   */
  status = rmts_get_buffer (rmts_dev->handle, &shared_ram_addr,
                            shared_ram_size, &os_params);

  if (status != RMTS_STATUS_SUCCESS)
  {
    FS_ERR_FATAL ("[%d ,%d, %d] EFS: rmts_get_buffer api failed", status,
                  shared_ram_size, (uint32)rmts_dev->handle);
  }

  if (shared_ram_addr == NULL)
  {
    FS_ERR_FATAL ("Invalid shared ram address from Apps", 0, 0, 0);
  }

  return shared_ram_addr;
}


static uint8*
fs_device_efs_rmts_get_golden_ram_addr (struct fs_device_efs_rmts *rmts_dev)
{
  uint8 *golden_ram_addr = NULL;
  (void) rmts_dev;

#ifdef SCL_GOLDEN_EFS_RAM_BASE
  golden_ram_addr = (uint8 *) SCL_GOLDEN_EFS_RAM_BASE;
#else
  /* No seperate Golden-RAM here */
  golden_ram_addr = NULL;
#endif

  return golden_ram_addr;
}

static void
fs_device_efs_rmts_set_modem_type (struct fs_device_efs_rmts *rmts_dev)
{
  enum fs_rmts_modem_type modem_type = FS_RMTS_MODEM_TYPE_STANDALONE;
  if (rmts_dev->golden_ram != NULL)
  {
    modem_type = FS_RMTS_MODEM_TYPE_FUSION;
  }
  rmts_dev->modem_type = modem_type;
}

static void*
fs_device_efs_rmts_get_rmts_client_handle (void)
{
  enum rmts_status status;
  void *rmts_handle = NULL;

  status = rmts_get_handle (FS_RMTS_CLIENT_ID_STRING, &(rmts_handle));
  if (status != RMTS_STATUS_SUCCESS)
  {
    FS_ERR_FATAL ("[%d] EFS: rmts_get_handle api failed", status, 0, 0);
  }

  if (rmts_handle == NULL)
  {
    FS_ERR_FATAL ("[%d] EFS: rmts_get_handle -> NULL handle", status, 0, 0);
  }

  return rmts_handle;
}

efs_device_t
fs_device_efs_rmts_open (uint32 which_device)
{
  efs_device_t base_dev;
  struct fs_device_efs_rmts *rmts_dev;
  uint8 *modem_ram, *golden_ram;
  uint32 page_size = FS_RMTS_PAGE_SIZE_IN_BYTES;
  uint32 ram_size = fs_rmts_efs_size;

  ASSERT (which_device < FS_DEVICE_EFS_RMTS_MAX_DEVICES);
  base_dev = &fs_rmts_devices[which_device];

  rmts_dev = &base_dev->priv.rmts_dev;
  ASSERT (rmts_dev != NULL);

  modem_ram = fs_device_efs_rmts_get_modem_ram_addr (rmts_dev);
  golden_ram = fs_device_efs_rmts_get_golden_ram_addr (rmts_dev);

  rmts_dev->handle = NULL;

#ifdef FS_UNIT_TEST
  {
    efs_device_t sim_dev = lookup_partition (EFS_PARTITION_NAME);
    ASSERT (sim_dev != NULL);
    if (sim_dev->efs_device_type != FS_DEVICE_TYPE_EFS_RMTS)
      return NULL;
    sim_dev->priv.rmts_dev.handle = NULL;
    sim_dev->priv.rmts_dev.is_dev_inited = 0;

    /* Reset the ram adresses in simulator. */
    modem_ram = sim_dev->priv.rmts_dev.modem_ram;
    golden_ram = sim_dev->priv.rmts_dev.golden_ram;
    ram_size = sim_dev->priv.rmts_dev.ram_size;
    page_size = sim_dev->priv.rmts_dev.dev_page_size;
  }
#endif

/* Initialize all values */
  fs_device_efs_rmts_init_dev (base_dev, ram_size);

  /* Set page size */
  rmts_dev->dev_page_size = page_size;


  /* Hookup EFS private ram Addresses. */
  rmts_dev->modem_ram = modem_ram;
  rmts_dev->golden_ram = golden_ram;

  /* Get Rmts client handle */
  rmts_dev->handle = fs_device_efs_rmts_get_rmts_client_handle ();
  rmts_dev->shared_ram = fs_device_efs_rmts_get_shared_ram_addr (rmts_dev);

  ASSERT (rmts_dev->shared_ram != NULL);
  ASSERT (rmts_dev->modem_ram != NULL);


  rmts_dev->is_dev_inited = 0;

  /* Determine rmts_modem_type */
  fs_device_efs_rmts_set_modem_type (rmts_dev);

  return base_dev;
}


int
fs_device_efs_rmts_close (efs_device_t base_dev)
{
  (void) rmts_close (base_dev->priv.rmts_dev.handle);
  base_dev->priv.rmts_dev.is_dev_inited = 0;
  return 0;
}

static void
fs_device_efs_rmts_validate_dev (efs_device_t base_dev)
{
  ASSERT (base_dev->priv.rmts_dev.ram_size != 0);
  ASSERT (base_dev->priv.rmts_dev.modem_ram != NULL);
  ASSERT (base_dev->priv.rmts_dev.handle != NULL);
  ASSERT (base_dev->priv.rmts_dev.shared_ram != NULL);
}


static uint32
fs_device_efs_rmts_block_count (efs_device_t base_dev)
{
  fs_device_efs_rmts_validate_dev (base_dev);
  return base_dev->priv.rmts_dev.total_sectors;
}

static uint32
fs_device_efs_rmts_page_size (efs_device_t base_dev)
{
  fs_device_efs_rmts_validate_dev (base_dev);
  return base_dev->priv.rmts_dev.dev_page_size;
}

static void
fs_device_efs_rmts_set_device_errno (struct fs_device_efs_rmts *rmts_dev,
                                     enum rmts_status status)
{
  struct fs_device_efs_rmts_dev_errors *errors = &(rmts_dev->errors);

  if (errors->net_error_counter > 0)
  {
    errors->prev_errors[errors->prev_error_index] = errors->current_error;
    ++(errors->prev_error_index);

    if (errors->prev_error_index >= FS_RMTS_DEVICE_ERROR_QUEUE_SIZE)
    {
      errors->prev_error_index = 0;
    }
  }
  errors->current_error = (int) status;
  ++(errors->net_error_counter);
}

enum fs_rmts_device_errno
fs_device_efs_rmts_get_fs_errno (struct fs_device_efs_rmts *rmts_dev,
                                 enum rmts_status status)
{
  enum fs_rmts_device_errno result;

  switch (status)
  {
    case RMTS_STATUS_SUCCESS:
      result = FS_RMTS_ERRNO_SUCCESS;
      break;

    case RMTS_STATUS_ERROR_NOT_SUPPORTED:
      result = FS_RMTS_ERRNO_NOT_SUPPORTED;
      break;

    case RMTS_STATUS_ERROR_TIMEOUT:
      result = FS_RMTS_ERRNO_TIMEOUT;
      break;

    case RMTS_STATUS_ERROR_PARAM:
      result = FS_RMTS_ERRNO_INVALID_PARAMS;
      break;

    case RMTS_STATUS_ERROR_WRITE_PROTECTED:
      result = FS_RMTS_ERRNO_WRITE_PROTECTED;
      break;

    default:
      result = FS_RMTS_ERRNO_API_FAILURE;
      break;
  }

  if (status != RMTS_STATUS_SUCCESS)
  {
    fs_device_efs_rmts_set_device_errno(rmts_dev, status);
  }

  return result;
}

static enum fs_rmts_device_errno
fs_device_efs_rmts_client_init (efs_device_t base_dev, fs_os_sigs_type sig,
                                uint32 timeout_in_ms)
{
  struct fs_device_efs_rmts *rmts_dev;

  ASSERT (base_dev != NULL);
  rmts_dev = &base_dev->priv.rmts_dev;

  ASSERT (rmts_dev != NULL);
  ASSERT (rmts_dev->is_dev_inited == 0);

  ASSERT (rmts_dev->handle != NULL);

  rmts_dev->sig = sig;
  rmts_dev->timeout_in_ms = timeout_in_ms;
  rmts_dev->is_dev_inited  = 1;

  return FS_RMTS_ERRNO_SUCCESS;
}

enum fs_rmts_modem_type
fs_device_efs_rmts_get_rmts_modem_type (efs_device_t base_dev)
{
  struct fs_device_efs_rmts *rmts_dev = &base_dev->priv.rmts_dev;
  return rmts_dev->modem_type;
}

#define FS_RMTS_MAX_IOVECS_SUPPORTED (1)

static enum fs_rmts_device_errno
fs_device_efs_rmts_read_write_iovec_helper (efs_device_t base_dev,
                               fs_iovec_t iovec, uint32 iovec_cnt,
                               enum fs_rmts_partition_id partition_id,
                               int do_write)
{
  struct fs_device_efs_rmts *rmts_dev;
 rmts_iovec_t remotefs_iovec[FS_RMTS_MAX_IOVECS_SUPPORTED];
  enum rmts_status status = RMTS_STATUS_SUCCESS;
  struct rmts_guid *partition_guid;
  rmts_os_params_t os_params;
  uint32 loop_cnt;
  enum fs_rmts_device_errno result = FS_RMTS_ERRNO_SUCCESS;
  enum fs_rmts_modem_type modem_type;

  ASSERT (base_dev != NULL);
  rmts_dev = &base_dev->priv.rmts_dev;
  ASSERT (iovec_cnt > 0 && iovec_cnt <= FS_RMTS_MAX_IOVECS_SUPPORTED);

  /* the RMTS client should already have been initialized. */
  ASSERT (rmts_dev->is_dev_inited == 1);

  ASSERT (partition_id != FS_RMTS_PARTITION_ID_MAX);

  modem_type = rmts_dev->modem_type;
  /* Casting to void* first to stop lint from complaining. */
  partition_guid = (struct rmts_guid*) (void*)
                 fs_rmts_guid_get_partition_guid (partition_id, modem_type);
  os_params.sig = rmts_dev->sig;
  os_params.timeout_in_ms = rmts_dev->timeout_in_ms;

  /* Translate the iovec from fs_iovec to remotefs_iovec */
  for (loop_cnt = 0; loop_cnt < iovec_cnt; loop_cnt++)
  {
    remotefs_iovec[loop_cnt].sector_addr   = iovec[loop_cnt].sector_id;
    remotefs_iovec[loop_cnt].num_sector    =
      iovec[loop_cnt].size / FS_RMTS_SECTOR_SIZE;
    remotefs_iovec[loop_cnt].data_phy_addr = (uint32)iovec[loop_cnt].buff_base;
  }

  if (do_write)
  {
    status = rmts_write_iovec (rmts_dev->handle, partition_guid,
                               remotefs_iovec, iovec_cnt, &os_params);
  }
  else
  {
    status = rmts_read_iovec (rmts_dev->handle, partition_guid, remotefs_iovec,
                              iovec_cnt, &os_params);
  }

  result = fs_device_efs_rmts_get_fs_errno (rmts_dev, status);

  return result;
}

static enum fs_rmts_device_errno
fs_device_efs_rmts_read_iovec (efs_device_t base_dev,
                               fs_iovec_t iovec, uint32 iovec_cnt,
                               enum fs_rmts_partition_id which_partition)
{
  return fs_device_efs_rmts_read_write_iovec_helper (base_dev, iovec,
                                                     iovec_cnt,
                                                     which_partition, 0);
}

static enum fs_rmts_device_errno
fs_device_efs_rmts_write_iovec (efs_device_t base_dev, fs_iovec_t iovec,
            uint32 iovec_cnt, enum fs_rmts_partition_id which_partition)
{
  return fs_device_efs_rmts_read_write_iovec_helper (base_dev,
                                                     iovec, iovec_cnt,
                                                     which_partition, 1);
}

static int
fs_device_efs_rmts_get_last_errno (efs_device_t base_dev)
{
  struct fs_device_efs_rmts *rmts_dev = &base_dev->priv.rmts_dev;
  struct fs_device_efs_rmts_dev_errors *errors = &(rmts_dev->errors);

  return (errors->current_error);
}

static void
fs_device_efs_rmts_init_ops (efs_device_t base_dev)
{
  fs_device_init_invalid_ops (base_dev);
  base_dev->block_count = fs_device_efs_rmts_block_count;
  base_dev->page_size = fs_device_efs_rmts_page_size;
  base_dev->rmts_client_init = fs_device_efs_rmts_client_init;
  base_dev->rmts_get_rmts_modem_type = fs_device_efs_rmts_get_rmts_modem_type;
  base_dev->rmts_read_iovec = fs_device_efs_rmts_read_iovec;
  base_dev->rmts_write_iovec = fs_device_efs_rmts_write_iovec;
  base_dev->rmts_get_last_errno = fs_device_efs_rmts_get_last_errno;
}

void
fs_device_efs_rmts_init_dev (efs_device_t base_dev, uint32 ram_size)
{
  struct fs_device_efs_rmts *rmts_dev;
  uint32 end_sect;

  rmts_dev = &base_dev->priv.rmts_dev;
  ASSERT (rmts_dev != NULL);

  ASSERT (ram_size > 0);
  ASSERT (ram_size % FS_RMTS_SECTOR_SIZE == 0);

  end_sect = (ram_size / FS_RMTS_SECTOR_SIZE);

  base_dev->efs_device_type = FS_DEVICE_TYPE_EFS_RMTS;
  rmts_dev->ram_size = ram_size;
  rmts_dev->total_sectors = end_sect;
  rmts_dev->is_dev_inited = 0;

  rmts_dev->modem_ram = NULL;
  rmts_dev->shared_ram = NULL;
  rmts_dev->golden_ram = NULL;

  rmts_dev->fs1.start_sect = 0;
  rmts_dev->fs1.end_sect = (end_sect-1);

  rmts_dev->fs2.start_sect = 0;
  rmts_dev->fs2.end_sect = (end_sect-1);

  rmts_dev->fsg.start_sect = 0;
  rmts_dev->fsg.end_sect = (end_sect-1);

  rmts_dev->handle = NULL;

  fs_device_efs_rmts_init_ops (base_dev);
}

#endif /* FEATURE_EFS_EFS2_ON_RMTS */
