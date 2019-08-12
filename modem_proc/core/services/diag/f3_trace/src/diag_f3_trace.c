/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Saved F3 message retrieval support

GENERAL DESCRIPTION
  Contains handlers to support F3 message trace retrieval from the EFS.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  No requirements

Copyright (c) 2004-2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                        EDIT HISTORY FOR MODULEW
$Header: //components/rel/core.mpss/3.5.c12.3/services/diag/f3_trace/src/diag_f3_trace.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/18/16   nk      Made changes in F3 trace logic for extended msgs
06/18/15   sa      Added support for time sync feature.
02/18/13   ph      Resolved concurrent case where index goes out of bounds.
12/19/13   rh      Save message args individually instead of as a group
11/19/13   rh      Added tracking for current source of F3 Trace settings 
11/18/13   ph      Remove intermediate buffer in F3 Trace as part of optimization.
10/25/13   ph      Removed memset in diag_f3_trace_buffer_init() to reduce
                    init time.
06/18/13   ph      Added new F3 mask check API.
05/29/13   rh      Actually handle DIAG_F3_TRACE_QSHRINK and _QSHRINK20 again
05/02/13   rh      QShrink 3.0
04/04/13   rs      Removed call to err_data_is_valid.
04/03/13   sr      Replace memcpy() and memmove() with memscpy() and memsmove()
02/25/13   sa      Added support for saving custom F3s in trace buffer
12/21/12   rh      Renamed several err_f3_trace_ types to diag_f3_trace_
12/19/12   rh      Moved file to Services; Added Diag command config option
08/22/12   mcg     File renamed to diag_f3_trace.c; APIs and variables
                   renamed accordingly.
12/14/09   tbg     File renamed to err_f3_trace.c; APIs and variables
                   renamed accordingly.
11/30/09   ps      Fixed compiler warnings & added F3trace support(SW).
09/09/09   jwh     Add FEATURE_DOGTEST_TASK code.
09/01/09   JV      Changes to migrate from EFS1 to EFS2 APIs.
06/11/09   JV      Introduced the OS abstraction layer for rex.
06/05/09   as      Reverted changes from EFS2 to EFS1 Api's
05/07/09   vk      changes made to move from EFS1 to EFS2 apis
05/07/09   vk      changed includes from external to internal headers
10/03/08   vg      Updated code to use PACK() vs. PACKED
04/24/08   tbg     Added method to verify logs being loaded from EFS match
                   the build currently loaded on the phone.
05/30/06   tbg     Added error check in err_f3_trace_write_to_efs()
10/31/05   as      Fixed lint errors.
10/04/05   tbg     Added ability to turn on F3 trace saving by default,
                   if FEATURE_SAVE_TRACE_ON_BY_DEFAULT is defined
08/15/05   eav     Removed calls to MSG_ERROR in run-time situations.
                   pop_oldest_record will return an error.  Upon error,
                   reinitialize the function
08/08/05   eav     Replaced ERR_FATAL with MSG_ERROR.  Do more checking of
                   buffer before saving to the EFS.
04/18/05   as      Fixed critical lint errors.
03/23/05   eav     Added FEATURE_LOG_FUNCTIONS.  Under this feature a NV item
                   will be read.  If this item is enabled, the function calls
                   will be logged in a RAM buffer.  This saving is handled in
                   rex.
02/25/04   as      Added featurization around msg_get_num_args()
02/18/04   as      Fixed critical lint errors.
11/30/04   as      Added FEATURE_DIAG_DEBUG_INVOKE_CRASH. This feature resets
                   the phone automatically without user intervention if the
                   phone encounters a critical failure
10/08/04   eav     Cross reference table now dynamically allocated and stored
                   in efs file.
08/26/04   eav     Added optimizations for saving the timestamps.  Now saving
                   only the delta between timestamps, instead of the
                   absolute timestamp.
08/25/04   eav     Added support for multiple EFS file handling.
08/12/04   eav     Fixed compilation errors if FEATURE_ERR_EXTENDED_STORE is
                   not defined
08/03/04   tbg     Added call to err_data_is_valid() to see if any errors
                   have been logged.
07/13/04   eav     Cleaned up Lint errors
05/12/04   eav     Added event logging support.  Made function calls more
                   generic.  Added mechanism to combine the event trace
                   and F3 trace into one EFS file.  Added
                   err_f3_trace_to_file to save all types into the
                   same RAM buffer.
04/01/04   eav     File created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

// FEATURE_ERR_F3_TRACE_STANDALONE -- MEANS NO DEPENDENCY TO ERR INTERNALS

#include "core_variation.h"
#include "comdef.h"
#include "stringl.h"
#include "osal.h"
#include "diagtarget.h"
#include "diag_f3_tracei.h" // internal
#include "err.h"
#include "msg.h"
#include "diag_f3_trace_cmd.h" // internal

#if !defined(FEATURE_ERR_F3_TRACE_STANDALONE)
#include "mobile.h"
#if defined(ERR_F3_TRACE_TO_SMEM)
#include "smem.h"
#include "smem_type.h"
#endif  //ERR_F3_TRACE_TO_SMEM
#endif    /* !FEATURE_ERR_F3_TRACE_STANDALONE */

//#include "diagcmd.h"
#include "stdlib.h"
#include "stdio.h"

#if defined(FEATURE_ERR_F3_TRACE_STANDALONE)
#include "time_svc.h"
#endif    /* !FEATURE_ERR_F3_TRACE_STANDALONE */

#define ERR_MALLOC(x)  malloc(x)
#define ERR_FREE(x)    free(x)

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#define DIAG_F3_TRACE_GET_TYPE(x)     ((x) >> 13)
#define DIAG_F3_TRACE_MSG_NUM_ARGS(x) (((x) >> 8) & 0x1F)
#define DIAG_F3_TRACE_MSG_TYPE(x) (((x) >> 4) & 0xF)
#define DIAG_F3_TRACE_BYTE_SIZED_ARGS(x) (((x) & 0xC) >> 2)
#define DIAG_F3_TRACE_GET_DETAILS(x)  ((x) & 0xF)
#define DIAG_F3_TRACE_IS_HEADER_VALID(x) ((x)==DIAG_F3_TRACE_VALID_HEADER_FLAG)

// MIGRATION NOTE: recover_f3.cmm depends upon err_f3_trace_wrap_flag
uint8                   err_f3_trace_wrap_flag;
// MIGRATION NOTE: recover_f3.cmm depends upon err_f3_trace_buffer
uint8                   err_f3_trace_buffer[DIAG_F3_TRACE_BUFFER_SIZE];
// MIGRATION NOTE: recover_f3.cmm depends upon err_f3_trace_buffer_head
uint32                  err_f3_trace_buffer_head;
err_f3_trace_id_type    err_f3_trace_buffer_id;

typedef struct
{
   void* const_data_ptr; /* can be ptr to msg_const_type or msg_qsr_const_type */
   diag_f3_trace_tstamp_type tstamp;

} msg_debug_store_type;

/* From msg.c */
extern uint32 msg_v3_ss_num_table_entries;
extern msg_v3_ssid_table_type *msg_v3_ss_table ;

static void diag_f3_trace_to_buffer(uint32, uint8*, int); // forward reference

#if defined(ERR_F3_TRACE_TO_SMEM)
static uint32 diag_f3_trace_find_next_entry(uint32 *); // forward reference
static int diag_f3_trace_calculate_entry_size(uint32); // forward reference
static uint32 diag_f3_trace_preprocess_buffer(uint32*); // forward reference
static void diag_f3_trace_from_buffer(uint8*, uint32, int); // forward reference
static char* diag_f3_trace_smem_log_buf; /* Pointer to buffer used for SMEM f3 log */
#endif

static boolean diag_f3_trace_buffer_init(void); // forward reference

static int diag_f3_trace_buffer_initialized = FALSE;
static int diag_f3_trace_buffer_init_begin = FALSE;

/* Mask to save F3s in the trace buffer.
  Only the Legacy F3s will be saved by default based on the default detail mask value */
static uint32 diag_f3_trace_mask = (DIAG_F3_TRACE_DETAIL_MASK_DEFAULT_VAL >> 3);

// these can be changed before diag_f3_trace_init runs, so defaults need to be
// statically initialized
static uint8 diag_f3_trace_control_mask = DIAG_F3_TRACE_CONTROL_MASK_DEFAULT_VAL;
static uint8 diag_f3_trace_detail_mask = DIAG_F3_TRACE_DETAIL_MASK_DEFAULT_VAL;

//Tracking variable for current mask source
uint32 diag_f3_trace_config_source = DIAG_F3_TRACE_CONFIG_DEFAULT;
/*Bit mask for the custom msg levels
  All custom masks are enabled by default but custom F3s will be saved in the
  f3 trace only if the reserved bit is set in Diag Debug Detail NV Item */

static uint32 diag_f3_trace_custom_mask = 0xFFFFFFE0;
extern uint8 msg_enabled_mask;  /*extern from msg.c */

extern boolean diag_time_initialized; /* extern from diag.c */

/*===========================================================================

FUNCTION DIAG_F3_TRACE_TO_BUFFER

DESCRIPTION
  Saves the buffer to the RAM buffer containing trace information.

DEPENDENCIES
  Interrupts must be locked.  FEATURE_ERR_EXTENDED_STORE must be defined.

RETURN VALUE
  If < 0, failure occurred.  Otherwise, success.

SIDE EFFECTS
  None
===========================================================================*/
static void diag_f3_trace_to_buffer(uint32 index, uint8* src, int length)
{
   int i;

   if (index >= DIAG_F3_TRACE_BUFFER_SIZE)
   {
      index-=DIAG_F3_TRACE_BUFFER_SIZE;
   }

   /* Save to the circular buffer according to where the head points */
   for (i = 0; i < length; i++)
   {
      err_f3_trace_buffer[index] = src[i];
      index++;
      if (index >= DIAG_F3_TRACE_BUFFER_SIZE)
      {
         index-=DIAG_F3_TRACE_BUFFER_SIZE;
      }
   }

   return;
}

/*===========================================================================

FUNCTION DIAG_F3_TRACE_BUFFER_INIT

DESCRIPTION
  Initializes the circular buffer indices.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean diag_f3_trace_buffer_init(void)
{
   /* Grab init_begin flag to make sure only one entity initializes the buffer */
   if (!osal_atomic_compare_and_set((osal_atomic_word_t*)&diag_f3_trace_buffer_init_begin, FALSE, TRUE))
      return(FALSE);

   /* Set up the buffer indices */
   err_f3_trace_buffer_head = 0;
   err_f3_trace_wrap_flag = FALSE;

#if !defined(FEATURE_ERR_F3_TRACE_STANDALONE)
   /* Initialize build ID associated with this run */
   /* (timestamp set when file is saved) */

   err_f3_trace_buffer_id.timestamp=0;
   err_f3_trace_buffer_id.buffer_ptr = (uint32)(&err_f3_trace_buffer_id);
   memset(err_f3_trace_buffer_id.build,0,DIAG_F3_TRACE_MAX_ID_LENGTH);
   strlcpy((char*)err_f3_trace_buffer_id.build,(char*)mob_sw_rev,
           DIAG_F3_TRACE_MAX_ID_LENGTH);
#endif /* FEATURE_ERR_F3_TRACE_STANDALONE */

   /* Once initialized to true, the trace records will be allowed to save */
   diag_f3_trace_buffer_initialized = TRUE;

   return(TRUE);
}

/*===========================================================================

FUNCTION DIAG_F3_TRACE_INIT

DESCRIPTION
  Initialization function for this module.  Registers the packet handlers.
  Reads the diag_debug NV Item to set the run time mask.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void diag_f3_trace_init(void)
{

#if !defined(FEATURE_ERR_F3_TRACE_STANDALONE)


#if defined(ERR_F3_TRACE_TO_SMEM)
   /* SMEM Trace log size needs:
        size of err_f3_trace_record_count (for # of records)
     +
     DIAG_F3_TRACE_BUFFER_SIZE (binary data)
    */
   diag_f3_trace_smem_log_buf =
                              (char* )smem_alloc(SMEM_ERR_F3_TRACE_LOG, sizeof(uint32)+DIAG_F3_TRACE_BUFFER_SIZE);

   //register the function with err
   err_crash_cb_register(diag_f3_trace_save_to_smem);
#endif /* ERR_F3_TRACE_TO_SMEM */

#endif /* if !FEATURE_ERR_F3_TRACE_STANDALONE */

   if (!diag_f3_trace_buffer_initialized)
   {
      /* Initialize the buffer */
      diag_f3_trace_buffer_init();
   }

   /* Initialize the time API to use if not initialized by this time */
   if(!diag_time_initialized)
   {
      diag_time_init();
   }
   

   diag_f3_trace_cmd_reg();
} /* END diag_f3_trace_init */

#if defined(ERR_F3_TRACE_TO_SMEM)
/*===========================================================================

FUNCTION DIAG_F3_TRACE_FIND_NEXT_ENTRY

DESCRIPTION
  Locates next valid entry from the designated offset within the buffer,
  based on the header_flag.

===========================================================================*/
static uint32 diag_f3_trace_find_next_entry(uint32 *buffer_position)
{
   /* start at designated offset,
      locate first valid header in buffer going forward.
      Return that value.
    */

   uint32 start_position = *buffer_position;
   uint32 current_position;
   diag_f3_trace_hdr_type record_hdr;
   boolean found_valid_flag=FALSE;

   current_position = *buffer_position;

   do
   {
      /* Read header flag from buffer */
      diag_f3_trace_from_buffer((uint8*) &record_hdr.header_flag,
                          current_position, sizeof(record_hdr.header_flag));

      if (DIAG_F3_TRACE_IS_HEADER_VALID(record_hdr.header_flag))
      {
         found_valid_flag = TRUE;
         *buffer_position = current_position;
         break;
      }

      else
      {
         /* Increment offset and wrap if needed */
         current_position++;
         if (current_position >= DIAG_F3_TRACE_BUFFER_SIZE)
         {
            current_position-=DIAG_F3_TRACE_BUFFER_SIZE;
         }
      }

   } while (current_position != start_position);

   return(found_valid_flag);
}

/*===========================================================================

FUNCTION DIAG_F3_TRACE_CALCULATE_ENTRY_SIZE

DESCRIPTION
  Count size of entry at desginated offset in buffer.
  Returns size in bytes.

===========================================================================*/
static int diag_f3_trace_calculate_entry_size(uint32 buffer_offset)
{
   diag_f3_trace_hdr_type record_hdr;
   int entry_size = 0;
   int type = 0;
   char details = 0;
   int byte_sized = 0;

   /* Get the header of the record stored in the RAM buffer */
   diag_f3_trace_from_buffer((uint8*) &record_hdr, buffer_offset, sizeof(diag_f3_trace_hdr_type));

   if (!DIAG_F3_TRACE_IS_HEADER_VALID(record_hdr.header_flag))
   {
      /* Expecting a valid header here, but we didn't get one.
       * Return error code. */
      return(DIAG_F3_TRACE_BUFFER_CORRUPTED);
   }

   type = DIAG_F3_TRACE_GET_TYPE(record_hdr.config);

   switch (type)
   {
   case DIAG_F3_TRACE_MSG_BIT:
      /* Increment past header and const type, at least */
      entry_size = sizeof(diag_f3_trace_hdr_type)+ sizeof(void*);

      details = DIAG_F3_TRACE_GET_DETAILS(record_hdr.config);

      if (details & DIAG_F3_TRACE_SAVETIME)
      {
         entry_size += sizeof(diag_f3_trace_tstamp_type);
      }

      if (details & DIAG_F3_TRACE_SAVEVARS)
      {
         byte_sized = DIAG_F3_TRACE_BYTE_SIZED_ARGS(details);
         entry_size += (DIAG_F3_TRACE_MSG_NUM_ARGS(record_hdr.config) *
                        (sizeof(msg_arg_type) - (uint32)byte_sized));
      }
      break;

   case DIAG_F3_TRACE_EVT_BIT:
      /* Events no longer supported */
   case DIAG_F3_TRACE_LOG_BIT:
   default:
      return(DIAG_F3_TRACE_BUFFER_CORRUPTED);
   }

   return(entry_size);
}

/*===========================================================================

FUNCTION DIAG_F3_TRACE_PREPROCESS_BUFFER

DESCRIPTION
  Finds the buffer's tail position and counts the total number of entries
  in the buffer. These valeus are needed for the EFS log file.

===========================================================================*/
static uint32 diag_f3_trace_preprocess_buffer(uint32* buffer_tail)
{
   uint32 record_count=0;
   uint32 current_position;
   boolean found_tail;
   boolean stop_flag = FALSE;

   /* Find first valid entry, which is the buffer_tail */
   if (err_f3_trace_wrap_flag)
   {
      /* If the buffer wrapped, then the tail should be shortly after the head. */
      current_position = err_f3_trace_buffer_head;
   }
   else
   {
      /* If we never wrapped, the tail should be at the start of the buffer.
       * This can save a lot of time searching if the buffer is only
       * partially full.
       */
      current_position = 0;
   }

   found_tail = diag_f3_trace_find_next_entry(&current_position);

   if (found_tail)
   {
      /* We located a valid tail; process rest of buffer and count valid entries
       * until we have wrapped around to the tail again */
      *buffer_tail = current_position;
      do
      {
         int size;
         record_count++;
         /* Move past current entry */
         size = diag_f3_trace_calculate_entry_size(current_position);
         if (size != DIAG_F3_TRACE_BUFFER_CORRUPTED)
         {
            current_position += size;
            if (current_position >= DIAG_F3_TRACE_BUFFER_SIZE)
            {
               current_position-=DIAG_F3_TRACE_BUFFER_SIZE;
            }

            if ((!err_f3_trace_wrap_flag) &&
                (current_position >= err_f3_trace_buffer_head))
            {
               /* If we're past the head pointer and the original buffer
                * never wrapped then stop counting */
               stop_flag = TRUE;
               break;
            }
         }
         else
         {
            /* Error when calculating size of entry; Abort processing */
            stop_flag = TRUE;
            record_count = 0;
            break;
         }

         diag_f3_trace_find_next_entry(&current_position);

         if (err_f3_trace_wrap_flag)
         {
            /* If buffer wrapped originally, stop counting when we reach the
             * initial (tail) pointer */
            if (current_position == *buffer_tail)
               stop_flag = TRUE;
         }
      } while (!stop_flag);
   }

   return(record_count);
}

/*===========================================================================

FUNCTION DIAG_F3_TRACE_FROM_BUFFER

DESCRIPTION
  Reads the circular buffer and places the bytes into the destination buffer

DEPENDENCIES
  Interrupts must be locked

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void diag_f3_trace_from_buffer(uint8* dst, uint32 start_index, int length)
{
   int i;
   uint32 index = start_index;

   for (i = 0; i < length; i++)
   {
      dst[i] = err_f3_trace_buffer[index];
      index++;
      if (index >= DIAG_F3_TRACE_BUFFER_SIZE)
      {
         index-=DIAG_F3_TRACE_BUFFER_SIZE;
      }
   }
}
#endif /* ERR_F3_TRACE_TO_SMEM */

static uint32 diag_f3_trace_secure_offset(uint32 length)
{
   uint32 old_head;
   uint32 new_head;

   do
   {
      old_head = err_f3_trace_buffer_head;
      new_head = old_head + length;
      if (new_head >= DIAG_F3_TRACE_BUFFER_SIZE)
      {
         new_head-=DIAG_F3_TRACE_BUFFER_SIZE;
         err_f3_trace_wrap_flag=TRUE;
      }
   }
   while (!osal_atomic_compare_and_set((osal_atomic_word_t*)&err_f3_trace_buffer_head, old_head, new_head));

   return(old_head);
}

/*===========================================================================

FUNCTION DIAG_F3_TRACE_SET_UP_MSG_HEADER

DESCRIPTION
  Creates the header for a F3 message record in the trace buffer.  The
  header structure is as follows:
  Total = 16 bits
    * 3 bits = Record type
    * 5 bits = number of arguments
    * 6 bits = reserved
    * 1 bit  = savetime
    * 1 bit  = savevars

DEPENDENCIES
  None

RETURN VALUE
  Header for the F3 record

SIDE EFFECTS
  None

===========================================================================*/
static void diag_f3_trace_set_up_msg_header(int nargs, char savevars, char savetime, char byte_sized, uint32 type, diag_f3_trace_hdr_type* hdr, uint8 version)
{
   /* Zero out config data to start */
   hdr->config=0;

   /*If its an extended msg, then set the bit for ext msg*/
   if(version)
   {
     version = 0x80;
   }
   /* Add config bits to header */
   hdr->config |= (DIAG_F3_TRACE_MSG_BIT << 13);
   hdr->config |= (nargs & 0x1F) << 8;
/*lint -save -e{734} */
   byte_sized = byte_sized << 2;
/*lint -restore */
   hdr->config |= (byte_sized | savevars | savetime | version);
   hdr->config |= (type & 0x7) << 4;

   /* Write header flag to header */
   hdr->header_flag = DIAG_F3_TRACE_VALID_HEADER_FLAG;
}

// THIS IS THE PRIMARY "PUBLIC" API

void err_f3_save_msg(const void * constant_data_ptr, msg_arg_type * args, uint8 nargs, void* msg, uint32 type, uint8 version, void* param)
{
   /* Basic data checks */
   if ((constant_data_ptr == NULL) || (type >= DIAG_F3_TRACE_LAST))
      return;

   /* Is F3 trace msg saving enabled? */
   if (!(diag_f3_trace_control_mask & DIAG_F3_TRACE_MSG_BIT))
      return;

   /* Check if the version is valid*/
   if(version > DIAGBUF_MAX_VERSION_NUM)
   { 
     return;
   }
   
   /*Check if we have a valid param if version is greater than DIAGBUF_VERSION_0*/  
   if((version > DIAGBUF_VERSION_0) && (param == NULL))
   {
     return;
   }
   
   /* Check masks based on ERR_F3_TRACEDIAG_MASK_BIT */
   if (diag_f3_trace_control_mask & DIAG_F3_TRACE_DIAG_MASK_BIT)
   {
      /* Use DIAG masks in this case...so just check to see if msg was created */
      if (!msg)
         return;
   }
   else
   {
      /* Not using DIAG masks in this case,
         so compare F3 trace detail mask with ss_mask */
      uint32 mask;
    /* QShrink 3 */
      if (type==DIAG_F3_TRACE_STANDARD)
         mask = ((msg_const_type*)constant_data_ptr)->desc.ss_mask;
      else if (type==DIAG_F3_TRACE_QSHRINK || type==DIAG_F3_TRACE_QSHRINK20)
	  {
         mask = ((msg_qsr_const_type*)constant_data_ptr)->desc.ss_mask;
	  }
      else if (((msg_v3_const_hash*)constant_data_ptr)->ssid_index < msg_v3_ss_num_table_entries)
	  {
         mask = msg_v3_ss_table[((msg_v3_const_hash*)constant_data_ptr)->ssid_index]->ss_mask;
	  }
      else
         mask = 0;

      if(!(diag_f3_trace_mask & mask))
         return;
   }

   /* Make sure buffer has been initialized (needed prior to intial entry) */
   if (diag_f3_trace_buffer_initialized == FALSE)
   {
      /* Abort if failure/conflict detected during buffer init */
      if (!diag_f3_trace_buffer_init())
         return;
   }

   /* Proceed with saving msg to buffer */
   {
      int i = 0;
      uint32 record_length=0;
      diag_f3_trace_hdr_type record_hdr;
      diag_f3_trace_ext_type ext_params;
      char savetime = 0, savevars = 0;
      int byte_sized = 3;
      int bytes_to_copy = 0;
      uint32 write_offset = 0;

      savetime = diag_f3_trace_detail_mask & DIAG_F3_TRACE_SAVETIME;
      savevars = diag_f3_trace_detail_mask & DIAG_F3_TRACE_SAVEVARS;

      if (savevars)
      {
         /* Calculate bytes needed to store the largest argument. */

         byte_sized = 3;
         for (i=0; i<nargs; i++)
         {
            if (args[i] > MAX_24BIT_VALUE)
            {
               byte_sized = 0;
               break; /* Can't get larger; bail out of loop now */
            }
            else if ((args[i] > MAX_16BIT_VALUE) && (byte_sized > 1))
            {
               byte_sized = 1;
            }
            else if ((args[i] > MAX_8BIT_VALUE) && (byte_sized > 2))
            {
               byte_sized = 2;
            }
         }
      }
      else
      {
         /* Not saving any variables, so set nargs to 0 */
         nargs = 0;
      }

      /*Calculate the trace record_length needed i.e. Add Size of header,
      size of msg_const_type, size of timestamp type and size of arguments if any */

      /*Add the size of header */
      record_length += sizeof(diag_f3_trace_hdr_type);

	  /*Add the length for extended msgs depending on version being passed in */
	  switch(version)
	  {
	    case DIAGBUF_VERSION_1:
			record_length += sizeof(diag_f3_trace_ext_type);
			break;
		default: break;
	  }
	  
      /*Size of the address of the msg_const_type*/
      record_length += sizeof(void*);

      /* Add size to save timestamp to the record */
      if(savetime)
      {
        record_length += sizeof(diag_f3_trace_tstamp_type);
      }

      /* Size of arguments to save */
      if(savevars)
      {
        bytes_to_copy = (sizeof(msg_arg_type)-(uint32)byte_sized);
        record_length += bytes_to_copy * nargs;
      }

      /* Secure an address to save the message to */
      write_offset = diag_f3_trace_secure_offset(record_length);

      /* Save to the circular buffer according to where the head points */
      /* Set up msg header */

      diag_f3_trace_set_up_msg_header(nargs, savetime, savevars, (char)byte_sized, type, &record_hdr, version);
      /* Save record to the main F3 trace buffer*/
      diag_f3_trace_to_buffer(write_offset, (uint8*) &record_hdr, sizeof(diag_f3_trace_hdr_type));
      /*Increment the offset by size of the header */
      write_offset+=sizeof(diag_f3_trace_hdr_type);

	  /*Save the version and ID if version is DIAGBUF_VERSION_1*/
	  switch(version)
	  {
	    case DIAGBUF_VERSION_1:
	      ext_params.version = DIAGBUF_VERSION_1;
	      ext_params.id = (uint32)*((uint32 *)param);
	      /* Save record to the main F3 trace buffer*/
	      diag_f3_trace_to_buffer(write_offset, (uint8*) &ext_params, sizeof(diag_f3_trace_ext_type));
	      /*Increment the offset by size of the header */
	      write_offset+=sizeof(diag_f3_trace_ext_type);
	      break;
	    default: 
	      break;
	  }
	  
      /* Save address of the msg_const_type */
      /* ptr size is same for standard and Qshrink messages */

      /* Save record to the main F3 trace buffer*/
      diag_f3_trace_to_buffer(write_offset, (uint8*) &constant_data_ptr, sizeof(void*));

      /*Increment the offset by size of the address */
      write_offset+=sizeof(void*);

      /* Save timestamp to the record */
      if (savetime)
      {
        qword tstamp={0,0};     /* Time-stamp to be used with this message       */
        diag_f3_trace_tstamp_type shortened_ts;

        if (msg)
        {
           /* Get the timestamp from the created F3 message */
           if (type == DIAG_F3_TRACE_STANDARD)
           {
              qw_equ(tstamp, ((msg_ext_store_type*)msg)->hdr.ts);
           }
           else
           {
		      if(version == DIAGBUF_VERSION_0)
			  {
              qw_equ(tstamp, ((msg_v3_store_type*)msg)->hdr.ts);
           }
			  else if(version == DIAGBUF_VERSION_1)
			  {
			    qw_equ(tstamp, ((msg_v3_multi_client_store_type*)msg)->msg.hdr.ts);
			  }
           }
        }
        else
        {
           /* Message was dropped and not sent to diag.  Generate own ts */
           msg_get_time(&tstamp);                              //lint !e545
        }

        /* Get rid of the last two bytes of the timestamp */
        shortened_ts.hi = qw_hi(tstamp);
        shortened_ts.lo = qw_lo(tstamp) >> HALF_WORD_BITS;

        /* Save record to the main F3 trace buffer*/
        diag_f3_trace_to_buffer(write_offset, (uint8*) &shortened_ts, sizeof(diag_f3_trace_tstamp_type));
        /*Increment the offset by size of the timestamp type */
        write_offset+=sizeof(diag_f3_trace_tstamp_type);
      }
      /* Save arguments to the record */
      if (savevars)
      {
         for (i = 0; i < nargs; i++)
         {
        /* Save record to the main F3 trace buffer*/
           diag_f3_trace_to_buffer(write_offset, (uint8*) &(args[i]), bytes_to_copy);

           write_offset+=bytes_to_copy;
         }
      }
    }
}

/*=========================================================================== 
FUNCTION  DIAG_F3_TRACE_ENABLED_CHECK
 
DESCRIPTION
    This routine checks if the F3 trace functionality is enabled, based on its
    current mask settings.

RETURN VALUE
    TRUE,  F3 trace is enabled.
    FALSE, F3 trace is not enabled.
===========================================================================*/
boolean diag_f3_trace_enabled_check(void)
{
    boolean return_val = FALSE; /*initialize return value to FALSE */

    /* Verify if the trace control mask i.e 0th bit is set*/
    if( (diag_f3_trace_control_mask & DIAG_F3_TRACE_MSG_BIT) )
    {
        /*Check if 3rd bit set which indicates Diag Mask Debug Mode is enabled */
        if (diag_f3_trace_control_mask & DIAG_F3_TRACE_DIAG_MASK_BIT)
        {
            /*Check if masks are enabled */
            if ( msg_enabled_mask )
            {
                return_val = TRUE;
            }
        }
        else
        {
            /* Check if any mesage level bits are set i.e(2nd-7th bits) ignoring F3 arguments (0th bit)
             and time stamp (1st bit) in detail mask */
            if ( (diag_f3_trace_detail_mask>>2) & (0xFF) )
            {
                return_val = TRUE;
            }
        }
    }
    return return_val;
}

#if !defined(FEATURE_DIAG_F3_TRACE_STANDALONE)

/*===========================================================================

FUNCTION DIAG_F3_TRACE_SAVE_TO_SMEM

DESCRIPTION
  Saves the F3 trace to smem buffer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void diag_f3_trace_save_to_smem(void)
{
#if defined (ERR_F3_TRACE_TO_SMEM)
   char* f3_buf=diag_f3_trace_smem_log_buf;
   uint32 num_bytes=0;
   uint32 buffer_tail=0;
   uint32 record_count = 0;
   uint32 record_length = 0;
   static boolean singleton_write_flag = FALSE;

   //only write f3trace once (
   //note : this should not technically be needed, and what is the harm if we write a second time?
   if (singleton_write_flag)
   {
      return
   }
   singleton_write_flag = TRUE;

   /* quick sanity: Anything to copy? */
   if ((err_f3_trace_buffer_head == 0)&&(err_f3_trace_wrap_flag == FALSE))
   {
      return;
   }

   /* If we have ptr to ERR F3 trace smem log region,
      and if trace pointers are within accepted range, copy log buffer */
   if ((f3_buf!=NULL) &&
       (err_f3_trace_buffer_head < DIAG_F3_TRACE_BUFFER_SIZE))
   {
      /* Copy F3 log data into SMEM buffer.
         The format should match the format of the main f3 file saved by
            diagdebug to the EFS after resetting
        Note: head == oldest byte, may or may not be a valid msg header.
              (head-1) == newest byte
              tail == oldest VALID entry
     */

      /* get record count and first valid entry */
      record_count = diag_f3_trace_preprocess_buffer(&buffer_tail);

      /* 1. Copy number of records */
      memscpy(f3_buf, sizeof(uint32)+DIAG_F3_TRACE_BUFFER_SIZE,
             (void*)&record_count,sizeof(record_count));
      record_length += sizeof(record_count);
      f3_buf+=sizeof(record_count);

      /* 2. Copy circular buffer to SMEM buffer. */
      if (buffer_tail < err_f3_trace_buffer_head)
      {
         /* --------------------------------------------------
          * |      | Tail |      |      | Head |      |      |
          * --------------------------------------------------
          */
         // simple linear case: copy from tail to (head-1)
         memscpy(f3_buf,sizeof(uint32)+DIAG_F3_TRACE_BUFFER_SIZE-record_length,
                (void*)(err_f3_trace_buffer + buffer_tail),
                (err_f3_trace_buffer_head - buffer_tail));
         record_length += (err_f3_trace_buffer_head - buffer_tail);
      }
      else
      {
         /* --------------------------------------------------
          * | Head |      | Tail |      |      |      |      |
          * --------------------------------------------------
          */
         /* First: copy from the tail to the end of the buffer */
         num_bytes = DIAG_F3_TRACE_BUFFER_SIZE - buffer_tail;
         memscpy(f3_buf, sizeof(uint32)+DIAG_F3_TRACE_BUFFER_SIZE-record_length,
                (void*)(err_f3_trace_buffer+buffer_tail),num_bytes);
         record_length += num_bytes;

         /* --------------------------------------------------
          * |      | Head | Tail |      |      |      |      |
          * --------------------------------------------------
          */
         /* Next: copy from the beginning of the array to (head-1),
                  accounting for edge case where (head-1) wraps to end */
         if (err_f3_trace_buffer_head != 0)
         {
            f3_buf+=num_bytes;
            memscpy(f3_buf,sizeof(uint32)+DIAG_F3_TRACE_BUFFER_SIZE-record_length,
                   (void*)err_f3_trace_buffer,err_f3_trace_buffer_head);
         }
      }
   }
#endif /* ERR_F3_TRACE_TO_SMEM */
}

/*===========================================================================

FUNCTION DIAG_F3_TRACE_INIT_CONFIGS

DESCRIPTION
  Initializes the configuration values.

DEPENDENCIES
  This function is not mutex protected.  It is assumed it will only be
  called from serialized init functions (tmc/rcinit).

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void diag_f3_trace_init_configs (uint32 control, uint32 detail)
{


   diag_f3_trace_control_mask = (control & 0xFF);
   diag_f3_trace_detail_mask = (detail & 0xFF);


   /* if custom bit is set, update the main save trace mask to include all custom levels and the legacy levels specified in the diag detail mask
         else update main mask with the legacy levels defined in the diag detail mask */

   if(diag_f3_trace_detail_mask & DIAG_F3_TRACE_SAVECUSTOM_BIT)
      diag_f3_trace_mask = ((diag_f3_trace_detail_mask >> 3) | diag_f3_trace_custom_mask);
   else
      diag_f3_trace_mask = (diag_f3_trace_detail_mask >> 3);


}

#endif /* !FEATURE_ERR_F3_TRACE_STANDALONE */
