/***********************************************************************
 * fs_counters.c
 *
 * This file holds the counters to track flash operations.
 * Copyright (C) 2008, 2010-2013 QUALCOMM Technologies, Inc.
 *
 * This file exposes the counters to track the read/write/erase operations
 * on the flash.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_counters.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-10-09   nr    Replace banned memory functions with safer versions.
2013-05-02   dks   Add flash_counter reset function.
2013-04-26   wek   Move all buffers to DDR when compiling for boot.
2012-11-28   nr    Implement EFS Profile framework.
2012-09-10   nr    Implement new FS Benchmarking scheme.
2012-04-03   rp    Code review comments for Separate EFS and Hotplug counters.
2012-03-14   rp    Separate EFS and Hotplug counters.
2011-11-03   dks   Make Hotplug independent of EFS.
2010-07-022  dks   Added commands to supress lint warnings on new counters.
2010-06-25   dks   Added counters for partial-write, begin and suspend-erase.
2008-12-12   rp    Create

===========================================================================*/

#include "fs_counters.h"
#include "assert.h"
#include "fs_protected_memory.h"
#include "fs_util.h"
#include <string.h>

/*
 * This structure maintains global counters for EFS operations.
 * Specifically, it is useful to measure how many page reads, page writes,
 * block erases, etc happen at the flash driver level.
 *
 * These counters are for INFORMATION and performance measurement
 * only, so no code should depend on them.
 *
 * The counters are present, whether benchmarking is enabled or not.
 */
struct fs_flash_counters fs_flash_counter = {0, 0, 0, 0, 0, 0};

struct fs_flash_time_counters fs_flash_time_counter;

uint32 fs_flash_pcache_miss_counter = 0;
uint32 fs_flash_pcache_hit_counter = 0;
uint32 fs_flash_buffer_miss_counter = 0;
uint32 fs_flash_buffer_hit_counter = 0;
uint32 fs_flash_dealloc_counter = 0;
uint32 fs_flash_bird_view_miss_counter = 0;
uint32 fs_flash_bird_view_hit_counter = 0;



/* These variables maintain a count of flash device operations for
 * performance analysis.  The values can be inspected by JTAG or by
 * the optional code below. */
static uint32 read_color_counters[NUM_FS_FLASHOP_CATEGORIES]           = { 0 };
static uint32 write_color_counters[NUM_FS_FLASHOP_CATEGORIES]          = { 0 };
static uint32 erase_color_counters[NUM_FS_FLASHOP_CATEGORIES]          = { 0 };
static uint32 write_partial_color_counters[NUM_FS_FLASHOP_CATEGORIES]  = { 0 };
static uint32 erase_begin_color_counters[NUM_FS_FLASHOP_CATEGORIES]    = { 0 };
static uint32 erase_suspend_color_counters[NUM_FS_FLASHOP_CATEGORIES]  = { 0 };
static uint32 buffer_fill_counters[NUM_FS_BUFOP_CATEGORIES]            = { 0 };


/* The counters should always be updated, but there might not be any
 * code to reference (read) them.  They are still useful from within a
 * debugger.  We explain it to Lint this way: */
//lint -esym(551, write_color_counters)
//lint -esym(551, read_color_counters)
//lint -esym(551, erase_color_counters)
//lint -esym(551, write_partial_color_counters)
//lint -esym(551, erase_begin_color_counters)
//lint -esym(551, erase_suspend_color_counters)
//lint -esym(551, buffer_fill_counters)
//lint -esym(551, fs_mnand_counters)

static void
fs_counters_update_timers (struct fs_time_counter *time_counter,
                           uint32 new_time)
{
  /* Add the new time to the total */
  time_counter->total_time += new_time;

  if (time_counter->max_time < new_time)
  {
    time_counter->max_time = new_time;
  }
  if (time_counter->min_time > new_time)
  {
    time_counter->min_time = new_time;
  }
}

static void
fs_update_read_counters (enum fs_flashop_category category, uint32 count,
                         uint32 new_time)
{
  fs_flash_read_page_counter += count;
  read_color_counters[category] += count;

  /* Update the timers */
  fs_counters_update_timers (&fs_flash_time_counter.read_timings, new_time);
}

static void
fs_update_write_counters(enum fs_flashop_category category, uint32 count,
                         uint32 new_time)
{
  fs_flash_write_page_counter += count;
  write_color_counters[category] += count;

  /* Update the timers */
  fs_counters_update_timers (&fs_flash_time_counter.write_timings, new_time);
}

static void
fs_update_erase_counters (enum fs_flashop_category category, uint32 count,
                          uint32 new_time)
{
  fs_flash_erase_block_counter += count;
  erase_color_counters[category] += count;

  /* Update the timers */
  fs_counters_update_timers (&fs_flash_time_counter.erase_block_timings,
                             new_time);
}

static void
fs_update_write_partial_counters (enum fs_flashop_category category,
                                  uint32 count)
{
  fs_flash_write_partial_counter += count;
  write_partial_color_counters[category] += count;
}

static void
fs_update_erase_begin_counters (enum fs_flashop_category category,
                                uint32 count)
{
  fs_flash_erase_begin_counter += count;
  erase_begin_color_counters[category] += count;
}

static void
fs_update_erase_suspend_counters (enum fs_flashop_category category,
                                  uint32 count, uint32 new_time)
{
  fs_flash_erase_suspend_counter += count;
  erase_suspend_color_counters[category] += count;

  /* Update the timers */
  fs_counters_update_timers (&fs_flash_time_counter.erase_suspend_timings,
                             new_time);
}


void
fs_counters_init (void)
{
  memset (&fs_flash_counter, 0, sizeof(fs_flash_counter));

  /* Reset the timers. */
  memset (&fs_flash_time_counter, 0, sizeof(fs_flash_time_counter));
  fs_flash_time_counter.read_timings.min_time = (~0u);
  fs_flash_time_counter.write_timings.min_time = (~0u);
  fs_flash_time_counter.erase_block_timings.min_time = (~0u);


  fs_flash_pcache_miss_counter = 0;
  fs_flash_pcache_hit_counter = 0;
  fs_flash_buffer_miss_counter = 0;
  fs_flash_buffer_hit_counter = 0;
  fs_flash_dealloc_counter = 0;
  fs_flash_bird_view_miss_counter = 0;
  fs_flash_bird_view_hit_counter = 0;

  memset (read_color_counters, 0, sizeof (read_color_counters));
  memset (write_color_counters, 0, sizeof (write_color_counters));
  memset (erase_color_counters, 0, sizeof (erase_color_counters));
  memset (write_partial_color_counters, 0,
          sizeof (write_partial_color_counters));
  memset (erase_begin_color_counters, 0, sizeof (erase_begin_color_counters));
  memset (erase_suspend_color_counters, 0,
          sizeof (erase_suspend_color_counters));
  memset (buffer_fill_counters, 0, sizeof (buffer_fill_counters));

}

void
fs_counters_update (enum fs_flashop op, enum fs_flashop_category category,
                    uint32 count, uint32 time)
{
  ASSERT (op < FS_NUM_FOP_OPS);
  ASSERT (category < NUM_FS_FLASHOP_CATEGORIES);

  switch (op)
  {
    case FS_FOP_READ:
      fs_update_read_counters (category, count, time);
      break;

    case FS_FOP_WRITE:
      fs_update_write_counters (category, count, time);
      break;

    case FS_FOP_ERASE:
      fs_update_erase_counters (category, count, time);
      break;

    case FS_FOP_WRITE_PARTIAL:
      fs_update_write_partial_counters (category, count);
      break;

    case FS_FOP_ERASE_BEGIN:
      fs_update_erase_begin_counters (category, count);
      break;

    case FS_FOP_ERASE_SUSPEND:
      fs_update_erase_suspend_counters (category, count, time);
      break;

    default:
      ASSERT (0);
      break;
  }
}

void
fs_counters_reset_flash_counters (void)
{
  memset (&fs_flash_counter, 0, sizeof(fs_flash_counter));
}

void
fs_device_buffer_fill (enum fs_bufop_category category)
{
  ASSERT (category < NUM_FS_BUFOP_CATEGORIES);
  buffer_fill_counters[category]++;
}


void
fs_counters_take_snaphot (struct fs_flash_counters *snapshot)
{
  fs_memscpy (snapshot, sizeof (struct fs_flash_counters), &fs_flash_counter,
           sizeof (struct fs_flash_counters));
}

#ifdef FS_UNIT_TEST
struct stat_info {
  const char *name;
  uint32 *counter;
  uint32 last_value;
  double min_factor;
  double max_factor;
  double total_factor;
};

/* Enable these to show the different kinds of counters. */
#undef SHOW_SUPER_COUNTERS
#undef SHOW_USER_COUNTERS
#undef SHOW_LOG_COUNTERS
#undef SHOW_DATA_GC_MOVE_COUNTERS
#undef SHOW_DATA_GC_RECOVERY_COUNTERS
#undef SHOW_USER_RECOVERY_COUNTERS
#undef SHOW_PTABLE_GC_MOVE_COUNTERS
#undef SHOW_PTABLE_COUNTERS
#undef SHOW_FACTORY_RECOVERY_COUNTERS
#undef SHOW_UNKNOWN_COUNTERS

#define SHOW_WRITE_COUNTERS         /* Nand & Nor */
#define SHOW_READ_COUNTERS          /* Nand & Nor */
#undef SHOW_ERASE_COUNTERS          /* Nand & Nor */
#undef SHOW_WRITE_PARTIAL_COUNTERS  /* Nor only */
#undef SHOW_ERASE_BEGIN_COUNTERS    /* Nor only */
#undef SHOW_ERASE_SUSPEND_COUNTERS  /* Nor only */

#undef SHOW_PCACHE_COUNTERS
#undef SHOW_BUFFER_COUNTERS
#undef SHOW_BUFFER_CACHE_COUNTERS

#undef SHOW_DEALLOC_COUNTER

static struct stat_info stat_table[] = {
#ifdef SHOW_WRITE_COUNTERS
  { "write_page", &fs_flash_write_page_counter, 0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_READ_COUNTERS
  { "read_page", &fs_flash_read_page_counter, 0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_ERASE_COUNTERS
  { "erase_block", &fs_flash_erase_block_counter, 0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_WRITE_PARTIAL_COUNTERS
  { "write_partial", &fs_flash_write_partial_counter, 0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_ERASE_BEGIN_COUNTERS
  { "erase_begin", &fs_flash_erase_begin_counter, 0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_ERASE_SUSPEND_COUNTERS
  { "erase_suspend", &fs_flash_erase_suspend_counter, 0, 0.0, 0.0, 0.0 },
#endif


  /* Write counters. */
#ifdef SHOW_WRITE_COUNTERS
#ifdef SHOW_SUPER_COUNTERS
  { "write_page-super", &write_color_counters[FS_FOP_SUPER],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_USER_COUNTERS
  { "write_page-user", &write_color_counters[FS_FOP_USER],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_LOG_COUNTERS
  { "write_page-log", &write_color_counters[FS_FOP_LOG],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_DATA_GC_MOVE_COUNTERS
  { "write_page-data_gc_move", &write_color_counters[FS_FOP_DATA_GC_MOVE],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_DATA_GC_RECOVERY_COUNTERS
  { "write_page-data_gc_recovery",
    &write_color_counters[FS_FOP_DATA_GC_RECOVERY],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_USER_RECOVERY_COUNTERS
  { "write_page-user_recovery", &write_color_counters[FS_FOP_USER_RECOVERY],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_PTABLE_GC_MOVE_COUNTERS
  { "write_page-ptable_gc_move", &write_color_counters[FS_FOP_PTABLE_GC_MOVE],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_PTABLE_COUNTERS
  { "write_page-ptable", &write_color_counters[FS_FOP_PTABLE],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_FACTORY_RECOVERY_COUNTERS
  { "write_page-factory_recovery",
    &write_color_counters[FS_FOP_FACTORY_RECOVERY],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_UNKNOWN_COUNTERS
  { "write_page-unknown", &write_color_counters[FS_FOP_UNKNOWN],
    0, 0.0, 0.0, 0.0 },
#endif
#endif


  /* Read counters. */
#ifdef SHOW_READ_COUNTERS
#ifdef SHOW_SUPER_COUNTERS
  { "read_page-super", &read_color_counters[FS_FOP_SUPER],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_USER_COUNTERS
  { "read_page-user", &read_color_counters[FS_FOP_USER],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_LOG_COUNTERS
  { "read_page-log", &read_color_counters[FS_FOP_LOG],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_DATA_GC_MOVE_COUNTERS
  { "read_page-data_gc_move", &read_color_counters[FS_FOP_DATA_GC_MOVE],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_DATA_GC_RECOVERY_COUNTERS
  { "read_page-data_gc_recovery",
    &read_color_counters[FS_FOP_DATA_GC_RECOVERY],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_USER_RECOVERY_COUNTERS
  { "read_page-user_recovery", &read_color_counters[FS_FOP_USER_RECOVERY],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_PTABLE_GC_MOVE_COUNTERS
  { "read_page-ptable_gc_move", &read_color_counters[FS_FOP_PTABLE_GC_MOVE],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_PTABLE_COUNTERS
  { "read_page-ptable", &read_color_counters[FS_FOP_PTABLE],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_FACTORY_RECOVERY_COUNTERS
  { "read_page-factory_recovery",
    &read_color_counters[FS_FOP_FACTORY_RECOVERY],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_UNKNOWN_COUNTERS
  { "read_page-unknown", &read_color_counters[FS_FOP_UNKNOWN],
    0, 0.0, 0.0, 0.0 },
#endif
#endif


  /* erase counters. */
#ifdef SHOW_ERASE_COUNTERS
#ifdef SHOW_SUPER_COUNTERS
  { "erase_block-super", &erase_color_counters[FS_FOP_SUPER],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_USER_COUNTERS
  { "erase_block-user", &erase_color_counters[FS_FOP_USER],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_LOG_COUNTERS
  { "erase_block-log", &erase_color_counters[FS_FOP_LOG],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_DATA_GC_MOVE_COUNTERS
  { "erase_block-data_gc_move", &erase_color_counters[FS_FOP_DATA_GC_MOVE],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_DATA_GC_RECOVERY_COUNTERS
  { "erase_block-data_gc_recovery",
    &erase_color_counters[FS_FOP_DATA_GC_RECOVERY],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_USER_RECOVERY_COUNTERS
  { "erase_block-user_recovery", &erase_color_counters[FS_FOP_USER_RECOVERY],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_PTABLE_GC_MOVE_COUNTERS
  { "erase_block-ptable_gc_move", &erase_color_counters[FS_FOP_PTABLE_GC_MOVE],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_PTABLE_COUNTERS
  { "erase_block-ptable", &erase_color_counters[FS_FOP_PTABLE],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_FACTORY_RECOVERY_COUNTERS
  { "erase_block-factory_recovery",
    &erase_color_counters[FS_FOP_FACTORY_RECOVERY],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_UNKNOWN_COUNTERS
  { "erase_block-unknown", &erase_color_counters[FS_FOP_UNKNOWN],
    0, 0.0, 0.0, 0.0 },
#endif
#endif


    /* Write-partial counters. */
#ifdef SHOW_WRITE_PARTIAL_COUNTERS
#ifdef SHOW_SUPER_COUNTERS
    { "write_partial-super", &write_partial_color_counters[FS_FOP_SUPER],
      0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_USER_COUNTERS
    { "write_partial-user", &write_partial_color_counters[FS_FOP_USER],
      0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_LOG_COUNTERS
    { "write_partial-log", &write_partial_color_counters[FS_FOP_LOG],
      0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_DATA_GC_MOVE_COUNTERS
    { "write_partial-data_gc_move",
      &write_partial_color_counters[FS_FOP_DATA_GC_MOVE],
      0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_DATA_GC_RECOVERY_COUNTERS
    { "write_partial-data_gc_recovery",
      &write_partial_color_counters[FS_FOP_DATA_GC_RECOVERY],
      0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_USER_RECOVERY_COUNTERS
    { "write_partial-user_recovery",
       &write_partial_color_counters[FS_FOP_USER_RECOVERY], 0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_PTABLE_GC_MOVE_COUNTERS
    { "write_partial-ptable_gc_move",
      &write_partial_color_counters[FS_FOP_PTABLE_GC_MOVE], 0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_PTABLE_COUNTERS
    { "write_partial-ptable",
      &write_partial_color_counters[FS_FOP_PTABLE],
      0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_FACTORY_RECOVERY_COUNTERS
    { "write_partial-factory_recovery",
      &write_partial_color_counters[FS_FOP_FACTORY_RECOVERY],
      0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_UNKNOWN_COUNTERS
    { "write_partial-unknown",
       &write_partial_color_counters[FS_FOP_UNKNOWN], 0, 0.0, 0.0, 0.0 },
#endif
#endif


  /* erase-begin counters. */
#ifdef SHOW_ERASE_BEGIN_COUNTERS
#ifdef SHOW_SUPER_COUNTERS
  { "erase_begin-super", &erase_begin_color_counters[FS_FOP_SUPER],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_USER_COUNTERS
  { "erase_begin-user", &erase_begin_color_counters[FS_FOP_USER],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_LOG_COUNTERS
  { "erase_begin-log", &erase_begin_color_counters[FS_FOP_LOG],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_DATA_GC_MOVE_COUNTERS
  { "erase_begin-data_gc_move",
    &erase_begin_color_counters[FS_FOP_DATA_GC_MOVE], 0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_DATA_GC_RECOVERY_COUNTERS
  { "erase_begin-data_gc_recovery",
    &erase_begin_color_counters[FS_FOP_DATA_GC_RECOVERY],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_USER_RECOVERY_COUNTERS
  { "erase_begin-user_recovery",
    &erase_begin_color_counters[FS_FOP_USER_RECOVERY], 0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_PTABLE_GC_MOVE_COUNTERS
  { "erase_begin-ptable_gc_move",
    &erase_begin_color_counters[FS_FOP_PTABLE_GC_MOVE], 0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_PTABLE_COUNTERS
  { "erase_begin-ptable", &erase_begin_color_counters[FS_FOP_PTABLE],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_FACTORY_RECOVERY_COUNTERS
  { "erase_begin-factory_recovery",
    &erase_begin_color_counters[FS_FOP_FACTORY_RECOVERY],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_UNKNOWN_COUNTERS
  { "erase_begin-unknown", &erase_begin_color_counters[FS_FOP_UNKNOWN],
    0, 0.0, 0.0, 0.0 },
#endif
#endif


  /* erase-suspend counters. */
#ifdef SHOW_ERASE_SUSPEND_COUNTERS
#ifdef SHOW_SUPER_COUNTERS
  { "erase_suspend-super", &erase_suspend_color_counters[FS_FOP_SUPER],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_USER_COUNTERS
  { "erase_suspend-user", &erase_suspend_color_counters[FS_FOP_USER],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_LOG_COUNTERS
  { "erase_suspend-log", &erase_suspend_color_counters[FS_FOP_LOG],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_DATA_GC_MOVE_COUNTERS
  { "erase_suspend-data_gc_move",
    &erase_suspend_color_counters[FS_FOP_DATA_GC_MOVE], 0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_DATA_GC_RECOVERY_COUNTERS
  { "erase_suspend-data_gc_recovery",
    &erase_suspend_color_counters[FS_FOP_DATA_GC_RECOVERY],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_USER_RECOVERY_COUNTERS
  { "erase_suspend-user_recovery",
    &erase_suspend_color_counters[FS_FOP_USER_RECOVERY], 0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_PTABLE_GC_MOVE_COUNTERS
  { "erase_suspend-ptable_gc_move",
    &erase_suspend_color_counters[FS_FOP_PTABLE_GC_MOVE], 0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_PTABLE_COUNTERS
  { "erase_suspend-ptable", &erase_suspend_color_counters[FS_FOP_PTABLE],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_FACTORY_RECOVERY_COUNTERS
  { "erase_suspend-factory_recovery",
    &erase_suspend_color_counters[FS_FOP_FACTORY_RECOVERY],
    0, 0.0, 0.0, 0.0 },
#endif
#ifdef SHOW_UNKNOWN_COUNTERS
  { "erase_suspend-unknown", &erase_suspend_color_counters[FS_FOP_UNKNOWN],
    0, 0.0, 0.0, 0.0 },
#endif
#endif


#ifdef SHOW_PCACHE_COUNTERS
  { "pcache_hit", &fs_flash_pcache_hit_counter, 0, 0.0, 0.0, 0.0 },
  { "pcache_miss", &fs_flash_pcache_miss_counter, 0, 0.0, 0.0, 0.0 },
#endif

#ifdef SHOW_BUFFER_COUNTERS
  { "buffer_fill-db", &buffer_fill_counters[FS_BOP_DB], 0, 0.0, 0.0, 0.0 },
  { "buffer_fill-fs", &buffer_fill_counters[FS_BOP_FS], 0, 0.0, 0.0, 0.0 },
  { "buffer_fill-alloc-tree", &buffer_fill_counters[FS_BOP_ALLOC_TREE],
    0, 0.0, 0.0, 0.0 },
  { "buffer_fill-alloc-map", &buffer_fill_counters[FS_BOP_ALLOC_MAP],
    0, 0.0, 0.0, 0.0 },
  { "buffer_fill-unknown", &buffer_fill_counters[FS_BOP_UNKNOWN],
    0, 0.0, 0.0, 0.0 },
#endif

#ifdef SHOW_BUFFER_CACHE_COUNTERS
  { "buffer_hit", &fs_flash_buffer_hit_counter, 0, 0.0, 0.0, 0.0 },
  { "buffer_miss", &fs_flash_buffer_miss_counter, 0, 0.0, 0.0, 0.0 },
#endif

#ifdef SHOW_DEALLOC_COUNTER
  { "dealloc", &fs_flash_dealloc_counter, 0, 0.0, 0.0, 0.0 },
#endif

  { NULL, NULL, 0, 0.0, 0.0, 0.0 },
};
uint32 sample_counter;

void
fs_device_reset_stats (void)
{
  struct stat_info *p;

  for (p = stat_table; p->name != NULL; p++) {
    *(p->counter) = 0;
    p->last_value = 0;
    p->min_factor = 1e32;
    p->max_factor = -1e32;
    p->total_factor = 0.0;
  }

  sample_counter = 0;
}

void
fs_device_take_sample (unsigned user_ops)
{
  struct stat_info *p;

  for (p = stat_table; p->name != NULL; p++) {
    double factor = (double) (*(p->counter) - p->last_value) / user_ops;

    if (factor < p->min_factor)
      p->min_factor = factor;
    if (factor > p->max_factor)
      p->max_factor = factor;
    p->total_factor += factor;

    p->last_value = *(p->counter);
  }

  sample_counter++;
}

unsigned
fs_device_number_stats (void)
{
  unsigned count = 0;
  struct stat_info *p;
  for (p = stat_table; p->name != NULL; p++) {
    count++;
  }
  return count;
}

struct stat_result
fs_device_get_stat (unsigned slot)
{
  struct stat_result res;
  struct stat_info *p;

  p = &stat_table[slot];
  res.name = p->name;
  res.min_factor = p->min_factor;
  res.max_factor = p->max_factor;
  res.avg_factor = p->total_factor / sample_counter;
  return res;
}

#endif /* FS_UNIT_TEST */

