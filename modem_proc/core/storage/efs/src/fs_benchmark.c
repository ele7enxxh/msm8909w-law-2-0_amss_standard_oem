/***********************************************************************
 *
 * fs_benchmark.c
 *
 * Filesystem Benchmark Suite
 *
 * Copyright (C) 2006-2014 QUALCOMM Technologies, Inc.
 *
 * This code provides (optional) functions to call the public EFS API
 * and measure the time of typical operations.  This is used to measure
 * on-target performance.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_benchmark.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-04-26   vm    Use correct FS_MSG macros to save RAM.
2013-11-04   nr    Fix return value when benchmark buffers are not inited.
2013-10-07   nr    Malloc the benchmark queue and report buffers.
2013-01-28   nr    Clean up references to tmc task.h.
2013-01-28   wek   Limit total enumerate entries by count instead of time.
2013-01-15   wek   Reset the microsecond counter before each test.
2012-12-05   nr    Fix compiler warnings.
2012-11-09   nr    Update rmts to use new api's.
2012-10-26   rp    Malloc the safe benchmark buffers.
2012-09-10   nr    Implement new FS Benchmarking scheme.
2012-05-30   wek   Use macros instead of functions for task stop and offline.
2012-05-18   wek   Remove jzap header file.
2012-04-30   rp    Add safe benchmark command to measure efs_sync timings.
2012-04-10   wek   Add feature around the initialization of hotplug counters.
2012-04-03   rp    Code review comments for Separate EFS and Hotplug counters.
2012-03-20   wek   Cleanup customer header file inclusion.
2012-03-14   rp    Separate EFS and Hotplug counters.
2012-02-29   rp    Make Tasks free-float (CPU affinity free)
2011-11-03   dks   Make Hotplug independent of EFS.
2011-11-02   wek   Use library supplied strl cpy and cat instead of fs_strl*.
2011-11-02   nr    Klocwork error fixes in Modem code.
2011-10-24   vm    Klocwork error fixes
2011-10-04   dks   Make EFS VU independent of Hotplug and Hfat VUs.
2011-06-01   dks   Migrate from TMC to rcinit.
2011-06-08   dks   use fs_path_buff[2] & [3] as they are not frequently used.
2011-04-21   wek   Remove compiler warnings.
2010-10-19   wek   Remove dependency with page size macro.
2010-09-29   vr    Use time_genoff_opr() instead of deprecated clk_read_ms()
2010-08-26   dks   Add benchmark command for listing files like ls command.
2010-08-26   rp    Allow safe benchmark tests on all SD devices.
2010-07-30   wek   Dynamically allocate all buffers dependent on page size.
2010-06-25   dks   Added counters for partial-write, begin and suspend-erase.
2010-05-13   wek   Make sync_no_wait non blocking.
2010-02-19   rp    Maintain same API files between 1.2 and 2.0 codebases.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-05   yg    Remove deprecated EFS1 API test code.
2009-11-04   dks   Added path arg in efs_image_prepare.
2009-10-29   wek   Remove UNSAFE benchmark feature undef.
2009-10-28   yg    Add UNSAFE benchmark feature undef.
2009-09-04   yg    Fix safe write to continue test for smaller space.
2009-08-11   yg    Fix compiler warning for unused local variable.
2009-07-06   ebb   Added #include fs_config.h to pull boot definitions.
2009-06-12   ebb   Moved rex_xxx calls to fs_osal.h abstraction.
2009-05-08   ebb   Renamed hotplug counters to blockdev counters.
2009-04-20   yg    Return blk_size based count for df bench test.
2009-04-03   wek   Added testing efs_utime() API.
2009-02-18   ebb   Added memory protection test to trigger fault.
2009-02-05   wek   Fix compiler erros on targets builds because of #elif.
2009-01-27   sh    Removed extra arguments from Bab Single Event messages
2009-01-26   wek   Add raw setvbuf & fflush, Fix raw stdio bugs.
2009-01-15   ebb   Added delay after writing files for safe_read_test.
2008-12-12   yg    Fix compiler warning for unused static functions.
2008-12-12   rp    Add counters for the reliable write functions.
2008-12-02   yg    Modify reset test to just return error.
2008-11-25   sh    Move hotplug open/close out of timed test.
2008-11-24   sh    Add random-write hotplug test.
2008-11-19   yg    Move cpu_hog task init from public, disable for safe bench.
2008-11-19   sh    Babylon references must be conditional on feature
2008-11-06   sh    Added Babylon support and port tests
2008-10-30   yg    Fix compiler warning for unused functions.
2008-09-15   yg    For safe enum test return the file/directory counts.
2008-05-07   sh    Correct the no-unsafe-benchmarks case.
2008-04-30   sh    Simplify hotplug stubs.
2008-03-28   ebb   Make hotplug related functions complilation conditional.
2008-03-06   sh    Revert benchmark command numbers to prior values.
2008-01-15   sc    Fixed compiler warning by casting packed array
2008-01-09   sc    Added safe benchmark tests that are always on
2007-12-07   rp    Enhancements to the statvfs struct.
2007-11-16   rp    Added efs_chown_recursive() and efs_chmod_recursive().
2007-08-19   rp    Changed FS_BENCHMARK_MAX_DESCRIPTORS from 128 to 16.
2007-08-17   yg    Aligned benchmark buffer.
2007-08-10   sh    Return a failure count for unrecognized commands.
2007-07-25   sh    Add opendir/readdir test.
2007-02-20   sh    Add efs_deltree test.
2007-01-12   dlb   Use safer string functions.
2006-12-05   dlb   Print maxwrite if it is known.
2006-11-22   dlb   Conditionalize benchmarks based on EFS capabilities.
2006-11-14   sh    Fix INTLOCK use in reset function.
2006-11-02   rp    Added stream based file-op functions.
2006-10-17   sh    Counters now exist in all builds.
2006-10-12   sh    Correct compiler warning
2006-10-06   yg    Add more benchmark functions for file access.
2006-10-06   sh    Correct fs_test_df byte counts
2006-10-02   sh    Add direct media block-access functions. (lint repair)
2006-09-29   sh    Lint cleanup
2006-09-25   yg    Remove unused table entry.
2006-09-14   sh    Removed FTL dependency
2006-08-28   yg    Added some benchmark config func for FTL cache.
2006-06-23   sh    Add stub functions to use when the feature is disabled
2006-05-25   sh    Add a format command
2006-05-15   sh    Include fs_hotplug.h
2006-05-11   sh    Renamed efs_hotplug_... to hotplug_...
2006-05-05   sh    Add hook to set hotplug poll rate
2006-01-26   sh    Treat any incomplete write operations as failure.
2005-11-18   sh    Added EFS1 read/write tests
2005-10-14   sh    Create  (Written by Kiran K, Summer 2005)

===========================================================================*/
#include "comdef.h"
#include "fs_config_i.h"

#include "fs_benchmark.h"
#include "fs_pathbuff.h"
#include "fs_util.h"
#include "assert.h"
#include "stringl/stringl.h"

/* The rest of the code is conditional on this feature.  By default,
 * this includes only the "safe" tests, so there's no real reason not
 * to enable it.
 */
#ifdef FEATURE_FS_BENCHMARK

/* Uncomment the next line to enable unsafe benchmark */
/* #define FEATURE_FS_UNSAFE_BENCHMARK */

#include <string.h>
#include <stdio.h>    /* For sprintf */
#include <stdlib.h>   /* For malloc  */

#include "fs_err.h"
#include "fs_errno.h"
#include "fs_public.h"
#include "fs_util.h"
#include "queue.h"
#include "ran.h"
#include "fs_osal.h"
#include "fs_task_i.h"
#include "time.h"
#include "fs_estdio.h"
#include "fs_privatedir.h"
#include "fs_babylon.h"
#include "fs_util.h"
#include "fs_sys_types.h"
#include "fs_sync.h"
#include "fs_priv_funcs.h"
#include "fs_timings.h"
#include "fs_timetick.h"

#ifdef FEATURE_FS_HOTPLUG
  #include "fs_hotplug.h"
  #include "fs_hotplug_counters.h"
#endif

/* This is the amount of time used by the safe benchmark tests to
 * transfer data to the device.  Since bandwidth will vary, we aim for
 * a specific amount of time in writing data.
 *
 * eg: safe read test needs to write data to the device before
 * measuring the read performance. So, it writes data for
 * BENCH_TIME_LIMIT amount of time before reading it back.
 */
#ifndef FS_BENCHMARK_TIME_MS
#define FS_BENCHMARK_TIME_MS (10 * 1000) /* Time in milliseconds */
#endif

/* Off-target, we advance 10% each time you ask */
#ifdef FS_UNIT_TEST
static int fake_time = 1;
#define fs_time_ms_u64() (fake_time += (FS_BENCHMARK_TIME_MS / 10))
#endif

/*
 * Common header for all queues
 */
typedef struct {
  q_link_type          link;         /* Queue link */
  q_type               *done_q_ptr;  /* Queue to place cmd */
} fs_benchmark_hdr_type;

/* Benchmark command queue element */
typedef struct
{
  fs_benchmark_hdr_type hdr;    /* queue header */
  fs_benchmark_test_cmd cmd;    /* Command packet */
  char whole_path[FS_PATH_MAX+1]; /* Path */
} fs_benchmark_queued_cmd;

/* Benchmark report type */
typedef struct
{
  fs_benchmark_hdr_type hdr;    /* header */
  fs_benchmark_report report;
} fs_benchmark_queued_rpt;


/* Task glue */
#define FS_BENCHMARK_STACK_SIZE 2048
static fs_os_tcb_type fs_benchmark_tcb;
static unsigned char fs_benchmark_stack [ FS_BENCHMARK_STACK_SIZE ];

/* Timers and signals */
#define FS_BENCHMARK_SLEEP_TIMER_SIG    0x0002
#define FS_BENCHMARK_CMD_Q_SIG          0x0004
#define FS_BENCHMARK_RPT_Q_SIG          0x0008

/* How many outstanding commands can be queued at once?
   Each one costs a full pathlength of memory */
#define FS_BENCHMARK_MAX_CMDS 20
#define FS_BENCHMARK_MAX_RPTS FS_BENCHMARK_MAX_CMDS

/* General purpose pause timer for tests */
static fs_os_timer_type fs_benchmark_sleep_timer;

/* Used to see if a command is being processed at a given time;
   set true when commands being processed, false when not */
static int test_running = 0;

/* True when all commands are being discarded from the queue. */
static int flushing = 0;

/*
 * The benchmark uses this buffer for storing the bulk data for read/write
 * It needs to be as big as the biggest required single operation.
 */
#define BENCH_BUFFER_BYTES  (64 * 1024)
#define BENCH_BUFFER_DWORDS (BENCH_BUFFER_BYTES / sizeof(dword))

/* The +32 is to allow an intentionally misaligned start address of 0
 * to 31 bytes */
#define BENCH_BUFFER_ACTUAL_BYTES (BENCH_BUFFER_BYTES + 32)

static dword *fs_bench_buffer = NULL;
static dword *fs_bench_read_buffer = NULL;

static uint8 fs_benchmark_buffers_initialized = 0;

#define FS_BENCHMARK_DIR "/bench"

/* Benchmark buffer is memset with this seed. */
static int bench_buffer_fill_seed = 1;

/* Benchmark time in microseconds. */
static uint64 benchmark_time_in_us = 0;

#ifdef FEATURE_FS_UNSAFE_BENCHMARK
/* Array of pointers to open EFS_EFILE instances. It controls how many files
 * can be opened simultaneously using fs_stdio calls. */
  #define FS_BENCHMARK_MAX_STREAMS      1
/* Size of internal fs_stdio buffer.
 * This size was chosen twice max transaction to test if there is a gain in
 * performance by buffering more than the max transaction. */
  #define STDIO_BUF_SIZE (FS_MAX_TRANSACTION_DATA_PAGES * FS_MAX_PAGE_SIZE * 2)
#else
  #define STDIO_BUF_SIZE                1
  #define FS_BENCHMARK_MAX_STREAMS      1
#endif
/* Stdio test buffer, only used on unsafe benchmark calls. */
FS_ALIGN static char stdio_buffer[FS_BENCHMARK_MAX_STREAMS][STDIO_BUF_SIZE];

/* Global count of how many bytes are read/written during a series of tests */
static uint32 total_bytes_write = 0;
static uint32 total_bytes_read = 0;

/*
 * The CPU HOG task will burn some amount of the CPU on command,
 * to simulate loading
 */
#define FS_CPU_HOG_STACK_SIZ 2048

static fs_os_tcb_type fs_cpu_hog_tcb;
static unsigned char fs_cpu_hog_stack [FS_CPU_HOG_STACK_SIZ];

/* These two values set the ratio of Busy to Sleep, in milliseconds */
static uint32 fs_cpu_hog_run_ms = 0;
static uint32 fs_cpu_hog_sleep_ms = 1000;

#define FS_CPU_HOG_TIMER_SIG  0x0001
static fs_os_timer_type fs_cpu_hog_timer;

/* The ran.[ch] code has one global seed value.  We set it at the
 * start of our task, but unfortunately, mc_init_ran() also sets it to
 * the ESN value.
 * Which is usually zero.
 * Which is an invalid seed value.
 */
static dword fs_benchmark_seed = 0xDECAFBAD;

/* Fill the benchmark buffer with a fresh block of psuedo-random data */
static void
randomize_bench_buffer (dword seed)
{
  unsigned int i;

  ASSERT (fs_bench_buffer != NULL);

  ran_seed (seed);
  for (i = 0; i < BENCH_BUFFER_DWORDS; i++)
    fs_bench_buffer[i] = ran_next();

  fs_benchmark_seed = ran_next();
}

#ifdef FS_UNIT_TEST
  /* Set by the tests to indicate the number of mallocs after each reset to
     allow before returning failure. Zero indicates infinite. */
  volatile int ut_fs_benchmark_allowed_malloc_count = 0;

  /* Count the total number of benchmark mallocs that were called since
    reset. */
  static int ut_fs_benchmark_total_malloc_count = 0;
#endif

static void*
fs_benchmark_malloc (uint32 size)
{
  void *ptr = NULL;

#ifdef FS_UNIT_TEST
  if ((ut_fs_benchmark_allowed_malloc_count) &&
      (ut_fs_benchmark_total_malloc_count >=
       ut_fs_benchmark_allowed_malloc_count))
  {
    return NULL;
  }
  ut_fs_benchmark_total_malloc_count++;
#endif

  ptr = malloc (size);
  return ptr;
}

static void
fs_benchmark_free (void *ptr)
{
  ASSERT (ptr != NULL);
  free (ptr);
}


/* All benchmark tests are expected to return successfully, since this is
   a performance test, not a quality-assurance test.
   In the event we get an errno value, we do this. */
static void
fs_benchmark_errno (char *msg, char *fname)
{
  (void) msg;
  (void) fname;

  EFS_BAB_DEBUG_PRINTF ("ERROR: %s on \"%s\" errno = %d\n\r", msg, fname,
                        efs_errno);
}

#if FS_BENCHMARK_MAX_STREAMS > FS_MAX_DESCRIPTORS
  #error "FS_BENCHMARK_MAX_STREAMS is > FS_MAX_DESCRIPTORS"
#endif

/*
 * This descriptor is used to seperate the open/close routines from
 * the actual read/write routines. If the script doesn't close the
 * file, then it would have a valid fd, which will be closed just
 * before opening another file. It is better to keep protected
 * static variable than getting the fd from script, since it is
 * difficult to verify if we get fd from outside. */
static int raw_fd = -1;

/* Array of streams to use on "fs_test_raw_" stdio calls */
static EFS_EFILE *raw_efs_efile[FS_BENCHMARK_MAX_STREAMS] = {NULL, };

/*
 * Sleep 'ms' milliseconds by yeilding our task.
 */
static void
fs_benchmark_sleep (int ms)
{
  fs_os_clr_sigs (&fs_benchmark_tcb, FS_BENCHMARK_SLEEP_TIMER_SIG);
  fs_os_set_timer(&fs_benchmark_sleep_timer, ms);
  fs_os_wait (FS_BENCHMARK_SLEEP_TIMER_SIG); /* Ignoring other signals */
}



/* ============== Benchmark Tests ============== */

/* Sleep for args[0] milliseconds */
int
fs_test_sleep_ms (int32 *args, char *path)
{
  (void) path;

  EFS_BAB_DEBUG_PRINTF ("Sleeping for %d mS...", args[0]);
  fs_benchmark_sleep (args[0]);
  return 0;
}

/* Do nothing at all */
int
fs_test_nop (int32 *args, char *path)
{
  (void) args;
  (void) path;

  return 0;
}

/* Do nothing at all, and do it poorly.  Always returns an error.  Used to
 * indicate features not present in the build. */
int
fs_test_error (int32 *args, char *path)
{
  (void) args;
  (void) path;

  return -1;
}

/* Fill the benchmark buffer with random values,
 * using the 32-bit seed in arg[0,1] */
int
fs_test_fill_rand (int32 *args, char *path)
{
  dword seed = (unsigned)args[0] << 16 | args[1];

  (void) path;

  randomize_bench_buffer (seed);

  return 0;
}

int
fs_test_stuff_pattern (int32 *args, char *path)
{
  dword seed = (unsigned)args[0] << 16 | args[1];
  int byte_count = args[2];
  unsigned int dword_count;
  unsigned int i;

  (void) path;

  ASSERT (fs_bench_buffer != NULL);

  dword_count = (byte_count + sizeof(dword) - 1) / sizeof(dword);
  if (dword_count > BENCH_BUFFER_DWORDS)
    dword_count = BENCH_BUFFER_DWORDS;

  ran_seed (seed);
  for (i = 0; i < dword_count; i++)
    fs_bench_buffer[i] = ran_next();
  return 0;
}

int
fs_test_check_pattern (int32 *args, char *path)
{
  dword seed = (unsigned)args[0] << 16 | args[1];
  int byte_count = args[2];
  unsigned int dword_count;
  unsigned int i;

  (void) path;

  ASSERT (fs_bench_buffer != NULL);

  dword_count = (byte_count + sizeof(dword) - 1) / sizeof(dword);
  if (dword_count > BENCH_BUFFER_DWORDS)
    dword_count = BENCH_BUFFER_DWORDS;

  ran_seed (seed);
  for (i = 0; i < dword_count; i++) {
    dword expect;
    expect = ran_next();
    if (fs_bench_buffer[i] != expect) {
      EFS_BAB_DEBUG_PRINTF ("mismatch @ offset %d! %x != %x\r\n",
                            i, fs_bench_buffer[i], expect);
      return -1;
    }
  }
  return 0;
}

int
fs_test_write (int32 *args, char *path)
{
  int write_size  = args[0];
  int write_count = args[1];
  int oflags = args[2];
  int write_where = args[3];

  int fd;
  int failed = 0;

  ASSERT (fs_bench_buffer != NULL);

  fd = efs_open (path, oflags, 0666);
  if (fd < 0)
  {
    fs_benchmark_errno ("efs_open()", path);
    return -1;
  }

  if (write_where & SEEK_END)
    efs_lseek (fd, 0, SEEK_END);

  while (write_count--)
  {
    fs_ssize_t written;

    written = efs_write (fd, fs_bench_buffer, write_size);

    if (written > 0)
      total_bytes_write += written;

    if (written != write_size) {
      EFS_BAB_DEBUG_PRINTF ("Incomplete write: %d out of %d\n\r", written,
                            write_size);
      fs_benchmark_errno ("efs_write()", path);
      failed = 1;
      break;
    }
  }

  if (efs_close (fd))
  {
    fs_benchmark_errno ("efs_close()", path);
    return -1;
  }

  return failed ? -1 : 0;
}

int
fs_test_read (int32 *args, char *path)
{
  int32 read_size  = args[0];
  int32 read_count = args[1];
  int32 oflags = args[2];
  int fd;

  if (read_size < 0)
    return -1;
  if (read_size > BENCH_BUFFER_ACTUAL_BYTES)
    return -1;

  fd = efs_open (path, oflags);
  if (fd < 0)
  {
    fs_benchmark_errno ("efs_open()", path);
    return -1;
  }

  while (read_count--)
  {
    int read_done;
    read_done = efs_read (fd, fs_bench_read_buffer, read_size);
    if (read_done == -1)
    {
      fs_benchmark_errno ("efs_read()", path);
      break;
    }
    if (read_done != read_size)
    {
      EFS_BAB_DEBUG_PRINTF ("Incomplete read: %d out of %d\n\r", read_done,
                            read_size);
    }
    if (read_done == 0)
      break;
    total_bytes_read += read_done;
  }

  if (efs_close (fd))
  {
    fs_benchmark_errno ("efs_close()", path);
    return -1;
  }

  return 0;
}

int
fs_test_raw_open (int32 *args, char *path)
{
  int oflags = args[2];
  int write_where = args[3];

  if (raw_fd > 0)
  {
    if (efs_close (raw_fd))
    {
      fs_benchmark_errno ("efs_close()", path);
      return -1;
    }
  }

  raw_fd = efs_open (path, oflags, 0666);
  if (raw_fd < 0)
  {
    fs_benchmark_errno ("efs_open()", path);
    return -1;
  }

  if (write_where & SEEK_END)
    efs_lseek (raw_fd, 0, SEEK_END);

  return 0;
}

int
fs_test_raw_close (int32 *args, char *path)
{
  (void) args;

  if (raw_fd < 0)
  {
    return -1;
  }

  if (efs_close (raw_fd))
  {
    fs_benchmark_errno ("efs_close()", path);
    return -1;
  }

  raw_fd = -1;

  return 0;
}

int
fs_test_raw_write (int32 *args, char *path)
{
  int write_size  = args[0];
  int write_count = args[1];
  int failed = 0;

  ASSERT (fs_bench_buffer != NULL);

  if (raw_fd < 0)
  {
    return -1;
  }

  while (write_count--)
  {
    fs_ssize_t written;

    written = efs_write (raw_fd, fs_bench_buffer, write_size);

    if (written > 0)
      total_bytes_write += written;

    if (written != write_size) {
      EFS_BAB_DEBUG_PRINTF ("Incomplete write: %d out of %d\n\r", written,
                            write_size);
      fs_benchmark_errno ("efs_write()", path);
      failed = 1;
      break;
    }
  }

  return failed ? -1 : 0;
}

int
fs_test_raw_read (int32 *args, char *path)
{
  int read_size  = args[0];
  int read_count = args[1];

  if (raw_fd < 0)
  {
    return -1;
  }

  if (read_size > BENCH_BUFFER_ACTUAL_BYTES)
    return -1;

  while (read_count--)
  {
    int read_done;
    read_done = efs_read (raw_fd, fs_bench_read_buffer, read_size);
    if (read_done == -1)
    {
      fs_benchmark_errno ("efs_read()", path);
      break;
    }
    if (read_done != read_size)
    {
      EFS_BAB_DEBUG_PRINTF ("Incomplete read: %d out of %d\n\r", read_done,
                            read_size);
    }
    if (read_done == 0)
      break;
    total_bytes_read += read_done;
  }

  return 0;
}

int
fs_test_efs1_fail (int32 *args, char *path)
{
  (void) args;
  (void) path;

  return -1;
}

/* The CPU Hog can be set to busy out the CPU for a specified load and
   duty cycle.  When enabled, it will sleep for "sleep" milliseconds,
   then churn for "run" milliseconds. */
int
fs_test_set_cpu_hog (int32 *args, char *path)
{
  (void) path;

  /* Initialize CPU hog. This will initialize only if its already not */
  fs_cpu_hog_init ();

  fs_cpu_hog_sleep_ms = args[0];
  fs_cpu_hog_run_ms = args[1];
  EFS_BAB_DEBUG_PRINTF ("fs_set_cpu_hog (sleep %d ms, run %d ms)\n\r",
                        fs_cpu_hog_sleep_ms, fs_cpu_hog_run_ms);
  return 0;
}

/* similar to fs_test_write, but writes random sizes each time */
int
fs_test_random_write (int32 *args, char *path)
{
  int write_size_min  = args[0];
  int write_size_max  = args[1];
  int write_count_min = args[2];
  int write_count_max = args[3];

  int write_size, write_count;

  char pathname[FS_PATH_MAX];
  int fd;
  int oflag;

  ASSERT (fs_bench_buffer != NULL);

  oflag = O_RDWR | O_CREAT;
  fd = efs_open (path, oflag, 0666);
  if (fd < 0)
  {
    fs_benchmark_errno ("efs_open()", pathname);
    return -1;
  }

  ran_seed (fs_benchmark_seed);
  write_count = ran_dist(ran_next(), write_count_min, write_count_max + 1);

  while (write_count--)
  {
    int written;

    write_size = ran_dist(ran_next(), write_size_min, write_size_max + 1);
    written = efs_write (fd, fs_bench_buffer, write_size);
    if (written != write_size)
    {
      EFS_BAB_DEBUG_PRINTF ("Incomplete write: %d out of %d\n\r", written,
                            write_size);
      fs_benchmark_errno ("efs_write()", pathname);
    }
    total_bytes_write += written;
  }
  fs_benchmark_seed = ran_next();

  if (efs_close (fd))
  {
    fs_benchmark_errno ("efs_close()", pathname);
    return -1;
  }

  return 0;
}

int
fs_test_truncate (int32 *args, char *path)
{
  fs_off_t length = args[0];

  if (efs_truncate (path, length) != 0)
  {
    fs_benchmark_errno ("efs_truncate()", path);
    return -1;
  }
  else
  {
    return 0;
  }
}

/* Fill the file-system to a given percentage */
int
fs_test_fill_fs_to_pct (int32 *args, char *path)
{
  int pct_full = args[0];
  int fd;

  ASSERT (fs_bench_buffer != NULL);

  fd = efs_open (path, O_RDWR | O_CREAT, S_IWRITE | S_IREAD);
  if (fd < 0)
  {
    fs_benchmark_errno ("efs_open()", path);
    return -1;
  }

  EFS_BAB_DEBUG_PRINTF ("Filling to %d%% full...\n\r", pct_full);

  while (1)
  {
    int write_size;
    int written;
    struct fs_statvfs sbuf;
    uint32 block_goal, block_now;

    if (efs_statvfs ("/", &sbuf))
    {
      EFS_BAB_DEBUG_PRINTF ("efs_statvfs() failed!");
      return -1;
    }

    block_goal = sbuf.f_blocks * pct_full / 100;
    block_now  = sbuf.f_blocks - sbuf.f_bavail;

    if (block_goal <= block_now)
      break;

    write_size = (block_goal - block_now) * sbuf.f_bsize;
    if (write_size > BENCH_BUFFER_BYTES)
      write_size = BENCH_BUFFER_BYTES;

    written = efs_write (fd, fs_bench_buffer, write_size);
    if (written <= 0)
    {
      EFS_BAB_DEBUG_PRINTF ("Incomplete write: %d out of %d\n\r", written,
                            write_size);
      fs_benchmark_errno ("efs_write()", path);
      break;
    }
    total_bytes_write += written;
  }

  if (efs_close (fd))
  {
    fs_benchmark_errno ("efs_close()", path);
    return -1;
  }

  return 0;
}

int
fs_test_delete (int32 *args, char *path)
{
  (void) args;

  if (efs_unlink (path) < 0)
  {
    fs_benchmark_errno ("efs_unlink()", path);
    return -1;
  }
  return 0;
}

int
fs_test_df (int32 *args, char *path)
{
  struct fs_statvfs sbuf;

  (void) args;

  if (efs_statvfs (path, &sbuf) == 0)
  {
    uint32 total_blocks = sbuf.f_blocks;
    uint32 free_blocks  = sbuf.f_bavail;
    uint32 used_blocks =  sbuf.f_blocks - sbuf.f_bavail;
    EFS_BAB_DEBUG_PRINTF ("df: %d bytes  (%02d%% free, %02d%% used)\n\r",
                          free_blocks * sbuf.f_bsize,
                          free_blocks * 100 / total_blocks,
                          used_blocks * 100 / total_blocks);

    args[0] = total_blocks;
    args[1] = free_blocks;
    args[2] = used_blocks;
    args[3] = sbuf.f_bsize;
    return 0;
  }
  else
  {
    EFS_BAB_DEBUG_PRINTF ("efs_statvfs() failed!");
    return -1;
  }
}

int
fs_test_mkdir (int32 *args, char *path)
{
  int result;

  (void) args;

  result = efs_mkdir ((const char *)path, 0777);
  if (result < 0)
  {
    fs_benchmark_errno ("efs_mkdir()", path);
    return -1;
  }

  return 0;
}

int
fs_test_rmdir (int32 *args, char *path)
{
  int result;

  (void) args;

  result = efs_rmdir(path);
  if (result < 0)
  {
    fs_benchmark_errno ("efs_rmdir()", path);
    return -1;
  }

  return 0;
}

int
fs_test_statvfs (int32 *args, char *path)
{
  struct fs_statvfs sbuf;

  (void) args;

  if (efs_statvfs (path, &sbuf) == 0)
  {
    EFS_BAB_DEBUG_PRINTF ("f_bsize   %d\n\r", (int)sbuf.f_bsize);
    EFS_BAB_DEBUG_PRINTF ("f_blocks  %d\n\r", (int)sbuf.f_blocks);
    EFS_BAB_DEBUG_PRINTF ("f_bfree   %d\n\r", (int)sbuf.f_bfree);
    EFS_BAB_DEBUG_PRINTF ("f_bavail  %d\n\r", (int)sbuf.f_bavail);
    EFS_BAB_DEBUG_PRINTF ("f_files   %d\n\r", (int)sbuf.f_files);
    EFS_BAB_DEBUG_PRINTF ("f_ffree   %d\n\r", (int)sbuf.f_ffree);
    EFS_BAB_DEBUG_PRINTF ("f_favail  %d\n\r", (int)sbuf.f_favail);
    EFS_BAB_DEBUG_PRINTF ("f_fsid    %d\n\r", (int)sbuf.f_fsid);
    EFS_BAB_DEBUG_PRINTF ("f_flag    %d\n\r", (int)sbuf.f_flag);
    EFS_BAB_DEBUG_PRINTF ("f_namemax %d\n\r", (int)sbuf.f_namemax);
    EFS_BAB_DEBUG_PRINTF ("f_maxwrite %d\n\r", (int)sbuf.f_maxwrite);
    EFS_BAB_DEBUG_PRINTF ("f_balloc %d\n\r", (int)sbuf.f_balloc);
    EFS_BAB_DEBUG_PRINTF ("f_hardalloc %d\n\r", (int)sbuf.f_hardalloc);
    EFS_BAB_DEBUG_PRINTF ("f_pathmax %d\n\r", (int)sbuf.f_pathmax);
    EFS_BAB_DEBUG_PRINTF ("f_is_case_sensitive %d\n\r",
                          (int)sbuf.f_is_case_sensitive);
    EFS_BAB_DEBUG_PRINTF ("f_is_case_preserving %d\n\r",
                          (int)sbuf.f_is_case_preserving);
    EFS_BAB_DEBUG_PRINTF ("f_max_file_size %d\n\r",
                          (int)sbuf.f_max_file_size);
    EFS_BAB_DEBUG_PRINTF ("f_max_file_size_unit %d\n\r",
                          (int)sbuf.f_max_file_size_unit);
    EFS_BAB_DEBUG_PRINTF ("f_max_open_files %d\n\r",
                          (int)sbuf.f_max_open_files);
    EFS_BAB_DEBUG_PRINTF ("f_name_rule %d\n\r", (int)sbuf.f_name_rule);
    EFS_BAB_DEBUG_PRINTF ("f_name_encoding %d\n\r", (int)sbuf.f_name_encoding);
    return 0;
  }
  else
  {
    fs_benchmark_errno ("efs_statvfs()", "");
    return -1;
  }
}

/*
 * This function ensures that deleted files are not kept pending but
 * actually returned to the free pool.
 */
int
fs_test_flush_deleted_data (int32 *args, char *path)
{
  (void) args;
  while (efs_image_prepare(path) > 0)
    ;
  return 0;
}

#ifdef FEATURE_FS_HOTPLUG
  /* Global hotplug device pointer */
  static struct hotplug_device *bench_hdev = NULL;
#endif

/* Open the Hotplug device */
int
fs_test_hotplug_setup (int32 *args, char *path)
{
  (void) args;
#ifdef FEATURE_FS_HOTPLUG
  bench_hdev = hotplug_open_device (path);
  if (!bench_hdev)
    return -1;

  return 0;
#else
  (void) path;
  return -1;
#endif
}

/* Close the Hotplug device */
int
fs_test_hotplug_teardown (int32 *args, char *path)
{
  (void) path;
  (void) args;
#ifdef FEATURE_FS_HOTPLUG
  (void) hotplug_close_device (bench_hdev);

  return 0;
#else
  return -1;
#endif
}

/* This function sets the Hotplug poll interval */
int
fs_test_hotplug_poll_interval (int32 *args, char *path)
{
  (void) path;
#ifdef FEATURE_FS_HOTPLUG
  hotplug_set_polling_interval (args[0]);
  return 0;
#else
  (void) args;
  return -1;
#endif
}

/* This function formats media that would be mounted at 'path' */
int
fs_test_hotplug_format (int32 *args, char *path)
{
  (void) args;
#ifdef FEATURE_FS_HOTPLUG
  return hotplug_format (path);
#else
  (void) path;
  return -1;
#endif
}

#ifdef FEATURE_FS_HOTPLUG
/* All hotplug media has 512 bytes per sector*/
#define HOTPLUG_BYTES_PER_BLOCK 512

/* SDCC can only buffer 64KB or less at a time */
#define MAX_BYTES_XFER (HOTPLUG_BYTES_PER_BLOCK * 128)
#endif

#ifdef FEATURE_FS_HOTPLUG
/* Helper function to do a chunked read or write */
static int
fs_test_hotplug_read_write_core (int32 *args, char *path,
                                 char *display,
                                 int xfer_func (struct hotplug_device *,
                                                uint32,
                                                unsigned char *,
                                                uint16),
                                 uint32 *bytecount)
{
  int lba_start        = args[0];
  int blocks_remaining = args[1];
  int chunk_blocks     = args[2];
  unsigned int misalign = args[3];
  int result = -1;

  (void) path;                  /* only used by hotplug prep */

  ASSERT (fs_bench_buffer != NULL);

  if (!bench_hdev)
    return -1;

  if (chunk_blocks == 0)
    chunk_blocks = MAX_BYTES_XFER / HOTPLUG_BYTES_PER_BLOCK;

  (void) display;               /* may not have EFS_BAB_DEBUG_PRINTF */
#if 0
  /* Debug only.  Causes significant slowdown */
  EFS_BAB_DEBUG_PRINTF ("%s %d blocks (chunks of %d) @ %d on %s...\n\r",
                        display, blocks_remaining, chunk_blocks, lba_start,
                        path);
#endif

  while (blocks_remaining > 0) {
    int blocks = blocks_remaining;

    if (blocks > chunk_blocks)
      blocks = chunk_blocks;

    /* Chunks can't be bigger than our buffer in RAM */
    if ((blocks * HOTPLUG_BYTES_PER_BLOCK) > BENCH_BUFFER_ACTUAL_BYTES)
    {
      blocks = BENCH_BUFFER_ACTUAL_BYTES / HOTPLUG_BYTES_PER_BLOCK;
    }

#ifdef MAX_BYTES_XFER
    /* Due to an SDCC bug, reads can't be bigger than 64K */
    if ((blocks * HOTPLUG_BYTES_PER_BLOCK) > MAX_BYTES_XFER)
      blocks = MAX_BYTES_XFER / HOTPLUG_BYTES_PER_BLOCK;
#endif

    result = xfer_func (bench_hdev, lba_start,
                        ((unsigned char *) fs_bench_buffer) + (misalign & 31),
                        blocks);
    if (result != 0)
      break;
    *bytecount += blocks * HOTPLUG_BYTES_PER_BLOCK;
    lba_start += blocks;
    blocks_remaining -= blocks;
  }

  return result;
}

#endif /* FEATURE_FS_HOTPLUG */

/* Read blocks from a hotplug device */
int
fs_test_hotplug_read (int32 *args, char *path)
{
#ifdef FEATURE_FS_HOTPLUG
  return fs_test_hotplug_read_write_core
    (args, path, "Reading",
     hotplug_dev_read, &total_bytes_read);
#else
  (void) args;
  (void) path;
  return -1;
#endif
}


/* Read blocks from a hotplug device */
int
fs_test_hotplug_write (int32 *args, char *path)
{
#ifdef FEATURE_FS_HOTPLUG
  return fs_test_hotplug_read_write_core
    (args, path, "Writing",
     hotplug_dev_write, &total_bytes_write);
#else
  (void) args;
  (void) path;
  return -1;
#endif
}


/* Write randomly through hotplug */
int
fs_test_hotplug_random_write (int32 *args, char *path)
{
#ifdef FEATURE_FS_HOTPLUG
  int lba_min          = args[0];
  int lba_max          = args[1];
  int block_count_min  = args[2];
  int block_count_max  = args[3];
  int remaining_blocks = args[4];
  int result = -1;
  void *buf = fs_bench_buffer;

  (void) path;                  /* Only used by hotplug setup */

  ASSERT (fs_bench_buffer != NULL);

  if (!bench_hdev)
    return -1;

  /* Limited by Hotplug maximum blocks */
  if (block_count_max > (MAX_BYTES_XFER / HOTPLUG_BYTES_PER_BLOCK))
    block_count_max = MAX_BYTES_XFER / HOTPLUG_BYTES_PER_BLOCK;

  /* Limited by benchmark's buffer size */
  if (block_count_max > (BENCH_BUFFER_BYTES / HOTPLUG_BYTES_PER_BLOCK))
    block_count_max = BENCH_BUFFER_BYTES / HOTPLUG_BYTES_PER_BLOCK;

  /* Positive values only for count */
  if (block_count_min < 1)
    block_count_min = 1;

  ran_seed (fs_benchmark_seed);

  while (remaining_blocks)
  {
    int lba, count;
    count = ran_dist(ran_next(), block_count_min, block_count_max + 1);
    if (count > remaining_blocks)
      count = remaining_blocks;

    lba = ran_dist(ran_next(), lba_min, lba_max + 1 - count);

    result = hotplug_dev_write (bench_hdev, lba, buf, count);
    if (result != 0)
      break;
    total_bytes_write += count * HOTPLUG_BYTES_PER_BLOCK;
    remaining_blocks -= count;
  }

  fs_benchmark_seed = ran_next();

  return result;
#else /* !FEATURE_FS_HOTPLUG */
  (void) args;
  (void) path;
  return -1;
#endif
}


/* Check the media that might be mounted and return the size and
 * status.  The longest part of this function is the is_present()
 * call, which varies quite a bit between media and between
 * targets.  */
int
fs_test_hotplug_check_media (int32 *args, char *path)
{
#ifdef FEATURE_FS_HOTPLUG
  struct hotplug_device *hdev;
  uint32 sector_cnt = 0;
  uint16 sector_size = 0;
  int is_mounted;
  int is_present;

  hdev = hotplug_open_device (path);
  if (!hdev)
    return -1;

  is_present = hotplug_dev_is_present (hdev);
  is_mounted = hotplug_dev_is_mounted (hdev);

  if (is_present)
    hotplug_dev_get_size (hdev, &sector_cnt, &sector_size);

  args[0] = sector_cnt;
  args[1] = sector_size;
  args[2] = is_present;
  args[3] = is_mounted;

  EFS_BAB_DEBUG_PRINTF ("[%s]: %dMB (%d * %d) %s, %s\n\r",
                        path, sector_cnt / 1024 * sector_size / 1024,
                        sector_cnt, sector_size,
                        is_present ? " IS present" : "not present",
                        is_mounted ? " IS mounted" : "not mounted" );

  hotplug_close_device (hdev);

  return 0;
#else  /* !FEATURE_FS_HOTPLUG */
  (void) args;
  (void) path;
  return -1;
#endif
}

int
fs_test_hotplug_lock (int32 *args, char *path)
{
  (void) args;
#ifdef FEATURE_FS_HOTPLUG
  bench_hdev = hotplug_open_device (path);
  if (bench_hdev == NULL)
    return -1;

  return hotplug_lock_dev (bench_hdev);
#else
  (void) path;
  return -1;
#endif
}

int
fs_test_hotplug_unlock (int32 *args, char *path)
{
  (void) args; (void) path;
#ifdef FEATURE_FS_HOTPLUG
  return hotplug_unlock_dev (bench_hdev);
#else
  return -1;
#endif
}


/* This function treats the first parameter mode as a string packed with
 * 4 characters, it extracts these 4 characters and stores it as a string
 * in the second parameter mode_str */
static void
fs_test_convert_mode_int_to_str (int mode, char mode_str[])
{
  int i;

  for ( i=0; i <= 3; ++i) {
    mode_str[3-i] = (char )(mode & 0xFF);
    mode >>= 8;
  }

  mode_str[4] = '\0';
}


/* This function opens the file-stream.
 * args[0] -> [IN]
      an int which contains 4 characters to specify the mode string.
 * args[1] -> [IN]
      where to start writing.
 * args[2] <- [OUT]
      The file-descriptor to the opened file-stream.
 */
int
fs_test_raw_fopen (int32 *args, char *path) {
  char mode[5];
  int write_where = args[1];
  int i;

  /* Doesn't need exclusive access to raw_efs_efile[] because only one task
   * (benchmark) is executing this code
   */
  /* find an empty slot on raw_efs_efile array */
  for (i = 0; i < FS_BENCHMARK_MAX_STREAMS && raw_efs_efile[i] != NULL;
       i++)
    ;

  if (i == FS_BENCHMARK_MAX_STREAMS)
  {
    /* to many files opened using benchmark. */
    return -1;
  }

  /* Extract the mode-string from the args[0] */
  fs_test_convert_mode_int_to_str (args[0], mode);

  raw_efs_efile[i] = efs_efopen (path, mode);
  if (raw_efs_efile[i] == NULL)
  {
    fs_benchmark_errno ("efs_efopen()", path);
    return -1;
  }

  if (write_where & SEEK_END)
  {
    efs_efseek (raw_efs_efile[i], 0, SEEK_END);
  }

  /* return the index in raw_efs_efile array */
  args[2] = i;

  return 0;
}

/* This function closes the file-stream.
 * args[0] -> [IN]
      Number returned by fs_test_raw_fopen() in args[2].
 */
int
fs_test_raw_fclose (int32 *args, char *path)
{
  int i = args[0];
  EFS_EFILE * fp;

  /* Check if index is inside array. */
  if (i < 0 || i >= FS_BENCHMARK_MAX_STREAMS)
    return -1;

  fp = raw_efs_efile[i];
  /* Don't check for NULL because efs_efclose should handle NULLs gracefully.
   */
  if (efs_efclose (fp))
  {
    fs_benchmark_errno ("efs_efclose()", path);
    return -1;
  }

  raw_efs_efile[i] = NULL;

  return 0;
}


/* This function writes to the file-stream.
 * args[0] -> [IN]
      Number returned by fs_test_raw_fopen() in args[2].
 * args[1] -> [IN]
      how many bytes to write.
 * args[2] -> [IN]
      how many times to write.
 */
int
fs_test_raw_fwrite (int32 *args, char *path)
{
  int i = args[0];
  int write_items  = args[1];
  int write_count = args[2];
  int failed = 0;
  EFS_EFILE * fp;

  ASSERT (fs_bench_buffer != NULL);

  /* Check if index is inside array. */
  if (i < 0 || i >= FS_BENCHMARK_MAX_STREAMS)
    return -1;

  fp = raw_efs_efile[i];
  /* Don't check for NULL because stdio should handle NULLs gracefully. */

  if (write_items > BENCH_BUFFER_ACTUAL_BYTES)
    return -1;

  while (write_count--)
  {
    fs_ssize_t written;

    written = efs_efwrite (fs_bench_buffer, 1, write_items, fp);

    if (written > 0)
      total_bytes_write += written;

    if (written != write_items)
    {
      EFS_BAB_DEBUG_PRINTF ("Incomplete write: %d out of %d\n\r", written,
                            write_items);
      fs_benchmark_errno ("efs_efwrite()", path);
      failed = 1;
      break;
    }
  }

  if (efs_efflush (fp))
    failed = 1;

  return failed ? -1 : 0;
}


/* This function reads from the file-stream.
 * args[0] -> [IN]
      Number returned by fs_test_raw_fopen() in args[2].
 * args[1] -> [IN]
      how many bytes to read.
 * args[2] -> [IN]
      how many times to read.
 */
int
fs_test_raw_fread (int32 *args, char *path)
{
  int i = args[0];
  int read_items  = args[1];
  int read_count = args[2];
  EFS_EFILE * fp;

  /* Check if index is inside array. */
  if (i < 0 || i >= FS_BENCHMARK_MAX_STREAMS)
    return -1;

  fp = raw_efs_efile[i];
  /* Don't check for NULL because stdio should handle NULLs gracefully. */

  if (read_items > BENCH_BUFFER_ACTUAL_BYTES)
    return -1;

  while (read_count--)
  {
    int read_done;
    read_done = efs_efread (fs_bench_read_buffer, 1, read_items, fp);
    if (read_done == -1)
    {
      fs_benchmark_errno ("efs_efread()", path);
      break;
    }
    if (read_done != read_items)
      EFS_BAB_DEBUG_PRINTF ("Incomplete read: %d out of %d\n\r", read_done,
                            read_items);

    if (read_done == 0)
      break;

    total_bytes_read += read_items;
  }

  return 0;
}

/* This functions allows/restricts access to the /.efs_privatedir folder.
 * args[0]: 0 to restrict access, non-zero to allow access */
int
fs_test_privatedir_set_access (int32 *args, char *path)
{
  int allow_access = args[0];

  (void) path;

  efs_privatedir_set_access (allow_access);

  return 0;
}

/* This function Doesn't reset the target anymore.
 * Use either brutforce method or power supply to reset or a
 * regular Diag packet to reset the target using QXDM */
int
fs_test_reset (int32 *args, char *path)
{
  (void) args;
  (void) path;

  return -1;
}

/* This function deletes an entire tree of files, without any checks.
 * Use with care!
 * (This is one of many reasons Benchmarking should NEVER be enabled
 * in production builds.) */
int
fs_test_deltree (int32 *args, char *path)
{
  (void) args;

  return efs_deltree (path);
}

/* This function enumerates every file in a given path.
 * If args[0] is TRUE, then also stat each file. */
int
fs_test_enumerate (int32 *args, char *path)
{
  int do_stat = args[0];
  struct fs_dirent *dirent;
  EFSDIR *dirp;

  dirp = efs_opendir (path);
  if (!dirp)
    return -1;

  do {
    dirent = efs_readdir (dirp);

    if (dirent && do_stat) {
      struct fs_stat stat_buf;
      (void) efs_lstat (dirent->d_name, &stat_buf);
    }
  } while (dirent);

  efs_closedir (dirp);

  return 0;
}

int
fs_test_chown_recursive (int32 *args, char *path)
{
  return efs_chown_recursive (path, args[0], args[1]);
}

int
fs_test_chmod_recursive (int32 *args, char *path)
{
  return efs_chmod_recursive (path, args[0]);
}

/* This function reads from the file-stream.
 * args[0] -> [IN]
      Number returned by fs_test_raw_fopen() in args[2].
 * args[1] -> [IN]
      Size of the buffer. If 0 No buffering mode is used.
 */
int
fs_test_raw_setvbuf (int32 *args, char *path)
{
  EFS_EFILE * fp;
  int i = args[0];
  uint32 buf_size  = args[1];

  if (buf_size > STDIO_BUF_SIZE ||
      i < 0 || i >= FS_BENCHMARK_MAX_STREAMS)
    return -1;

  fp = raw_efs_efile[i];
  if (buf_size)
  {
    if (efs_esetvbuf (fp, stdio_buffer[i], EFS_IOFBF, buf_size))
    {
      fs_benchmark_errno ("efs_setvbuf()",path);
      return -1;
    }
  }
  else
  {
    if (efs_esetvbuf (fp, NULL, EFS_IONBF, 0))
    {
      fs_benchmark_errno ("efs_setvbuf()",path);
      return -1;
    }
  }
  return 0;
}

/* This function reads from the file-stream.
 * args[0] -> [IN]
      Number returned by fs_test_raw_fopen() in args[2].
      Remember if this index is not valid and the file poitner
      is NULL it will flush all open streams.
  */
int
fs_test_raw_fflush (int32 *args, char *path)
{
  EFS_EFILE * fp;
  int i = args[0];

  if (i < 0 || i >= FS_BENCHMARK_MAX_STREAMS)
    return -1;

  fp = raw_efs_efile[i];

  if (efs_efflush (fp))
  {
    fs_benchmark_errno ("efs_efflush()",path);
    return -1;
  }

  return 0;
}

int
fs_test_utime (int32 *args, char *path)
{
  struct fs_utimbuf times;
  int result;

  times.actime = args[0];
  times.modtime = args[1];

  result = efs_utime (path, &times);
  if (result < 0)
  {
    fs_benchmark_errno ("efs_open()", path);
    return -1;
  }

  return 0;
}


int
fs_test_babylon_short_test (int32 *args, char *path)
{
#ifndef FEATURE_BABYLON
  (void) args;
  (void) path;
#else
  int count = args[0];

  (void) path;
  EFS_BAB_START (EFS_BAB_TESTING);
  while (count--) {
    /* (Type: BAB_TYPE_SINGLE_EVENT) << 8 | (Stream: BAB_STREAM_EFS)
     * (Extended: EFS_BAB_TEST) << 8 | count */
    BAB_SINGLE_EVENT (BAB_STREAM_EFS, /* Stream */
                      (count & 0xFF), /* Event ID */
                      EFS_BAB_TEST); /* Extended */
    }
  EFS_BAB_END (EFS_BAB_TESTING);
#endif

  return 0;
}

int
fs_test_babylon_speed_test (int32 *args, char *path)
{
#ifndef FEATURE_BABYLON
  (void) args;
  (void) path;
#else
  int count = args[0];
  int cycle;

  (void) path;

  if (count == 0)
    count = 1;

  /* Single Event */
  for (cycle = count; cycle; cycle--)
  {
    EFS_BAB_API (EFS_BAB_TESTING);
    EFS_BAB_API (EFS_BAB_TESTING);
  }

  /* Markers */
  for (cycle = count; cycle; cycle--)
  {
    EFS_BAB_START (EFS_BAB_TESTING);
    EFS_BAB_END (EFS_BAB_TESTING);
    EFS_BAB_START (EFS_BAB_TESTING);
    EFS_BAB_END (EFS_BAB_TESTING);
  }

  /* 16-bit value */
  for (cycle = count; cycle; cycle--) {
    EFS_BAB_VALUE (EFS_BAB_TESTING, cycle, TRUE, TRUE, FALSE);
    EFS_BAB_VALUE (EFS_BAB_TESTING, cycle, TRUE, TRUE, FALSE);
  }

  /* 32-bit value */
  for (cycle = count; cycle; cycle--) {
    EFS_BAB_VALUE (EFS_BAB_TESTING, cycle, TRUE, TRUE, TRUE);
    EFS_BAB_VALUE (EFS_BAB_TESTING, cycle, TRUE, TRUE, TRUE);
  }

  /* Fixed string */
  for (cycle = count; cycle; cycle--) {
    EFS_BAB_PRINT (EFS_BAB_TESTING, "Fixed A");
    EFS_BAB_PRINT (EFS_BAB_TESTING, "Fixed B");
  }
#endif
  return 0;
}

int
fs_test_babylon_word_test (int32 *args, char *path)
{
#ifndef FEATURE_BABYLON
  (void) args;
  (void) path;
#else
  int count = args[0];
  int start = args[1];

  (void) path;
  (void) start;
  EFS_BAB_START (EFS_BAB_TESTING);
  while (count--) {
    EFS_BAB_VALUE (EFS_BAB_TESTING,
                      start++ & 0xFFFF,
                   TRUE,        /* decimal */
                   FALSE,       /* unsigned  */
                   FALSE);      /* 16-bit */
  }
  EFS_BAB_END (EFS_BAB_TESTING);
#endif

  return 0;
}

int
fs_test_babylon_port_test (int32 *args, char *path)
{
  (void) args;
  (void) path;
#ifdef FEATURE_BABYLON
  bab_port_test();
#endif

  return 0;
}

/* Exists in fs_public.c but we don't want to publish it. */
extern int fs_dummy_protected_var;
int
fs_test_memory_protection (int32 *args, char *path)
{
  (void) args;
  (void) path;

  fs_dummy_protected_var++;
  return -1;
}


int
fs_test_sync (int32 *args, char *path)
{
  (void) args;
  (void) path;

  return (efs_sync ("/"));
}


struct fs_ls
{
  /* The following run time variables set the file number count when we start
     and stop writing the file information of the entry into the filepointer
     provided. This is helpful if there is not enough space to create the
     file holding the data about all entries in one go. We can send the iter
     command multiple times and collect data about different set of entries
     each time. */

  /* used to keep track of how many files we processed already in the
     current run. */
  uint32 ls_curr_cnt;
  /* when curr_cnt reaches this count we start processing the file's data.
     Change to previous call's stop limit each time. */
  uint32 ls_start_limit;

  /* Used to count different file types */
  uint32 ls_file_cnt;
  uint32 ls_dir_cnt;
  uint32 ls_item_file_cnt;
  uint32 ls_link_cnt;
  uint32 ls_misc_cnt;

  EFS_EFILE *fp;

  /* 11 bytes for the mode, 10 bytes for size and 3 bytes for the
     seperator and new line */
  char ls_buff [FS_PATH_MAX + 11 + 10 + 3];
};

static struct fs_ls fs_ls_inst;

/* Callback function registered from fs_test_ls for call-back from efs_iter
   everytime it encounters an entry in the directory being iterated through.
   The function prints the mode and size information of the entry to the file
   pointer provided. Based on the mode, also counts the number of files,
   directories and item files in the FS which will also be appended when the
   call returns to the fs_test_ls */
int
efs_bench_iter_callback (char *path, struct fs_stat *sb, const void *param)
{
  int result, len;
  char mode [11];
  struct fs_ls *ls;

  ls = (struct fs_ls *) (param);
  ASSERT (ls != NULL);

  len = strlen (path);
  ASSERT (len > 0);

  ls->ls_curr_cnt++;
  if (ls->ls_curr_cnt <= ls->ls_start_limit)
  {
    return 0;
  }

  /* convert mode ls-style */
  if (S_ISDIR (sb->st_mode))
  {
    mode[0] = 'd';
    ls->ls_dir_cnt++;
  }
  else if (S_ISLNK (sb->st_mode))
  {
    mode[0] = 'l';
    ls->ls_link_cnt++;
  }
  else if (S_ISREG (sb->st_mode))
  {
    mode[0] = '-';
    ls->ls_file_cnt++;
  }
  else if (S_ISITM (sb->st_mode))
  {
    mode[0] = 'i';
    ls->ls_item_file_cnt++;
  }
  else
  {
    mode[0] = 'u';  /* we don't care about other types, so u for unknow */
    ls->ls_misc_cnt++;
  }

  mode[1] = ((sb->st_mode & S_IRUSR) == S_IRUSR) ? 'r' : '-';
  mode[2] = ((sb->st_mode & S_IWUSR) == S_IWUSR) ? 'w' : '-';
  mode[3] = ((sb->st_mode & S_IXUSR) == S_IXUSR) ? 'x' : '-';
  mode[4] = ((sb->st_mode & S_IRGRP) == S_IRGRP) ? 'r' : '-';
  mode[5] = ((sb->st_mode & S_IWGRP) == S_IWGRP) ? 'w' : '-';
  mode[6] = ((sb->st_mode & S_IXGRP) == S_IXGRP) ? 'x' : '-';
  mode[7] = ((sb->st_mode & S_IROTH) == S_IROTH) ? 'r' : '-';
  mode[8] = ((sb->st_mode & S_IWOTH) == S_IWOTH) ? 'w' : '-';
  mode[9] = ((sb->st_mode & S_IXOTH) == S_IXOTH) ? 'x' : '-';
  mode[10] = '\0';

  len += 11 + 10 + 3;
  memset (fs_ls_inst.ls_buff, 0x0, sizeof (fs_ls_inst.ls_buff));

  ASSERT (len < FS_PATH_MAX + 11 + 10 + 3);
  snprintf (ls->ls_buff, sizeof (ls->ls_buff), "%s,%10lu,%s\n", mode,
              sb->st_size, path);

  result = efs_efwrite (ls->ls_buff, 1, len, ls->fp);
  if (result != len)
  {
    ASSERT (efs_errno == ENOSPC);
    result = -ENOSPC;
  }
  else
  {
    result = 0;
  }

  return result;
}

/* args[0] -> start limit. */
int
fs_test_ls (int32 *args, char *path)
{
  int result = 0;
  int length;

  /* reset counters before each call */
  memset (&fs_ls_inst, 0, sizeof (fs_ls_inst));

  fs_ls_inst.ls_start_limit = args[0];

  fs_ls_inst.fp = efs_efopen ("/list_of_files.csv", "w");
  if (fs_ls_inst.fp == NULL)
  {
    fs_benchmark_errno ("efs_efopen()", path);
    return -1;
  }

  if (efs_esetvbuf (fs_ls_inst.fp, stdio_buffer[0], EFS_IOFBF,
                    sizeof (stdio_buffer[0])))
  {
    fs_benchmark_errno ("efs_setvbuf()",path);
    return -1;
  }


  result = efs_iter (path, 1, efs_bench_iter_callback, &fs_ls_inst);
  /* we need to return error number in case of error */
  if (result != 0)
  {
    fs_benchmark_errno ("efs_iter()", path);
    result = -1;
  }

  snprintf (fs_ls_inst.ls_buff, sizeof (fs_ls_inst.ls_buff),
            "Dir:%lu\nReg:%lu\nItm:%lu\nLnk:%lu\nMisc:%lu\n",
            fs_ls_inst.ls_dir_cnt, fs_ls_inst.ls_file_cnt,
            fs_ls_inst.ls_item_file_cnt, fs_ls_inst.ls_link_cnt,
            fs_ls_inst.ls_misc_cnt);

  length = strlen (fs_ls_inst.ls_buff);

  /* lets try to append the counters in before closing */
  efs_efwrite (fs_ls_inst.ls_buff, 1, length, fs_ls_inst.fp);
  efs_efclose (fs_ls_inst.fp);

  return result;
}


/* The following function is used to create a directory called 'bench' in
   the given path. All the safe benchmark tests are done inside 'bench'
   sub directory. If there is already a 'bench directory present, delete it.
   Note: This function updates fs_path_buff[2] */
static int
create_bench_dir (char *path)
{
  strlcpy (fs_path_buff[2], path, sizeof(fs_path_buff[2]));
  strlcat (fs_path_buff[2], FS_BENCHMARK_DIR, sizeof(fs_path_buff[2]));

  (void) efs_deltree (fs_path_buff[2]); /* Okay to fail */

  return efs_mkdir ((const char *) fs_path_buff[2], 0777);
}

/* This function deletes the 'bench' directory. It is called by the
   cleanup routine after the safe benchmark test is performed */
static int
cleanup_bench_dir (int32 *args, char *path)
{
  (void) args;
  strlcpy (fs_path_buff[2], path, sizeof(fs_path_buff[2]));
  strlcat (fs_path_buff[2], FS_BENCHMARK_DIR, sizeof(fs_path_buff[2]));
  return efs_deltree (fs_path_buff[2]);
}

int
fs_safe_write_prepare (int32 *args, char *path)
{
  (void) args;
  return create_bench_dir (path);
}

/* The following function writes data to the device for 'FS_BENCHMARK_TIME_MS'
   period of time */
int
fs_test_safe_write (int32 *args, char *path)
{
  int fd;
  uint64 start_time, time_elapsed;
  int write_size = BENCH_BUFFER_BYTES;
  int written;
  int failed = 0;

  (void) args; /* Ignored */

  ASSERT (fs_bench_buffer != NULL);

  strlcpy (fs_path_buff[2], path, sizeof (fs_path_buff[2]));
  strlcat (fs_path_buff[2], FS_BENCHMARK_DIR "/bench_file",
           sizeof (fs_path_buff[2]));

  fd = efs_open (fs_path_buff[2], O_CREAT | O_RDWR, 0666);

  if (fd < 0)
  {
    fs_benchmark_errno ("efs_open()", path);
    return -1;
  }

  randomize_bench_buffer (fs_benchmark_seed);

  start_time = fs_time_ms_u64();

  do {

    written = efs_write (fd, fs_bench_buffer, write_size);

    if (written > 0)
      total_bytes_write += written;
    else {
      /* Its ok if we ran out of space on device while trying to fill it up
       * but we should have written at least 32 KB for these numbers to be
       * any meaningful  */
      if ((efs_errno != ENOSPC) || (total_bytes_write < (32 * 1024)))
      {
        fs_benchmark_errno ("efs_write()", path);
        failed = 1;
      }
      break;
    }

    time_elapsed = fs_time_ms_u64() - start_time;

  } while (time_elapsed < FS_BENCHMARK_TIME_MS);

  if (efs_close (fd))
  {
    fs_benchmark_errno ("efs_close()", path);
    return -1;
  }

  return failed ? -1 : 0;
}

#define DELAY_BEFORE_READ_MS  3000  /* 3 seconds */

int
fs_safe_read_prepare (int32 *args, char *path)
{
  /* Prepare a file for reading... by writing it: */
  int rv =  (fs_safe_write_prepare (args, path)
             || fs_test_safe_write (args, path));

  /* Add delay before read to ensure all NOR block
   * erases as a result of GC have completed. */
  fs_util_wait (DELAY_BEFORE_READ_MS);

  return rv;
}

int
fs_test_safe_read (int32 *args, char *path)
{
  int fd;
  int read_count;
  int read_size = BENCH_BUFFER_BYTES;
  int failed = 0;

  (void) args;

  strlcpy (fs_path_buff[2], path, sizeof (fs_path_buff[2]));
  strlcat (fs_path_buff[2], FS_BENCHMARK_DIR "/bench_file",
           sizeof(fs_path_buff[2]));

  fd = efs_open (fs_path_buff[2], O_RDWR, 0666 );

  if (fd < 0)
  {
    fs_benchmark_errno ("efs_open()", path);
    return -1;
  }

  do {
    read_count = efs_read (fd, fs_bench_read_buffer, read_size);

    if (read_count == -1)
    {
      fs_benchmark_errno ("efs_read()", path);
      failed++;
      break;
    }

    total_bytes_read += read_count;

  } while (read_count > 0);

  if (efs_close (fd))
  {
    fs_benchmark_errno ("efs_close()", path);
    failed++;
  }

  return failed ? -1 : 0;
}


#define FS_BENCH_DEFAULT_ENUM_ENTRY_COUNT 1000
#define FS_BENCH_DEFAULT_ENUM_ENTRIES_PER_LEVEL 10

/* Prepares the enumerate directory with many files and 1 subdirectory
 * per level.
 *
 * The total number of file system entries created will be determined by
 * arg[0], it defaults to FS_BENCH_DEFAULT_ENUM_ENTRY_COUNT if zero.
 * These entries will be divided in a chain of subdirectories. The arg[1]
 * will determine how many entries exist in each subdirectory. If arg[1]
 * is zero it will default to FS_BENCH_DEFAULT_ENUM_ENTRIES_PER_LEVEL.
 * For example, if arg[1] is 2 and arg[0] is 4 it will create:
 * ./file1
 * ./d/
 * ./d/file3
 * ./d/d/
 *
 * arg[2] controls the number of regular files in each level. If arg[1] <
 * arg[2] means that no item files 2 will be created, instead regular files
 * and directories will be created.
 *
 * The fs_test_safe_enum assumes subdirectories are named "d". A short
 * directory name is used to enable deeper directories, it avoids hitting
 * the maximum size of path limit in EFS. The maximum number of directories
 * will depend on length of the path supplied.
 *
 * Depth of directory tree: arg[0] / arg[1] (defaults to 100)
 * Max depth of directory tree:
 * (FS_PATH_MAX - strlen(path) - strlen(FS_BENCHMARK_DIR)) / (strlen(d) + 1)
 * Typically: (1024 - 7) / 2 = 508
 *
 * args[0] -> Total number of entries to create (files, items or directories)
 * args[1] -> Number of entries on each level. After this number of entries
 *            is reached a new subdirectory is created.
 * args[2] -> Number of regular files to create in each level.
 */
int
fs_safe_enum_prepare (int32 *args, char *path)
{
  int fd;
  unsigned entry_counter, len;
  uint32 total_count, count, reg_per_level, level_count;
  char *dir_path = fs_path_buff[2];
  char *full_path = fs_path_buff[3];

  total_count = (args[0] > 0) ? args[0] : FS_BENCH_DEFAULT_ENUM_ENTRY_COUNT;
  count = (args[1] > 0) ? args[1] : FS_BENCH_DEFAULT_ENUM_ENTRIES_PER_LEVEL;
  reg_per_level = args[2];

  create_bench_dir (path);

  level_count = 0;
  for (entry_counter = 0; entry_counter < total_count;)
  {
    entry_counter++;
    /* Check if it is time to create a subdirectory. */
    if (entry_counter % count == 0)
    {
      level_count = 0;
      len = strlcat (dir_path, "/d", sizeof (fs_path_buff[2]));
      if (len >= sizeof (fs_path_buff[2]))
      {
        fs_benchmark_errno ("Path too large", dir_path);
        return -1;
      }

      /* Create a new directory */
      if (efs_mkdir (dir_path, 0777))
      {
        fs_benchmark_errno ("efs_mkdir()", dir_path);
        return -1;
      }
    }
    else
    {
      len = snprintf (full_path, sizeof (fs_path_buff[3]),
                      "%s/f%d", dir_path, entry_counter);
      if (len >= sizeof (fs_path_buff[3]))
      {
        fs_benchmark_errno ("Path too large", full_path);
        return -1;
      }

      fd = efs_open (full_path, O_CREAT, 0666);
      if (fd < 0)
      {
        fs_benchmark_errno ("efs_open()", full_path);
        return -1;
      }

      if (efs_close (fd) != 0)
      {
        fs_benchmark_errno ("efs_close()", full_path);
        return -1;
      }

      if (level_count < reg_per_level)
      {
        if (efs_utime (full_path, NULL) != 0)
        {
          fs_benchmark_errno ("efs_utime()", full_path);
          return -1;
        }
      }

      level_count++;
    }
  }

  return 0;
}
/* This test sees how fast entries can be enumerated in the file system.
 * It overloads the total_bytes_read and total_bytes_write counters to
 * return the number of entries enumerated.
 * total_bytes_read = total number of entries enumerated, including '.'
 *                    and '..'
 * total_bytes_write = total number of subdirectories encountered.
 *
 * The script needs to use these values to calculate the speed of entry
 * enumeration. */
int
fs_test_safe_enum (int32 *args, char *path)
{
  struct fs_dirent *dirent;
  EFSDIR *dirp;
  int found_dir = 0;
  struct fs_stat stat_buf;

  (void) args;

  strlcpy (fs_path_buff[2], path, sizeof (fs_path_buff[2]));
  strlcat (fs_path_buff[2], FS_BENCHMARK_DIR, sizeof (fs_path_buff[2]));

  dirp = efs_opendir (fs_path_buff[2]);
  total_bytes_write++;

  if (!dirp)
    return -1;

  while (1)
  {
    dirent = efs_readdir (dirp);

    if (!dirent)
    {
      if (!found_dir)
        break;

      /* This means we found a sub directory. Close the previous one
         and open the new sub directory */
      efs_closedir (dirp);

      strlcat (fs_path_buff[2], "/d", sizeof (fs_path_buff[2]));

      dirp = efs_opendir (fs_path_buff[2]);
      total_bytes_write++;
      found_dir = 0;
    }
    else
    {
      /* It is a valid entry, update counters and process it. */
      total_bytes_read++;

      if (!strcmp (dirent->d_name, ".") ||
          !strcmp (dirent->d_name, ".."))
        continue;

      if (FS_MASK_CHECK ( dirent->d_stats_present, FS_DIRENT_HAS_ST_MODE))
      {
        if (S_ISDIR (dirent->d_stat.st_mode))
          found_dir = 1;
      }
      else
      {
        strlcpy (fs_path_buff[3], fs_path_buff[2],
                    sizeof (fs_path_buff[3]));
        strlcat (fs_path_buff[3], "/" , sizeof(fs_path_buff[3]));
        strlcat (fs_path_buff[3], dirent->d_name, sizeof(fs_path_buff[3]));

        if (efs_lstat (fs_path_buff[3], &stat_buf))
          return -1;

        if (S_ISDIR (stat_buf.st_mode))
          found_dir = 1;
      }
    }
  }

  efs_closedir (dirp);

  return 0;
}

static int
fs_safe_sync_prepare (int32 *args, char *path)
{
  int result;

  (void) args;

  /* Purposely dirty the FileSystem. */
  result = efs_utime (path, NULL);

  return result;
}

int
fs_test_safe_sync (int32 *args, char *path)
{
  struct fs_timings_type *timings;
  int result;

  timings = fs_timings_get_instance ();

  timings->efs_sync_timings.encryption_time = 0;
  timings->efs_sync_timings.write_iovec_time = 0;

  result = efs_sync (path);

  /* The Benchmark response packet structure is documented in our
   * EFS-Diag packets ISOD and hence we cannot add any new members
   * to this structure. So we have to use existing structure members
   * to convert EFS-Sync timings in the benchmark response packet.
   */
  args[0] = timings->efs_sync_timings.encryption_time;
  args[1] = timings->efs_sync_timings.write_iovec_time;

  return result;
}



static void
reset_bench_buffer_fill_seed (void)
{
  bench_buffer_fill_seed = 1;
}

static void
fill_bench_buffer_with_seed (int fill_size, int max_transaction)
{
  ASSERT (fs_bench_buffer != NULL);

  /* Memset the min amount required. */
  fill_size = (fill_size > max_transaction) ? max_transaction : fill_size;
  memset (fs_bench_buffer, bench_buffer_fill_seed, fill_size);

  bench_buffer_fill_seed++;
  if (bench_buffer_fill_seed == 0)
  {
    bench_buffer_fill_seed = 1;
  }
}

static int
fs_benchmark_create_file_of_size (char *path, int file_size,
                                  int max_transaction)
{
  int fd, result, offset, count;

  ASSERT (fs_bench_buffer != NULL);

  fd = efs_open (path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0)
  {
    fs_benchmark_errno ("efs_open()", path);
    return -1;
  }

  offset = 0;
  while (offset < file_size)
  {
    count = file_size - offset;
    if (count > max_transaction)
    {
      count = max_transaction;
    }

    result = efs_write (fd, fs_bench_buffer, count);
    if (result < 0)
    {
      fs_benchmark_errno ("efs_write()", path);
      offset = -1;
      break;
    }

    offset += result;
  }

  if (efs_close (fd))
  {
    fs_benchmark_errno ("efs_close()", path);
    return -1;
  }

  return offset;
}

static int
fs_benchmark_read_file_of_size (char *path, int file_size,
                                int max_transaction)
{
  int fd, result, offset, count;

  fd = efs_open (path, O_RDONLY, 0644);
  if (fd < 0)
  {
    fs_benchmark_errno ("efs_open()", path);
    return -1;
  }

  offset = 0;
  while (offset < file_size)
  {
    count = file_size - offset;
    if (count > max_transaction)
    {
      count = max_transaction;
    }

    result = efs_read (fd, fs_bench_read_buffer, count);
    if (result < 0)
    {
      fs_benchmark_errno ("efs_read()", path);
      offset = -1;
      break;
    }

    offset += result;
  }

  if (efs_close (fd))
  {
    fs_benchmark_errno ("efs_close()", path);
    return -1;
  }

  return offset;
}

static int
fs_benchmark_get_max_transaction_size (char *path)
{
  struct fs_statvfs sbuf;
  int max_transaction;

  /* Stat should not fail */
  if (efs_statvfs (path, &sbuf) != 0)
  {
    return -1;
  }

  max_transaction = (int)sbuf.f_maxwrite;
  if (max_transaction > BENCH_BUFFER_BYTES)
  {
    max_transaction = BENCH_BUFFER_BYTES;
  }

  return max_transaction;
}

static int
fs_benchmark_fill_to_percent (char *path, int fill_percent)
{
  struct fs_statvfs sbuf;
  int pages_to_write, max_transaction, current_fill_percent;
  int result = 0, fill_file_size;
  uint32 used_blocks;

  /* Stat should not fail */
  if (efs_statvfs (path, &sbuf) != 0)
  {
    return -1;
  }

  max_transaction = fs_benchmark_get_max_transaction_size (path);

  used_blocks = sbuf.f_blocks - sbuf.f_bavail;
  current_fill_percent = (int)((used_blocks * 100) / sbuf.f_blocks);
  if (current_fill_percent > fill_percent)
  {
    return -1;
  }

  fill_percent -= current_fill_percent;

  pages_to_write = (fill_percent * sbuf.f_blocks) / 100;
  fill_file_size = pages_to_write * sbuf.f_bsize;

  if (fill_file_size > 0)
  {
    strlcpy (fs_path_buff[2], path, sizeof (fs_path_buff[2]));
    strlcat (fs_path_buff[2], FS_BENCHMARK_DIR "/fill_file",
             sizeof (fs_path_buff[2]));
    fill_bench_buffer_with_seed (fill_file_size, max_transaction);
    result = fs_benchmark_create_file_of_size (fs_path_buff[2], fill_file_size,
                                               max_transaction);
  }

  if (result != fill_file_size)
  {
    result = -1;
  }
  else
  {
    result = max_transaction;
  }

  return result;
}

/* Define benchmarking defaults */
#define FS_BENCH_DEFAULT_START_FILL 50 /* Start test at 50 percent fill */
#define FS_BENCH_DEFAULT_FILE_COUNT 5 /* Five files. */
#define FS_BENCH_DEFAULT_DATA_SIZE (1* 1024 * 1024) /* 1 MB of data */
#define FS_BENCH_PERCENT_DATA_FOR_BENCHMARKING 5 /* Only fill 5 percent */

/* Get the size of each benchmark file. */
static int
fs_benchmark_get_bench_file_size (char *path, uint32 number_bench_files)
{
  struct fs_statvfs sbuf;
  int fill_percent = FS_BENCH_PERCENT_DATA_FOR_BENCHMARKING;
  int efs_size, size_of_file;

  /* Stat should not fail */
  if (efs_statvfs (path, &sbuf) != 0)
  {
    return -1;
  }

  efs_size = (int)(sbuf.f_blocks * sbuf.f_bsize);
  size_of_file = (int)(fill_percent * efs_size) / (number_bench_files * 100);

  /* Round down (i.e floor) the file_size  to a multiple of page size */
  size_of_file = (size_of_file / sbuf.f_bsize) * sbuf.f_bsize;

  return size_of_file;
}

/*
 * Safe Write Benchmarking command args
 *
 * args[0] -> How much percent should EFS be full before running this command?
 *            If 0 is provided, then default start percent of 50% is assumed.
 *            If EFS is already filled more than the start percent provided,
              then this command will return error.
 * args[1] -> How many benchmark files to create while filling EFS?
 *            If 0 is provided, then default value of 5-files is assumed.
 * args[2] -> How much data to write to EFS while running this command?
 *            If 0 is provided, then default value of 1MB of data is assumed.
 *
 */
int
fs_safe_write_prepare2 (int32 *args, char *path)
{
  int start_percent, number_bench_files, total_bench_data_size;

  start_percent = (args[0] > 0) ? args[0] : FS_BENCH_DEFAULT_START_FILL;
  number_bench_files = (args[1] > 0) ? args[1] : FS_BENCH_DEFAULT_FILE_COUNT;
  total_bench_data_size = (args[2] > 0) ? args[2] : FS_BENCH_DEFAULT_DATA_SIZE;

  args[0] = start_percent;
  args[1] = number_bench_files;
  args[2] = total_bench_data_size;

  return create_bench_dir (path);
}

int
fs_test_safe_write2 (int32 *args, char *path)
{
  int start_percent, number_bench_files, total_bench_data_size;
  int bench_file_size, max_transaction_size, result, file_number;
  fs_timetick start_tick, net_ticks;

  start_percent = args[0];
  number_bench_files = args[1];
  total_bench_data_size = args[2];

  /* Reset the fill seed */
  reset_bench_buffer_fill_seed ();

  max_transaction_size = fs_benchmark_fill_to_percent (path, start_percent);
  if (max_transaction_size <= 0)
  {
    return -1;
  }

  bench_file_size = fs_benchmark_get_bench_file_size (path,number_bench_files);
  if (bench_file_size <= 0)
  {
    return -1;
  }

  /* Reset the bench mark global params */
  benchmark_time_in_us = 0;
  total_bytes_read = 0;
  total_bytes_write = 0;

  /* Reset flash and hotplug counters */
  fs_counters_init ();

  file_number = 0;
  while ((int)total_bytes_write < total_bench_data_size)
  {
    snprintf (fs_path_buff[2],sizeof (fs_path_buff[2]), "%s%s/bench_file_%4d",
             path, FS_BENCHMARK_DIR, file_number);

    fill_bench_buffer_with_seed (bench_file_size, max_transaction_size);

    start_tick = fs_timetick_get_timetick ();

    result = fs_benchmark_create_file_of_size (fs_path_buff[2],
               bench_file_size, max_transaction_size);

    net_ticks = fs_timetick_get_timetick () - start_tick;

    benchmark_time_in_us += fs_timetick_timetick_diff_to_us (net_ticks);

    if (result != bench_file_size)
    {
      return -1;
    }

    file_number++;
    if (file_number >= number_bench_files)
    {
      file_number = 0;
    }

    total_bytes_write += bench_file_size;
  }

  return 0;
}

/*
 * Safe Read Benchmarking command args
 *
 * args[0] -> How much percent should EFS be full before running this command?
 *            If 0 is provided, then default start percent of 50% is assumed.
 *            If EFS is already filled more than the start percent provided,
              then this command will return error.
 * args[1] -> How many benchmark files to create while filling EFS?
 *            If 0 is provided, then default value of 5-files is assumed.
 * args[2] -> How much data to read from EFS while running this command?
 *            If 0 is provided, then default value of 1MB of data is assumed.
 *
 */
int
fs_safe_read_prepare2 (int32 *args, char *path)
{
  int bench_file_size, max_transaction_size, result;
  int start_percent, number_bench_files, total_bench_data_size;
  int file_number = 0;


  start_percent = (args[0] > 0) ? args[0] : FS_BENCH_DEFAULT_START_FILL;
  number_bench_files = (args[1] > 0) ? args[1] : FS_BENCH_DEFAULT_FILE_COUNT;
  total_bench_data_size = (args[2] > 0) ? args[2] : FS_BENCH_DEFAULT_DATA_SIZE;

  args[0] = start_percent;
  args[1] = number_bench_files;
  args[2] = total_bench_data_size;

  /* Prepare a file for reading... by writing it: */
  result = fs_safe_write_prepare (args, path);
  if (result != 0)
  {
    return -1;
  }

  /* Reset the fill seed */
  reset_bench_buffer_fill_seed ();

  max_transaction_size = fs_benchmark_fill_to_percent (path, start_percent);
  if (max_transaction_size <= 0)
  {
    return -1;
  }

  bench_file_size = fs_benchmark_get_bench_file_size (path,number_bench_files);
  if (bench_file_size <= 0)
  {
    return -1;
  }

  for (file_number = 0; file_number < number_bench_files ; file_number++)
  {
    snprintf (fs_path_buff[2],sizeof (fs_path_buff[2]), "%s%s/bench_file_%4d",
             path, FS_BENCHMARK_DIR, file_number);

    fill_bench_buffer_with_seed (bench_file_size, max_transaction_size);

    result = fs_benchmark_create_file_of_size (fs_path_buff[2],
               bench_file_size, max_transaction_size);
    if (result != bench_file_size)
    {
      return -1;
    }
  }

  /* Add delay before read to ensure all NOR block
   * erases as a result of GC have completed. */
  fs_util_wait (DELAY_BEFORE_READ_MS);

  return 0;
}


int
fs_test_safe_read2 (int32 *args, char *path)
{
  int number_bench_files, total_bench_data_size;
  int result, max_transaction_size, bench_file_size, file_number;
  fs_timetick start_tick, net_ticks;

  number_bench_files = args[1];
  total_bench_data_size = args[2];

  max_transaction_size = fs_benchmark_get_max_transaction_size (path);
  if (max_transaction_size < 0)
  {
    return -1;
  }

  bench_file_size = fs_benchmark_get_bench_file_size (path,number_bench_files);
  if (bench_file_size <= 0)
  {
    return -1;
  }

  /* Reset the bench mark global params */
  benchmark_time_in_us = 0;
  total_bytes_read = 0;
  total_bytes_write = 0;

  /* Reset flash and hotplug counters */
  fs_counters_init ();

  file_number = 0;
  while ((int)total_bytes_read < total_bench_data_size)
  {
    snprintf (fs_path_buff[2],sizeof (fs_path_buff[2]), "%s%s/bench_file_%4d",
             path, FS_BENCHMARK_DIR, file_number);

    start_tick = fs_timetick_get_timetick ();

    result = fs_benchmark_read_file_of_size (fs_path_buff[2],
               bench_file_size, max_transaction_size);

    net_ticks = fs_timetick_get_timetick () - start_tick;

    benchmark_time_in_us += fs_timetick_timetick_diff_to_us (net_ticks);

    if (result != bench_file_size)
    {
      return -1;
    }

    file_number++;
    if (file_number >= number_bench_files)
    {
      file_number = 0;
    }

    total_bytes_read += bench_file_size;
  }

  return 0;
}

/* ============== Task Handling ============== */

/* Here is the map of command codes to test functions */
const struct fs_benchmark_test fs_benchmark_tests[] = {

  /* Most benchmark tests are "unsafe" or security-violating, so they
   * should only be accessible when requested. */
#ifdef FEATURE_FS_UNSAFE_BENCHMARK
  // Code   Function to call for test       Prep  Cleanup
  { 0x0001, fs_test_sleep_ms,               NULL, NULL },
  { 0x0002, fs_test_nop,                    NULL, NULL },
  { 0x0003, fs_test_df,                     NULL, NULL },
  { 0x0004, fs_test_write,                  NULL, NULL },
  { 0x0005, fs_test_set_cpu_hog,            NULL, NULL },
  { 0x0006, fs_test_random_write,           NULL, NULL },
  { 0x0007, fs_test_truncate,               NULL, NULL },
  { 0x0008, fs_test_fill_fs_to_pct,         NULL, NULL },
  { 0x0009, fs_test_delete,                 NULL, NULL },
  { 0x000A, fs_test_mkdir,                  NULL, NULL },
  { 0x000B, fs_test_rmdir,                  NULL, NULL },
  { 0x000C, fs_test_statvfs,                NULL, NULL },
  { 0x000D, fs_test_flush_deleted_data,     NULL, NULL },
  { 0x000E, fs_test_read,                   NULL, NULL },
  { 0x000F, fs_test_efs1_fail,              NULL, NULL },
  { 0x0010, fs_test_efs1_fail,              NULL, NULL },
  { 0x0011, fs_test_efs1_fail,              NULL, NULL },
  { 0x0012, fs_test_efs1_fail,              NULL, NULL },
  { 0x0013, fs_test_hotplug_poll_interval,  NULL, NULL },
  { 0x0014, fs_test_hotplug_format,         fs_test_hotplug_setup,
                                               fs_test_hotplug_teardown },
  { 0x0015, fs_test_hotplug_read,           fs_test_hotplug_setup,
                                               fs_test_hotplug_teardown },
  { 0x0016, fs_test_hotplug_write,          fs_test_hotplug_setup,
                                               fs_test_hotplug_teardown },
  { 0x0017, fs_test_hotplug_check_media,    NULL, NULL },
  { 0x0018, fs_test_hotplug_lock,           NULL, NULL },
  { 0x0019, fs_test_hotplug_unlock,         NULL, NULL },
  { 0x001A, fs_test_raw_open,               NULL, NULL },
  { 0x001B, fs_test_raw_read,               NULL, NULL },
  { 0x001C, fs_test_raw_write,              NULL, NULL },
  { 0x001D, fs_test_raw_close,              NULL, NULL },
  { 0x001E, fs_test_stuff_pattern,          NULL, NULL },
  { 0x001f, fs_test_check_pattern,          NULL, NULL },
  { 0x0020, fs_test_raw_fopen,              NULL, NULL },
  { 0x0021, fs_test_raw_fclose,             NULL, NULL },
  { 0x0022, fs_test_raw_fwrite,             NULL, NULL },
  { 0x0023, fs_test_raw_fread,              NULL, NULL },
  { 0x0024, fs_test_privatedir_set_access,  NULL, NULL },
  { 0x0025, fs_test_reset,                  NULL, NULL },
  { 0x0026, fs_test_deltree,                NULL, NULL },
  { 0x0027, fs_test_enumerate,              NULL, NULL },
  { 0x0028, fs_test_chown_recursive,        NULL, NULL },
  { 0x0029, fs_test_chmod_recursive,        NULL, NULL },
  { 0x002A, fs_test_raw_setvbuf,            NULL, NULL },
  { 0x002B, fs_test_raw_fflush,             NULL, NULL },
  { 0x002C, fs_test_utime,                  NULL, NULL },
  { 0x0032, fs_test_babylon_short_test,     NULL, NULL },
  { 0x0033, fs_test_babylon_word_test,      NULL, NULL },
  { 0x0034, fs_test_hotplug_random_write,   fs_test_hotplug_setup,
                                               fs_test_hotplug_teardown },
  { 0x0035, fs_test_babylon_speed_test,     NULL, NULL },
  { 0x0036, fs_test_babylon_port_test,      NULL, NULL },
  { 0x0037, fs_test_memory_protection,      NULL, NULL },
  { 0x0038, fs_test_sync,                   NULL, NULL },
  { 0x0039, fs_test_ls,                     NULL, NULL },
#endif

  /* These functions are considered "safe", and should be permitted on
   * any commercial phone, for use by any user. */
  /* ---------- START OF SAFE TESTS  ---------- */
  { 0x0080,
    fs_test_safe_read, fs_safe_read_prepare, cleanup_bench_dir },
  { 0x0081,
    fs_test_safe_write, fs_safe_write_prepare, cleanup_bench_dir },
  { 0x0082,
    fs_test_safe_enum, fs_safe_enum_prepare, cleanup_bench_dir },
  { 0x0083, fs_test_hotplug_format,         NULL, NULL },
  { 0x0084, fs_test_safe_sync, fs_safe_sync_prepare, NULL },

  /* New Safe profile functions */
  { 0x0085,
    fs_test_safe_read2, fs_safe_read_prepare2, cleanup_bench_dir },
  { 0x0086,
    fs_test_safe_write2, fs_safe_write_prepare2, cleanup_bench_dir },

  /* ---------- END OF SAFE TESTS ---------- */

  { 0x0000, NULL, NULL, NULL}   /* Must be last! */
};


static q_type cmd_q;            /* Command queue for benchmark task */
static q_type cmd_free_q;       /* Free command queue. */
static q_type rpt_q;            /* Report queue */
static q_type rpt_free_q;       /* Free report queue */

static uint8 fs_benchmark_queue_buffers_initialized = 0;
static fs_benchmark_queued_cmd *cmds = NULL;
static fs_benchmark_queued_rpt *rpts = NULL;
static void fs_benchmark_do_cmd (fs_benchmark_queued_cmd *cmd);
static void process_next_benchmark_cmd (void);

/*
 * Receives a single command packet from DIAG and queues it for processing
 * Returns 0 on success.
 */

static int32
fs_benchmark_initialize_queues (void)
{
  int i;
  uint32 cmd_buffer_size, rpt_buffer_size;

  if (fs_benchmark_queue_buffers_initialized == 1)
  {
    return 0;
  }

  cmd_buffer_size = sizeof (fs_benchmark_queued_cmd) * FS_BENCHMARK_MAX_CMDS;
  rpt_buffer_size = sizeof (fs_benchmark_queued_rpt) * FS_BENCHMARK_MAX_RPTS;

  cmds = (fs_benchmark_queued_cmd *) fs_benchmark_malloc (cmd_buffer_size);
  if (cmds == NULL)
  {
    return -ENOMEM;
  }


  rpts = (fs_benchmark_queued_rpt *) fs_benchmark_malloc (rpt_buffer_size);
  if (rpts == NULL)
  {
    fs_benchmark_free (cmds);
    return -ENOMEM;
  }


  /* ------------- Initialize Command Buffers --------------- */
  (void) q_init (&cmd_q);
  (void) q_init (&cmd_free_q);

  for (i = 0; i < FS_BENCHMARK_MAX_CMDS; i++)
  {
    cmds[i].hdr.done_q_ptr = &cmd_free_q;
    q_put (&cmd_free_q, q_link (&cmds[i], &cmds[i].hdr.link));
  }

  /* ------------- Initialize Report Buffers --------------- */
  (void) q_init (&rpt_q);
  (void) q_init (&rpt_free_q);

  for (i = 0; i < FS_BENCHMARK_MAX_RPTS; i++)
  {
    rpts[i].hdr.done_q_ptr = &rpt_free_q;
    q_put (&rpt_free_q, q_link (&rpts[i], &rpts[i].hdr.link));
  }

  fs_benchmark_queue_buffers_initialized = 1;
  return 0;
}

int
fs_benchmark_put_cmd (fs_benchmark_test_cmd *cmd)
{
  fs_benchmark_queued_cmd *new_cmd;
  int32 init_queue_result;

  if (!cmd)
    return 1;

  init_queue_result = fs_benchmark_initialize_queues ();

  if (init_queue_result != 0)
    return 2;

  /* Make sure they didn't give us an enormous path */
  if ((strlen ((const char *) cmd->path) + 1) > sizeof (new_cmd->whole_path))
    return 3;

  /* Get a new command item */
  new_cmd = (fs_benchmark_queued_cmd *) q_get (&cmd_free_q);
  if (new_cmd == NULL)
    return 4;

  /* Copy the command packet and the path */
  new_cmd->cmd = *cmd;
  strlcpy (new_cmd->whole_path, (const char *) cmd->path,
                                    sizeof (new_cmd->whole_path));

  /* Queue the new command */
  q_link (new_cmd, &new_cmd->hdr.link);
  q_put (&cmd_q, &new_cmd->hdr.link);

  /* Signal the benchmark task */
  fs_os_set_sigs (&fs_benchmark_tcb, FS_BENCHMARK_CMD_Q_SIG);

#ifdef FS_UNIT_TEST
  /* There is no task to execute it off-target, so do it right now */
  flushing = 0;
  process_next_benchmark_cmd();
#endif

  return 0;
}

/* Dequeue a single command packet from our benchmark queue and process it */
static void
process_next_benchmark_cmd (void)
{
  fs_benchmark_queued_cmd *got_cmd;

  if (fs_benchmark_queue_buffers_initialized == 0)
  {
    FS_MSG_ERROR_0 ("Processing EFS Benchmark cmds before queue init!");
    return; /* We should never have reached here so return gracefully. */
  }

  test_running = 1;
  while ((got_cmd = (fs_benchmark_queued_cmd *) q_get (&cmd_q)) != NULL)
  {
    if (!flushing)
      fs_benchmark_do_cmd (got_cmd);

    /* return command buffer to free pool */
    if (got_cmd->hdr.done_q_ptr)
      q_put (got_cmd->hdr.done_q_ptr, &got_cmd->hdr.link);
  }
  flushing = 0;
  test_running = 0;
}

/* Takes a report off the rpt_q, copies it to the user's provided
   report buffer, and returns the status of the queue.

   Note that this allocates ONE static report buffer, and
   returns a pointer to it.  Use while fresh!
*/
enum fs_benchmark_queue_status_t
fs_benchmark_get_rpt (fs_benchmark_report *report)
{
  fs_benchmark_queued_rpt *rpt_ptr;

  if (fs_benchmark_queue_buffers_initialized == 0)
  {
    return FS_BENCHMARK_IDLE; /* Return Idle so scripts are happy. */
  }

  rpt_ptr = q_get (&rpt_q);
  if (rpt_ptr)
  {
    *report = rpt_ptr->report;  /* Copy the whole structure */
    /* Free the report */
    if (rpt_ptr->hdr.done_q_ptr)
      q_put (rpt_ptr->hdr.done_q_ptr, &rpt_ptr->hdr.link);
    return FS_BENCHMARK_RESULT;
  }
  else
  {
    /* We have no report to give */
    memset ((void *)report, 0, sizeof(*report));

    if (!test_running && !q_cnt(&cmd_q))
      return FS_BENCHMARK_IDLE;
    else
      return FS_BENCHMARK_RUNNING; /* Please come again */
  }
}

/*
  Stops all currently running tests; however, the single test running
  at the time this function is called does not get interrupted, as the
  execution might have gone into filesyestem code already
*/
void
fs_benchmark_flush (void)
{
  flushing = 1;
}

static void*
fs_benchmark_align_pointer (void *ptr, uint32 align_size)
{
  uint32 aligned_ptr;

  /* Make sure align is power of 2. */
  ASSERT ((align_size & (align_size - 1)) == 0);

  aligned_ptr  = (uint32)ptr;
  aligned_ptr += (align_size - 1);
  aligned_ptr &= ~(align_size - 1);
  ptr = (void *)aligned_ptr;

  return ptr;
}

static int
fs_benchmark_init_buffers (void)
{
  uint32 bench_buffer_size;
  void *buf1, *buf2;

  if (fs_benchmark_buffers_initialized)
  {
    return 0; /* Benchmark buffers are already initialized, return success. */
  }

  /* malloc memory taking alignment into account. */
  bench_buffer_size = BENCH_BUFFER_ACTUAL_BYTES + FS_DM_ALIGN_BYTES;
  buf1 = buf2 = NULL;

  buf1 = fs_benchmark_malloc (bench_buffer_size);
  if (buf1 == NULL)
  {
    return -ENOMEM;
  }

  buf2 = fs_benchmark_malloc (bench_buffer_size);
  if (buf2 == NULL)
  {
    fs_benchmark_free (buf1);
    return -ENOMEM;
  }

  fs_bench_buffer = fs_benchmark_align_pointer (buf1, FS_DM_ALIGN_BYTES);
  fs_bench_read_buffer = fs_benchmark_align_pointer (buf2, FS_DM_ALIGN_BYTES);

  randomize_bench_buffer (fs_benchmark_seed);

  fs_benchmark_buffers_initialized = 1;
  return 0;
}


/* Main work loop to process one test command */
static void
fs_benchmark_do_cmd (fs_benchmark_queued_cmd *cmd)
{
  fs_benchmark_queued_rpt *rpt;
  const struct fs_benchmark_test *test = fs_benchmark_tests;
  uint32 hotplug_read_counter = 0, hotplug_write_counter = 0;
  int init_result;

  if (!cmd || !cmd->cmd.command)
    return;                     /* ??? */

  /* Search for a matching function call for this cmd */
  while (test->func != NULL)
  {
    if (test->cmd == cmd->cmd.command)
      break;
    test++;
  }
  /* No Match? test now points to the null test */

  /* If only the safe tests are allowed, validate the path by making
   * sure it is exactly one of these allowed strings: */
#ifndef FEATURE_FS_UNSAFE_BENCHMARK
  if (strcmp (cmd->whole_path, "/") &&
      strcmp (cmd->whole_path, "/mmc1") &&
      strcmp (cmd->whole_path, "/mmc2") &&
      strcmp (cmd->whole_path, "/mmc3") &&
      strcmp (cmd->whole_path, "/mmc4") &&
      strcmp (cmd->whole_path, "/usb1") &&
      strcmp (cmd->whole_path, "/ftl1"))
    return;
#endif

  /* Do the one-time initialization of the benchmark buffers. */
  init_result = fs_benchmark_init_buffers ();
  if (init_result != 0)
  {
    rpt = (fs_benchmark_queued_rpt *) q_get (&rpt_free_q);
    if (!rpt)
    {
      EFS_BAB_DEBUG_PRINTF ("Failed to get report buffer?");
    }
    else
    {
      /* fill the report buffer */
      fs_benchmark_report *r = &rpt->report;

      memset (r, 0, sizeof (fs_benchmark_report));

      r->cmd = cmd->cmd;  /* Copy whole command to report */
      r->failures = 1;    /* Record any errors */

      /* Queue the response in the report queue */
      q_link (rpt, &rpt->hdr.link);
      q_put (&rpt_q, &rpt->hdr.link);
    }

    return;
  }

  /* Process the test command */
  {
    int failure_count;          /* Number of times a test failed */
    uint64 stopwatch;
    uint16 cycles_remaining;
    int i;
    int32 args[FS_BENCHMARK_ARGS];

    failure_count = 0;

    /* The cmd.args[] are PACKED, and likely misaligned.
     * We have to copy them out into native words */
    for (i = 0; i < FS_BENCHMARK_ARGS; i++)
      args[i] = cmd->cmd.args[i];

    /* Reset flash and hotplug counters */
    fs_counters_init ();
#ifdef FEATURE_FS_HOTPLUG
    hotplug_counters_init ();
#endif
    cycles_remaining = cmd->cmd.repeat;

    /* Unknown tests (not listed in table) return a failure */
    if (test->func == NULL) {
      EFS_BAB_DEBUG_PRINTF ("Unrecognized test command %d", cmd->cmd.command);
      cycles_remaining = 0;
      failure_count = 1;
    }

    /* Do any necessary prep before starting the actual test.
       eg. create/delete directories */
    if (test->prepare && (test->prepare (args, cmd->whole_path) != 0))
    {
      EFS_BAB_DEBUG_PRINTF ("Test prepare failed! %d", cmd->cmd.command);
      cycles_remaining = 0;
      failure_count = 1;
    }

    /* Forget about any read/write counts during preparation */
    total_bytes_write = 0;
    total_bytes_read = 0;
    benchmark_time_in_us = 0;

    stopwatch = fs_time_ms_u64(); /* Go! */
    while (cycles_remaining && !flushing)
    {
      int result;
      EFS_BAB_DEBUG_PRINTF ("Starting test 0x%04X", test->cmd);

      EFS_BAB_START (EFS_BAB_BENCHMARK_TEST);
      result = test->func (args, cmd->whole_path);
      EFS_BAB_END (EFS_BAB_BENCHMARK_TEST);

      /* Because some commands (like df) can pass values back through args,
       * we have to copy them back into the Diag command. */
      for (i = 0; i < FS_BENCHMARK_ARGS; i++)
        cmd->cmd.args[i] = args[i];

      if (result != 0)
        failure_count++;

      cycles_remaining--;
    }
    stopwatch = fs_time_ms_u64() - stopwatch; /* Stop! */

    /* Do any necessary cleanup after the test is done. */
    if (test->cleanup && (test->cleanup (args, cmd->whole_path) != 0))
    {
      EFS_BAB_DEBUG_PRINTF ("Test cleanup failed! %d", cmd->cmd.command);
      failure_count++;
    }

    rpt = (fs_benchmark_queued_rpt *) q_get (&rpt_free_q);
    if (!rpt) {
      EFS_BAB_DEBUG_PRINTF ("Failed to get report buffer?");
    } else {
      /* fill the report buffer */
      fs_benchmark_report *r = &rpt->report;

      r->cmd = cmd->cmd;        /* Copy whole command to report */
      r->failures = failure_count; /* Record any errors */

      if (stopwatch > 0xFFFFFFFF)
        r->milliseconds = 0xFFFFFFFF;
      else
        r->milliseconds = (uint32) stopwatch;

      if (benchmark_time_in_us > 0xFFFFFFFF)
        r->microseconds = 0xFFFFFFFF;
      else
        r->microseconds = (uint32) benchmark_time_in_us;

      r->total_bytes_write           = total_bytes_write;
      r->total_bytes_read            = total_bytes_read;
      r->flash_write_partial_counter =
                            fs_flash_counter.flash_write_partial_counter;
      r->flash_erase_block_counter   =
                            fs_flash_counter.flash_erase_block_counter;
      r->flash_erase_begin_counter =
                            fs_flash_counter.flash_erase_begin_counter;
      r->flash_erase_suspend_counter =
                            fs_flash_counter.flash_erase_suspend_counter;

      r->flash_read_min_time = fs_flash_time_counter.read_timings.min_time;
      r->flash_read_max_time = fs_flash_time_counter.read_timings.max_time;
      r->flash_read_total_time = fs_flash_time_counter.read_timings.total_time;

      r->flash_write_min_time = fs_flash_time_counter.write_timings.min_time;
      r->flash_write_max_time = fs_flash_time_counter.write_timings.max_time;
      r->flash_write_total_time=fs_flash_time_counter.write_timings.total_time;

      r->flash_erase_block_min_time =
                            fs_flash_time_counter.erase_block_timings.min_time;
      r->flash_erase_block_max_time =
                            fs_flash_time_counter.erase_block_timings.max_time;
      r->flash_erase_block_total_time =
                          fs_flash_time_counter.erase_block_timings.total_time;

      r->flash_erase_suspend_min_time =
                        fs_flash_time_counter.erase_suspend_timings.min_time;
      r->flash_erase_suspend_max_time =
                        fs_flash_time_counter.erase_suspend_timings.max_time;
      r->flash_erase_suspend_total_time =
                        fs_flash_time_counter.erase_suspend_timings.total_time;

#ifdef FEATURE_FS_HOTPLUG
      hotplug_write_counter = hotplug_counter.hotplug_write_sector_counter;
      hotplug_read_counter = hotplug_counter.hotplug_read_sector_counter;
#endif

      /* I'm not proud of this.. because of the fixed nature of the
         Hotplug diag packet, we can't add new fields easily.  So, for
         the sake of benchmarking hotplug, we substitute the blockdev
         counters for flash counters if it's obvious the test did only
         non-flash operations */
      if (fs_flash_counter.flash_write_page_counter == 0
          && fs_flash_counter.flash_read_page_counter == 0
          && (hotplug_write_counter != 0
          || hotplug_write_counter != 0))
      {
        r->flash_write_page_counter = hotplug_write_counter;
        r->flash_read_page_counter = hotplug_read_counter;
      }
      else
      {
        r->flash_write_page_counter =fs_flash_counter.flash_write_page_counter;
        r->flash_read_page_counter  = fs_flash_counter.flash_read_page_counter;
      }

      /* Queue the response in the report queue */
      q_link (rpt, &rpt->hdr.link);
      q_put (&rpt_q, &rpt->hdr.link);
    }
  }
}

/* The benchmark task loop */
static void
fs_benchmark_task (dword parm)
{
  fs_os_sigs_type task_sigs;

  (void) parm;

  /* Init */
  fs_os_def_timer (&fs_benchmark_sleep_timer,
                   &fs_benchmark_tcb,
                   FS_BENCHMARK_SLEEP_TIMER_SIG);

  EFS_BAB_DEBUG_PRINTF ("Started fs_benchmark_task.");

  /* Loop forever handling commands. */
  while (1)
  {
    task_sigs = fs_os_wait ((fs_os_sigs_type)(FS_BENCHMARK_CMD_Q_SIG));

    /* when new command arrives */
    if (task_sigs & FS_BENCHMARK_CMD_Q_SIG)
    {
      fs_os_clr_sigs (&fs_benchmark_tcb, FS_BENCHMARK_CMD_Q_SIG);
      process_next_benchmark_cmd ();
    }
  }
}

/* Benchmark init function;
   initializes the cmd_q, cmd_free_q, rpt_q and rpt_free_q
   Also starts the benchmark task running */
void
fs_benchmark_init (void)
{
  fs_benchmark_queue_buffers_initialized = 0;
  fs_benchmark_buffers_initialized = 0;
  fs_bench_buffer = fs_bench_read_buffer = NULL;

  #ifdef FS_UNIT_TEST
    ut_fs_benchmark_total_malloc_count = 0;
  #endif

  fs_os_def_task_ext (&fs_benchmark_tcb,
                      fs_benchmark_stack,
                      FS_BENCHMARK_STACK_SIZE,
                      FS_BENCHMARK_PRI,
                      fs_benchmark_task,
                      0,
                      "FS Benchmark",
                      0,
                      FS_DOG_INVALID_RPT);
}


/* A task that tries to hog a specified amount of CPU. */
static void
fs_cpu_hog_task (dword parm)
{
  static int fs_cpu_hog_dummy_var = 0;
  uint64 t0, t1;

  (void) parm;

  fs_os_def_timer (&fs_cpu_hog_timer,
                   &fs_cpu_hog_tcb,
                   FS_CPU_HOG_TIMER_SIG);

  while (1) {
    /* Work */
    t0 = fs_time_ms_u64();  /* get start time */
    do {
      int i;
      for (i = 0; i < 10000; i++) /* waste valuable computations */
        fs_cpu_hog_dummy_var++;

      t1 = fs_time_ms_u64();  /* get current time */
    } while ((t1 - t0) < fs_cpu_hog_run_ms);

    /* Then sleep */
    fs_os_clr_sigs (&fs_cpu_hog_tcb, FS_CPU_HOG_TIMER_SIG);
    fs_os_set_timer (&fs_cpu_hog_timer, fs_cpu_hog_sleep_ms);
    fs_os_wait (FS_CPU_HOG_TIMER_SIG);
  } /* Repeat endlessly, and try to find time to smell the flowers */
}

/* Initialization function for the CPU Hog task, must be called once. */
void
fs_cpu_hog_init (void)
{
  static int cpu_hog_initialized = 0;

  if (!cpu_hog_initialized) {
    /* Start up the fs_cpu_hog task */
    fs_os_def_task_ext (&fs_cpu_hog_tcb,
                       fs_cpu_hog_stack,
                       FS_CPU_HOG_STACK_SIZ,
                       FS_CPU_HOG_PRI,
                       fs_cpu_hog_task,
                       0,
                       "FS CPU Hog",
                       0,
                       FS_DOG_INVALID_RPT);
    cpu_hog_initialized = 1;
  }
}

#else  /* !FEATURE_FS_BENCHMARK */
/*
 * If FEATURE_FS_BENCHMARK is NOT defined, provide failing stubs for
 * all public functions, to avoid #ifdefs elsewhere.
 */
void fs_benchmark_init (void) {}
void fs_cpu_hog_init (void) {}
void fs_benchmark_flush (void) {}

int  fs_benchmark_put_cmd (fs_benchmark_test_cmd *cmd)
{
  (void) cmd;

  return -1;                    /* Always Fail */
}

enum fs_benchmark_queue_status_t
fs_benchmark_get_rpt (fs_benchmark_report *report)
{
  (void) report;

  return FS_BENCHMARK_IDLE;
}

#endif /* !FEATURE_FS_BENCHMARK */
