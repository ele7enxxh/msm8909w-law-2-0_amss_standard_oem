/* =========================================================================

DESCRIPTION
  Memory utilities including the following:
  HeapCache class
     Helps prevent memory fragmentation for application that use new/delete 
     frequently
  HeapCache C interface functions
     C interface functions allowing C code to use the HeapCache
  operator new
  operator delete
  memory tracking functions
     Used for tracking memory leaks

Copyright (c) 2004 - 2009     by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================ */

/* =========================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/services/utils/src/memutils.cpp#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
06/01/09   sri     Avoid compiler warnings for m_cacheBinCount in boolean expr.
04/15/09   sri     Replaced sprintf() calls with snprintf()
01/31/05   spl     Modified code to properly track non-cached buffers 
                   (size is less than 1 or greater than 65536),  for
                   leak tracking and statistics purposes.  Added code
                   required for off-target development environments.
10/19/05   as      Fixed compiler warnings.
02/02/04   spl     Initial Implementation.
05/26/04   spl     Added enhanced debugging features to the heap cache
============================================================================ */

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#include "memutils.h"
#include "assert.h"
#include <string.h>

#if defined(FEATURE_MEMUTILS_MEMORY_TRACKING) || defined(FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING)
extern "C" {
#include "fs_public.h"
#include "fs_fcntl.h"
#include "stdio.h"
#include "clk.h"
}
#endif

/* ------------------------------------------------------------------------
** Data Structures
** ------------------------------------------------------------------------ */
#ifdef FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING
typedef struct InfoEnhancedStruct
{
   // Used for the buffer list.  The buffer list will link all blocks
   // of memory allocated by the heap cache. The memory list can be used 
   // to search all buffers allocated by the heap cache looking for things 
   // such as memory overwrites
   struct InfoPreStruct *bufferLinkNext;
   struct InfoPreStruct *bufferLinkPrev;
   // Allocation file name
   const char *fileName;
   // Allocation line number
   unsigned long lineNum;
   // In Use flag used to track memory leaks
   unsigned long inUse;
   // The order in which this buffer was requested from the heap cache
   // this value can help in tracking down a specific leak.
   // First call to GetBuffer == 1, second call == 2, ...
   unsigned long index;
   unsigned long time;
} InfoEnhancedType;
#endif

typedef struct InfoPreStruct
{
   // Before buffer flag used for validation/memory overwrite checking
   unsigned long flag;
   // Bin index or free link
   unsigned long binFreeLink;
   // Size of the buffer
   unsigned long size;
#ifdef FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING
   InfoEnhancedType enhanced;
#endif
   // address of buffer_start can be as the real buffer address
   // this simlifies the code that returning the buffer pointer
   unsigned long bufferStart;
} InfoPreType;

typedef struct InfoPostEnum
{
   // After buffer flag used for validation/memory overwrite checking
   unsigned long flag;
} InfoPostType;


/* ------------------------------------------------------------------------
** Defines
** ------------------------------------------------------------------------ */
// Before buffer padding (size required to store the HeapCacheInfoPre data
#define PADDING_PRE ((long)(sizeof(InfoPreType)-sizeof(unsigned long)))
// After buffer padding (size required to store the HeapCacheInfoPost data
#define PADDING_POST ((long)sizeof(InfoPostType))
#define FLAG_PRE   0xABCDEF01 // Flag before buffer
#define FLAG_POST  0x10FEDCBA // Flag after buffer

// Casts the buffer to the InfoPre type
#define INFO_PRE(buf) ((InfoPreType *)(buf))
// Pointer to the HeapCacheInfoPost data 
#define INFO_POST(buf) ((InfoPostType *)(((char *)buf) + (((INFO_PRE(buf)->size + 3) / 4) * 4) + PADDING_PRE))

#ifdef T_WINNT
#error code not present
#endif

extern "C" void *OEM_Malloc(unsigned long dwBytes);
extern "C" void  OEM_Free(void *ptr);

/* ------------------------------------------------------------------------
** Glocal Variables
** ------------------------------------------------------------------------ */
#ifdef FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING
extern "C"
{
unsigned long  heap_cache_allocation_index = 0;
InfoPreType   *heap_cache_buffer_list = NULL;
InfoPreType   *heap_cache_buffer_end  = NULL;
const char    *heap_cache_error_file  = NULL;
unsigned long  heap_cache_error_line  = 0;

// Net heap usage statistics (minus  memory overhead added by the heap cache)
long  heap_cache_curr_count     = 0;
long  heap_cache_peak_count     = 0;
long  heap_cache_curr_usage_net = 0;
long  heap_cache_peak_usage_net = 0;

// Statistics for buffers that are greater than the maximum size for caching
long  heap_cache_not_cached_curr_count = 0;
long  heap_cache_not_cached_peak_count = 0;
long  heap_cache_not_cached_curr_usage = 0;
long  heap_cache_not_cached_peak_usage = 0;

// Total heap usage statistics (including any memory overhead added by the heap cache)
long  heap_cache_curr_usage_total = 0;
long  heap_cache_peak_usage_total = 0;
}
#endif

/* ===========================================================================
FUNCTION HEAPCACHEGETBUFFERDEBUG
DESCRIPTION
   Get a buffer from the heap cache, or the memory system if the heap cache 
   is not enabled or if there is no available buffer in the heap cache

   This function is called automatically when new is called from a C++ file.
   You may also call this function directly instead of calling malloc 
   (or OEM_Malloc)
============================================================================== */
void* HeapCacheGetBufferDebug(size_t size, const char *file, unsigned long line)
{
  /*in case the debug feature was defined but the file name 
   was passed as a NULL pointer, then pass the FILE name using 
   the macro */
  if(NULL == file) 
  {
    return HeapCache::GetBuffer(size, __FILE__,__LINE__);
  }
  else
  {
    return HeapCache::GetBuffer(size, file, line);
  }
}

/* ===========================================================================
FUNCTION HEAPCACHEGETBUFFERPROD
DESCRIPTION
   Get a buffer from the heap cache, or the memory system if the heap cache 
   is not enabled or if there is no available buffer in the heap cache

   This function is called automatically when new is called from a C++ file.
   You may also call this function directly instead of calling malloc 
   (or OEM_Malloc)
============================================================================== */
void* HeapCacheGetBufferProd(size_t size)
{
  return HeapCache::GetBuffer(size);
}

/* ===========================================================================
FUNCTION HEAPCACHERETURNBUFFER
DESCRIPTION
   Return a buffer to the heap cache, or the memory system
   if cahcing is not enabled, or the buffer is bigger than the 
   heap cache limit

   This function is called automatically when delete is called from a C++ file.
   You may also call this function directly instead of calling free 
   (or OEM_Free), but you MUST have obtained the buffer by calling 
   HeapCacheGetBuffer!!!!
============================================================================== */
void HeapCacheReturnBuffer(void *buffer)
{
   HeapCache::ReturnBuffer(buffer);
}

/* ===========================================================================
FUNCTION HEAPCACHEADDREF
DESCRIPTION
   Call this function to activate heap caching for you application.  
============================================================================== */
void HeapCacheAddRef(void)
{
   HeapCache::AddRef();
}

/* ===========================================================================
FUNCTION HEAPCACHERELEASE
DESCRIPTION
   Call this when exiting an application that called HeapCacheAddRef
============================================================================== */
void HeapCacheRelease(void)
{
   HeapCache::Release();
}

/* ===========================================================================
FUNCTION HEAPCACHEFINDMEMORYOVERWRITE
DESCRIPTION
   Searches the list of allocated buffers looking for memory overwrites
============================================================================== */
void HeapCacheFindMemoryOverwrite(void)
{
#ifdef FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING
   InfoPreType *buffer;
   unsigned long index = 0;
   INTLOCK()
   buffer = heap_cache_buffer_list;
   while (buffer)
   {
      index++;
      if (INFO_POST(buffer)->flag != FLAG_POST)
      {
         // Memory overwrite found, get the file and line number then assert(FALSE)
         heap_cache_error_file = buffer->enhanced.fileName;
         heap_cache_error_line = buffer->enhanced.lineNum;
         ASSERT(FALSE);
      }
      buffer = buffer->enhanced.bufferLinkNext;
   }
   INTFREE()
#else
  MSG_HIGH("HeapCacheFindMemoryOverwrite(): "
          "FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING NOT defined",0,0,0);
#endif
}

/* ===========================================================================
FUNCTION HEAPCACHELEAKREPORT
DESCRIPTION
   Searched the list of allocated buffers and prints out every one that
   has not been released
============================================================================== */
void HeapCacheLeakReport(void)
{
#ifdef FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING
   static int heapcache_report_num = 0;
   char fname[29];
   char str[1024];

   heapcache_report_num++;
   snprintf(fname, sizeof(fname), "heapcacheleaks%d.txt", heapcache_report_num);
   int fd = efs_open(fname, O_WRONLY | O_TRUNC | O_CREAT, S_IREAD | S_IWRITE);
   if (fd != -1)
   {
      InfoPreType *info;
      unsigned long index = 0;

      snprintf(str, sizeof(str), "Net   - Memory usage minus heap cache overhead\r\n"); 
      snprintf(str, sizeof(str), "Total - Memory usage including heap cache overhead\r\n"); 
      snprintf(str, sizeof(str), "     Peak Count  Peak Bytes\r\n"); 
      efs_write(fd, str, strlen(str));
      snprintf(str, sizeof(str), "Net:   %8ld    %8ld\r\n", 
         heap_cache_peak_count, heap_cache_peak_usage_net);
      efs_write(fd, str, strlen(str));
      snprintf(str, sizeof(str), "Total: %8ld    %8ld\r\n", 
         heap_cache_peak_count, heap_cache_peak_usage_total);
      efs_write(fd, str, strlen(str));

      snprintf(str, sizeof(str), "      Time\t      Addr\t Size\tFile                                     \t Line\r\n");
      efs_write(fd, str, strlen(str));

      INTLOCK()
      info = heap_cache_buffer_list;
      while (info)
      {
         index++;
#ifdef T_WINNT
         #error code not present
#else
         if (info->enhanced.inUse)
#endif
         {
            snprintf(str, sizeof(str), "%10ld\t0x%08lx\t%5ld\t%-40s\t%5ld\r\n", 
               info->enhanced.time, (unsigned long)info, info->size, info->enhanced.fileName, info->enhanced.lineNum);
            efs_write(fd, str, strlen(str));
         }
         info = info->enhanced.bufferLinkNext;
      } 
      INTFREE()
      efs_close(fd);
   }
   // Set the allocation index to even values of 1000000 * the nuber of times 
   // this report is printed to make it easier to spot new allocations after 
   // the each leak report is printed
   heap_cache_allocation_index = heapcache_report_num * 1000000;
#else
  MSG_HIGH("HeapCacheLeakReport(): "
          "FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING NOT defined",0,0,0);
#endif
}

/* ===========================================================================
CLASS HEAPCACHE
DESCRIPTION
============================================================================== */
int HeapCache::m_refCnt = 0;



// Bin Array is divided into two sections
// Section 1 is in   32 byte increments from >    0 to <=  1024
// Section 2 is in 1024 byte increments from > 1024 to <= 65536
// Each bin holds buffers whose length is <= the bin's m_size value 
// and > the previous bin's m_size value  

HeapCache::CacheBinType HeapCache::m_cacheBin[] = 
{
   {NULL, 0x0020, 0}, // Beginning of Section 1
   {NULL, 0x0040, 0},
   {NULL, 0x0060, 0},
   {NULL, 0x0080, 0},
   {NULL, 0x00A0, 0},
   {NULL, 0x00C0, 0},
   {NULL, 0x00E0, 0},
   {NULL, 0x0100, 0},
   {NULL, 0x0120, 0},
   {NULL, 0x0140, 0},
   {NULL, 0x0160, 0},
   {NULL, 0x0180, 0},
   {NULL, 0x01A0, 0},
   {NULL, 0x01C0, 0},
   {NULL, 0x01E0, 0},
   {NULL, 0x0200, 0},
   {NULL, 0x0220, 0},
   {NULL, 0x0240, 0},
   {NULL, 0x0260, 0},
   {NULL, 0x0280, 0},
   {NULL, 0x02A0, 0},
   {NULL, 0x02C0, 0},
   {NULL, 0x02E0, 0},
   {NULL, 0x0300, 0},
   {NULL, 0x0320, 0},
   {NULL, 0x0340, 0},
   {NULL, 0x0360, 0},
   {NULL, 0x0380, 0},
   {NULL, 0x03A0, 0},
   {NULL, 0x03C0, 0},
   {NULL, 0x03E0, 0},
   {NULL, 0x0400, 0}, // End of Section 1
   {NULL, 0x0800, 0}, // Beginning of Section 2
   {NULL, 0x0C00, 0},
   {NULL, 0x1000, 0},
   {NULL, 0x1400, 0},
   {NULL, 0x1800, 0},
   {NULL, 0x1C00, 0},
   {NULL, 0x2000, 0},
   {NULL, 0x2400, 0},
   {NULL, 0x2800, 0},
   {NULL, 0x2C00, 0},
   {NULL, 0x3000, 0},
   {NULL, 0x3400, 0},
   {NULL, 0x3800, 0},
   {NULL, 0x3C00, 0},
   {NULL, 0x4000, 0},
   {NULL, 0x4400, 0},
   {NULL, 0x4800, 0},
   {NULL, 0x4C00, 0},
   {NULL, 0x5000, 0},
   {NULL, 0x5400, 0},
   {NULL, 0x5800, 0},
   {NULL, 0x5C00, 0},
   {NULL, 0x6000, 0},
   {NULL, 0x6400, 0},
   {NULL, 0x6800, 0},
   {NULL, 0x6C00, 0},
   {NULL, 0x7000, 0},
   {NULL, 0x7400, 0},
   {NULL, 0x7800, 0},
   {NULL, 0x7C00, 0},
   {NULL, 0x8000, 0},
   {NULL, 0x8400, 0},
   {NULL, 0x8800, 0},
   {NULL, 0x8C00, 0},
   {NULL, 0x9000, 0},
   {NULL, 0x9400, 0},
   {NULL, 0x9800, 0},
   {NULL, 0x9C00, 0},
   {NULL, 0xA000, 0},
   {NULL, 0xA400, 0},
   {NULL, 0xA800, 0},
   {NULL, 0xAC00, 0},
   {NULL, 0xB000, 0},
   {NULL, 0xB400, 0},
   {NULL, 0xB800, 0},
   {NULL, 0xBC00, 0},
   {NULL, 0xC000, 0},
   {NULL, 0xC400, 0},
   {NULL, 0xC800, 0},
   {NULL, 0xCC00, 0},
   {NULL, 0xD000, 0},
   {NULL, 0xD400, 0},
   {NULL, 0xD800, 0},
   {NULL, 0xDC00, 0},
   {NULL, 0xE000, 0},
   {NULL, 0xE400, 0},
   {NULL, 0xE800, 0},
   {NULL, 0xEC00, 0},
   {NULL, 0xF000, 0},
   {NULL, 0xF400, 0},
   {NULL, 0xF800, 0},
   {NULL, 0xFC00, 0},
   {NULL, 0x10000, 0} // End of Section 2
};

int HeapCache::m_cacheBinCount = sizeof(m_cacheBin) / sizeof(CacheBinType);

/* ===========================================================================
FUNCTION HEAPCACHE::ADDREF
DESCRIPTION
============================================================================== */
void HeapCache::AddRef()
{

   INTLOCK()
   m_refCnt++;
   if (m_refCnt == 1)
   {
#ifdef FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING
      heap_cache_peak_count       = 0;
      heap_cache_peak_usage_net   = 0;
      heap_cache_peak_usage_total = 0;
#endif
#ifdef FEATURE_MEMUTILS_MEMORY_TRACKING
      memory_tracker_initialize();
#endif
   }
   INTFREE()
   return;
}

/* ===========================================================================
FUNCTION HEAPCACHE::RELEASE
DESCRIPTION
============================================================================== */
void HeapCache::Release(void)
{
   int i;
   InfoPreStruct *buffer;

   m_refCnt--;

#ifdef FEATURE_MEMUTILS_HEAP_CACHE_USAGE_STATISTICS
   if (m_refCnt == 0)
   {
      HeapCache::UsageStatistics();
   }
#endif

#if defined FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING
   if (m_refCnt == 0)
   {
      HeapCacheFindMemoryOverwrite();
   }
#endif // FEATURE_MEMUTILS_MEMORY_TRACKING || FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING

   for (i = 0; m_refCnt == 0 && i < m_cacheBinCount; i++)
   {
      buffer = NULL;
      do
      {
         INTLOCK()
         buffer = (InfoPreStruct *)m_cacheBin[i].m_freeBuffer;
         if (buffer)
         {
            m_cacheBin[i].m_freeBuffer = (void *)buffer->binFreeLink;
#ifdef FEATURE_MEMUTILS_HEAP_CACHE_USAGE_STATISTICS
            m_cacheBin[i].m_count--;
#endif
         }
         INTFREE()

         if (buffer)
         {
            // setting bin to not cached forces ReturnBuffer to free the buffer
            buffer->binFreeLink = HEAP_CACHE_NOT_CACHED;
            // Set inUse to TRUE to prevent an assert that verifies that you are not 
            // double-deleting a buffer.
#if defined (FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING)
            buffer->enhanced.inUse = TRUE;
#endif
            ReturnBuffer(&buffer->bufferStart);
         }
      } while (buffer != NULL && m_refCnt == 0);
   }

#if defined (FEATURE_MEMUTILS_MEMORY_TRACKING)
   if (m_refCnt == 0)
   {
      memory_tracker_terminate();
   }
#endif 

#if defined FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING && defined FEATURE_MEMUTILS_HEAP_CACHE_LEAK_REPORT
   if (m_refCnt == 0)
   {
      HeapCacheLeakReport();
   }
#endif 

   return;
} /* END Terminate */

/* ===========================================================================
FUNCTION GETBUFFER
DESCRIPTION
============================================================================== */
void *HeapCache::GetBuffer(size_t size, 
#if defined (FEATURE_MEMUTILS_MEMORY_TRACKING) || defined (FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING)
                           const char *file, unsigned long line
#else
                           const char * /*file*/, unsigned long /*line*/
#endif
                           )
{
   InfoPreType *buffer = NULL;
#ifdef FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING
   unsigned long index = ++heap_cache_allocation_index;
#endif

   // Round the buffer size up to the nearest dword
   size_t bufferSize = ((size + 3) / 4) * 4;
   unsigned long bin = HEAP_CACHE_NOT_ENABLED;
   
#ifdef T_WINNT
   #error code not present
#endif
   {
      if (m_refCnt > 0)
      {
         bin = GetBin(size);
         if (bin < (unsigned long)m_cacheBinCount)
         {
            bufferSize = m_cacheBin[bin].m_size;
            INTLOCK()
            if (m_cacheBin[bin].m_freeBuffer != NULL)
            {
               buffer = (InfoPreType *)m_cacheBin[bin].m_freeBuffer;
               m_cacheBin[bin].m_freeBuffer = (void *)buffer->binFreeLink;
            }
            INTFREE()
            /* enable this code to zero out cached blocks before returning them 
            if (buffer != NULL)
            {
               memset(buffer, 0, bufferSize + (PADDING_PRE + PADDING_POST));
            }
            */
         }
      }
   }
   if (buffer == NULL)
   {

#ifdef T_WINNT
      #error code not present
#else
      buffer = (InfoPreType *)malloc(bufferSize + PADDING_PRE + PADDING_POST);
#endif

#ifdef FEATURE_MEMUTILS_HEAP_CACHE_USAGE_STATISTICS
      if (buffer && bin < m_cacheBinCount)
      {
         m_cacheBin[bin].m_count++;
      }
#endif

#ifdef FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING
      if (buffer)
      {
         buffer->enhanced.bufferLinkNext = NULL;
         buffer->enhanced.bufferLinkPrev = NULL;
         if (bin < m_cacheBinCount || bin == HEAP_CACHE_NOT_CACHED)
         {
            INTLOCK()
            if (heap_cache_buffer_list == NULL)
            {
               heap_cache_buffer_list = buffer;
            }
            // insert at the end of the buffer list
            buffer->enhanced.bufferLinkPrev = heap_cache_buffer_end;
            if (heap_cache_buffer_end != NULL)
            {
               heap_cache_buffer_end->enhanced.bufferLinkNext = buffer;
            }
            heap_cache_buffer_end = buffer;
            INTFREE()
         }
      }
#endif // FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING
   }

   if (buffer != NULL)
   {
#ifdef FEATURE_MEMUTILS_MEMORY_TRACKING
   #ifdef T_WINNT
      #error code not present
#endif
      {
         // Pass in bufferSize to track true memory usage
         memory_tracker_new(buffer, bufferSize, file, line);
      }
#endif
      buffer->flag = FLAG_PRE;
      buffer->binFreeLink = bin;
      buffer->size = size;
#ifdef FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING
   #ifdef T_WINNT
      #error code not present
#endif
      {
         // Net heap usage statistics (minus  memory overhead added by the heap cache)
         heap_cache_curr_usage_net += size;
         if (heap_cache_peak_usage_net < heap_cache_curr_usage_net)
         {
            heap_cache_peak_usage_net = heap_cache_curr_usage_net;
         }
         heap_cache_curr_count++;
         if (heap_cache_peak_count < heap_cache_curr_count)
         {
            heap_cache_peak_count = heap_cache_curr_count;
         }
         heap_cache_curr_usage_total += (bufferSize + PADDING_PRE + PADDING_POST);
         if (heap_cache_peak_usage_total < heap_cache_curr_usage_total)
         {
            heap_cache_peak_usage_total = heap_cache_curr_usage_total;
         }
         if (bin == HEAP_CACHE_NOT_CACHED)
         {
            heap_cache_not_cached_curr_usage += size;
            heap_cache_not_cached_curr_count++;
            if (heap_cache_not_cached_peak_usage < heap_cache_not_cached_curr_usage)
            {
               heap_cache_not_cached_peak_usage = heap_cache_not_cached_curr_usage;
            }
            if (heap_cache_not_cached_peak_count < heap_cache_not_cached_curr_count)
            {
               heap_cache_not_cached_peak_count = heap_cache_not_cached_curr_count;
            }
         }
      }

      buffer->enhanced.fileName = file;
      buffer->enhanced.lineNum  = line;
      buffer->enhanced.inUse    = TRUE;
      buffer->enhanced.index    = index;
      buffer->enhanced.time     = clk_uptime();
#endif
      INFO_POST(buffer)->flag = FLAG_POST;
      buffer = (InfoPreType *)&buffer->bufferStart;
   }
#ifdef FEATURE_MEMUTILS_HEAP_CACHE_LEAK_REPORT
   else
   {
      HeapCacheLeakReport();
   }
#endif
   return (void *)buffer;
}


/* ===========================================================================
FUNCTION HEAPCACHE::RETURNBUFFER
DESCRIPTION
============================================================================== */
void HeapCache::ReturnBuffer(void *buffer_in)
{
   if (buffer_in != NULL)
   {
      InfoPreType *buffer =  (InfoPreType *)(&((char *)buffer_in)[-PADDING_PRE]);
      unsigned long bin = buffer->binFreeLink;
#ifdef FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING
#ifdef T_WINNT
      #error code not present
#endif
      {
         if (buffer->flag != FLAG_PRE)
         {
            HeapCacheFindMemoryOverwrite();
         }
      }
#endif
      // If this assert fires, then this memory was either not allocated using 
      // GetBuffer, or there was a memory overwrite before this buffer.  
      // Enabling FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING will call 
      // HeapCacheFindMemoryOverwrite when this happens, and may uncover the 
      // source of this overwrite
      ASSERT(buffer->flag == FLAG_PRE);

#ifdef FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING
      if (INFO_POST(buffer)->flag != FLAG_POST)
      {
         heap_cache_error_file = buffer->enhanced.fileName;
         heap_cache_error_line = buffer->enhanced.lineNum;
      }
#endif
      // If this assert fires, then this memory was either not allocated using 
      // GetBuffer, or there was a memory overwrite in this buffer
      // Enabling FEATURE_MEMUTILS_MEMORY_TRACKING will call 
      // memory_tracker_get_location to get the allocation location of this buffer
      ASSERT(INFO_POST(buffer)->flag == FLAG_POST);

#ifdef FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING
      ASSERT(buffer->enhanced.inUse == TRUE);

      // Total heap usage statistics (including any memory overhead added by the heap cache)
      if (bin < m_cacheBinCount || bin == HEAP_CACHE_NOT_CACHED)
      {
         // Net heap usage statistics (minus  memory overhead added by the heap cache)
         heap_cache_curr_usage_net -= buffer->size;
         heap_cache_curr_count--;
      }
      if (bin < m_cacheBinCount)
      {
         heap_cache_curr_usage_total -= (m_cacheBin[bin].m_size + PADDING_PRE + PADDING_POST);
      }
      else if (bin == HEAP_CACHE_NOT_CACHED)
      {
         heap_cache_curr_usage_total -= (buffer->size + PADDING_PRE + PADDING_POST);
         heap_cache_not_cached_curr_usage -= buffer->size;
         heap_cache_not_cached_curr_count--;
      }
      buffer->enhanced.inUse = FALSE;
#endif

#ifdef FEATURE_MEMUTILS_MEMORY_TRACKING
#ifdef T_WINNT
      #error code not present
#endif
      {
         memory_tracker_delete(buffer);
      }
#endif
      if ( ((unsigned long)m_refCnt > 0) && (bin < (unsigned long)m_cacheBinCount) )
      {
#ifdef FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING
         // set the returned buffer data to 0x00 to indicate that this was deleted.  
         // This may help catch code that is using freed buffers.
         memset(&buffer->bufferStart, 0x00, buffer->size);
#endif

         INTLOCK()
         buffer->binFreeLink = (unsigned long)m_cacheBin[bin].m_freeBuffer;
         m_cacheBin[bin].m_freeBuffer = buffer;
         INTFREE()

         buffer = NULL;
      }
      if (buffer != NULL)
      {
#ifdef FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING
         INTLOCK()
            // remove this from the buffer link
            if (buffer->enhanced.bufferLinkNext)
            {
               buffer->enhanced.bufferLinkNext->enhanced.bufferLinkPrev = buffer->enhanced.bufferLinkPrev;
            }
            else if (buffer == heap_cache_buffer_end)
            {
               heap_cache_buffer_end = buffer->enhanced.bufferLinkPrev;
            }

            if (buffer->enhanced.bufferLinkPrev)
            {
               buffer->enhanced.bufferLinkPrev->enhanced.bufferLinkNext = buffer->enhanced.bufferLinkNext;
            }
            else if (buffer == heap_cache_buffer_list)
            {
               heap_cache_buffer_list = buffer->enhanced.bufferLinkNext;
            }
            INTFREE()
#endif
#ifdef T_WINNT
               #error code not present
#else
               free(buffer);
#endif // T_WINNT
      }
   }
   return;
}

extern "C"
{
void HeapCacheUsageStatistics(void)
{
   HeapCache::UsageStatistics();
}
}
/* ===========================================================================
FUNCTION HEAPCACHEUSAGESTATISTICS
DESCRIPTION
============================================================================== */
void HeapCache::UsageStatistics(void)
{
#ifdef FEATURE_MEMUTILS_HEAP_CACHE_USAGE_STATISTICS
   static int heapcache_report_num = 0;
   char fname[25];
   char str[1024];
   int i;

   heapcache_report_num++;
   snprintf(fname, sizeof(fname), "heapcacheusage%d.txt", heapcache_report_num);
   int fd = efs_open(fname, O_WRONLY | O_TRUNC | O_CREAT, S_IREAD | S_IWRITE);
   if (fd != -1)
   {
      unsigned long total = 0;
      for (i = 0; i < m_cacheBinCount; i++)
      {
         total += (m_cacheBin[i].m_size * m_cacheBin[i].m_count);
      }

      total += heap_cache_not_cached_peak_usage;

      snprintf(str, sizeof(str), "Total Cached Bytes: %8ld\r\n", total);
      efs_write(fd, str, strlen(str));

      snprintf(str, sizeof(str), "   Index\t      Size\t     Count\t     Bytes\r\n");
      efs_write(fd, str, strlen(str));

      for (i = 0; i < m_cacheBinCount; i++)
      {
         snprintf(str, sizeof(str), "%8ld\t  %8lu\t  %8ld\t  %8ld\r\n", i, m_cacheBin[i].m_size, m_cacheBin[i].m_count,
            m_cacheBin[i].m_size * m_cacheBin[i].m_count);
         efs_write(fd, str, strlen(str));
      }
      snprintf(str, sizeof(str), "      NC\t%10lu\t  %8ld\t  %8ld\r\n", 0xFFFFFFFF, heap_cache_not_cached_peak_count,
         heap_cache_not_cached_peak_usage);
      efs_write(fd, str, strlen(str));
      efs_close(fd);
   }
#else
  MSG_HIGH("FEATURE_MEMUTILS_HEAP_CACHE_USAGE_STATISTICS not defined",0,0,0);
#endif
}


#ifdef FEATURE_MEMUTILS_NEW_DELETE

#undef new
#undef delete

//////////////////////////////////////////////////////////////////////////////////
//
// C++ dynamic memory allocation
//
//////////////////////////////////////////////////////////////////////////////////

// Overload the global operators
/* ===========================================================================
FUNCTION OPERATOR NEW
DESCRIPTION
============================================================================== */
void * operator new(size_t size) 
{
#ifdef FEATURE_MEMUTILS_NEW_DELETE_HEAP_CACHE
   return HeapCache::GetBuffer(size);
#else
#ifdef T_WINNT
   #error code not present
#else
   return malloc(size);
#endif
#endif
}

/* ===========================================================================
FUNCTION OPERATOR NEW[]
DESCRIPTION
============================================================================== */
void * operator new[](size_t size)
{
   return operator new(size);
}

/* ===========================================================================
FUNCTION OPERATOR DELETE
DESCRIPTION
============================================================================== */
void operator delete(void *p) 
{
   if (p != NULL)
   {
#ifdef FEATURE_MEMUTILS_NEW_DELETE_HEAP_CACHE
      HeapCache::ReturnBuffer(p);
#else
#ifdef T_WINNT
      #error code not present
#else
      free(p);
#endif
#endif
   }
}

/* ===========================================================================
FUNCTION OPERATOR DELETE[]
DESCRIPTION
============================================================================== */
void operator delete[](void *p) 
{
   operator delete(p);
}

/* ===========================================================================
FUNCTION OPERATOR NEW
DESCRIPTION
============================================================================== */
void * operator new(size_t size, const char *file, unsigned long line) 
{
#ifdef FEATURE_MEMUTILS_NEW_DELETE_HEAP_CACHE
   return HeapCache::GetBuffer(size, file, line);
#else
   (void) file; /* to supress compiler warning */
   (void) line; /* to supress compiler warning */
#ifdef T_WINNT
   #error code not present
#else
   return malloc(size);
#endif
#endif
}

/* ===========================================================================
FUNCTION OPERATOR NEW[]
DESCRIPTION
============================================================================== */
void * operator new[](size_t size, const char *file, unsigned long line)
{
   return operator new(size, file, line);
}

/* ===========================================================================
FUNCTION OPERATOR DELETE
DESCRIPTION
============================================================================== */
void operator delete(void *p, const char * /*file*/, unsigned long /*line*/) 
{
   operator delete(p);
}

/* ===========================================================================
FUNCTION OPERATOR DELETE[]
DESCRIPTION
============================================================================== */
void operator delete[](void *p, const char *file, unsigned long line) 
{
   operator delete(p, file, line);
}

#endif // FEATURE_MEMUTILS_NEW_DELETE

#if defined(FEATURE_MEMUTILS_MEMORY_TRACKING) || defined(FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING)

#define MEMORY_TRACKER_ARRAY_SIZE 4096
#define DISABLE_MT_ADDRESS 0xFFFFFFFF

typedef struct memory_tracker_struct
{
   unsigned long number;
   unsigned long size;
   unsigned long address;
   unsigned long thread_id;
   unsigned long thread_pri;
   unsigned long used;
   const char *file;
   int line;
   struct memory_tracker_struct *next;
}memory_tracker_type;

memory_tracker_type *memory_tracker_free_list     = NULL;
memory_tracker_type *memory_tracker_used_list     = NULL;
memory_tracker_type *memory_tracker_used_list_end = NULL;
unsigned long memory_tracker_num_instance = 0;
unsigned long memory_tracker_init = 0;
unsigned long memory_tracker_num  = 0;
unsigned long memory_peak_count   = 0;
unsigned long memory_peak_usage   = 0;
unsigned long memory_inst_usage   = 0;
unsigned long memory_inst_count   = 0;
unsigned long memory_init_peak_count = 0;
unsigned long memory_init_peak_usage = 0;
unsigned long memory_init_inst_usage = 0;
unsigned long memory_init_inst_count = 0;
memory_tracker_type memory_array[MEMORY_TRACKER_ARRAY_SIZE] = {0};

#endif /* #if defined(FEATURE_MEMUTILS_MEMORY_TRACKING) || defined(FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING) */
/* ===========================================================================
FUNCTION MEMORY_TRACKER_LEAK_REPORT
DESCRIPTION
============================================================================== */
void memory_tracker_leak_report(void)
{
#ifdef FEATURE_MEMUTILS_MEMORY_TRACKING
   static int memleaks_report_num = 0;
   char fname[25];
   char str[1024];

   memleaks_report_num++;
   snprintf(fname, sizeof(fname), "memleaks%d.txt", memleaks_report_num);
   int fd = efs_open(fname, O_WRONLY | O_TRUNC | O_CREAT, S_IREAD | S_IWRITE);
   if (fd != -1)
   {
      memory_tracker_type *mt;
      unsigned long address;

      snprintf(str, sizeof(str), "                  Usage Count  Usage Bytes  Peak Count   Peak Bytes\r\n"); 
      efs_write(fd, str, strlen(str));
      snprintf(str, sizeof(str), "Starting Statistics: %8ld     %8ld    %8ld    %8ld\r\n", 
         memory_init_inst_count, memory_init_inst_usage, memory_init_peak_count, memory_init_peak_usage);
      efs_write(fd, str, strlen(str));
      snprintf(str, sizeof(str), "Ending Statistics:   %8ld     %8ld    %8ld    %8ld\r\n", 
         memory_inst_count, memory_inst_usage, memory_peak_count, memory_peak_usage);
      efs_write(fd, str, strlen(str));
      snprintf(str, sizeof(str), "  Num\t      Addr\t Size\t TID\tPri\tFile                                     \tLine\r\n");
      efs_write(fd, str, strlen(str));
      INTLOCK()
      // Get first MT object in the list
      mt = memory_tracker_used_list;
      if (mt != NULL)
      {
         address = mt->address;
         // Disable MT object (prevent the Memory Tracker from using this address while we have it)
         mt->address = DISABLE_MT_ADDRESS;
      }
      INTFREE()
      while (mt)
      {
         snprintf(str, sizeof(str), "%5ld\t0x%08lx\t%5ld\t%4ld\t%3ld\t%-40s\t%5d\r\n", 
            mt->number, address, mt->size, mt->thread_id, mt->thread_pri, mt->file, mt->line);
         efs_write(fd, str, strlen(str));
         INTLOCK()
         // Re-enable MT object
         mt->address = address;
         // Get next MT object in the list
         mt = mt->next;
         if (mt != NULL)
         {
            address = mt->address;
            // Disable MT object
            mt->address = DISABLE_MT_ADDRESS;
         }
         INTFREE()
      } 
      efs_close(fd);
   }
#else
  MSG_HIGH("memory_tracker_leak_report(): FEATURE_MEMUTILS_MEMORY_TRACKING "
           "feature NOT enabled",0,0,0);
#endif
}

/* ===========================================================================
FUNCTION MEMORY_TRACKER_INITIALIZE
DESCRIPTION
============================================================================== */
void memory_tracker_initialize(void)
{
#ifdef FEATURE_MEMUTILS_MEMORY_TRACKING
   int i;

   memory_tracker_num = memory_tracker_num_instance * 1000000;

   memory_tracker_free_list = memory_array;
   memory_tracker_init = 1;
   memory_init_inst_usage = memory_inst_usage;
   memory_init_inst_count = memory_inst_count;
   memory_init_peak_count = memory_peak_count;
   memory_init_peak_usage = memory_peak_usage;


   for(i = 0; i < MEMORY_TRACKER_ARRAY_SIZE - 1; i++)
   {
      memory_array[i].next = &memory_array[i + 1];
   }
#else
  MSG_HIGH("memory_tracker_initialize(): FEATURE_MEMUTILS_MEMORY_TRACKING "
           "feature NOT enabled",0,0,0);
#endif
}

/* ===========================================================================
FUNCTION MEMORY_TRACKER_TERMINATE
DESCRIPTION
============================================================================== */
void memory_tracker_terminate(void)
{
#ifdef FEATURE_MEMUTILS_MEMORY_TRACKING
   memory_tracker_num_instance++;
   if (memory_tracker_num_instance == 1)
   {
      memory_tracker_num = memory_tracker_num_instance * 500000;
   }
   else
   {
      memory_tracker_num = memory_tracker_num_instance * 1500000;
   }
#ifdef FEATURE_MEMUTILS_LEAK_REPORT
   memory_tracker_leak_report();
#endif

#else
  MSG_HIGH("memory_tracker_terminate(): FEATURE_MEMUTILS_MEMORY_TRACKING "
           "feature NOT enabled",0,0,0);
#endif
}

/* ===========================================================================
FUNCTION MEMORY_TRACKER_NEW
DESCRIPTION
============================================================================== */
void memory_tracker_new(void *p, long size, const char *file, int line)
{
#ifdef FEATURE_MEMUTILS_MEMORY_TRACKING

#ifdef T_WINNT
   #error code not present
#endif
   if (memory_tracker_init && (unsigned long)p != DISABLE_MT_ADDRESS)
   {
      memory_tracker_type *mt;
      rex_tcb_type *tcb = rex_self();

      INTLOCK()
      mt = memory_tracker_free_list;
      if (mt)
      {
         memory_tracker_free_list = mt->next;
      }
      INTFREE()

      memory_tracker_num++;
      if (mt)
      {
         memory_inst_usage += size;
         memory_inst_count++;
         if (memory_inst_usage > memory_peak_usage)
         {
            memory_peak_usage = memory_inst_usage;
         }
         if (memory_inst_count > memory_peak_count)
         {
            memory_peak_count = memory_inst_count;
         }

         mt->used    = 1;
         mt->address = (unsigned long)p;
         mt->size    = size;
         mt->number  = memory_tracker_num;
         mt->file    = file;
         mt->line    = line;
         mt->thread_id = tcb->thread_id;
         mt->thread_pri = tcb->pri;
         mt->next    = NULL;
         INTLOCK()
         if (memory_tracker_used_list_end != NULL)
         {
            memory_tracker_used_list_end->next = mt;
         }
         memory_tracker_used_list_end = mt;
         if (memory_tracker_used_list == NULL)
         {
            memory_tracker_used_list = mt;
         }
         INTFREE()
      }
   }
#else
  MSG_HIGH("memory_tracker_new(): FEATURE_MEMUTILS_MEMORY_TRACKING "
           "feature NOT enabled",0,0,0);
#endif
}

/* ===========================================================================
FUNCTION MEMORY_TRACKER_DELETE
DESCRIPTION
============================================================================== */
void memory_tracker_delete(void *p)
{
#ifdef FEATURE_MEMUTILS_MEMORY_TRACKING

#ifdef T_WINNT
   #error code not present
#endif
   if (memory_tracker_init && (unsigned long)p != DISABLE_MT_ADDRESS)
   {
      memory_tracker_type *mt = NULL;
      memory_tracker_type *mt_prev = NULL;
      int found = 0;

      INTLOCK()
      mt = memory_tracker_used_list;
      while (mt && found == 0)
      {
         if (mt->address == (unsigned long)p)
         {
            if (mt_prev == NULL)
            {
               memory_tracker_used_list = mt->next;
            }
            else
            {
               mt_prev->next = mt->next;
            }
            if (memory_tracker_used_list_end == mt)
            {
               memory_tracker_used_list_end = mt_prev;
            }
            mt->next = memory_tracker_free_list;
            memory_tracker_free_list = mt;
            found = 1;
         }
         else
         {
            mt_prev = mt;
            mt = mt->next;
         }
      }
      INTFREE()

      if (mt)
      {
         memory_inst_usage -= mt->size;
         memory_inst_count--;
         mt->address = 0;
         mt->size    = 0;
         mt->number  = 0;
         mt->thread_id = 0;
         mt->thread_pri = 0;
      } 
   }
#else
  MSG_HIGH("memory_tracker_delete(): FEATURE_MEMUTILS_MEMORY_TRACKING "
           "feature NOT enabled",0,0,0);
#endif
}

/* ===========================================================================
FUNCTION MEMORY_TRACKER_GET_LOCATION
DESCRIPTION
============================================================================== */
void memory_tracker_get_location(void *p, char **file, unsigned long *line)
{
#ifdef FEATURE_MEMUTILS_MEMORY_TRACKING

   int found = 0;
   *file = NULL;
   *line = 0;
   if (memory_tracker_init)
   {
      memory_tracker_type *mt = NULL;

      INTLOCK()
      mt = memory_tracker_used_list;
      while (mt && found == 0)
      {
         if (mt->address == (unsigned long)p)
         {
            *file = (char *)mt->file;
            *line = mt->line;
            found = 1;
         }
         else
         {
            mt = mt->next;
         }
      }
      INTFREE()
   }
#else
  MSG_HIGH("memory_tracker_get_location(): FEATURE_MEMUTILS_MEMORY_TRACKING "
           "feature NOT enabled",0,0,0);
#endif
   return;
}




