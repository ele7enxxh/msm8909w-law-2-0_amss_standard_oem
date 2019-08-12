/* =========================================================================

DESCRIPTION
  Implementation of a simple sub-allocator to manage memory allocations
  and deallocations using a Next Fit strategy.  

Copyright (c) 1997-2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================ */

/* =========================================================================

                             Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/memheap.c_v   1.2   22 Mar 2002 16:54:42   rajeevg  $
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/services/utils/src/memheap.c#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
08/04/010  sri     Added checks for size variable overflow in mem_malloc()
08/04/009  sri     Added NULL checks where ever applicable
06/01/009  sri     Avoid compiler warnings based on MEMHEAP_ASSERT_NONE
04/14/009  sri     Replaced sprintf, strcat with snprintf, std_strlcat 
                   respectively, memheap_write_data_to_file() is changed to 
                   return void.
03/31/009  sri     qmutex attribute updated for qmutex_create()
09/01/004   gr     Deleted the context argument to the debug versions of
                   the memheap functions; the context is no longer used.
04/06/04    gr     Fixed a bug in the definition of the BEGIN_CRITICAL_SECTION
                   and END_CRITICAL_SECTION macros.
11/25/03    gr     Fixes for compiler warnings when FEATURE_MEM_DEBUG is
                   turned on.
11/18/03    rj     Added check for mem_malloc success before saving header 
                   info.
11/07/03    gr     More debug support.
09/23/03    gr     Added functions to retrieve information about a heap.
09/19/03    gr     Added functionality to store information in flash when
                   an ASSERT fails.
09/17/03    gr     Mainlined FEATURE_HEAP_DEBUG. Added support for storing a
                   filename hash and a line number in the block header.
07/08/03    gr     Simplified mem_heap_task_lock and mem_heap_task_free.
06/26/03    gr     Renamed ASSERT to MEMHEAP_ASSERT. By default, MEMHEAP_ASSERT
                   throws a fatal error and prints out the task id.
06/25/03    gr     Added more ASSERTS. Changed the behavior of
                   FEATURE_MEM_DEBUG to keep just the last n stores and
                   frees. The id of the allocating task is now stored in
                   the info array. Set the block header pad to a non-zero
                   value to try and catch heap corruption.
06/21/03    gr     Added ASSERTS in mem_realloc and mem_free to catch bad
                   pointers passed in.
06/03/03   kjm     Fixed problem in realloc when block is actually grown.
                   If nextBlock is pointing to the free block merged onto
                   the end of the grown block, nextBlock was failing to
                   get updated to point to the following block.
03/11/03    gr     Added support for a heap-specific locking mechanism. This
                   allows some heaps to be restricted to task-context use,
                   and others to be usable from ISRs.
01/20/03   lad     Changed critical section macros to use INTLOCK() instead
                   of TASKLOCK().  Featurized ASSERT calls to optimize.
03/22/02    gr     Corrected a bug in mem_malloc.
10/10/01    gr     BEGIN_CRITICAL_SECTION and END_CRITICAL_SECTION now map
                   to TASKLOCK and TASKFREE, instead of to rex_enter_crit_sect
                   and rex_leave_crit_sect.
10/01/01    gr     Added support for some basic heap statistics.
11/09/00   jct     Revised for coding standard, integrate with REX critical
                   sections
09/09/99   kjm     Added even more ASSERTs to detect corrupt heaps.
04/22/99   kjm     mem_realloc now correctly updates used_bytes, max_used and
                     max_request.
02/17/99   kjm     Added realloc function.  Added support for two
                     variations of heap block headers.  Support backing up
                     and auto-reset when last block is freed to reduce
                     fragmentation.
11/24/98   kjm     Ported to C-only code for use in phone software from
                     original SubAllocator.cp file.
08/22/97   kjm     Original SubAllocator.cp file created.
============================================================================ */


/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#include "memheap.h"
#include <stdio.h>
#include <string.h>
#include "err.h"
#include "AEEstd.h"

#if defined(FEATURE_QUBE)
  #include <qube.h>
#elif defined(FEATURE_WINCE)
  #include "wce_rex.h"
#else 
  #include "rex.h"
#endif

#ifdef FEATURE_MEM_DEBUG
  #include "crc.h"
  #include "msg.h"
#endif

#ifdef FEATURE_WINCE
  #define MEMHEAP_ASSERT_WARN 
#endif /* FEATURE_WINCE */

/* ------------------------------------------------------------------------
** Defines
** ------------------------------------------------------------------------ */

/* NOTUSED */
#define NOTUSED(i) if(i){}

#ifdef FEATURE_QUBE
#define TCB_TYPE              qthread_t
#define TCB_SELF()            qthread_myself()
#define rex_is_in_irq_mode()  FALSE

#ifdef INTLOCK
#undef INTLOCK
#endif /* INTLOCK */
#define INTLOCK()             mem_heap_lock_mutex()

#ifdef INTFREE
#undef INTFREE
#endif /* INTFREE */
#define INTFREE()             mem_heap_unlock_mutex()

/*lint -emacro(506, MEMHEAP_ASSERT) */
/*lint -emacro(506, MEMHEAP_ASSERT_EX) */
#else
#define TCB_TYPE     rex_tcb_type*
#define TCB_SELF()   rex_self()
#endif /* FEATURE_QUBE */

/* By default, MEMHEAP_ASSERT will generate a fatal error.
*/
#ifdef MEMHEAP_ASSERT_NONE
   #define MEMHEAP_ASSERT( xx_exp ) ((void) 0)
   #define MEMHEAP_ASSERT_EX( xx_exp, xx_file, xx_line ) \
      MEMHEAP_ASSERT( xx_exp )
#elif defined( MEMHEAP_ASSERT_WARN )

   #ifndef FEATURE_WINCE

   #define MEMHEAP_ASSERT( xx_exp ) \
     if( !(xx_exp) ) \
     { \
        ERR( "In task 0x%x, Assertion " #xx_exp " failed", \
           (unsigned long) TCB_SELF(), 0, 0 ); \
     }

   #else /* FEATURE_WINCE */

     #define MEMHEAP_ASSERT( xx_exp ) \
       if( !(xx_exp) ) \
       { \
          ERR( "In task 0x%x, Assertion  %d failed", \
             (unsigned long) TCB_SELF(), xx_exp , 0 ); \
       }

   #endif /* FEATURE_WINCE */

   #define MEMHEAP_ASSERT_EX( xx_exp, xx_file, xx_line ) \
      MEMHEAP_ASSERT( xx_exp )
#else
   #ifdef MEMHEAP_ASSERT_LOG
   #include "fs_public.h"
   #else
      #define memheap_log_error(a, b, c)
   #endif /* MEMHEAP_ASSERT_LOG */

   #define MEMHEAP_ASSERT( xx_exp ) \
     if( !(xx_exp) ) \
     { \
        TCB_TYPE  self = TCB_SELF();                             \
        memheap_log_error( #xx_exp, "", 0 ); \
        ERR_FATAL( "In task 0x%x, Assertion " #xx_exp " failed", \
                   (unsigned long) self, 0, 0 );                 \
     }

   #define MEMHEAP_ASSERT_EX( xx_exp, xx_file, xx_line ) \
     if( !(xx_exp) ) \
     { \
        TCB_TYPE  self = TCB_SELF();                             \
        memheap_log_error( #xx_exp, xx_file, xx_line ); \
        ERR_FATAL( "In task 0x%x, Assertion " #xx_exp " failed", \
                   (unsigned long) self, 0, 0 );                 \
     }

   #ifdef MEMHEAP_ASSERT_LOG
   #define MEMHEAP_ASSERT_BUF_SIZE 512
   static char memheap_assert_buffer[MEMHEAP_ASSERT_BUF_SIZE];
   static char temp_buf[40];
   static char *memheap_assert_log_dir  = "memheap_dir";
   static char *memheap_assert_log_file = "memheap_dir/report.txt";
   #endif /* MEMHEAP_ASSERT_LOG */
#endif

/* Code to enter and exit critical sections.
*/
#ifdef FEATURE_MEMHEAP_MT
   #define BEGIN_CRITICAL_SECTION(heap) \
     do { \
       if ((heap)->lock_fnc_ptr) \
         (heap)->lock_fnc_ptr(); \
     } while (0)
   #define END_CRITICAL_SECTION(heap) \
     do { \
       if ((heap)->free_fnc_ptr) \
         (heap)->free_fnc_ptr(); \
     } while (0)
#else
   #define BEGIN_CRITICAL_SECTION(heap) (void) 0
   #define END_CRITICAL_SECTION(heap) (void) 0
#endif

mem_allocator_failed_proc_type mem_allocator_failed_hook = NULL;

static void mem_init_block_header(mem_block_header_type *, unsigned long);
static mem_block_header_type *mem_find_free_block(mem_heap_type*,unsigned long);


#ifdef FEATURE_MEM_DEBUG_EX
   #define MEM_INFO_ARRAY_SIZE    1000
   #define MEM_INFO_FILENAME_SIZE   13
   typedef struct {
      void          *mem_ptr;
      unsigned long  size;
      unsigned long  task_id;
      unsigned short linenum;
      char           filename[MEM_INFO_FILENAME_SIZE];
      unsigned char  freed;
   } mem_info_type;
   static mem_info_type mem_info_array[ MEM_INFO_ARRAY_SIZE ] = { 0 };
#endif

#ifdef FEATURE_MEMHEAP_MT
#ifdef FEATURE_QUBE
static qmutex_t  memheap_mutex = 0;
/* Lock function that works using Qube mutexes.
*/
static void
mem_heap_lock_mutex( void )
{
#ifdef MEMHEAP_ASSERT_NONE
  (void) qmutex_lock( memheap_mutex );
#else /* MEMHEAP_ASSERT_NONE */
  int status = qmutex_lock( memheap_mutex );
  MEMHEAP_ASSERT( status == EOK );
#endif /* MEMHEAP_ASSERT_NONE */
} /* END mem_heap_int_lock */

/* Matching free function for mem_heap_lock_mutex().
*/
static void
mem_heap_unlock_mutex( void )
{
#ifdef MEMHEAP_ASSERT_NONE
  (void) qmutex_unlock( memheap_mutex );
#else /* MEMHEAP_ASSERT_NONE */
  int status = qmutex_unlock( memheap_mutex );
  MEMHEAP_ASSERT( status == EOK );
#endif /* MEMHEAP_ASSERT_NONE */
} /* END mem_heap_int_free */
#else
/* Lock function that works by locking interrupts. A heap that uses this
** lock function can be used by both ISRs and tasks.
*/
static int ints_sav;
static void
mem_heap_int_lock( void )
{
    INTLOCK_SAV(ints_sav); 
} /* END mem_heap_int_lock */

/* Matching free function for mem_heap_int_lock().
*/
static void
mem_heap_int_free( void )
{
    INTFREE_SAV(ints_sav); 
} /* END mem_heap_int_free */


/* Lock function that works by disabling task switching. A heap that uses
** this lock function can only be used in task context.
*/
static void
mem_heap_task_lock( void )
{
   rex_task_lock( );
} /* END mem_heap_task_lock */

/* Matching free function for mem_heap_task_lock().
*/
static void
mem_heap_task_free( void )
{
   rex_task_free( );
} /* END mem_heap_task_free */
#endif /* FEATURE_QUBE */
#endif /* FEATURE_MEMHEAP_MT */

#ifdef FEATURE_QUBE
/*lint -emacro(506,mem_heap_callable_by_isr) */
#define mem_heap_callable_by_isr(h)  FALSE
#else
static int
mem_heap_callable_by_isr( mem_heap_type *heap_ptr )
{
#ifdef FEATURE_MEMHEAP_MT
   if (heap_ptr->lock_fnc_ptr == NULL ||
       heap_ptr->lock_fnc_ptr == mem_heap_int_lock)
      return 1;
   else
      return 0;
#else
   return 1;
#endif
} /* END mem_heap_callable_by_isr */
#endif /* FEATURE_QUBE */

#ifdef FEATURE_MEM_DEBUG_EX
/*===========================================================================
FUNCTION MEM_INFO_ENQUEUE

DESCRIPTION
   Stores information about the current allocation.
===========================================================================*/
static void store_alloc_info(
   void          *mem_ptr,
   unsigned long  size,
   char          *filename,
   unsigned short linenum
)
{
   static int i = 0;

   MEMHEAP_ASSERT(mem_ptr);

   INTLOCK( );

   /* If out of space, overwrite the first entry.
   */

   if (i == MEM_INFO_ARRAY_SIZE)
      i = 0;

   mem_info_array[ i ].mem_ptr = mem_ptr;
   mem_info_array[i].size      = size;
   mem_info_array[i].task_id   = (unsigned long) TCB_SELF( );
   memcpy(
      mem_info_array[ i ].filename,
      filename,
      MEM_INFO_FILENAME_SIZE - 1
   );
   mem_info_array[ i ].filename[MEM_INFO_FILENAME_SIZE-1] = '\0';
   mem_info_array[ i ].linenum = linenum;
   mem_info_array[ i ].freed   = 0;

   i++;

   INTFREE( );

   return;
} /* END store_alloc_info */


/*===========================================================================
FUNCTION STORE_FREE_INFO

DESCRIPTION
   Marks an entry in the allocation info array as freed. 
===========================================================================*/
static void store_free_info(
   void *mem_ptr
)
{
   int i;

   MEMHEAP_ASSERT(mem_ptr);

   INTLOCK( );

   for ( i = 0; i < MEM_INFO_ARRAY_SIZE; i++ )
   {
      if ( mem_info_array[i].mem_ptr == mem_ptr )
      {
         mem_info_array[i].freed = 1;
         break;
      }
   }

   INTFREE( );

   return;
} /* END store_free_info */

#endif /* FEATURE_MEM_DEBUG_EX */

#if !defined(MEMHEAP_ASSERT_NONE) && !defined(MEMHEAP_ASSERT_WARN) && defined(MEMHEAP_ASSERT_LOG)

extern rex_tcb_type dog_tcb;
extern rex_tcb_type fs_tcb;

/*===========================================================================
FUNCTION MEMHEAP_CREATE_ERR_DIR_AND_FILE

DESCRIPTION
   Creates the directory in which to store error information and opens the
   file to which error information will be written. Returns a filehandle if
   
===========================================================================*/
static int
memheap_create_err_dir_and_file( void )
{
#ifdef FEATURE_EFS2
   (void) efs_mkdir( memheap_assert_log_dir, 0666 );
   return efs_open( memheap_assert_log_file, O_CREAT | O_RDWR, 0755 );
#else
   fs_rsp_msg_type fs_rsp;
   fs_open_xparms_type open_params;

   fs_mkdir( memheap_assert_log_dir, NULL, &fs_rsp );

   fs_remove( memheap_assert_log_file, NULL, &fs_rsp );

   open_params.create.cleanup_option   = FS_OC_CLOSE;
   open_params.create.buffering_option = FS_OB_ALLOW;
   open_params.create.attribute_mask   = FS_FA_UNRESTRICTED;
   fs_open( memheap_assert_log_file, FS_OA_CREATE,
      &open_params, NULL, &fs_rsp );
  if ( ( fs_rsp.open.status != FS_OKAY_S ) ||
       ( fs_rsp.open.handle == FS_NULL_HANDLE ) )
     return -1;
  else
     return fs_rsp.open.handle;
#endif
} /* END memheap_create_err_dir_and_file */

/*===========================================================================
FUNCTION MEMHEAP_WRITE_DATA_TO_FILE

DESCRIPTION
   Writes specified data into the file specified using the input fd.
===========================================================================*/
void
memheap_write_data_to_file( int fd, void *buf, unsigned int nbytes )
{
#ifdef FEATURE_EFS2
   (void) efs_write( fd, buf, nbytes );
   (void) efs_close( fd );
#else
   fs_rsp_msg_type fs_rsp;
   fs_write( fd, buf, nbytes, NULL, &fs_rsp );
   fs_close( fd, NULL, &fs_rsp );
#endif
} /* END memheap_write_data_to_file */

/*===========================================================================
FUNCTION MEMHEAP_LOG_ERROR

DESCRIPTION
   Logs an error to flash. Tries to dump out useful information such as the
   current task, part of the current stack, etc.
===========================================================================*/
void
memheap_log_error( const char *assertion, char *filename,
   unsigned int linenum )
{
   int fd;
   char dword_buf[8];
   TCB_TYPE  curr_task;
   unsigned int chars_to_print;
   unsigned int chars_printed;
   char *stack_ptr;
   int i;
   int j;
   char char1;
   char char2;

   if ( rex_is_in_irq_mode( ) )
      return;

   /* Bump up the priority of the Dog task, this task, and the FS task
   ** temporarily so that no other tasks run. After the priorities
   ** have been set, free tasks so that these three tasks can be
   ** scheduled.
   */
   if ( !TASKS_ARE_LOCKED( ) )
      rex_task_lock( );
   if ( dog_tcb.pri < 0xffffffff )
      rex_task_pri( &dog_tcb, dog_tcb.pri + 2 );
   rex_set_pri( dog_tcb.pri - 1 );
   rex_task_pri( &fs_tcb, dog_tcb.pri - 2 );
   while ( TASKS_ARE_LOCKED( ) )
      rex_task_free( );

   fd = memheap_create_err_dir_and_file( );
   if ( fd < 0 )
      return;

   curr_task = TCB_SELF( );
   
   snprintf(memheap_assert_buffer, sizeof(memheap_assert_buffer), 
                                      "Assertion: %s\r\n", assertion);
   snprintf(temp_buf, sizeof(temp_buf), "Filename: %s\r\n", filename);
   std_strlcat(memheap_assert_buffer, temp_buf, sizeof(memheap_assert_buffer));
   snprintf(temp_buf, sizeof(temp_buf), "Line Number: %u\r\n", linenum);
   std_strlcat(memheap_assert_buffer, temp_buf, sizeof(memheap_assert_buffer));
   snprintf(temp_buf, sizeof(temp_buf), 
            "Task Info\r\n  Name: %s\r\n", curr_task->task_name );
   std_strlcat(memheap_assert_buffer, temp_buf, sizeof(memheap_assert_buffer));
   snprintf(temp_buf, sizeof(temp_buf), "  Priority: %u\r\n",
             (unsigned int) curr_task->pri );
   std_strlcat(memheap_assert_buffer, temp_buf, sizeof(memheap_assert_buffer));
   snprintf(temp_buf, sizeof(temp_buf), "  Set sigs: 0x%X\r\n",
                                   (unsigned int) curr_task->sigs );
   std_strlcat(memheap_assert_buffer, temp_buf, sizeof(memheap_assert_buffer));
   std_strlcat(memheap_assert_buffer, "Stack Dump\r\n", sizeof(memheap_assert_buffer));

   chars_printed  = strlen( memheap_assert_buffer );
   chars_to_print = sizeof( memheap_assert_buffer ) -  chars_printed;
   if ( chars_to_print > curr_task->stack_size )
     chars_to_print = curr_task->stack_size;
   chars_to_print = (chars_to_print * 4 ) / 9;
   stack_ptr = (char *) curr_task->stack_limit +
      curr_task->stack_size - chars_to_print;
   while ( (unsigned long) stack_ptr & 0x3 )
      stack_ptr++;

   for ( i = 0; i < chars_to_print; i += 8 )
   {
      for ( j = 7; j >= 0; )
      {
         char1 = *stack_ptr & 0xf;
         if ( char1 >= 0xa )
            char1 += 'A' - 0xa;
         else
            char1 += '0';
         char2 = ( *stack_ptr >> 4 ) & 0xf;
         if ( char2 >= 0xa )
            char2 += 'A' - 0xa;
         else
            char2 += '0';

         dword_buf[j--] = char1;
         dword_buf[j--] = char2;

         stack_ptr++;
      }

      memcpy( &memheap_assert_buffer[chars_printed], dword_buf, 8 );
      chars_printed += 8;

      if ( (i != 0 ) && ( ( i & 0x1f) == 0x00 ) )
      {
         memheap_assert_buffer[chars_printed++] = '\r';
         memheap_assert_buffer[chars_printed++] = '\n';
      }
      else
         memheap_assert_buffer[chars_printed++] = ' ';
   }

   memheap_write_data_to_file( fd, memheap_assert_buffer,
     sizeof( memheap_assert_buffer ) );

} /* END memheap_log_error */

#endif /* !MEMHEAP_ASSERT_NONE && !MEMHEAP_ASSERT_WARN && MEMHEAP_ASSERT_LOG */

/*===========================================================================
FUNCTION MEM_GET_NEXT_BLOCK

DESCRIPTION
  Return the next block header in the heap for the block following the
  given one.  If the given block is the last block in the heap, return
  the first block in the heap.  Never returns NULL.

  Returns a pointer the the memory block header of the block following the given
  block (or the first block of the heap if the given block was the last
  block of the heap).

===========================================================================*/
static mem_block_header_type *mem_get_next_block
(
   const mem_heap_type         *heap_ptr,
     /*  The heap the given block belongs to -- may NOT be NULL!
     */
   const mem_block_header_type *block_ptr
     /*  The block in the heap for which to return the following heap
         block
     */
)
{
   MEMHEAP_ASSERT(block_ptr != NULL);
   MEMHEAP_ASSERT(heap_ptr  != NULL);
   MEMHEAP_ASSERT(block_ptr->pad1 == MEMHEAP_PAD_CHAR);
   return block_ptr->last_flag  ? heap_ptr->first_block
          : (mem_block_header_type *) ((char *) block_ptr + block_ptr->forw_offset);
} /* END mem_get_next_block */


/*===========================================================================
FUNCTION MEM_INIT_HEAP

DESCRIPTION
  Initializes the heap_ptr object and sets up inMemoryChunk for use with the
  heap_ptr object.  inMemoryChunk may be aligned on any boundary.  Beginning
  bytes will be skipped until a paragraph boundary is reached.  Do NOT pass
  in NULL pointers.  infail_fnc_ptr may be NULL in which case no function will
  be called if mem_malloc or mem_calloc is about to fail.  If infail_fnc_ptr
  is provided, it will be called once and then the allocation will be
  attempted again.  See description of my_allocator_failed_proc for details.
  There is no protection for initializing a heap more than once.  If a heap
  is re-initialized, all pointers previously allocated from the heap are
  immediately invalidated and their contents possibly destroyed.  If that's
  the desired behavior, a heap may be initialized more than once.
===========================================================================*/
/*lint -sem(mem_init_heap,1p,2p,2P>=3n) */
void mem_init_heap(
   mem_heap_type                 *heap_ptr,
      /* Statically allocated heap structure
      */
   void                          *heap_mem_ptr,
      /* Pointer to contiguous block of memory used for this heap
      */
   unsigned long                  heap_mem_size,
      /* The size in bytes of the memory pointed to by heap_mem_ptr
      */
   mem_allocator_failed_proc_type fail_fnc_ptr
      /* Function to call when allocation fails, can be NULL
      */
)
{
  char *memory_end_ptr;
    /* 1 beyond computed end of memory passed in to use as heap. 
    */
  char *memory_start_ptr;
    /* The computed beginning of the memory passed in to use as heap.  This
       computed value guarantees the heap actually starts on a paragraph
       boundary.
    */
  unsigned long chunks;
    /* How many whole blocks of size kMinChunkSize fit in the area of
       memory starting at memory_start_ptr and ending at (memory_end_ptr-1)
    */

  MEMHEAP_ASSERT(heap_ptr);
  MEMHEAP_ASSERT(heap_mem_ptr);
  MEMHEAP_ASSERT(heap_mem_size);
  MEMHEAP_ASSERT(heap_mem_size >= (2*kMinChunkSize-1));

#ifdef FEATURE_MEMHEAP_MT
#ifdef FEATURE_QUBE
  /* Initialize the memheap mutex */
  if( memheap_mutex == 0 )
  {
    qmutex_attr_t  qmutexattr;
    qmutexattr.type = QMUTEX_LOCAL;
#ifdef MEMHEAP_ASSERT_NONE
    (void) qmutex_create( &memheap_mutex, &qmutexattr );
#else /* MEMHEAP_ASSERT_NONE */
    int status = qmutex_create( &memheap_mutex, &qmutexattr );
    MEMHEAP_ASSERT( status == EOK );
#endif /* MEMHEAP_ASSERT_NONE */
  }
  heap_ptr->lock_fnc_ptr = mem_heap_lock_mutex;
  heap_ptr->free_fnc_ptr = mem_heap_unlock_mutex;
#else
  heap_ptr->lock_fnc_ptr = mem_heap_task_lock;
  heap_ptr->free_fnc_ptr = mem_heap_task_free;
#endif /* FEATURE_QUBE */
#endif /* FEATURE_MEMHEAP_MT */

  memory_start_ptr = (char *)heap_mem_ptr;
  memory_end_ptr   = memory_start_ptr + heap_mem_size;
  
  /* Advance to the nearest paragraph boundary. This while loop should work
  ** regardless of how many bits are required for address pointers near or
  ** far, etc.
  **
  ** Turn off lint "size incompatibility" warning because cast from pointer
  ** to unsigned long will lose bits, but we don't care because we're only
  ** interested in the least significant bits and we never cast back into a
  ** pointer, so the warning can be safely ignored
  */
  /*lint --e(507)*/
  while( (((unsigned long)memory_start_ptr) & 0x000FUL) )
  { 
     ++memory_start_ptr;
  }
  
  chunks = (unsigned long) ((memory_end_ptr - memory_start_ptr) / kMinChunkSize);

  heap_ptr->first_block            = (mem_block_header_type *) memory_start_ptr;
  heap_ptr->next_block             = heap_ptr->first_block;
  mem_init_block_header(heap_ptr->first_block, chunks * kMinChunkSize);
  heap_ptr->first_block->last_flag = (char) kLastBlock;
  heap_ptr->total_blocks           = 1;
  heap_ptr->max_used               = 0;
  heap_ptr->max_request            = 0;
  heap_ptr->used_bytes             = 0;
  heap_ptr->total_bytes            = chunks * kMinChunkSize;
  heap_ptr->fail_fnc_ptr           = fail_fnc_ptr;
  return;
} /* END mem_heap_init */


/*===========================================================================
FUNCTION MEM_INIT_BLOCK_HEADER

DESCRIPTION
  Initializes a memory block header to control a block of memory in the
  heap.  The header may still need to some of its fields adjusted after
  this call if it will be a used block or the last block in the heap.
===========================================================================*/
void mem_init_block_header(
   mem_block_header_type *block_ptr,
     /* Memory header block to be initialized 
     */
   unsigned long          size
     /* The size of the block of memory controlled by this
        memory header block INCLUDING the size of the
        header block itself
     */
)
{
  MEMHEAP_ASSERT(block_ptr);
  MEMHEAP_ASSERT(size >= sizeof(mem_block_header_type));

  block_ptr->free_flag   = (char) kBlockFree;
  block_ptr->last_flag   = 0;
  block_ptr->pad1        = MEMHEAP_PAD_CHAR;
  block_ptr->extra       = 0;
  block_ptr->forw_offset = size;
#if !FEATURE_HEAP_SMALLER_OVERHEAD
  block_ptr->backOffset = 0;
#endif
  return;
} /* END mem_init_block_header */


/*===========================================================================
FUNCTION MEM_CALLOC

DESCRIPTION
  Allocates enough space for elt_count elements each of elt_size bytes
  from the heap and initializes the space to nul bytes.  If
  heap_ptr is NULL or elt_count or elt_size is 0, the NULL pointer will
  be silently returned.

  Returns a pointer to the newly allocated block, or NULL if the block
  could not be allocated.
===========================================================================*/
/*lint -sem(mem_calloc,1p,2n>=0&&3n>=0&&(@p==0||@P==2n*3n)) */
#ifdef FEATURE_MEM_DEBUG
   void* mem_calloc_debug(
      mem_heap_type *heap_ptr,
         /* Heap from which to allocate
         */
      size_t         elt_count,
         /* Number of elements to allocate
         */
      size_t         elt_size,
         /* Size of each element
         */
      char          *file_name,
         /* Name of file from which mem_calloc was called
         */
      unsigned int   line_number
         /* Line number corresponding to mem_calloc call
         */
   )
#else
   void* mem_calloc(
      mem_heap_type *heap_ptr,
         /* Heap from which to allocate
         */
      size_t         elt_count,
         /* Number of elements to allocate
         */
      size_t         elt_size
         /* Size of each element
         */
   )
#endif
{
  register size_t total_length = elt_count * elt_size;
    /* the computed total length of the allocation request in bytes
    */
  void *answer;
    /* the block of memory to return to satisfy the request
    */

  #ifdef FEATURE_MEM_DEBUG
  MEMHEAP_ASSERT_EX(!rex_is_in_irq_mode() || mem_heap_callable_by_isr(heap_ptr),
     file_name, line_number);
  #else
  MEMHEAP_ASSERT(!rex_is_in_irq_mode() || mem_heap_callable_by_isr(heap_ptr));
  #endif

  if (rex_is_in_irq_mode() && !mem_heap_callable_by_isr(heap_ptr))
     return NULL;

  #ifdef FEATURE_MEM_DEBUG
     answer = mem_malloc_debug(
                 heap_ptr,
                 total_length,
                 file_name,
                 line_number
              );
  #else
     answer = mem_malloc(heap_ptr, total_length);
  #endif

  if( answer )
  {
    /* calloc always returns cleared memory blocks
    */
    memset(answer, 0, total_length);
  }
  return answer;
} /* END mem_calloc */


/*===========================================================================
FUNCTION MEM_MALLOC

DESCRIPTION
  Allocates a block of size bytes from the heap.  If heap_ptr is NULL
  or size is 0, the NULL pointer will be silently returned.

  Returns a pointer to the newly allocated block, or NULL if the block
  could not be allocated.
===========================================================================*/
/*lint -sem(mem_malloc,1p,2n>=0&&(@p==0||@P==2n)) */
#ifdef FEATURE_MEM_DEBUG
   void* mem_malloc_debug(
      mem_heap_type *heap_ptr,
         /* Heap from which to allocate
         */
      size_t         size,
         /* Number of bytes to allocate
         */
      char          *file_name,
         /* Name of file from which mem_malloc was called
         */
      unsigned int   line_number
         /* Line number corresponding to mem_malloc call
         */
   )
#else
   void* mem_malloc(
      mem_heap_type *heap_ptr,
         /* Heap from which to allocate
         */
      size_t         size
         /* Number of bytes to allocate
         */
   )
#endif
{
  register int attempts = 2;
    /* how many times maximum to try to find a block of the requested
       size.  The second attempt is only made if the first attempt
       fails AND an allocator failed proc is called */

  unsigned long chunks;
    /* the computed minimum size of the memory block in chunks needed
       to satisfy the request */

  unsigned long actualSize;
    /* the computed minimum size of the memory block in bytes needed
       to satisfy the request */

  unsigned char bonusBytes;
    /* the computed number of unused bytes at the end of the allocated
       memory block.  Will always be < kMinChunkSize */

  mem_block_header_type *freeBlock = NULL;
    /* the free block found of size >= actualSize */

  void *answer = NULL;
    /* the address of memory to be returned to the caller */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MEM_DEBUG
  MEMHEAP_ASSERT_EX( heap_ptr != NULL, file_name, line_number );
  MEMHEAP_ASSERT_EX(!rex_is_in_irq_mode() || mem_heap_callable_by_isr(heap_ptr),
     file_name, line_number);

  #else
  MEMHEAP_ASSERT(heap_ptr != NULL);
  MEMHEAP_ASSERT(!rex_is_in_irq_mode() || mem_heap_callable_by_isr(heap_ptr));
  #endif

  if (!heap_ptr || !size) return NULL;

  if (rex_is_in_irq_mode() && !mem_heap_callable_by_isr(heap_ptr))
     return NULL;
  
  /* quick check if requested size of memory is available */
  if( (unsigned long) size > heap_ptr->total_bytes ) return NULL;

  /* chunks overflow check : check max memory that can be malloc'd at a time */
  if( (0xFFFFFFFF - (unsigned long) size) < (sizeof(mem_block_header_type) 
                                            + kMinChunkSize - 1) ) return NULL;

  chunks = ((unsigned long) size + sizeof(mem_block_header_type)
            + kMinChunkSize - 1) / kMinChunkSize;
  actualSize = chunks * kMinChunkSize;
  bonusBytes = (unsigned char)
                (actualSize - size - sizeof(mem_block_header_type));

  do {
    /* This odd contortion of a do while loop exists to
       allow definitions for the BEGIN & END CRITICAL macros to
       introduce a new block nesting level.  Furthermore, we don't
       want a critical section active during the allocator failed
       callback */
  
    if (attempts == 1) {
      /* If there's a fail proc, call it now OUTSIDE of any
         critical section and then try the allocation again.
         Otherwise, we fail now */
      
      mem_allocator_failed_proc_type saveProc;
      
      BEGIN_CRITICAL_SECTION(heap_ptr);
      saveProc = heap_ptr->fail_fnc_ptr;
      heap_ptr->fail_fnc_ptr = NULL;
      END_CRITICAL_SECTION(heap_ptr);
      if (saveProc) {
        /* Must call failure procedure -- notice
           that no critical section is active at
           the moment */
        (*saveProc)(heap_ptr, size);
        
        /* Now must restore failure proc */
        BEGIN_CRITICAL_SECTION(heap_ptr);
        heap_ptr->fail_fnc_ptr = saveProc;
        END_CRITICAL_SECTION(heap_ptr);
      }
      else {
        break;  /* don't bother trying to find
                   a free block again because
                   there was no failure proc
                   to call to free up memory */
      }
    }
    
    BEGIN_CRITICAL_SECTION(heap_ptr);
  
    freeBlock = mem_find_free_block(heap_ptr, actualSize);
    if (freeBlock)
    {
      /* split the block (if necessary) and return the new block */
 
      MEMHEAP_ASSERT(freeBlock->forw_offset > 0);

      if (freeBlock->forw_offset >=
          (actualSize + sizeof(mem_block_header_type)))
      {
        /* must split into two free blocks */

#if !FEATURE_HEAP_SMALLER_OVERHEAD
        mem_block_header_type *afterBlock =
                              mem_get_next_block(heap_ptr, freeBlock);
#endif        
        mem_block_header_type *newBlock = (mem_block_header_type *)
                                          ((char *) freeBlock + actualSize);
        mem_init_block_header(newBlock, freeBlock->forw_offset - actualSize);
        newBlock->last_flag = freeBlock->last_flag;
        freeBlock->forw_offset = actualSize;
        freeBlock->last_flag = 0;
        ++heap_ptr->total_blocks;
#if !FEATURE_HEAP_SMALLER_OVERHEAD
        newBlock->backOffset = actualSize;
        if (afterBlock > newBlock) {
          afterBlock->backOffset -= actualSize;
        }
#endif
      }
      
      /*  set up next block to search for
          next allocation request */
      heap_ptr->next_block = mem_get_next_block(heap_ptr, freeBlock);
  
      /* mark the block as used and return it */
  
      freeBlock->free_flag = kBlockUsed;
      freeBlock->extra = bonusBytes;
      heap_ptr->used_bytes += size;
      MEMHEAP_ASSERT(heap_ptr->total_bytes >= heap_ptr->used_bytes);
      
      if (heap_ptr->used_bytes > heap_ptr->max_used) {
        heap_ptr->max_used = heap_ptr->used_bytes;
      }
      if (size > heap_ptr->max_request) {
        heap_ptr->max_request = size;
      }
      answer = (char *) freeBlock + sizeof(mem_block_header_type);
    }
  
    END_CRITICAL_SECTION(heap_ptr);

  } while (--attempts && !answer);

   if (!answer && mem_allocator_failed_hook) {
      (*mem_allocator_failed_hook)(heap_ptr, size);
   }

#ifdef FEATURE_MEM_DEBUG_EX
  store_alloc_info (answer, size, file_name, line_number);
#endif

#ifdef FEATURE_MEM_DEBUG
  if(answer != NULL)
  {
    MEMHEAP_ASSERT( freeBlock != NULL );
    freeBlock->filenamehash =
      (uint16) crc_16_calc( (unsigned char *) file_name, strlen( file_name ) );
    freeBlock->linenum = (uint16) line_number;
  }
#endif

  return answer;
} /* END mem_malloc */


/*===========================================================================
FUNCTION MEM_FREE

DESCRIPTION
  Deallocates the ptr block of memory.  If ptr is NULL, heap_ptr is NULL or
  ptr is outside the range of memory managed by heap_ptr, then this function
  call does nothing (and is guaranteed to be harmless).  This function will
  ASSERT if it can detect an attempt to free an already freed block.  (This
  is not always reliable though, so it might not catch it.)
===========================================================================*/
/*lint -sem(mem_free,1p) */
#ifdef FEATURE_MEM_DEBUG
   void mem_free_debug(
      mem_heap_type *heap_ptr,
         /* Heap from which to allocate
         */
      void          *ptr,
         /* Memory to free
         */
      char          *file_name,
         /* Name of file from which mem_free was called
         */
      unsigned int   line_number
         /* Line number corresponding to mem_free call
         */
   )
#else
   void mem_free(
      mem_heap_type *heap_ptr,
         /* Heap in which to free memory
         */
      void          *ptr
         /* Memory to free
         */
   )
#endif
{
  mem_block_header_type *theBlock;
    /* The computed address of the memory header block in the heap that
       controls the memory referenced by ptr */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MEM_DEBUG
  MEMHEAP_ASSERT_EX( heap_ptr != NULL, file_name, line_number );
  MEMHEAP_ASSERT_EX(ptr != NULL, file_name, line_number);
  MEMHEAP_ASSERT_EX(!rex_is_in_irq_mode() || mem_heap_callable_by_isr(heap_ptr),
     file_name, line_number);
  #else
  MEMHEAP_ASSERT(heap_ptr != NULL);
  MEMHEAP_ASSERT(ptr != NULL);
  MEMHEAP_ASSERT(!rex_is_in_irq_mode() || mem_heap_callable_by_isr(heap_ptr));
  #endif

  if (rex_is_in_irq_mode() && !mem_heap_callable_by_isr(heap_ptr))
     return;

  /* free the passed in block */

  if (!heap_ptr || !ptr) return; /* NULL is a NOP */

  #ifdef FEATURE_MEM_DEBUG
  MEMHEAP_ASSERT_EX( heap_ptr->first_block, file_name, line_number );
  #else
  MEMHEAP_ASSERT(heap_ptr->first_block);
  #endif

  theBlock = (mem_block_header_type *)
                          ((char *) ptr - sizeof(mem_block_header_type));

  #ifdef FEATURE_MEM_DEBUG
  MEMHEAP_ASSERT_EX((theBlock >= heap_ptr->first_block &&
     (char *) ptr < (char *) heap_ptr->first_block + heap_ptr->total_bytes),
     file_name, line_number);
  #else
  MEMHEAP_ASSERT(theBlock >= heap_ptr->first_block &&
         (char *) ptr < (char *) heap_ptr->first_block +
                          heap_ptr->total_bytes);
  #endif

  if (theBlock < heap_ptr->first_block ||
       (char *) ptr >= (char *) heap_ptr->first_block + heap_ptr->total_bytes) {
    return; /* Outside heap memory is NOP */
  }

  /* Try to detect corruption. */
  #ifdef FEATURE_MEM_DEBUG
  MEMHEAP_ASSERT_EX(theBlock->pad1 == MEMHEAP_PAD_CHAR,
     file_name, line_number);
  MEMHEAP_ASSERT_EX(!theBlock->free_flag,
     file_name, line_number);
  MEMHEAP_ASSERT_EX(theBlock->forw_offset >= sizeof(mem_block_header_type),
     file_name, line_number);
  MEMHEAP_ASSERT_EX(theBlock->extra < kMinChunkSize,
     file_name, line_number);
  #else
  MEMHEAP_ASSERT(theBlock->pad1 == MEMHEAP_PAD_CHAR);
  MEMHEAP_ASSERT(!theBlock->free_flag);  /* Attempt to detect multiple
                                            frees of same block */
  /* Make sure forw_offset is reasonable */
  MEMHEAP_ASSERT(theBlock->forw_offset >= sizeof(mem_block_header_type));
  /* Make sure extra is reasonable */
  MEMHEAP_ASSERT(theBlock->extra < kMinChunkSize);
  #endif

  BEGIN_CRITICAL_SECTION(heap_ptr);

  if (!theBlock->free_flag) /* Be intelligent about not munging the heap if
                              a multiple free of the same block is detected */
  {
    mem_block_header_type *next_block;

    #ifdef FEATURE_MEM_DEBUG
    MEMHEAP_ASSERT_EX((theBlock->forw_offset - sizeof(mem_block_header_type)
                                  - theBlock->extra) <= heap_ptr->used_bytes,
       file_name, line_number);
    #else
    MEMHEAP_ASSERT((theBlock->forw_offset - sizeof(mem_block_header_type)
                                  - theBlock->extra) <= heap_ptr->used_bytes);
    #endif
    heap_ptr->used_bytes -= theBlock->forw_offset - sizeof(mem_block_header_type)
                                  - theBlock->extra;
    MEMHEAP_ASSERT(heap_ptr->total_bytes >= heap_ptr->used_bytes);
    
    theBlock->free_flag = (char) kBlockFree;
    
    /* now backup the next pointer if applicable */
    next_block = mem_get_next_block(heap_ptr, theBlock);
    if (next_block == heap_ptr->next_block) {
      /* Backup now to lessen possible fragmentation */
      heap_ptr->next_block = theBlock;
    }
    
    /* reset heap to initial state if everything is now freed */
    if (!heap_ptr->used_bytes) {
	    char reset[] ="reset - heap";
      /* reset heap now, but retain statistics */
      heap_ptr->next_block = heap_ptr->first_block;
      mem_init_block_header(heap_ptr->first_block, heap_ptr->total_bytes);
      heap_ptr->first_block->last_flag = (char) kLastBlock;
      heap_ptr->total_blocks = 1;
#ifdef FEATURE_MEM_DEBUG_EX
      store_alloc_info(heap_ptr,0,reset,0);
#endif
    }
  }

#ifdef FEATURE_MEM_DEBUG_EX
  store_free_info( ptr );
#endif

#ifdef FEATURE_MEM_DEBUG
  theBlock->filenamehash =
    (uint16) crc_16_calc( (unsigned char *) file_name, strlen( file_name ) );
  theBlock->linenum = (uint16) line_number;
#endif

  END_CRITICAL_SECTION(heap_ptr);
} /* END mem_free */


/*===========================================================================
FUNCTION MEM_REALLOC

DESCRIPTION
  Resizes the ptr block of memory to be size bytes while preserving the
  block's contents.  If the block is shortened, bytes are discarded off the
  end.  If the block is lengthened, the new bytes added are not initialized
  and will have garbage values.  If heap_ptr is NULL, the contents of ptr are
  unchanged and the function silently returns NULL.  If ptr is NULL, the
  functions behaves exactly like mem_malloc (see above).  If ptr is not NULL
  and size is 0, the function behaves exactly like mem_free (see below).  If
  the block cannot be resized, ptr is not NULL and size is not 0, then NULL
  is returned and the original block is left untouched.  If the ptr block IS
  successfully resized and the returned value is different from the ptr value
  passed in, the old block passed in must be considered deallocated and no
  longer useable (i.e. do NOT try to mem_free it!)  This function will ASSERT
  if it can detect a bad pointer or a pointer to a free block within the
  range of memory managed by heap_ptr.  However, it's not always possible to
  do this.  Passing in a ptr block that is outside the memory managed by
  heap_ptr will result in a silent return of NULL with the contents of ptr
  unchanged.  This function will call mem_malloc if it can't grow the block
  in place, so the allocater failed function WILL be called if the heap is
  out of room to grow the block.

  Returns a pointer to the beginning of the resized block of memory (which
  may be different than ptr) or NULL if the block cannot be resized.
===========================================================================*/
/*lint -sem(mem_realloc,1p,3n>=0&&(@p==0||@P==3n)) */
#ifdef FEATURE_MEM_DEBUG
   void* mem_realloc_debug(
      mem_heap_type *heap_ptr,
         /* Heap from which to allocate
         */
      void          *ptr,
         /* A block previously allocated from heap_ptr
         */
      size_t         size,
         /* Number of bytes to allocate
         */
      char          *file_name,
         /* Name of file from which mem_realloc was called
         */
      unsigned int   line_number
         /* Line number corresponding to mem_realloc call
         */
   )
#else
   void *mem_realloc(
      mem_heap_type *heap_ptr,
         /* Heap in which to re-allocate
         */
      void          *ptr,
         /* A block previously allocated from heap_ptr 
         */
      size_t         size
         /* New size (in bytes) of the ptr block of memory
         */
   )
#endif
{
  size_t oldSize;
    /* the logical size of the old block passed in */

  unsigned long chunks;
    /* the minimum number of memory chunks needed to satisfy the request */

  unsigned long actualSize;
    /* the size in bytes of the minimum number of chunks needed to satisfy 
       the request */

  unsigned char bonusBytes;
    /* the computed number of unused bytes in a block satisfying the
       new block size request */

  long sizeChange;
    /* a signed value representing the increase (positive value) in block
       size or the decrease (negative value) in block size following a
       successful mem_realloc request -- number is in logical bytes */

  mem_block_header_type *theBlock;
    /* The address of the memory block header controlling the memory
       addressed by the ptr argument */

  mem_block_header_type *followingBlock;
    /* A loop variable used when attempting to collapse adjacent following
       free blocks to satisfy a grow request */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MEM_DEBUG
  MEMHEAP_ASSERT_EX(heap_ptr != NULL, file_name, line_number);
  MEMHEAP_ASSERT_EX(!rex_is_in_irq_mode() || mem_heap_callable_by_isr(heap_ptr),
     file_name, line_number);
  #else
  MEMHEAP_ASSERT(heap_ptr != NULL);
  MEMHEAP_ASSERT(!rex_is_in_irq_mode() || mem_heap_callable_by_isr(heap_ptr));
  #endif

  if (!heap_ptr) return NULL;

  if (rex_is_in_irq_mode() && !mem_heap_callable_by_isr(heap_ptr))
     return NULL;

  /*
   * This is one nasty evil function to implement -- it has
   * to know everything about the heap and tweak it in wierd
   * ways to grow a block.  Ugh.
   */
  
  if (!ptr)
  {
    #ifdef FEATURE_MEM_DEBUG
      return mem_malloc_debug(heap_ptr, size, file_name, line_number);
    #else
      return mem_malloc(heap_ptr, size);
    #endif
  }
  if (!size)
  {
    #ifdef FEATURE_MEM_DEBUG
      mem_free_debug(heap_ptr, ptr, file_name, line_number);
    #else
      mem_free(heap_ptr, ptr);
    #endif
    return NULL;
  }

  #ifdef FEATURE_MEM_DEBUG
  MEMHEAP_ASSERT_EX(heap_ptr->first_block, file_name, line_number);
  #else
  MEMHEAP_ASSERT(heap_ptr->first_block);
  #endif
  theBlock = (mem_block_header_type *)
                    ((char *) ptr - sizeof(mem_block_header_type));

  #ifdef FEATURE_MEM_DEBUG
  MEMHEAP_ASSERT_EX(theBlock >= heap_ptr->first_block &&
         (char *) ptr < (char *) heap_ptr->first_block +
                          heap_ptr->total_bytes,
    file_name, line_number);       
  #else
  MEMHEAP_ASSERT(theBlock >= heap_ptr->first_block &&
         (char *) ptr < (char *) heap_ptr->first_block +
                          heap_ptr->total_bytes);
  #endif

  if (theBlock < heap_ptr->first_block ||
       (char *) ptr >= (char *) heap_ptr->first_block + heap_ptr->total_bytes) {
    return NULL; /* Outside heap memory is NOP */
  }

  /* Attempt to detect a free block passed in */
  #ifdef FEATURE_MEM_DEBUG
  MEMHEAP_ASSERT_EX(!theBlock->free_flag, file_name, line_number);
  #else
  MEMHEAP_ASSERT(!theBlock->free_flag);
  #endif

  if (theBlock->free_flag) return NULL; /* Behave sensibly then */

  oldSize = (size_t) mem_get_block_logical_size(theBlock);
  if (oldSize == size) return ptr; /* No size change, return same pointer */

  sizeChange = (long) size - (long) oldSize;
  chunks = ((unsigned long) size + sizeof(mem_block_header_type)
                                    + kMinChunkSize - 1) / kMinChunkSize;
  actualSize = chunks * kMinChunkSize;
  bonusBytes = (unsigned char)
                      (actualSize - size - sizeof(mem_block_header_type));

  if (theBlock->forw_offset < actualSize) {
    /* We must grow the block.  This can take one
       of two forms:
         1) Grow the block in place if there are
            enough adjacent free block(s).
         2) Malloc a new block, copy the preserved
            bytes into it and free the old block.
       Option (1) is kinda tricky and that code
       is done here.  Option (2) is handled by just
       calling malloc, memcpy, & free */

    void *answer = NULL;
    mem_block_header_type *newBlock, *nextFollowingBlock;
    unsigned long sizeNeeded = actualSize - theBlock->forw_offset;
    
    /* Begin by attempting (1) */
    
    BEGIN_CRITICAL_SECTION(heap_ptr);
    
    /* This code looks a lot like mem_find_free_block, but it's not
       exactly the same */
    
    followingBlock = mem_get_next_block(heap_ptr, theBlock);
    if (followingBlock > theBlock && followingBlock->free_flag) {
      nextFollowingBlock = mem_get_next_block(heap_ptr, followingBlock);
      while (followingBlock->forw_offset < sizeNeeded
             && nextFollowingBlock->free_flag
             && nextFollowingBlock > followingBlock) {
        /* collapse adjacent free blocks into one if it
         * will allow us to satisfy the request
         */
  
#if !FEATURE_HEAP_SMALLER_OVERHEAD
        unsigned long sizeOldBlock = followingBlock->forw_offset;
#endif
        unsigned char fixupNext = heap_ptr->next_block == nextFollowingBlock;
        followingBlock->forw_offset += nextFollowingBlock->forw_offset;
        followingBlock->last_flag = nextFollowingBlock->last_flag;
        --heap_ptr->total_blocks;
        nextFollowingBlock = mem_get_next_block(heap_ptr, followingBlock);
#if !FEATURE_HEAP_SMALLER_OVERHEAD
        if (nextFollowingBlock > followingBlock) {
          nextFollowingBlock->backOffset += sizeOldBlock;
        }
#endif
        if (fixupNext) heap_ptr->next_block = nextFollowingBlock;
      }
      if (followingBlock->forw_offset >= sizeNeeded) {
        unsigned char fixupNext;
#if !FEATURE_HEAP_SMALLER_OVERHEAD
        unsigned long sizeOldBlock;
#endif
        if (followingBlock->forw_offset > sizeNeeded) {
          /* must split free block into two free blocks */
          
          nextFollowingBlock = mem_get_next_block(heap_ptr, followingBlock);
          newBlock = (mem_block_header_type *)
                                  ((char *) followingBlock + sizeNeeded);
          mem_init_block_header(newBlock,
                                followingBlock->forw_offset - sizeNeeded);
          newBlock->last_flag = followingBlock->last_flag;
          followingBlock->forw_offset = sizeNeeded;
          followingBlock->last_flag = 0;
          ++heap_ptr->total_blocks;
#if !FEATURE_HEAP_SMALLER_OVERHEAD
          newBlock->backOffset = sizeNeeded;
          if (nextFollowingBlock > newBlock) {
            nextFollowingBlock->backOffset -= sizeNeeded;
          }
#endif
          if (heap_ptr->next_block == nextFollowingBlock) {
            /* Backup our next fit searching pointer */
            heap_ptr->next_block = newBlock;
          }
        }
        /* Now join the correct size free block onto the end of theBlock */
#if !FEATURE_HEAP_SMALLER_OVERHEAD
        sizeOldBlock = theBlock->forw_offset;
#endif
        theBlock->last_flag = followingBlock->last_flag;
        theBlock->forw_offset += followingBlock->forw_offset;
        --heap_ptr->total_blocks;
        heap_ptr->used_bytes = (unsigned long)
                             ((long) heap_ptr->used_bytes + sizeChange);
	
        MEMHEAP_ASSERT(heap_ptr->total_bytes >= heap_ptr->used_bytes);

        theBlock->extra = bonusBytes;
        fixupNext = (heap_ptr->next_block == followingBlock);
        if (heap_ptr->used_bytes > heap_ptr->max_used) {
          heap_ptr->max_used = heap_ptr->used_bytes;
        }
        if (size > heap_ptr->max_request) {
          heap_ptr->max_request = size;
        }
        followingBlock = mem_get_next_block(heap_ptr, theBlock);
#if !FEATURE_HEAP_SMALLER_OVERHEAD
        if (followingBlock > theBlock) {
          followingBlock->backOffset += sizeOldBlock;
        }
#endif
        if (fixupNext) heap_ptr->next_block = followingBlock;
        MEMHEAP_ASSERT(theBlock->forw_offset == actualSize);
        answer = ptr; /* Trigger return when out of critical section */
      }
    }
    
    END_CRITICAL_SECTION(heap_ptr);
    
    if (answer)
    {
#ifdef FEATURE_MEM_DEBUG
      theBlock->filenamehash =
        (uint16) crc_16_calc( (unsigned char *) file_name, strlen( file_name ) );
      theBlock->linenum = (uint16) line_number;
#endif
      return ptr; /* (1) succeeded, return now */
    }

    /* Must use method (2) */
    
    #ifdef FEATURE_MEM_DEBUG
      answer = mem_malloc_debug(heap_ptr, size, file_name, line_number);
    #else
      answer = mem_malloc(heap_ptr, size);
    #endif
    if (!answer) return NULL; /* No room return NULL, orig. ptr unchanged */
    memcpy(answer, ptr, oldSize);
    #ifdef FEATURE_MEM_DEBUG
      mem_free_debug(heap_ptr, ptr, file_name, line_number);
    #else
      mem_free(heap_ptr, ptr);
    #endif
    return answer; /* Return new block */
  }

  BEGIN_CRITICAL_SECTION(heap_ptr);
  
  if (theBlock->forw_offset > actualSize) {
    /* We must shrink the block down in size which
       means splitting it into two and marking the
       new block as free */
    mem_block_header_type *newBlock;
    
    followingBlock = mem_get_next_block(heap_ptr, theBlock);
    newBlock = (mem_block_header_type *) ((char *) theBlock + actualSize);
    mem_init_block_header(newBlock, theBlock->forw_offset - actualSize);
    newBlock->last_flag = theBlock->last_flag;
    theBlock->forw_offset = actualSize;
    theBlock->last_flag = 0;
    ++heap_ptr->total_blocks;
#if !FEATURE_HEAP_SMALLER_OVERHEAD
    newBlock->backOffset = actualSize;
    if (followingBlock > newBlock) {
      followingBlock->backOffset -= actualSize;
    }
#endif
    if (heap_ptr->next_block == followingBlock) {
      /* Backup our next fit searching pointer */
      heap_ptr->next_block = newBlock;
    }
  }

  heap_ptr->used_bytes = (unsigned long)
                       ((long) heap_ptr->used_bytes + sizeChange);

  MEMHEAP_ASSERT(heap_ptr->total_bytes >= heap_ptr->used_bytes);
  theBlock->extra = bonusBytes;
  if (heap_ptr->used_bytes > heap_ptr->max_used) {
    heap_ptr->max_used = heap_ptr->used_bytes;
  }
  if (size > heap_ptr->max_request) {
    heap_ptr->max_request = size;
  }
  
  END_CRITICAL_SECTION(heap_ptr);
  
#ifdef FEATURE_MEM_DEBUG
  theBlock->filenamehash =
    (uint16) crc_16_calc( (unsigned char *) file_name, strlen( file_name ) );
  theBlock->linenum = (uint16) line_number;
#endif

  return ptr;
} /* END mem_realloc */


/*===========================================================================
FUNCTION MEM_HEAP_USED

DESCRIPTION
  Returns the number of allocated bytes, excluding overhead.
===========================================================================*/
size_t mem_heap_used (mem_heap_type *heap_ptr)
{
  size_t used = 0;
  
  if (heap_ptr != NULL)
  {
    used = heap_ptr->used_bytes;
  }

  return used;
}

/*===========================================================================
FUNCTION MEM_HEAP_OVERHEAD

DESCRIPTION
  Returns the number of bytes of overhead, per allocation, for the heap.
===========================================================================*/
size_t mem_heap_overhead (mem_heap_type *heap_ptr)
{
#ifdef FEATURE_QUBE  
  NOTUSED(heap_ptr);
#endif
  return sizeof (mem_block_header_type);
}

/*===========================================================================
FUNCTION MEM_HEAP_CONSUMED

DESCRIPTION
  Returns the number of bytes consumed by allocation and overhead.
===========================================================================*/
size_t mem_heap_consumed (mem_heap_type *heap_ptr)
{
  size_t consumed_cnt = 0;

  if (heap_ptr)
  {
    consumed_cnt = heap_ptr->used_bytes;

    consumed_cnt += heap_ptr->total_blocks * sizeof (mem_block_header_type);
  }

  return consumed_cnt;
}

/*===========================================================================
FUNCTION MEM_FIND_FREE_BLOCK

DESCRIPTION
  Find a free block of at least inSizeNeeded total bytes.  Collapse
  adjacent free blocks along the way.

  Returns a pointer to a memory block header describing a free block
  of at least inSizeNeeded total bytes.  Returns NULL if no such free
  block exists or can be created by collapsing adjacent free blocks.
===========================================================================*/
mem_block_header_type *mem_find_free_block(
   mem_heap_type *heap_ptr,
     /*  The heap to search for a free block
     */
   unsigned long  size_needed
     /*  The minimum size in bytes of the block needed (this size
         INCLUDES the size of the memory block header itself)
     */
)
{
  long searchBlocks;
    /* The maximum number of blocks to search.  After searching this
       many, we've been through the heap once and the allocation fails
       if we couldn't find/create a satisfactory free block */

  mem_block_header_type *followingBlock;
    /* a loop variable used to walk through the blocks of the heap */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  MEMHEAP_ASSERT(heap_ptr);
  MEMHEAP_ASSERT(heap_ptr->first_block);
        
  searchBlocks = (long) heap_ptr->total_blocks;
#if !FEATURE_HEAP_SMALLER_OVERHEAD
  /* As long as the current & prior block is free,
     backup the next_block pointer to avoid unnecessary
     fragmentation */
  if (heap_ptr->next_block->free_flag) {
    while (heap_ptr->next_block->backOffset) {
      mem_block_header_type *precedingBlock;
      precedingBlock = (mem_block_header_type *)
                ((char *) heap_ptr->next_block - heap_ptr->next_block->backOffset);
      if (!precedingBlock->free_flag) break;
      heap_ptr->next_block = precedingBlock;
    }
  }
#endif
  followingBlock = mem_get_next_block(heap_ptr, heap_ptr->next_block);
  
  for (; searchBlocks > 0; --searchBlocks, heap_ptr->next_block=followingBlock,
                  followingBlock=mem_get_next_block(heap_ptr, heap_ptr->next_block))
  {
    MEMHEAP_ASSERT(heap_ptr->next_block->forw_offset > 0);
    MEMHEAP_ASSERT(followingBlock->forw_offset > 0);

    if (heap_ptr->next_block->free_flag)
    {
      while (heap_ptr->next_block->forw_offset < size_needed
             && followingBlock->free_flag && followingBlock > heap_ptr->next_block)
      {
        /* collapse adjacent free blocks into one if it
         * will allow us to satisfy a request
         */
#if !FEATURE_HEAP_SMALLER_OVERHEAD
        unsigned long sizeOldBlock = heap_ptr->next_block->forw_offset;
#endif
        heap_ptr->next_block->forw_offset += followingBlock->forw_offset;
        heap_ptr->next_block->last_flag = followingBlock->last_flag;
        --heap_ptr->total_blocks;
        --searchBlocks; /* don't search more than we need */
        followingBlock = mem_get_next_block(heap_ptr, heap_ptr->next_block);
#if !FEATURE_HEAP_SMALLER_OVERHEAD
        if (followingBlock > heap_ptr->next_block) {
          followingBlock->backOffset += sizeOldBlock;
        }
#endif
      }
      if (heap_ptr->next_block->forw_offset >= size_needed) {
        return heap_ptr->next_block;
      }
    }
  }
  return 0;       /* didn't find anything */
} /* END mem_find_free_block */


/*===========================================================================
FUNCTION MEM_GET_BLOCK_LOGICAL_SIZE

DESCRIPTION
  Returns the logical block size of a memory block in the heap

  Returns the size (in bytes) of the block excluding block header overhead and
  unused bytes at the end of the block.
===========================================================================*/
/*lint -sem(mem_get_block_logical_size,1p) */
unsigned long mem_get_block_logical_size(
   const mem_block_header_type *block_hdr_ptr
     /*  A memory block to get the logical size of
     */
)
{
  MEMHEAP_ASSERT(block_hdr_ptr);
  return block_hdr_ptr->forw_offset - sizeof(mem_block_header_type)
            - (block_hdr_ptr->free_flag ? 0 : block_hdr_ptr->extra);
} /* mem_get_block_logical_size */

/* ------------------------------------------------------------------------
** 
** Routines that return information about the heap
**
** ------------------------------------------------------------------------ */

/*===========================================================================
FUNCTION MEM_HEAP_BLOCK_ITERATOR_INIT

DESCRIPTION
  Initializes an iterator structure to be used for walking the blocks
  in the specified heap.
===========================================================================*/
void mem_heap_block_iterator_init(
   mem_heap_block_iterator_type *block_iter_ptr,
     /*  The Iterator structure to be initialized
     */
   const mem_heap_type          *heap_ptr
     /*  The heap this iterator should iterate over
     */
)
{
  MEMHEAP_ASSERT(block_iter_ptr);
  MEMHEAP_ASSERT(heap_ptr);

  block_iter_ptr->mAllocator = heap_ptr;
  mem_heap_block_iterator_reset(block_iter_ptr);
} /* mem_heap_block_iterator_init */


/*===========================================================================
FUNCTION MEM_HEAP_BLOCK_ITERATOR_RESET

DESCRIPTION
  Reset a heap block iterator to begin the iteration of heap blocks from
  the beginning.
===========================================================================*/
void mem_heap_block_iterator_reset(
   mem_heap_block_iterator_type *block_iter_ptr
     /* The iterator structure to be reset
     */
)
{
  MEMHEAP_ASSERT(block_iter_ptr);
  block_iter_ptr->mBlock = NULL;
} /* mem_heap_block_iterator_reset */


/*===========================================================================
FUNCTION MEM_HEAP_BLOCK_ITERATOR_NEXT

DESCRIPTION
  Return a memory block header for the next block (or next virtual block)
  in the heap being iterated.  Description is only valid if the function
  returns a non-zero value.

  The heap being iterated should not be changed during the iteration as the
  iterator may get lost or return bogus values.  There is no protection
  against this however.

  Returns:
      zero      There are no more heap blocks to return
      non-zero  The next heap block was returned in outBlock successfully
===========================================================================*/
int mem_heap_block_iterator_next(
   mem_heap_block_iterator_type *block_iter_ptr,
     /* Iterator to return next iterated block from
     */
   mem_block_header_type        *out_block_ptr,
     /* Pointer to a location where a copy of a memory block header
        is placed describing the returned memory block.  Only
        valid if the function returns a non-zero value.
     */
   int                           fake_free_coalesced
     /* Input flag.  zero => return the next block header from the heap
        exactly as it is.  non-zero => return the next virtual block
        header from the heap.  Virtual block headers are created by
        collapsing all adjacent free blocks into one.
     */
)
{
  int ans;
    /* Holds function result */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MEMHEAP_ASSERT(block_iter_ptr);
  MEMHEAP_ASSERT(out_block_ptr);
  
  ans = 0;
  if (!block_iter_ptr->mBlock)
  {
    block_iter_ptr->mBlock = block_iter_ptr->mAllocator->first_block;
    if (block_iter_ptr->mBlock) {
      ans = 1;
    }
  }
  else if (!block_iter_ptr->mBlock->last_flag)
  {
    block_iter_ptr->mBlock = mem_get_next_block(block_iter_ptr->mAllocator, block_iter_ptr->mBlock);
    ans = 1;
  }
  memset(out_block_ptr, 0, sizeof(mem_block_header_type));
  if (ans)
  {
    *out_block_ptr = *block_iter_ptr->mBlock;
    if (fake_free_coalesced && out_block_ptr->free_flag)
    {
      /* Pretend as though adjacent free blocks were coalesced */
      for (;;)
      {
        mem_block_header_type *next_block;
        if (block_iter_ptr->mBlock->last_flag) break;
        next_block = mem_get_next_block(block_iter_ptr->mAllocator, block_iter_ptr->mBlock);
        if (!next_block->free_flag) break;
        block_iter_ptr->mBlock = next_block;
        out_block_ptr->forw_offset += block_iter_ptr->mBlock->forw_offset;
      }
    }
  }
  return ans;
} /* mem_heap_block_iterator_next */


/*===========================================================================
FUNCTION MEM_HEAP_GET_TOTALS

DESCRIPTION
  Returns heap totals for the heap.  This function IS thread safe.
  Heap must already have been initialized.
  Return totals for the heap.
===========================================================================*/
void mem_heap_get_totals(
   mem_heap_type  *heap_ptr,
      /* The heap to return totals for
      */
   mem_heap_totals_type *totals
      /* A pointer to a totals structure to receive the various
         heap information
      */
)
{
  mem_heap_block_iterator_type heapIter;
    /* heap iterator used to collect heap statistics */

  mem_block_header_type theBlock;
    /* holds info returned by heap iterator */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  MEMHEAP_ASSERT(heap_ptr);
  MEMHEAP_ASSERT(totals);

  BEGIN_CRITICAL_SECTION(heap_ptr);
  memset(totals, 0, sizeof(*totals));
  mem_heap_block_iterator_init(&heapIter, heap_ptr);
  totals->max_logical_used = heap_ptr->max_used;
  totals->max_logical_request = heap_ptr->max_request;
  while (mem_heap_block_iterator_next(&heapIter, &theBlock, 1))
  {
    unsigned long blkSize = mem_get_block_logical_size(&theBlock);
    ++totals->current_block_count;
    totals->header_bytes += sizeof(mem_block_header_type);
    if (theBlock.free_flag)
    {
      totals->free_bytes += blkSize;
      if (blkSize > totals->largest_free_block)
        totals->largest_free_block = blkSize;
    }
    else
    {
      ++totals->used_blocks;
      totals->used_bytes += blkSize;
      totals->wasted_bytes += theBlock.extra;
    }
  }
  totals->total_physical_bytes = totals->free_bytes
                                    + totals->used_bytes
                                    + totals->wasted_bytes
                                    + totals->header_bytes;
  MEMHEAP_ASSERT(totals->used_bytes == heap_ptr->used_bytes);
  MEMHEAP_ASSERT(totals->current_block_count >= 1);
  MEMHEAP_ASSERT(totals->current_block_count <= heap_ptr->total_blocks);
  MEMHEAP_ASSERT(totals->total_physical_bytes == heap_ptr->total_bytes);
  END_CRITICAL_SECTION(heap_ptr);
} /* mem_heap_get_totals */


/*===========================================================================
FUNCTION MEM_HEAP_SET_INT_LOCK

DESCRIPTION
  This function sets up the specified heap to use interrupt locking and 
  freeing at its locking mechanism.
  Returns 1 on success and 0 on failure.
===========================================================================*/
#ifdef FEATURE_QUBE
int
mem_heap_set_int_lock( mem_heap_type *heap_ptr )
{
  NOTUSED(heap_ptr);
  return 0;
}
#else
int
mem_heap_set_int_lock( mem_heap_type *heap_ptr )
{
   MEMHEAP_ASSERT (heap_ptr != NULL);

#ifdef FEATURE_MEMHEAP_MT
   if ( heap_ptr != NULL )
   {
      heap_ptr->lock_fnc_ptr = mem_heap_int_lock;
      heap_ptr->free_fnc_ptr = mem_heap_int_free;
      return 1;
   }
   else
      return 0;
#else
   return 1;
#endif
} /* END mem_heap_set_int_lock */
#endif /* FEATURE_QUBE */

/*===========================================================================
FUNCTION MEM_HEAP_SET_TASK_LOCK

DESCRIPTION
  This function sets up the specified heap to use task locking and 
  freeing at its locking mechanism.
  Returns 1 on success and 0 on failure.
===========================================================================*/
#ifdef FEATURE_QUBE
int
mem_heap_set_task_lock( mem_heap_type *heap_ptr )
{
  NOTUSED(heap_ptr);
  return 0;
}
#else
int
mem_heap_set_task_lock( mem_heap_type *heap_ptr )
{
   MEMHEAP_ASSERT (heap_ptr != NULL);

#ifdef FEATURE_MEMHEAP_MT
   if ( heap_ptr != NULL )
   {
      heap_ptr->lock_fnc_ptr = mem_heap_task_lock;
      heap_ptr->free_fnc_ptr = mem_heap_task_free;
      return 1;
   }
   else
      return 0;
#else
   return 1;
#endif
} /* END mem_heap_set_task_lock */
#endif /* FEATURE_QUBE */

/*===========================================================================
FUNCTION MEM_HEAP_SET_NO_LOCK

DESCRIPTION
  This function sets up the specified heap to use no locking. This implies
  that this heap should only be used from the context of one task.
  Returns 1 on success and 0 on failure.
===========================================================================*/
int
mem_heap_set_no_lock( mem_heap_type *heap_ptr )
{
   MEMHEAP_ASSERT (heap_ptr != NULL);

#ifdef FEATURE_MEMHEAP_MT
   if ( heap_ptr != NULL )
   {
      heap_ptr->lock_fnc_ptr = NULL;
      heap_ptr->free_fnc_ptr = NULL;
      return 1;
   }
   else
      return 0;
#else
   return 1;
#endif
} /* END mem_heap_set_no_lock */


/*===========================================================================
FUNCTION MEM_HEAP_GET_FREEBLOCK_INFO

DESCRIPTION
  Returns information about the sizes of the largest n free blocks in the
  heap.
  The heap must already have been initialized. The buffer passed in must
  have enough space to hold the requested information.
===========================================================================*/
void
mem_heap_get_freeblock_info(
  mem_heap_type *heap_ptr,
  unsigned int   num_blocks,
  unsigned int  *buf
)
{
  mem_heap_block_iterator_type heap_iter;
  mem_block_header_type the_block;
  unsigned int blk_size;
  unsigned int min_blk_size;
  unsigned int min_blk_index;
  unsigned int i;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  MEMHEAP_ASSERT( heap_ptr );
  MEMHEAP_ASSERT( buf );
  MEMHEAP_ASSERT( num_blocks >= 1 );

  BEGIN_CRITICAL_SECTION( heap_ptr );

  memset( buf, 0, num_blocks * sizeof( int ) );

  mem_heap_block_iterator_init( &heap_iter, heap_ptr );

  while ( mem_heap_block_iterator_next( &heap_iter, &the_block, 1 ) )
  {
    if ( the_block.free_flag )
    {
      blk_size = mem_get_block_logical_size( &the_block );
      min_blk_size  = buf[0];
      min_blk_index = 0;
      for ( i = 1; i < num_blocks; i++ )
      {
        if ( min_blk_size > buf[i] )
        {
          min_blk_size  = buf[i];
          min_blk_index = i;
        }
      }
      if ( blk_size > min_blk_size )
        buf[min_blk_index] = blk_size;
    }
  }
  END_CRITICAL_SECTION(heap_ptr);
} /* mem_heap_get_freeblock_info */


#ifdef FEATURE_MEM_DEBUG

/*===========================================================================
FUNCTION MEM_HEAP_GET_ALLOC_INFO

DESCRIPTION
  Returns information about the sizes of the first n allocated blocks in the
  heap.
  The heap must already have been initialized. The buffer passed in must
  have enough space to hold the requested information.
===========================================================================*/
void
mem_heap_get_alloc_info(
  mem_heap_type            *heap_ptr,
  unsigned int              num_blocks,
  mem_heap_alloc_info_type *buf
)
{
  mem_heap_block_iterator_type heap_iter;
  mem_block_header_type the_block;
  char *alloc_ptr;
  uint32 i = 0;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  MEMHEAP_ASSERT( heap_ptr );
  MEMHEAP_ASSERT( buf );
  MEMHEAP_ASSERT( num_blocks >= 1 );

  BEGIN_CRITICAL_SECTION( heap_ptr );

  memset( buf, 0, num_blocks * sizeof( mem_heap_alloc_info_type ) );

  mem_heap_block_iterator_init( &heap_iter, heap_ptr );

  alloc_ptr = (char *) heap_ptr->first_block;
  
  while ( mem_heap_block_iterator_next( &heap_iter, &the_block, 1 ) )
  {
    if ((i >= num_blocks) || (the_block.forw_offset == 0))
      break;

    if ( !the_block.free_flag )
    {
      buf[i].ptr                = alloc_ptr;    
      buf[i].size               = mem_get_block_logical_size( &the_block );
      buf[i].filenamehash = the_block.filenamehash;
      buf[i].linenum         = the_block.linenum;
       i++;
    }

   alloc_ptr +=  the_block.forw_offset; 
  }

  END_CRITICAL_SECTION(heap_ptr);
} /* mem_heap_get_freeblock_info */

#endif /* FEATURE_MEM_DEBUG */

/*===========================================================================
FUNCTION MEM_HEAP_GET_CHUNK_SIZE

DESCRIPTION
  Returns the minimum chunk size used for allocations from a heap. All
  allocations from a specific heap are multiples of this size.
===========================================================================*/
unsigned int
mem_heap_get_chunk_size( mem_heap_type *heap_ptr )
{
#ifdef FEATURE_QUBE  
  NOTUSED(heap_ptr);
#endif
  return (unsigned int) kMinChunkSize;
} /* END mem_heap_get_chunk_size */

