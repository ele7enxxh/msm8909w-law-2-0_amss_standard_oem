#ifndef MEMUTILS_H
#define MEMUTILS_H

/**
  @file memutils.h
  @brief

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_memory" 
	  group description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*=========================================================================== 
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
===========================================================================*/
/*
Copyright (c) 2004-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
*/
/*===========================================================================

                                 Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/memutils.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
03/16/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/11/10   EBR     Doxygenated File.
05/26/04   spl     Added enhanced debugging features to the heap cache
02/02/04   spl     initial implementation.
===========================================================================*/

/** @addtogroup utils_memory
@{ */
#include "coreapi_variation.h"
#include <stdlib.h>



/** @cond
*/
#define HEAP_CACHE_SECTION1_INCREMENT  0x0020
#define HEAP_CACHE_SECTION1_MAX        0x0400
#define HEAP_CACHE_SECTION2_INCREMENT  HEAP_CACHE_SECTION1_MAX
#define HEAP_CACHE_SECTION2_MAX        0x10000
#define HEAP_CACHE_SECTION2_OFFSET     31
#define HEAP_CACHE_NOT_ENABLED         0xFFFFFFFF
#define HEAP_CACHE_NOT_CACHED          0xFFFFFFFE
/** @endcond */

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

============================================================================== */
/** @cond
*/
#if defined (FEATURE_MEMUTILS_MEMORY_TRACKING) || defined (FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING)
#define HeapCacheGetBuffer(s) HeapCacheGetBufferDebug(s,__FILE__,__LINE__)
#else
#define HeapCacheGetBuffer(s) HeapCacheGetBufferProd(s)
#endif
/** @endcond */

/**
  Gets a buffer from the heap cache. If the heap cache is not enabled or 
  if there is no available buffer in the heap cache, the function gets a 
  buffer from the memory system. This function is used for debugging.
 
  Users may call this function directly instead of calling mem_malloc().

  @param[in] size Size to allocate in bytes.
  @param[in] file File from which the function was called.
  @param[in] line Line from which the function was called.

  @return
  Returns a pointer to the allocated buffer, or NULL if the buffer 
  could not be allocated.

  @dependencies
  None.
*/
void *HeapCacheGetBufferDebug(size_t size, const char *file, unsigned long line);

/**
  Gets a buffer from the heap cache. If the heap cache is not enabled or 
  if there is no available buffer in the heap cache, the function gets a 
  buffer from the memory system.
   
  Users may call this function directly instead of calling mem_malloc().

  @param[in] size Size to allocate in bytes.

  @return
  Returns a pointer to the allocated buffer, or NULL if the buffer 
  could not be allocated.

  @dependencies
  None.
*/
void *HeapCacheGetBufferProd(size_t size);


/**
  Returns a buffer to the heap cache. If caching is not 
  enabled, or the buffer is bigger than the heap cache limit,
  the function returns a buffer from the memory system.

  @param[out] buffer Buffer to be queried.

  @return
  None.

  @dependencies
  Users may call this function directly instead of calling mem_free() 
  ONLY if they obtained the buffer by calling HeapCacheGetBufferProd().
*/
void HeapCacheReturnBuffer(void *buffer);


/**
  Activates heap caching for an application.

  @return
  None.

  @dependencies
  HeapCacheRelease() must be called when exiting the application.
*/
void HeapCacheAddRef(void);


/**
  Disables heap caching for an application. This function must be 
  called when exiting an application that called HeapCacheAddRef().

  @return
  None.

  @dependencies
  HeapCacheAddRef() must have been called by the application.
*/
void HeapCacheRelease(void);


/**
  Searches a list of allocated buffers and looks for memory overwrites.

  @return
  None.

  @dependencies
  None.
*/
void HeapCacheFindMemoryOverwrite(void);


/**
  Searches a list of allocated buffers and logs all buffers that 
  have not been released.

  The log is stored in the EFS root directory and is named heapcacheleaksX.txt,
  where X is an incrementing index that is reset across sessions.

  @return
  None.

  @dependencies
  None.
*/
void HeapCacheLeakReport(void);


/**
  Initializes the memory tracker.

  @return
  None.

  @dependencies
  None.
*/
void memory_tracker_initialize(void);


/**
  Terminates the memory tracker.

  @return
  None.

  @dependencies
  None.
*/
void memory_tracker_terminate(void);


/**
  Spawns a new memory tracker.

  @note1hang Users who want to use this function must send email to 
  @latexonly \rm\color{blue}{\underline
  {support.cdmatech@qualcomm.com}\color{black}.
  @endlatexonly

  @param[in] p    Pointer.
  @param[in] size Size.
  @param[in] file File.
  @param[in] line Line.

  @return
  None.

  @dependencies
  None.
*/
void memory_tracker_new(void *p, long size, const char *file, int line);


/**
  Deletes a memory tracker.

  @note1hang Users who want to use this function must send email to 
  @latexonly \rm\color{blue}{\underline
  {support.cdmatech@qualcomm.com}\color{black}.
  @endlatexonly

  @param[in] p Pointer to the memory tracke.

  @return
  None.

  @dependencies
  None.
*/
void memory_tracker_delete(void *p);


/**
  Gets the location of a memory tracker.

  @note1hang Users who want to use this function must send email to 
  @latexonly \rm\color{blue}{\underline
  {support.cdmatech@qualcomm.com}\color{black}.
  @endlatexonly

  @param[in] p    Pointer.
  @param[in] file File.
  @param[in] line Line.

  @return
  None.

  @dependencies
  None.
*/
void memory_tracker_get_location(void *p, char **file, unsigned long *line);


#ifdef __cplusplus
}  // end brace for extern "C" {

//////////////////////////////////////////////////////////////////////////////////
//
// C++ dynamic memory allocation
//
//////////////////////////////////////////////////////////////////////////////////

/**
@brief Class used to manage the heap.
*/
class HeapCache
{
public:
   /** @brief Contains the stored cache bin. It holds the structure of the bin 
   variables that maintain the properties of the bin. 
   
   The bins are used for effectively maintaining the heap. This is intended 
   to reduce fragmentation.
   */
   typedef struct CacheBinStruct
   {
      void *m_freeBuffer;  /**< Free buffer. */
      size_t m_size;       /**< Size of the bin. */
      size_t m_count;      /**< Number of items in the bin. */
   } CacheBinType;

   
   /**
   Gets a buffer from the heap cache. If the heap cache is not enabled or 
   if there is no available buffer in the heap cache, the function gets a 
   buffer from the memory system.
	
   @note1hang This function is called automatically when new is called from 
   a C++ file.
   
   @param[in] size Size to allocate in bytes.
   @param[in] file Used for debugging.
   @param[in] line Used for debugging.
   
   @return
   Returns a pointer to the allocated buffer, or NULL if the buffer 
   could not be allocated.
   
   @dependencies
   None.
   */
   static void *GetBuffer(size_t size, const char *file = 0, unsigned long line = 0);

   
   /**
   Returns a buffer to the heap cache. If caching is not enabled or 
   the buffer is bigger than the heap cache limit, the function returns a 
   buffer from the memory system.
   
   @note1hang This function is called automatically when delete is 
   called from a C++ file.
   
   @param[in] buffer Buffer to be returned.
   
   @return
   None.
   
   @dependencies
   None.
   */
   static void ReturnBuffer(void *buffer);

   /**
   Activates heap caching for an application.
      
   @return
   None.
   
   @dependencies
   Release() must be called when exiting the application.

   */
   static void AddRef(void);

   /**
   Disables heap caching for an application. This function must be 
   called when exiting an application that called AddRef().
   
   @return
   None.
   
   @dependencies
   AddRef() must have been called by the application.
   */
   static void Release(void);  

   
   /**
   Outputs usage statistics into a predetermined file.
   
   @return
   None.
   
   @dependencies
   None.
   
   @sideeffects
   Statistics are written to a predetermined file.
   */
   static void UsageStatistics(void);

private:
   /** @cond
   */
   /**
   Returns the index of the first bin whose value is greater than or equal 
   to size. The bin increments have been selected such that the optimizer can 
   turn the divide into a right shift.
	
   @param[in] size Size (in bytes) to be found.
   
   @return
   Index of the bin that fulfills the requirements.
   
   @dependencies
   None.
   */
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
   /** @endcond */

   static int m_cacheBinCount;
   static CacheBinType m_cacheBin[];
   static int m_refCnt;
}; /* END HeapCache */

#ifdef FEATURE_MEMUTILS_NEW_DELETE
// Overload the global new/delete operators so we can do error checks
// and mem diagnostics
/**
  Allocates a new block of memory. The file and line options are only 
  required for Debug mode and can be omitted from the function call.

  @param[in] size Size of the new amount of memory.
  @param[in] file File name from which the function was called 
                  (only used in Debug mode).
  @param[in] line Line number from which the function was called 
                  (only used in Debug mode).

  @return
  Return a pointer to the allocated memory, or NULL if the memory could not 
  be allocated.

  @dependencies
  None.
*/
void * operator new(size_t size, const char* file, unsigned long line);
/** @cond
*/
void * operator new[](size_t size, const char *file, unsigned long line);
void * operator new(size_t size);
void * operator new[](size_t size);
/** @endcond */

/** @cond
*/
#define DEBUG_NEW new(__FILE__,__LINE__)
#define new DEBUG_NEW
/** @endcond */

/**
  Frees the memory associated with a pointer. The file and line options 
  are only required for Debug mode and can be omitted from the function call.

  @param[in] p    Pointer to the memory to be freed.
  @param[in] file File name from which the function was called 
                  (only used in Debug mode).
  @param[in] line Line number from which the function was called 
                  (only used in Debug mode).

  @return
  None.

  @dependencies
  None.
*/
void operator delete(void *p, const char* file, unsigned long line);
/** @cond 
*/
void operator delete[](void *p, const char* file, unsigned long line);
void operator delete(void *p);
void operator delete[](void *p);
/** @endcond */
#endif // FEATURE_MEMUTILS_NEW_DELETE

#endif /* __cplusplus */
/** @} */ /* end_addtogroup utils_memory */
#endif // MEMUTILS_H
