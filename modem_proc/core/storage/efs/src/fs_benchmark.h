/***********************************************************************
 * fs_benchmark.h
 *
 * Copyright (C) 2005-2008, 2010, 2012 QUALCOMM Technologies, Inc.
 *
 * Description: This file declares the functions which can be used for
 *              testing the performance of EFS2
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_benchmark.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-09-10    nr    Implement new FS Benchmarking scheme.
2012-03-08   wek    Rename macros and remove header files to compile for boot.
2010-06-25   dks    Added counters for partial-write, begin and suspend-erase.
2008-12-15    yg    Moved the file from public to private folder.
2008-09-16   yog    Remove trailing commas from the enum declaration.
2008-05-23    rp    Move fs_benchmark_counters to fs_counters.h
2008-04-03    eb    Added PACKED_POST directive for GCC support.
2008-03-06    sh    Replaced helper function with prepare/cleanup funcs
2008-01-09    sc    Added helper function to fs_benchmark_test
2007-10-26    rp    Removed the inclusion of rex.h
2006-10-17    sh    Added hotplug sector counters
2005-08-12    kk    Added benchmark queue data declarations
2005-03-29    sh    Created (based on fs_testapp.h)

===========================================================================*/

#ifndef __FS_BENCHMARK_H__
#define __FS_BENCHMARK_H__

#include "comdef.h"
#include "fs_sys_types.h"
#include "fs_counters.h"

/*
 * These are the posible states of the command and report queues
 */
enum fs_benchmark_queue_status_t {
  FS_BENCHMARK_ERROR    = -1,   /* Something wrong */
  FS_BENCHMARK_IDLE     =  0,   /* No benchmark running */
  FS_BENCHMARK_RESULT   =  1,   /* Result is ready */
  FS_BENCHMARK_RUNNING  =  2   /* Running, but no result yet */
};

typedef enum {
  BENCH_PREPARE,
  BENCH_CLEANUP
} fs_benchmark_prep_type;

/* This kind of function is a benchmark test procedure */
typedef int (*fs_benchmark_func) (int32 *, char *);
typedef int (*fs_benchmark_prepare_func) (int32 *, char *);
typedef int (*fs_benchmark_cleanup_func) (int32 *, char *);

/* Each test has a command code and function to call.
 * Helper function does the preparation and cleanup work.
 * All tests take the same arguments. */
struct fs_benchmark_test
{
  uint32 cmd;
  fs_benchmark_func func;
  fs_benchmark_prepare_func prepare;
  fs_benchmark_cleanup_func cleanup;
};

extern const struct fs_benchmark_test fs_benchmark_tests[];

#define FS_BENCHMARK_ARGS 5

/*
 * Command packet that Benchmark receives from Diag to start a test
 */
typedef PACKED struct PACKED_POST {
  uint16 sequence;              /* Incrementing number to match packets */
  uint16 repeat;                /* How many times to run this test */
  uint16 command;               /* Which test to run */
  int32 args[FS_BENCHMARK_ARGS]; /* We allow up to five arguments */
  char path[1];                 /* Pathname is variable-length */
} fs_benchmark_test_cmd;

/*
 * The results of a benchmark test
 */
typedef PACKED struct PACKED_POST
{
  fs_benchmark_test_cmd cmd;    /* The command (sans path) */
  uint16 failures;              /* Failure count */
  uint32 milliseconds;          /* Time in milliseconds */
  uint32 flash_write_page_counter;
  uint32 flash_write_partial_counter;
  uint32 flash_read_page_counter;
  uint32 flash_erase_block_counter;
  uint32 flash_erase_suspend_counter;
  uint32 total_bytes_write;
  uint32 total_bytes_read;
  uint32 flash_erase_begin_counter;

  /* Read page timings */
  uint32 flash_read_min_time;
  uint32 flash_read_total_time;
  uint32 flash_read_max_time;

  /* Write page timings */
  uint32 flash_write_min_time;
  uint32 flash_write_total_time;
  uint32 flash_write_max_time;

  /* Erase block timings */
  uint32 flash_erase_block_min_time;
  uint32 flash_erase_block_total_time;
  uint32 flash_erase_block_max_time;

  /* Erase suspend timings */
  uint32 flash_erase_suspend_min_time;
  uint32 flash_erase_suspend_total_time;
  uint32 flash_erase_suspend_max_time;

  uint32 microseconds;          /* Time in microseconds */
} fs_benchmark_report;

/* See fs_benchmark.c for descriptions */
void fs_benchmark_init (void);
void fs_cpu_hog_init (void);
void fs_benchmark_flush (void);
int fs_benchmark_put_cmd (fs_benchmark_test_cmd *cmd);
enum fs_benchmark_queue_status_t fs_benchmark_get_rpt (
  fs_benchmark_report *report);

#endif /* not __FS_BENCHMARK_H__ */
