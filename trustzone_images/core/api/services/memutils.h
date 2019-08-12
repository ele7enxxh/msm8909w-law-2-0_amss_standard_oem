#ifndef MEMUTILS_H
#define MEMUTILS_H
/*===========================================================================

DESCRIPTION
  Memory utilities including the following:
  operator new
  operator delete
  HeapCache class
     Helps prevent memory fragmentation for application that use new/delete 
     frequently
  HeapCache C interface functions
     C interface functions allowing C code to use the HeapCache
  memory tracking functions
     Used for tracking memory leaks when calling new/delete

Copyright (c) 2004      by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                                 Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/memutils.h#1 $ 
$DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     --------------------------------------------------------
02/02/04   spl     initial implementation.
05/26/04   spl     Added enhanced debugging features to the heap cache
===========================================================================*/

#include <stdlib.h>

#if defined CUST_H
   #include "customer.h"
#endif

#define HEAP_CACHE_SECTION1_INCREMENT  0x0020
#define HEAP_CACHE_SECTION1_MAX        0x0400
#define HEAP_CACHE_SECTION2_INCREMENT  HEAP_CACHE_SECTION1_MAX
#define HEAP_CACHE_SECTION2_MAX        0x10000
#define HEAP_CACHE_SECTION2_OFFSET     31
#define HEAP_CACHE_NOT_ENABLED         0xFFFFFFFF
#define HEAP_CACHE_NOT_CACHED          0xFFFFFFFE

// Enable this feature to enable the memutils version of new and delete 
//#define FEATURE_MEMUTILS_NEW_DELETE

// Enable this feature to have new and delete call HeapCache::GetBuffer and
// HeapCahce::ReturnBuffer.  HeapCache::GetBuffer and HeapCache::ReturnBuffer
// will call malloc and free when the heap cache is not enabled.  However,
// even when the heap cache is disabled, the heap cache code still adds 
// overhead bytes to each malloc operation, and provides memory overwrite 
// checking (memory overwrite checking requires that 
// FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING be enabled).
// The feature requires that FEATURE_MEMUTILS_NEW_DELETE be enabled as well.
//#define FEATURE_MEMUTILS_NEW_DELETE_HEAP_CACHE

// Enable this feature to track memory leaks
// Then call memory_tracker_initialize() to start tracking leaks.
// and memory_tracker_terminate() to write a leak report to memleakn.txt 
// (where n is the number of times initialize and terminate are called).
//#define FEATURE_MEMUTILS_MEMORY_TRACKING

// Enable this feature to automatically print a leak report to memleaksX.txt
// in the EFS root directory when memory_tracker_terminate is called.  
// memory_tracker_terminate is called from HeapCache::Release when 
// FEATURE_MEMUTILS_MEMORY_TRACKING is enabled.  X is an index that is 
// incremented each time the leak reports are printed during a single session.
// If this feature is disabled, you may still call memory_tracker_leak_report 
// manually to print out a leak report.
//#define FEATURE_MEMUTILS_LEAK_REPORT

// Enable this feature to increase heap cache debbugging information
// This will slightly increase both memory usage and the time
// it takes to get and return heap cache memory buffers
// Enahanced debugging capabilities include:
// 1) A simplified version of the memory tracker:
//    a) A simplified, and faster version of memory leak tracking.
//    b) Current and peak memory usage information.
// 2) Improved memory overwrite checking (can report the file and line number)
//    where the offending buffer was allocated.
//#define FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING

// Enable this feature to automatically call HeapCacheLeakReport when HeapCache::Release 
// is called.  If this feature is disabled, you may still call HeapCacheLeakReport 
// manually to print out a leak report.  This feature requires that 
// FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING is enabled
//#define FEATURE_MEMUTILS_HEAP_CACHE_LEAK_REPORT

// Enable this feature to collect and print out heap usage statistics.
// This will track the number of buffers allocated/cached for each 
// cache bin, then print the statistics to the file heapcacheusageX.txt
// in the EFS root directory, where X is an index that is incremented 
// each time the usage statistics are printed during a single session
//#define FEATURE_MEMUTILS_HEAP_CACHE_USAGE_STATISTICS

#ifdef __cplusplus
extern "C" {
#endif
/* ===========================================================================
HEAPCACHE Interface functions
DESCRIPTION
============================================================================== */
/* ===========================================================================
FUNCTION HEAPCACHEGETBUFFER
DESCRIPTION
   Get a buffer from the heap cache, or the memory system if the heap cache 
   is not enabled or if there is no available buffer in the heap cache

   You may call this function directly instead of calling malloc (or OEM_Malloc)
============================================================================== */
#if defined (FEATURE_MEMUTILS_MEMORY_TRACKING) || defined (FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING)
#define HeapCacheGetBuffer(s) HeapCacheGetBufferDebug(s,__FILE__,__LINE__)
#else
#define HeapCacheGetBuffer(s) HeapCacheGetBufferProd(s)
#endif

void *HeapCacheGetBufferDebug(size_t size, const char *file, unsigned long line);
void *HeapCacheGetBufferProd(size_t size);
/* ===========================================================================
FUNCTION HEAPCACHERETURNBUFFER
DESCRIPTION
   Return a buffer to the heap cache, or the memory system if cahcing is not 
   enabled, or the buffer is bigger than the heap cache limit

   You may call this function directly instead of calling free (or OEM_Free), 
   but you MUST have obtained the buffer by calling HeapCacheGetBuffer!!!!
============================================================================== */
void HeapCacheReturnBuffer(void *buffer);

/* ===========================================================================
FUNCTION HEAPCACHEADDREF
DESCRIPTION
   Call this function to activate heap caching for you application.  
============================================================================== */
void HeapCacheAddRef(void);

/* ===========================================================================
FUNCTION HEAPCACHERELEASE
DESCRIPTION
   Call this when exiting an application that called HeapCacheAddRef
============================================================================== */
void HeapCacheRelease(void);

/* ===========================================================================
FUNCTION HEAPCACHEFINDMEMORYOVERWRITE
DESCRIPTION
   Searches the list of allocated buffers looking for memory overwrites
============================================================================== */
void HeapCacheFindMemoryOverwrite(void);

/* ===========================================================================
FUNCTION HEAPCACHELEAKREPORT
DESCRIPTION
   Searches the list of allocated buffers and writes to a file every one that
   has not been released.
   File is stored in the EFS root directory, and is named heapcacheleaksX.txt,
   where X is an index that is incremented each time the leak reports are 
   printed during a single session.

============================================================================== */
void HeapCacheLeakReport(void);

/* ===========================================================================
FUNCTION MEMORY_TRACKER_INITIALIZE
DESCRIPTION
============================================================================== */
void memory_tracker_initialize(void);

/* ===========================================================================
FUNCTION MEMORY_TRACKER_TERMINATE
DESCRIPTION
============================================================================== */
void memory_tracker_terminate(void);

/* ===========================================================================
FUNCTION MEMORY_TRACKER_NEW
DESCRIPTION
============================================================================== */
void memory_tracker_new(void *p, long size, const char *file, int line);

/* ===========================================================================
FUNCTION MEMORY_TRACKER_DELETE
DESCRIPTION
============================================================================== */
void memory_tracker_delete(void *p);

/* ===========================================================================
FUNCTION MEMORY_TRACKER_GET_LOCATION
DESCRIPTION
============================================================================== */
void memory_tracker_get_location(void *p, char **file, unsigned long *line);


#ifdef __cplusplus
}  // end brace for extern "C" {

//////////////////////////////////////////////////////////////////////////////////
//
// C++ dynamic memory allocation
//
//////////////////////////////////////////////////////////////////////////////////

/* ===========================================================================
CLASS HEAPCACHE
DESCRIPTION
============================================================================== */
class HeapCache
{
public:
   typedef struct CacheBinStruct
   {
      void *m_freeBuffer;
      size_t m_size;
      size_t m_count;
   } CacheBinType;

   /* ===========================================================================
   FUNCTION HEAPCACHEGETBUFFER
   DESCRIPTION
      Get a buffer from the heap cache, or the memory system if the heap cache 
      is not enabled or if there is no available buffer in the heap cache

      This function is called automatically when new is called from a C++ file.
   ============================================================================== */
   static void *GetBuffer(size_t size, const char *file = 0, unsigned long line = 0);

   /* ===========================================================================
   FUNCTION HEAPCACHERETURNBUFFER
   DESCRIPTION
      Return a buffer to the heap cache, or the memory system
      if cahcing is not enabled, or the buffer is bigger than the 
      heap cache limit

      This function is called automatically when delete is called from a C++ file.
   ============================================================================== */
   static void ReturnBuffer(void *buffer);

   /* ===========================================================================
   FUNCTION HEAPCACHEADDREF
   DESCRIPTION
      Call this function to activate heap caching for you application.  
   ============================================================================== */
   static void AddRef(void);

   /* ===========================================================================
   FUNCTION HEAPCACHERELEASE
   DESCRIPTION
      Call this when exiting an application that called HeapCacheAddRef
   ============================================================================== */
   static void Release(void);  

   /* ===========================================================================
   FUNCTION USAGESTATISTICS
   DESCRIPTION
   ============================================================================== */
   static void UsageStatistics(void);

private:
   /* ===========================================================================
   FUNCTION GETBIN
   DESCRIPTION
      Returns the index of the first bin whose value is greater than or equal 
      to size.  The bin increments have been selected such that the optimizer can 
      turn the divide into a right shift.
   ============================================================================== */
   static inline unsigned long GetBin(size_t size)
   {
      unsigned long bin = HEAP_CACHE_NOT_CACHED;

      if (size > 0)
      {
         if (size < HEAP_CACHE_SECTION1_MAX)
         {
            bin = (size - 1) / HEAP_CACHE_SECTION1_INCREMENT;
         }
         else if (size < HEAP_CACHE_SECTION2_MAX)
         {
            bin = ((size - 1) / HEAP_CACHE_SECTION2_INCREMENT) + HEAP_CACHE_SECTION2_OFFSET;
         }
      }
      return bin;
   };

   static int m_cacheBinCount;
   static CacheBinType m_cacheBin[];
   static int m_refCnt;
}; /* END HeapCache */

#ifdef FEATURE_MEMUTILS_NEW_DELETE
// Overload the global new/delete operators so we can do error checks
// and mem diagnostics
void * operator new(size_t size, const char* file, unsigned long line);
void * operator new[](size_t size, const char *file, unsigned long line);
void * operator new(size_t size);
void * operator new[](size_t size);
#define DEBUG_NEW new(__FILE__,__LINE__)
#define new DEBUG_NEW

void operator delete(void *p, const char* file, unsigned long line);
void operator delete[](void *p, const char* file, unsigned long line);
void operator delete(void *p);
void operator delete[](void *p);
#endif // FEATURE_MEMUTILS_NEW_DELETE

#endif /* __cplusplus */

#endif //_ZREX_MEM_H_
