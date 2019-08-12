/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Mobile Diagnostic Message Services

General Description
  All the declarations, definitions, and routines necessary to support
  reporting messages for errors and debugging.

Initialzation and Sequencing Requirements
  

Copyright (c) 2010-2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                           Edit History 

$Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/rtos/src/msg_api.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/18/16   nk      Made changes in F3 trace logic for extended msgs
06/18/15   sa      Added support for time sync feature.
01/08/15   rs      Added check for msg_service_init.
07/06/14   ph      Fixed KW errors.
04/24/14   tbg     Fixed issues with extended msg pkts.
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
10/14/13   sr      Added support to differentiate F3s coming from different
                   instances of the same processor.
10/01/13   sa      Fixed race conditions with accessing freed extended listeners
09/20/13   tbg     Added support for Diag over STM.
08/02/13   sr      Added support for extended message APIs
07/29/12   sa      Fixed the listener notify function to correctly handle
                   different f3 command codes
07/04/13   ph      Fixed race condition which avoids double init crit sect of
                   diag_f3_listeners_ext.
06/13/13   ph      Corrected msg_sprintf to use strlcpy().
05/29/13   rh      Fix QShrink 3.0 functions so they no longer filter messages
                   based on tool-set masks before sending to F3 Trace
04/22/13   ph      converted diag_listeners heap usage to system heap.
04/15/13   sr      Support for WCDMA to register with Diag to obtain the
                   frame num
03/28/13   rh      QShrink 3.0
03/25/13   ph      Corrected msg_sprintf to use strlcpy().
03/18/13   rh      Fixed use-after-free bug in diag_remove_f3_listener_ext
02/08/13   sr      Fixed compiler  and Klockwork warnings
12/20/12   rh      Renamed err_f3_trace.h to diag_f3_trace.h
12/07/12   is      Warnings fixes
11/07/12   is      Support for preset masks
08/03/12   rh      F3 listeners now use diag_listeners_malloc and free
07/24/12   sa      Support for Calling Registered Listeners for MSG_SPRINTF_* F3's
07/10/12   ra      F3 Mask Check Optimization
06/13/12   sa      Support for Extended F3 listeners
05/16/12   rh      Support for F3 listeners
07/20/11   is      Support for QShrink 2.0
05/25/11   vs      Added msg_status
12/20/10   mad     Changes for dual diag debug message mask
10/29/10   vs      Moved msg_get_time to diagtarget.h.
                   Other changes for debugtools for lock-less implementation
09/28/10   sg      Moved diag_time_get to diag_cfg.h
09/17/10   is      Migrate from deprecated clk driver APIs
08/25/10   sg      Fixed compiler warnings
08/18/10   vs      Merged in changes from Tim to add QXDM masking for f3_save
08/09/10   sg      Removed std_strlprintf() wrapper function calls
06/30/10   sg      Inclusion of err.h
03/29/10   sg      created file

============================================================================*/
#include "core_variation.h"
#include "comdef.h"

#include <stringl/stringl.h>  /*For usage of strlcpy() */
#include <stdarg.h>     /* support for variable argument lists */

#include "msg.h"        /* Message typedefs and prototypes */
#include "msg_qsr.h"

#include "diagcmd.h"    /* diag command codes */
#include "diagbuf_v.h"  /* diag outpout buffer allocation
                                 manager. */
#include "qw.h"         /* definition of qword */
#include "err.h"
#include <stdio.h>
#include <stdarg.h>  /* Definitions va_start, va_end, etc */

#if defined (FEATURE_SAVE_DEBUG_TRACE)
#include "diag_f3_tracei.h"
#endif /* FEATURE_SAVE_DEBUG_TRACE */

#include "diagtarget.h"
#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h"
#endif

#include "diagi_v.h"
#include "diagdiag_v.h"
#include "msgi.h"
#include "diag_cfg.h"

#include "diag_stm.h"
#include "msg_arrays_i.h"

#include <stdlib.h>

  /* Time Get Function Pointer from diag.c */
  extern uint8 (*diag_time_get)(qword time);
  
  /* Number of bits in the frame number that is inserted into the timestamp,
     ** and the mask to extract these bits.
   */
#define MSG_FN_NUM_BITS 10
#define MSG_FN_MASK 0x3ff


#ifndef MSG_FMT_STR_ARG_SIZE
#define MSG_FMT_STR_ARG_SIZE 100 /* 100 is guess */
#endif

#define MSG_CHECK_IS_HASH(X) ( ((((uint32)(X)) & 0x03) != 0x0) ? \
                               QSR_BITFLAG_IS_HASH : QSR_BITFLAG_UNHASHED )

extern uint8 diag_cur_preset_id; /* from diag.c */
extern uint32 msg_dropped;  /* number of dropped messages */
extern uint32 msg_drop_delta;   /* number of dropped messages */

typedef PACK(struct)
{
  diag_cmd_ext_v1_type ext_hdr;
  msg_ext_type msg;
}msg_sprintf_ext_desc_type;

/*diag_wcdma_wl1_frame_cb from diag.c*/
extern diag_wcdma_wl1_frame_cb_type diag_wcdma_wl1_frame_cb;

typedef struct
{
  diag_f3_listener listener;
  uint32 ss_mask;              /* Sus-system mask */
  void *param;                 /* User's parameter */
}
diag_f3_listener_entry_type;

typedef struct
{
  diag_f3_listener_ext listener;
  uint32 ss_mask;              /* Sus-system mask */
  void *param;                 /* User's parameter */
}
diag_f3_listener_ext_entry_type;

extern  uint32* diag_f3_listeners_heap_to_delete[];
diag_searchlist_type diag_f3_listeners = {FALSE, {0}, {{0}}, 0, 0, 0, 0, 0, NULL, NULL};
diag_f3_searchlist_ext_type diag_f3_listeners_ext = {{NULL,NULL,0,0,0}, FALSE, 0,0, {{0}}};

#ifdef FEATURE_DEBUG_DIAG_HEAP
uint32 diag_f3_listener_match_and_free_cnt = 0;
uint32 diag_add_f3_listener_malloc_cnt = 0;
uint32 diag_add_f3_listener_free_cnt = 0;
#endif

extern boolean msg_service_init;

static void * qsr_v3_msg_send_prep ( const msg_v3_const_hash * const_blk,
   unsigned int num_args, uint32 isHash, byte stream_id, uint32 version, uint32 id);

static void * msg_send_prep (const msg_const_type * const_blk, unsigned int num_args,
                    uint64 timestamp, boolean ts_valid, byte stream_id, uint32 version, uint32 id);

static int diag_f3_ssid_compare_func(void* item_ptr, void* compare_val);
static int diag_f3_listener_compare_func(void * item_ptr,void * compare_val);


static uint32 qsr_msg_dropped;  /* number of dropped messages */
static uint32 qsr_msg_drop_delta;   /* number of dropped messages */

#ifdef FEATURE_DIAG_STM

/* STREAM_STM and STM enabled: Need to allocate and send over STM immediately */
/* If TRUE, packet is needed for STM and should be malloc'd from STM pool */
/* (This might be in addition to allocating from the diagbuf if needed for listeners or other streams) */

/* NOTE: For both MSG_STM and MSG_DIAGBUF, calling functions must pass in the value
   'flag' indicatign if diag STM is enabled or not. Each function should manage this
   value to ensure persistence through-out the function and avoid accessing the
   global variable diag_stm_enabled when possible. */

#define MSG_STM(s,flag) (((s & DIAG_STREAM_STM) && (flag)) ? TRUE : FALSE)

/* STREAM_STM and STM not enabled, or stm_enabled with a stream other than STREAM_STM */
/* If TRUE, packet needs to be allocated in diagbuf */
#define MSG_DIAGBUF(s,flag) (((!(flag) && s) || ((flag) && (s & ~DIAG_STREAM_STM))) ? TRUE : FALSE)

/* Used to compute packet size needed for QSR packet */
#define STM_QSR_EXT_MSG_PKT_SIZE(numargs) \
  sizeof(msg_hdr_type) + sizeof(msg_qsr_const_type) + (4*(numargs))

/* Used to compute packet size needed for non-QSR packet */
#define STM_EXT_MSG_PKT_SIZE(fmt_len,num_args) \
  sizeof(msg_hdr_type) + sizeof(msg_desc_type) + (fmt_len) + (4*(num_args))

#else

#define MSG_DIAGBUF(s) ((s) ? TRUE : FALSE)

#endif

/* QShrink 3.0 variables defined in msg.c */
extern uint32 msg_v3_ss_num_table_entries;
extern msg_v3_ssid_table_type *msg_v3_ss_table;
extern uint16 msg_stream_ssid_tbl_mask[DIAG_MAX_STREAM_ID][DIAG_MAX_PRESET_ID];

extern uint8 msg_enabled_mask; /* Bit mask to check if F3 masks, listeners and Extended Listeners are set */

#if defined (FEATURE_SAVE_DEBUG_TRACE)
#define MAX_VAR_ARGS 9

extern osal_mutex_arg_t         diag_debug_record_buf_cs;
static void msg_save_trace
(
  const void * constant_data_ptr,
  msg_arg_type * args,
  uint8 nargs,
  void * msg,
  uint32 type,
  uint8 version,
  void* param);
static int diag_f3_ext_init_start = FALSE;
static boolean diag_f3_listeners_ext_init(void);

/*===========================================================================

FUNCTION MSG_SAVE_TRACE

DESCRIPTION
  Extracts arguments based on the constant_data_ptr type and saves it to the
  circular buffer.  If timestamp-saving option is checked, it will be saved
  to the circular buffer as well.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes copied to the buffer.

SIDE EFFECTS
  None

===========================================================================*/
static void
msg_save_trace
(
  const void * constant_data_ptr,
  msg_arg_type * args,
  uint8 nargs,
  void * msg,
  uint32 type,
  uint8 version,
  void* param
)
{
  /* Is DIAG services initialized? */
  if (!diag_services_initialized)
    return;

  err_f3_save_msg(constant_data_ptr, args, nargs, msg, type, version, param);

  return;
}



/*===========================================================================

FUNCTION MSG_SAVE_3

DESCRIPTION
  Saves the F3 message to the RAM buffer.  Also called from the ERR_FATAL
  macro.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void msg_save_3(const msg_const_type* const_blk,
                uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3,
                msg_ext_store_type* msg)
{
    msg_arg_type f3_args[3];

    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;
    f3_args[2] = xx_arg3;

    msg_save_trace((void*)const_blk, f3_args, 3, (void*)msg, DIAG_F3_TRACE_STANDARD, 0, NULL);
    return;
}
#endif /* FEATURE_SAVE_DEBUG_TRACE */


uint8
msg_get_time(qword * ts)
{
  uint8 time_type = 0;
  /* Get timestamp for this message ASAP */
  time_type = diag_time_get (*ts);

#ifndef FEATURE_WINCE
  {
    /* Get the frame number (either SFN or CFN, depending on the
    ** state of the mobile), extract its least significant 10 bits
    ** and add these bits to the timestamp.
    */
    uint32 frame_number = 0;
    if (diag_wcdma_wl1_frame_cb != NULL)
    {
      frame_number = diag_wcdma_wl1_frame_cb() & MSG_FN_MASK;
    }
      (*ts)[0] >>= MSG_FN_NUM_BITS;
      (*ts)[0] <<= MSG_FN_NUM_BITS;
      (*ts)[0] |= frame_number;

  }
#endif

  return time_type;
}

typedef union
{
    unsigned long  ts[2];
    uint64 timestamp;
}msg_timestamp;

/*===========================================================================

FUNCTION MSG_STATUS

DESCRIPTION
  Returns whether the specific SSID and subsystem mask is enabled or not.

DEPENDENCIES
   None

===========================================================================*/

boolean msg_status(uint16 ss_id, uint32 ss_mask) {

    uint32 rt_mask = 0;
  boolean valid_ssid = FALSE;

  #ifdef DIAG_STREAM_2_MASK_CHECK
  uint32 rt_mask_ctrl = 0;
  boolean valid_ssid_ctrl = FALSE;
  #endif

    byte stream_id = 0;

  valid_ssid =
      msg_get_ssid_masks_adv (MSG_MASK_RT, ss_id,
              ss_id, &rt_mask, DIAG_STREAM_1, diag_cur_preset_id);
    if((valid_ssid && (ss_mask & rt_mask)))
      stream_id |= DIAG_STREAM_1;

  #ifdef DIAG_STREAM_2_MASK_CHECK
    valid_ssid_ctrl = msg_get_ssid_masks_adv (MSG_MASK_RT, ss_id,
              ss_id, &rt_mask_ctrl, DIAG_STREAM_2, DIAG_DCI_MASK_1);
    if((valid_ssid_ctrl && (ss_mask & rt_mask_ctrl)))
      stream_id |= DIAG_STREAM_2;
  #endif

    if (stream_id) {
            return TRUE;
    }
    else {
            return FALSE;
    }

}

/*===========================================================================

FUNCTION MSG_STREAM_CHECK

DESCRIPTION
  This helper function performs a check to see which streams a given message
  is enabled on. In cases where STM is enabled, a LISTENER stream bit is also
  set if listeners are enabled.

DEPENDENCIES

===========================================================================*/
static byte msg_stream_check(const msg_desc_type *const_desc)
{
  uint32 rt_mask = 0;
  boolean valid_ssid = FALSE;
  boolean status = FALSE;
  byte stream_id = 0;
  #ifdef DIAG_STREAM_2_MASK_CHECK
    uint32 rt_mask_ctrl = 0;
    boolean valid_ssid_ctrl = FALSE;
  #endif

  if(const_desc != NULL)
  {
    /* Check the runtime mask */
    valid_ssid =
      msg_get_ssid_masks_adv (MSG_MASK_RT, const_desc->ss_id,
          const_desc->ss_id, &rt_mask, DIAG_STREAM_1, diag_cur_preset_id);
    if((valid_ssid && (const_desc->ss_mask & rt_mask)))
      stream_id |= DIAG_STREAM_1;

    #ifdef DIAG_STREAM_2_MASK_CHECK
    valid_ssid_ctrl = msg_get_ssid_masks_adv (MSG_MASK_RT, const_desc->ss_id,
      const_desc->ss_id, &rt_mask_ctrl, DIAG_STREAM_2, diag_cur_preset_id);
    if((valid_ssid_ctrl && (const_desc->ss_mask & rt_mask_ctrl)))
      stream_id |= DIAG_STREAM_2;
    #endif

    /* If RT Masks are not enabled, check for the existence of listeners */
    /* If STM in use, also need to check for listeners */
    if((!stream_id) || (diag_stm_enabled))
    {
       status = msg_listeners_masks_enabled(const_desc->ss_id, const_desc->ss_mask, F3_LISTENER);
       if(!status)
      {
         status = msg_listeners_masks_enabled(const_desc->ss_id, const_desc->ss_mask, F3_LISTENER_EXT);
      }
        if(status)
        {
          /* Use LISTENER stream bit for listeners */
          stream_id |= DIAG_STREAM_LISTENER;
      }
    }
  }
  return stream_id;
}

/*===========================================================================

FUNCTION MSG_V3_STREAM_CHECK

DESCRIPTION
  This helper function performs a check to see which streams a given QSR3 message
  is enabled on. In cases where STM is enabled, a LISTENER stream bit is also set
  if listeners are enabled.

DEPENDENCIES

===========================================================================*/
static byte msg_v3_stream_check(const msg_v3_const_hash * const_blk)
{
  #ifdef DIAG_STREAM_2_MASK_CHECK
  uint32 rt_mask_ctrl = 0;
  boolean valid_ssid_ctrl = FALSE;
  #endif
  byte stream_id = 0;
  boolean status = FALSE;

  if(const_blk != NULL)
  {
    /* Check the runtime mask */
    if (const_blk->ssid_index < msg_v3_ss_num_table_entries)
    {
      if((msg_v3_ss_table[const_blk->ssid_index]->rt_mask) &
        msg_stream_ssid_tbl_mask[DIAG_STREAM_1-1][diag_cur_preset_id-1])
      {
        stream_id |= DIAG_STREAM_1;
      }
  #if defined(DIAG_STREAM_2_MASK_CHECK)
      if((msg_v3_ss_table[const_blk->ssid_index]->rt_mask) &
        msg_stream_ssid_tbl_mask[DIAG_STREAM_2-1][DIAG_DCI_MASK_1-1])
      {
    stream_id |= DIAG_STREAM_2;
      }
  #endif
    }

    /* If RT Masks are not enabled, check for existence of listeners */
    /* Also check for listeners if STM is enabled */
    if((!stream_id) || (diag_stm_enabled))
    {
      /* qshrink3 */
      status = diag_f3_searchlist_ext(msg_v3_ss_table[const_blk->ssid_index]->ss_id, msg_v3_ss_table[const_blk->ssid_index]->ss_mask, NULL);
      if(status)
      {
        /* Use LISTENER stream bit for listeners */
        stream_id |= DIAG_STREAM_LISTENER;
      }
      else
      {
        status = diag_searchlist_search_all(msg_v3_ss_table[const_blk->ssid_index]->ss_id, &diag_f3_listeners, NULL, NULL);
        if(status)
        {
          /* Use LISTENER stream bit for listeners */
          stream_id |= DIAG_STREAM_LISTENER;
        }
      }
    }
  }
  return stream_id;
}

/*===========================================================================

FUNCTION MSG_SEND_PREP

DESCRIPTION
  This message performs the common operations used by all messages.
  It allocates, fills in all data except arguments, and returns a pointer
  to the allocated message buffer.  It also handles message statisitics.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

PARAMETERS
 const_blk  static constant constructed using the XX_MSG_CONST macro
 num_args   Number of arguments
 timestamp  This value of timestamp will be used if ts_valid is set to TRUE
 ts_valid   Indicates whether the timestamp is valid or not
 version    0: if the message being generated is in msg_ext_store_type type.
            1: if the message being generated is in msg_send_multi_client_type type.
 id         ID passed in by user if version = 1;

===========================================================================*/
static void *
msg_send_prep (const msg_const_type * const_blk, unsigned int num_args,
              uint64 timestamp, boolean ts_valid, byte stream_id, uint32 version, uint32 id)
{
  msg_timestamp temp_timestamp;
  msg_ext_store_type *msg = NULL;
  unsigned int rsp_len = 0;
  msg_send_multi_client_type *msg_ext = NULL;

    if (stream_id)
  {
    /*-----------------------------------------------------------------------
      Get a pointer to a buffer.  If it's a NULL pointer, there aren't any
      free buffers.  No effort is made to save higher priority messages in
      favor of lower priority messages, as message flooding hides enough
      details to make the usable margin on such a feature too small for
      implementation.
    -----------------------------------------------------------------------*/
    rsp_len = FPOS(msg_ext_store_type, args) + num_args * FSIZ(msg_ext_store_type,
                                                                 args[0]);

   if (version == DIAGBUF_VERSION_1)
    {
      rsp_len += FPOS(msg_send_multi_client_type, msg);
    }

      msg = (void *)diagbuf_alloc(DIAG_EXT_MSG_TERSE_F, rsp_len, 0, DIAGBUF_ALLOC_REQUEST_MSG, 0, 0);

    if (msg)
    {
      diagbuf_write_stream_id (msg,stream_id);
      }
      else
      {
        msg_drop_delta++;
        msg_dropped++;        /* count up one more ignored message */
        return NULL;
      }

      if (version == DIAGBUF_VERSION_1)
      {
        /*Fill out the header for DSDA msg pkts*/
        msg_ext = (msg_send_multi_client_type*) msg;
        msg_ext->cmd_code = DIAG_CMD_EXT_F;
        msg_ext->version = DIAGBUF_VERSION_1;
        msg_ext->id = id;
        msg_ext->proc_id = DIAG_MY_PROC_ID;
        msg = &(msg_ext->msg);
        msg->hdr.cmd_code = DIAG_EXT_MSG_TERSE_F;
      }
      else
      {
        msg_ext = NULL;
      }

      if (msg)
      {
        if(ts_valid) /* client timestamp is valid, copy that into the header */
          {
            temp_timestamp.timestamp = timestamp;
            msg->hdr.ts[0] =(unsigned long) temp_timestamp.ts[0];
            msg->hdr.ts[1] = (unsigned long)temp_timestamp.ts[1];
          }
          else
          {
              /*lint -save -e{545} Suspicious use of & */
              msg_get_time(&(msg->hdr.ts));
              /*lint -restore */
          }
if (diag_wcdma_wl1_frame_cb != NULL)
      msg->hdr.ts_type = MSG_TS_TYPE_GW;
else
      msg->hdr.ts_type = MSG_TS_TYPE_CDMA_FULL;

      msg->hdr.num_args = (uint8)num_args;

      msg->hdr.drop_cnt = (uint8) ((msg_drop_delta > 255) ? 255 : msg_drop_delta);

      msg_drop_delta = 0;   /* Reset delta drop count */

      /*-----------------------------------------------------------------------
        Set the pointer to the constant blk, to be expanded in DIAG context.
      -----------------------------------------------------------------------*/
      msg->const_data_ptr = const_blk;
        if (msg_ext)
        {
          return msg_ext;
    }
    else
    {
          return msg;
        }
    }
  }
  return NULL;
}               /* msg_send_prep() */

/*===========================================================================

FUNCTION MSG_SEND

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send (const msg_const_type * const_blk)
{
  msg_ext_store_type *msg = NULL;
  const unsigned int num_args = 0;  /* # of message arguments */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    byte stream_id = 0;

    stream_id = msg_stream_check(&const_blk->desc);
    msg = msg_send_prep (const_blk, num_args, 0, FALSE, stream_id, 0, 0);

    if (msg)
    {
      diagbuf_commit (msg);
    }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

    msg_save_trace((void *)const_blk, NULL, 0, (void *)msg, DIAG_F3_TRACE_STANDARD, 0, NULL);

#endif /* FEATURE_SAVE_DEBUG_TRACE */
}               /* msg_send() */


/*===========================================================================

FUNCTION MSG_SEND_TS

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with timestamp as user supplied arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_ts (const msg_const_type * const_blk, uint64 timestamp)
{
  msg_send_ts_ext(const_blk, 0, NULL, timestamp);
}               /* msg_send_ts() */

/*===========================================================================

FUNCTION MSG_SEND_TS_EXT

DESCRIPTION
  This function is similar to msg_send_ts function, expect that it takes
  additional parameters : version and void *. Based on the version value,
  void* can be interpreted as follows-
  Version      Pointer
     0          NULL
     1          uint32*

  In future, if there is a need for F3 macros to take additional parameters,
  a new version ID can be added and void* can be interpreted differently
  as per the requirements.

  This functions enqueues such a message with timestamp as user supplied arguments.

  Do not call directly; use macro MSG_SEND_TS_EXT defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_ts_ext (const msg_const_type * const_blk, uint32 version, void *parameter, uint64 timestamp)
{
  msg_send_multi_client_type *msg = NULL;
  const unsigned int num_args = 0;  /* # of message arguments */
  uint32 *id_ptr = NULL;
  uint32 id = 0;
  byte stream_id = 0;

  if (msg_enabled_mask == 0 || msg_service_init != TRUE)
  {
    return;
  }

  switch (version)
  {
     case DIAGBUF_VERSION_0:
       id = 0;
       break;
     case DIAGBUF_VERSION_1:
       if (parameter == NULL)
       {
         return;
       }
       id_ptr = (uint32 *)parameter;
       id = *id_ptr;
       break;
     default:
       return;
  }

    stream_id = msg_stream_check(&const_blk->desc);

  msg = (msg_send_multi_client_type *)msg_send_prep(const_blk, num_args, timestamp, TRUE, stream_id, version, id);

      if (msg)
      {
        diagbuf_commit(msg);
      }

#if defined (FEATURE_SAVE_DEBUG_TRACE)
  if((version > DIAGBUF_VERSION_0) && (version <= DIAGBUF_MAX_VERSION_NUM))
  {
    if(parameter)
	{
	  msg_save_trace((void *)const_blk, NULL, 0, (void *)msg, DIAG_F3_TRACE_STANDARD, (uint8)version, parameter);	  
	}
  }
  else
  {
 	msg_save_trace((void *)const_blk, NULL, 0, (void *)msg, DIAG_F3_TRACE_STANDARD, 0, NULL ); 
  }

#endif /* FEATURE_SAVE_DEBUG_TRACE */

}               /* msg_send_ts_ext() */
/*===========================================================================

FUNCTION MSG_SEND_1

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 1 argument.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_1 (const msg_const_type * const_blk, uint32 xx_arg1)
{
  msg_ext_store_type *msg = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 1;  /* # of message arguments */
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[1];
#endif /* FEATURE_SAVE_DEBUG_TRACE */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    byte stream_id = 0;

    stream_id = msg_stream_check(&const_blk->desc);
    msg = (msg_ext_store_type *) msg_send_prep (const_blk, num_args, 0, FALSE, stream_id, 0, 0);

    if (msg)
    {
      /*-----------------------------------------------------------------------
        Store the arguments in the buffer.
      -----------------------------------------------------------------------*/
      args = msg->args;

      args[0] = xx_arg1;

      diagbuf_commit (msg);
    }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

    f3_args[0] = xx_arg1;
    msg_save_trace((const void *)const_blk, f3_args, 1, (void *)msg, DIAG_F3_TRACE_STANDARD, 0, NULL);

#endif /* FEATURE_SAVE_DEBUG_TRACE */
}               /* void msg_send_1() */

/*===========================================================================

FUNCTION MSG_SEND_2

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 2 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_2 (const msg_const_type * const_blk, uint32 xx_arg1, uint32 xx_arg2)
{
  msg_ext_store_type *msg = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 2;  /* # of message arguments */
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[2];
#endif /* FEATURE_SAVE_DEBUG_TRACE */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    byte stream_id = 0;

    stream_id = msg_stream_check(&const_blk->desc);

    msg = (msg_ext_store_type *) msg_send_prep (const_blk, num_args, 0, FALSE, stream_id, 0, 0);

    if (msg)
    {
      /*-----------------------------------------------------------------------
        Store the arguments in the buffer.
      -----------------------------------------------------------------------*/
      args = MSG_LARGE_ARGS(msg->args);

      args[0] = xx_arg1;
      args[1] = xx_arg2;

      diagbuf_commit (msg);
    }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;
    msg_save_trace((const void *)const_blk, f3_args, 2, (void *)msg, DIAG_F3_TRACE_STANDARD, 0, NULL);

#endif /* FEATURE_SAVE_DEBUG_TRACE */
}               /* msg_send_2() */

/*===========================================================================

FUNCTION MSG_SEND_3

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 3 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_3 (const msg_const_type * const_blk, uint32 xx_arg1,
        uint32 xx_arg2, uint32 xx_arg3)
{
  msg_ext_store_type *msg = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 3;  /* # of message arguments */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    byte stream_id = 0;

    stream_id = msg_stream_check(&const_blk->desc);
    msg = msg_send_prep (const_blk, num_args, 0, FALSE, stream_id, 0, 0);

    if (msg)
    {
      /*-----------------------------------------------------------------------
        Store the arguments in the buffer.
      -----------------------------------------------------------------------*/
      args = MSG_LARGE_ARGS(msg->args);

      args[0] = xx_arg1;
      args[1] = xx_arg2;
      args[2] = xx_arg3;

      diagbuf_commit (msg);
    }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

    msg_save_3(const_blk, xx_arg1, xx_arg2, xx_arg3, msg);

#endif /* FEATURE_SAVE_DEBUG_TRACE */
}    /* msg_send_3() */



/*===========================================================================

FUNCTION MSG_SEND_VAR

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with a variable number of
    arguments, specified by 'num_args'.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_var (const msg_const_type * const_blk, uint32 num_args, ...)
{
  msg_ext_store_type *msg = NULL;
  uint32 *args = NULL;
  va_list arg_list;     /* ptr to the variable argument list */
  unsigned int i;
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[9];  // Assuming at most 9 arguments
#endif /* FEATURE_SAVE_DEBUG_TRACE */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    byte stream_id = 0;

    stream_id = msg_stream_check(&const_blk->desc);
    msg = msg_send_prep (const_blk, num_args, 0, FALSE, stream_id, 0, 0);

    if (msg)
    {
      /*-----------------------------------------------------------------------
        Store the arguments in the buffer.
      -----------------------------------------------------------------------*/
      args = MSG_LARGE_ARGS(msg->args);

  /*lint -save -e{64,10,78,718,746,628,732,530} */
      /* Initialize variable arguments */
      va_start (arg_list, num_args);
  /*lint -restore */

      /* Store arguments from variable list. */
  /*lint -save -e{64,10,78,718,746,628,732,530} */
      for (i = 0; i < num_args; i++)
      {
        args[i] = va_arg (arg_list, uint32);
      }               /* for */
  /*lint -restore */
      /* Reset variable arguments */
      va_end (arg_list);

      diagbuf_commit (msg);
    }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

/*lint -save -e{64,10,718,746,628} */
    /* Initialize variable arguments */
    va_start (arg_list, num_args);

    /* Store arguments from variable list. */
    for (i = 0; i < MIN(num_args,MAX_VAR_ARGS); i++)
    {
      f3_args[i] = va_arg (arg_list, uint32);
    }               /* for */

    /* Reset variable arguments */
    va_end (arg_list);
/*lint -restore */
    msg_save_trace((const void *)const_blk, f3_args, MIN(num_args,MAX_VAR_ARGS), (void *)msg, DIAG_F3_TRACE_STANDARD, 0, NULL);

#endif /* FEATURE_SAVE_DEBUG_TRACE */
}               /* msg_send_var() */



#ifdef FEATURE_DIAG_STM
static uint32 stm_msg_drop_delta = 0;
static uint32 stm_msg_dropped = 0;

/*===========================================================================

FUNCTION DIAG_STM_MSG_HDR_PREP

DESCRIPTION
  This helper functions fills out commonly used fields in the msg header
  for a message packet being sent out over STM.

DEPENDENCIES

===========================================================================*/
static void diag_stm_msg_hdr_prep(msg_hdr_type *msg_hdr, unsigned int num_args)
{
  /* This is common to all F3s being sent over STM */
  msg_hdr->num_args = num_args;
  msg_get_time(&(msg_hdr->ts));
  if (diag_wcdma_wl1_frame_cb != NULL)
  msg_hdr->ts_type = MSG_TS_TYPE_GW;
  else
  msg_hdr->ts_type = MSG_TS_TYPE_CDMA_FULL;

  msg_hdr->drop_cnt = (uint8) ((stm_msg_drop_delta > 255) ? 255 : stm_msg_drop_delta);
  stm_msg_drop_delta = 0;   /* Reset delta drop count */
}

/*===========================================================================

FUNCTION DIAG_STM_QSR_MSG_PREP

DESCRIPTION
  This functions allocates a diag STM message packet and populates all packet
  fields, except for arguments. It is used for packets being sent over STM,
    which must be fully expanded in the context of the caller.

DEPENDENCIES

===========================================================================*/

void *diag_stm_qsr_msg_prep(const msg_qsr_const_type * const_blk, unsigned int num_args, uint32 isHash, uint32 *len, uint32 version, uint32 id)
{
  uint32 msg_size = 0;
  msg_v3_multi_client_store_type *msg_ext = NULL;
  void * msg;

  /* Determine size of full packet */
  if (isHash)
  {
    /* Qhrink message (DIAG_QSR_EXT_MSG_TERSE_F) */
    /* msg_size = msg_hdr_type + qsr_const_type + args for Qshrink messages */
    msg_size = STM_QSR_EXT_MSG_PKT_SIZE(num_args);
  }
  else
  {
    /* Non Qshrink message (DIAG_EXT_MSG_F) */
    /* Determine string length (including NULL terminating character) */

    /* Notes on msg/filename fmt
       current == msg_hash -> "filename:string"
       Can determine length needed based on the string length of msg_hash. */

    msg_size = STM_EXT_MSG_PKT_SIZE(strlen((const char*)const_blk->msg_hash)+1,num_args);
  }

  if(version == DIAGBUF_VERSION_1)
  {
    /* Add additional space for multi-client header for packet if required */
    msg_size += FPOS(msg_v3_multi_client_store_type, msg);
  }

  /* Allocate packet for STM */
  msg = diag_stm_msg_alloc(msg_size);

  /* Populate common fields in buffer */
  if (msg)
  {
    msg_hdr_type *msg_hdr = (msg_hdr_type *)msg;
    char *copy_ptr = NULL;
    uint32 copy_buf_size = msg_size;

    if(version == DIAGBUF_VERSION_1)
    {
      /* Fill out the header for multiclient msg pkts */
      msg_ext = (msg_v3_multi_client_store_type*) msg;
      msg_ext->cmd_code = DIAG_CMD_EXT_F;
      msg_ext->version = DIAGBUF_VERSION_1;
      msg_ext->id = id;
      msg_ext->proc_id = DIAG_MY_PROC_ID;
      msg_ext->msg.hdr.cmd_code = DIAG_QSR_EXT_MSG_TERSE_F;
      /* Update msg pointer to correct location */
      msg_hdr = &(msg_ext->msg.hdr);
    }
    else
    {
      /* No additional header required */
      msg_hdr = (msg_hdr_type *)msg;
    }

    /*  Set packet length to size allocated */
    *len = msg_size;

    /* Set msg_header fields */
    diag_stm_msg_hdr_prep(msg_hdr, num_args);

    /* Copy descriptor fields */
    copy_ptr = (char*)msg + sizeof(msg_hdr_type);
    copy_buf_size -= sizeof(msg_hdr_type);
    memscpy(copy_ptr, copy_buf_size,
            (void*)(&const_blk->desc), sizeof(msg_desc_type));
    copy_ptr += sizeof(msg_desc_type);
    copy_buf_size -= sizeof(msg_desc_type);

    if (isHash)
    {
      /* QSR message DIAG_QSR_EXT_MSG_TERSE_F */
      msg_hdr->cmd_code = DIAG_QSR_EXT_MSG_TERSE_F;

      /* Copy hash value */
      memscpy(copy_ptr,copy_buf_size,(void*)(&const_blk->msg_hash),sizeof(uint32));
    }
    else
    {
      /* Non-QSR message (no hash) DIAG_EXT_MSG_F */
      char *fmt_ptr;
      int filename_len, str_len;

      msg_hdr->cmd_code = DIAG_EXT_MSG_F;

      /* Need to determine length of filename (location where fmt string starts) */
      for (fmt_ptr = (char *) const_blk->msg_hash; *fmt_ptr != ':'; fmt_ptr++);
      filename_len = fmt_ptr - (char *)const_blk->msg_hash;

      /* Copy format string */

      /* Increment fmt_ptr past ':' character */
      fmt_ptr++;
      /*  Reserve space for arguments */
      copy_ptr += (num_args*4);
      copy_buf_size -= (num_args*4);
      str_len = MIN(MSG_MAX_STRLEN, (int)strlen(fmt_ptr));

      memscpy(copy_ptr,copy_buf_size,(void*)fmt_ptr, str_len);
      copy_ptr += str_len;

      /* NULL terminate format string */
      *copy_ptr++ = '\0';

      copy_buf_size -= (str_len + 1);

      /* Copy file name */
      filename_len = MIN(MSG_MAX_STRLEN,filename_len);
      memscpy(copy_ptr,copy_buf_size,(void*)(const_blk->msg_hash),filename_len);
      copy_ptr += filename_len;

      /* NULL terminate filename */
      *copy_ptr++ = '\0';
    }
  }
  else
  {
    /* Unable to allocate memory */

    /* Set packet length to zero */
    *len = 0;

    stm_msg_drop_delta++;
    if(stm_msg_dropped < MAX_VALUE_UINT32)
    {
      stm_msg_dropped++;
    }
  }
  return msg;
}

/*===========================================================================

FUNCTION DIAG_STM_QSR_V3_MSG_PREP

DESCRIPTION
  This functions allocates a diag STM QSR3 message packet and populates all packet
  fields, except for arguments. It is used for packets being sent over STM,
  which must be fully expanded in the context of the caller.

DEPENDENCIES

===========================================================================*/
void *diag_stm_qsr_v3_msg_prep(const msg_v3_const_hash * const_blk, unsigned int num_args, uint32 isHash, uint32 *len, uint32 version, uint32 id)
{
  msg_v3_multi_client_store_type *msg_ext = NULL;
  uint32 msg_size = 0;
  uint16 ssid_index = ~0;
  msg_desc_type desc={0,0,0};
  void *msg;

  /* Determine size of full packet */
  if (isHash)
  {
    /* Qhrink message (DIAG_QSR_EXT_MSG_TERSE_F) */
    /* msg_size = msg_hdr_type + qsr_const_type + args for Qshrink messages */
    msg_size = STM_QSR_EXT_MSG_PKT_SIZE(num_args);
  }
  else
  {
    /* Non Qshrink message (DIAG_EXT_MSG_F) */
    /* Determine string length (including NULL terminating character) */

    /* Notes on msg/filename fmt
       current == msg_hash -> "filename:string"
       Can determine length needed based on the string length of msg_hash. */

    msg_size = STM_EXT_MSG_PKT_SIZE(strlen((const char*)const_blk->msg)+1,num_args);
  }

  if(version == DIAGBUF_VERSION_1)
  {
    /* Add additional space for multi-client header for packet if required */
    msg_size += FPOS(msg_v3_multi_client_store_type, msg);
  }

  if(const_blk->ssid_index< msg_v3_ss_num_table_entries)
  {
    ssid_index=const_blk->ssid_index;
    /*form the descriptor */
    desc.ss_id = msg_v3_ss_table[ssid_index]->ss_id;
    desc.ss_mask = msg_v3_ss_table[ssid_index]->ss_mask;
    desc.line=const_blk->line;
  }
  else
  {
    /*Invalid index, drop the data */
    /* Set packet length to zero */
    *len = 0;
    stm_msg_drop_delta++;
    if(stm_msg_dropped < MAX_VALUE_UINT32)
    {
      stm_msg_dropped++;
    }
    return NULL;

  }

  /* Allocate packet for STM */
  msg = diag_stm_msg_alloc(msg_size);

  /* Populate common fields in buffer */
  if (msg)
  {
    msg_hdr_type *msg_hdr = NULL;
    uint32 copy_buf_size = msg_size;

    if(version == DIAGBUF_VERSION_1)
    {
      /* Fill out the header for multiclient msg pkts */
      msg_ext = (msg_v3_multi_client_store_type*) msg;
      msg_ext->cmd_code = DIAG_CMD_EXT_F;
      msg_ext->version = DIAGBUF_VERSION_1;
      msg_ext->id = id;
      msg_ext->proc_id = DIAG_MY_PROC_ID;
      msg_ext->msg.hdr.cmd_code = DIAG_QSR_EXT_MSG_TERSE_F;
      /* Update msg pointer to correct location */
      msg_hdr = &(msg_ext->msg.hdr);
    }
    else
    {
      /* No additional header required */
      msg_hdr = (msg_hdr_type *)msg;
    }

    char *copy_ptr = NULL;

    /*  Set packet length to size allocated */
    *len = msg_size;

    /* Set msg_header fields */
    diag_stm_msg_hdr_prep(msg_hdr, num_args);

    /* Copy descriptor fields */
    copy_ptr = (char*)msg + sizeof(msg_hdr_type);
    copy_buf_size -= sizeof(msg_hdr_type);
    memscpy(copy_ptr, copy_buf_size, (void*)(&desc), sizeof(msg_desc_type));
    copy_ptr += sizeof(msg_desc_type);
    copy_buf_size -= sizeof(msg_desc_type);

    if (isHash)
    {
      /* QSR message DIAG_QSR_EXT_MSG_TERSE_F */
      msg_hdr->cmd_code = DIAG_QSR_EXT_MSG_TERSE_F;

      /* Copy hash value */
      memscpy(copy_ptr,copy_buf_size,(void*)(&const_blk->msg),sizeof(uint32));
    }
    else
    {
      /* Non-QSR message (no hash) DIAG_EXT_MSG_F */
      char *fmt_ptr;
      int filename_len, str_len;

      msg_hdr->cmd_code = DIAG_EXT_MSG_F;

      /* Need to determine length of filename (location where fmt string starts) */
      for (fmt_ptr = (char *) const_blk->msg; *fmt_ptr != ':'; fmt_ptr++);
      filename_len = fmt_ptr - (char *)const_blk->msg;

      /* Copy format string */

      /* Increment fmt_ptr past ':' character */
      fmt_ptr++;
      /*  Reserve space for arguments */
      copy_ptr += (num_args*4);
      copy_buf_size -= (num_args*4);
      str_len = MIN(MSG_MAX_STRLEN, (int)strlen(fmt_ptr));

      memscpy(copy_ptr,copy_buf_size,(void*)fmt_ptr, str_len);
      copy_ptr += str_len;

      /* NULL terminate format string */
      *copy_ptr++ = '\0';

      copy_buf_size -= (str_len + 1);

      /* Copy file name */
      filename_len = MIN(MSG_MAX_STRLEN,filename_len);
      memscpy(copy_ptr,copy_buf_size,(void*)(const_blk->msg),filename_len);
      copy_ptr += filename_len;

      /* NULL terminate filename */
      *copy_ptr++ = '\0';
    }
  }
  else
  {
    /* Unable to allocate memory */

    /* Set packet length to zero */
    *len = 0;

    stm_msg_drop_delta++;
    stm_msg_dropped++;
  }

  return msg;
}

#endif /* FEATURE_DIAG_STM */

/*===========================================================================

FUNCTION MSG_SPRINTF

DESCRIPTION

  This will queue a debug message in Extended Message Format.  The message
  will be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This will build a message sprintf diagnostic Message with var #
  of parameters.

  Do not call directly; use macro MSG_SPRINTF_* defined in msg.h

  Send a message through diag output services.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_sprintf (const msg_const_type * const_blk, ...)
{
  /* Declared union for future use */
  typedef union
  {
    msg_ext_type ext;
  } msg_sprintf_desc_type;

  msg_sprintf_desc_type *msg = NULL;

  char *str = NULL;   /* Used to copy the file name and fmt string to the msg */
  va_list arg_list;   /* ptr to the variable argument list */

  unsigned int int_cnt = 0;           /* Calculate the # args, to allocate buffer */
  unsigned int fname_length = 0;      /* Stores the file name along with '\0'     */
  unsigned int fmt_length = 0;        /* Stores the fmt length,'\0' and arg size  */
  unsigned int total_allocated = 0;   /* Total buffer allocated                   */
  unsigned int fmt_len_available = 0; /* Remaining buffer for format string       */
  boolean diagbuf_flag = TRUE;        /* Flag indicating that buffer came from diagbuf (and not STM) */
  uint32 pkt_length = 0;        /* Final packet length */
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

  fname_length = strlen (const_blk->fname) + 1;

  /* Copy the length of the fmt string along with MSG_LENGTH_OF_ARGS */
  fmt_length = strlen (const_blk->fmt) + 1 + MSG_FMT_STR_ARG_SIZE;

  /* Calculate # of arguments to ensure enough space is allocated. */
  int_cnt = sizeof (msg_desc_type) - FSIZ (msg_ext_store_type, const_data_ptr) +
            fmt_length + fname_length;

  /* Calculates number of uint32 required    */
  int_cnt = (int_cnt + sizeof (uint32) - 1) / sizeof (uint32);

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
      byte stream_id = 0;

      stream_id = msg_stream_check(&const_blk->desc);
    /*  Allocates the buffer required, fills in the header  */

if MSG_DIAGBUF(stream_id,stm_enabled)
      {
          /* Need to allocate from diagbuf */
          msg = (msg_sprintf_desc_type *) msg_send_prep (const_blk,  int_cnt, 0, FALSE, stream_id, 0, 0);
      }
#ifdef FEATURE_DIAG_STM
      else if MSG_STM(stream_id,stm_enabled)
      {
          /* Allocate space for STM buffer */
        msg = (msg_sprintf_desc_type*)diag_stm_msg_alloc(STM_EXT_MSG_PKT_SIZE(fmt_length,int_cnt));

        if (msg)
        {
            msg_hdr_type *msg_hdr = (msg_hdr_type *)msg;
            diag_stm_msg_hdr_prep(msg_hdr, int_cnt);
        }

        //msg = (msg_sprintf_desc_type*)diag_stm_qsr_msg_prep((msg_qsr_const_type*)const_blk, int_cnt, FALSE, &pkt_length);
        diagbuf_flag = FALSE;
      }
#endif /* FEATURE_DIAG_STM */

    if (msg)
    {
      /* Queue a debug message in Extended Message Format. */
      msg->ext.hdr.cmd_code = DIAG_EXT_MSG_F;

      /* This function embedds the argument in the string itself.
      ** Hence the num_args is assigned 0 */
      msg->ext.hdr.num_args = 0;

      /* Copy 'desc'. */
      msg->ext.desc = const_blk->desc;

      /* Copy the format string where the argument list would start.
         Since there are no arguments, the format string starts in the 'args'
         field. */
      str = (char *) msg->ext.args;

      /* Calculate the buffer allocated */
      total_allocated = FPOS (msg_ext_store_type, args) + int_cnt *
                            FSIZ (msg_ext_store_type, args[0]);

      /* Calculate the buffer left to copy the format string */
      fmt_len_available = total_allocated - (FPOS (msg_ext_type, args) + fname_length);

      if( fmt_len_available < fmt_length)
      {
         fmt_length = fmt_len_available;
      }

      /* Initialize variable argument list */
  /*lint -save -e{64,718,746,516} */
      va_start(arg_list, const_blk);
  /*lint -restore */

      /* Copy the format string with arguments */
  #if defined __GNUC__
      (void) vsnprintf (str, fmt_length, const_blk->fmt, arg_list);
       str[fmt_length-1] = '\0';
  #else
      (void) _vsnprintf (str, fmt_length, const_blk->fmt, arg_list);
      str[fmt_length-1] = '\0';
  #endif

    /* Reset variable arguments */
      va_end(arg_list);

      /* Move the str pass the format string, strlen excludes the terminal
      ** NULL hence 1 is added to include NULL. */
      str += strlen((const char *)str) + 1;

        /* Copy the filename and strlcpy adds the ending NULL terminal */
      /*Since the str dest size is computed before, there would not be any buffer overflow */
      (void)strlcpy(str, (const char *)const_blk->fname, fname_length );

      /* Move the str pass the filename, strlen excludes the terminal NULL
      ** hence 1 is added to include NULL. */
      str += strlen((const char *)str) + 1;

      /* str is now pointing to the byte after the last valid byte. str - msg
             gives the total length required. */
      pkt_length = (uint32)(str - (char *)msg);

#ifdef FEATURE_DIAG_STM
      if MSG_STM(stream_id,stm_enabled)
      {
        /* Send data to STM */
        diag_stm_write_pkt(msg,pkt_length);
      }

      if (!diagbuf_flag)
      {
        /* Data not allocated from diagbuf, so free data */
        diag_stm_msg_free(msg);

        /* clear stream_stm bit to avoid resending */
        stream_id &= ~DIAG_STREAM_STM;
      }
      else
      {
        diagbuf_write_stream_id (msg,stream_id);
      }
#endif /* FEATURE_DIAG_STM */
      if (diagbuf_flag)
      {
        /* Use diagbuf_shorten to release the buffer */
        diagbuf_shorten (msg, pkt_length);
        diagbuf_commit (msg);
      }
    }
  }

  return;
}  /* msg_sprintf */

/*===========================================================================

FUNCTION msg_sprintf_ext

DESCRIPTION
  This function is similar to msg_sprintf function, expect that it takes
  additional parameters : version and void *. Based on the version value,
  void* can be interpreted as follows-
  Version      Pointer
     0          NULL
     1          uint32*

  In future, if there is a need for F3 macros to take additional parameters,
  a new version ID can be added and void* can be interpreted differently
  as per the requirements.

PARAMETERS
  Version            Indicates the version of the packet.
  parameter          Based on the version value, parameter can be interpreted as follows-
                       Version      Pointer
                         0          NULL
                         1          uint32*
  num_args           Number of arguments being passed in
 *const_blk          Static constant constructed using the different
                     xx_msg_const macros

  Do not call directly; use macro MSG_SPRINTF_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_sprintf_ext (uint32 version, void * parameter, uint32 num_args,const msg_const_type * const_blk, ...)
{
  /* Declared union for future use */
  typedef union
  {
    msg_ext_type ext;
  } msg_sprintf_desc_type;

  msg_sprintf_desc_type *msg = NULL;
  msg_sprintf_ext_desc_type *msg_ext = NULL;
  void *msg_ptr = NULL; /* pointer to commit/send */

  char *str = NULL;   /* Used to copy the file name and fmt string to the msg */
  va_list arg_list;     /* ptr to the variable argument list */

  unsigned int int_cnt = 0;           /* Calculate the # args, to allocate buffer */
  unsigned int fname_length = 0;      /* Stores the file name along with '\0'     */
  unsigned int fmt_length = 0;        /* Stores the fmt length,'\0' and arg size  */
  unsigned int total_allocated = 0;   /* Total buffer allocated                   */
  unsigned int fmt_len_available = 0; /* Remaining buffer for format string       */
  uint32 *id = NULL;
  byte stream_id = 0;
  uint32 pkt_length = 0;
  boolean diagbuf_flag = TRUE;
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

  /* Exit immediately if no masks are enabled */
  if(!msg_enabled_mask || msg_service_init != TRUE)
    return;

  stream_id = msg_stream_check(&const_blk->desc);
    /*  Allocates the buffer required, fills in the header  */

  fname_length = strlen (const_blk->fname) + 1;

  /* Copy the length of the fmt string along with MSG_LENGTH_OF_ARGS */
  fmt_length = strlen (const_blk->fmt) + 1 + MSG_FMT_STR_ARG_SIZE;

        /* Initialize variable argument list */
  /*lint -save -e{64,718,746,516} */
      va_start(arg_list, const_blk);
  /*lint -restore */

  /* Calculate # of arguments to ensure enough space is allocated. */
    int_cnt = sizeof (msg_desc_type) - FSIZ (msg_ext_store_type, const_data_ptr) +
              fmt_length + fname_length;

  /* Calculates number of uint32 required    */
  int_cnt = (int_cnt + sizeof (uint32) - 1) / sizeof (uint32);

  if (version == DIAGBUF_VERSION_0)
  {
    if MSG_DIAGBUF(stream_id,stm_enabled)
    {
      /*  Allocates the buffer required, fills in the header  */
      msg = (msg_sprintf_desc_type *) msg_send_prep (const_blk,  int_cnt, 0, FALSE, stream_id, 0, 0);
    }
#ifdef FEATURE_DIAG_STM
      else if MSG_STM(stream_id,stm_enabled)
      {
          /* Allocate space for STM buffer */
          msg = (msg_sprintf_desc_type*)diag_stm_msg_alloc(STM_EXT_MSG_PKT_SIZE(fmt_length,int_cnt));

      if (msg)
      {
        msg_hdr_type *msg_hdr = (msg_hdr_type *)msg;
        diag_stm_msg_hdr_prep(msg_hdr, int_cnt);
      }
          diagbuf_flag = FALSE;
      }
#endif /* FEATURE_DIAG_STM */
    /* Pointer will be used later to send/commit the message */
    msg_ptr = msg;

  }
  else if (version == DIAGBUF_VERSION_1)
  {
    if (parameter == NULL)
    {
      return;
    }
    id = (uint32 *) parameter;
    if MSG_DIAGBUF(stream_id,stm_enabled)
    {
      msg_ext = (msg_sprintf_ext_desc_type *) msg_send_prep (const_blk,  int_cnt, 0, FALSE, stream_id, version, *id);
    }
#ifdef FEATURE_DIAG_STM
      else if MSG_STM(stream_id,stm_enabled)
      {
      uint32 pkt_size = STM_EXT_MSG_PKT_SIZE(fmt_length,int_cnt);
      pkt_size += FPOS(msg_v3_multi_client_store_type, msg);

      /* Allocate space for STM buffer */
          msg_ext = (msg_sprintf_ext_desc_type*)diag_stm_msg_alloc(pkt_size);

      if (msg_ext)
      {
        msg_hdr_type *msg_hdr;

        /* Fill out the header for multiclient msg pkts */
        msg_ext->ext_hdr.cmd_code = DIAG_CMD_EXT_F;
        msg_ext->ext_hdr.version = DIAGBUF_VERSION_1;
        msg_ext->ext_hdr.id = *id;
        msg_ext->ext_hdr.proc_id = DIAG_MY_PROC_ID;

        /* Update msg pointer to correct location */
        msg_hdr = &(msg_ext->msg.hdr);

        diag_stm_msg_hdr_prep(msg_hdr, int_cnt);
      }

      diagbuf_flag = FALSE;
    }
#endif /* FEATURE_DIAG_STM */
    /* Pointer will be used later to send/commit the message */
    msg_ptr = msg_ext;

    if (msg_ext)
    {
      msg = (msg_sprintf_desc_type *) &(msg_ext->msg);
    }
  }

  if (msg)
  {
    /* Queue a debug message in Extended Message Format. */
    msg->ext.hdr.cmd_code = DIAG_EXT_MSG_F;

    /* This function embedds the argument in the string itself.
     ** Hence the num_args is assigned 0 */
    msg->ext.hdr.num_args = 0;

    /* Copy 'desc'. */
    msg->ext.desc = const_blk->desc;

    /* Copy the format string where the argument list would start.
       Since there are no arguments, the format string starts in the 'args'
       field. */
    str = (char *) msg->ext.args;

    /* Calculate the buffer allocated */
    total_allocated = FPOS (msg_ext_store_type, args) + int_cnt *
                            FSIZ (msg_ext_store_type, args[0]);

    /* Calculate the buffer left to copy the format string */
    fmt_len_available = total_allocated - (FPOS (msg_ext_type, args) + fname_length);

    if( fmt_len_available < fmt_length)
    {
      fmt_length = fmt_len_available;
    }

    /* Copy the format string with arguments */
  #if defined __GNUC__
    (void) vsnprintf (str, fmt_length, const_blk->fmt, arg_list);
    str[fmt_length-1] = '\0';
  #else
    (void) _vsnprintf (str, fmt_length, const_blk->fmt, arg_list);
    str[fmt_length-1] = '\0';
  #endif

    /* Reset variable arguments */
    va_end(arg_list);

    /* Move the str pass the format string, strlen excludes the terminal
    ** NULL hence 1 is added to include NULL. */
    str += strlen((const char *)str) + 1;

    /* Copy the filename and strlcpy adds the ending NULL terminal */
    /*Since the str dest size is computed before, there would not be any buffer overflow */
    (void)strlcpy(str, (const char *)const_blk->fname, fname_length );

    /* Move the str pass the filename, strlen excludes the terminal NULL
    ** hence 1 is added to include NULL. */
    str += strlen((const char *)str) + 1;

    /* str is now pointing to the byte after the last valid byte. str - msg
       gives the total length required. */
    pkt_length = (uint32)(str - (char*)msg_ptr);

#ifdef FEATURE_DIAG_STM
    if MSG_STM(stream_id,stm_enabled)
    {
        /* Send data to STM */
        diag_stm_write_pkt(msg_ptr,pkt_length);
    }

    if (!diagbuf_flag)
    {
        /* Data not allocated from diagbuf, so free data */
        diag_stm_msg_free(msg_ptr);

        /* clear stream_stm bit to avoid resending */
        stream_id &= ~DIAG_STREAM_STM;
    }
    else
    {
        diagbuf_write_stream_id (msg_ptr,stream_id);
    }
#endif /* FEATURE_DIAG_STM */
    if(diagbuf_flag)
    {
        diagbuf_shorten (msg_ptr, pkt_length);
        diagbuf_commit (msg_ptr);
    }
  }

  return;
}  /* msg_sprintf_ext */

/*===========================================================================

FUNCTION qsr_msg_send_prep

DESCRIPTION
  This message performs the common operations used by all messages.
  It allocates, fills in all data except arguments, and returns a pointer
  to the allocated message buffer.  It also handles message statisitics.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
static msg_qsr_store_type *
qsr_msg_send_prep (const msg_qsr_const_type * const_blk,unsigned int num_args, uint32 isHash, byte stream_id)
{


  msg_qsr_store_type *msg = NULL;
  unsigned int rsp_len = FPOS (msg_qsr_store_type, args) + num_args * FSIZ (msg_qsr_store_type,
                               args[0]);

  if (stream_id)
  {
    /*-----------------------------------------------------------------------
      Get a pointer to a buffer.  If it's a NULL pointer, there aren't any
      free buffers.  No effort is made to save higher priority messages in
      favor of lower priority messages, as message flooding hides enough
      details to make the usable margin on such a feature too small for
      implementation.
    -----------------------------------------------------------------------*/
    msg = (void *) diagbuf_alloc (DIAG_QSR_EXT_MSG_TERSE_F, rsp_len, 0, DIAGBUF_ALLOC_REQUEST_MSG, 0, 0);

    if (msg)
    {
        diagbuf_write_stream_id (msg,stream_id);
/*lint -save -e{545} Suspicious use of & */
      msg_get_time(&(msg->hdr.ts));
/*lint -restore */

if (diag_wcdma_wl1_frame_cb != NULL)
      msg->hdr.ts_type = MSG_TS_TYPE_GW;
else
      msg->hdr.ts_type = MSG_TS_TYPE_CDMA_FULL;

      msg->hdr.num_args = (uint8)num_args;

      msg->hdr.drop_cnt = (uint8) ((qsr_msg_drop_delta > 255) ? 255 : qsr_msg_drop_delta);

      qsr_msg_drop_delta = 0;   /* Reset delta drop count */

      /*-----------------------------------------------------------------------
        Set the pointer to the constant blk, to be expanded in DIAG context.
      -----------------------------------------------------------------------*/
      msg->qsr_const_data_ptr = const_blk;
      msg->qsr_flag = (isHash & QSR_BITFLAG_IS_HASH);
    }
    else
    {
      qsr_msg_drop_delta++;
      qsr_msg_dropped++;        /* count up one more ignored message */
    }
  }

  return msg;
}               /* qsr_msg_send_prep() */

/*===========================================================================

FUNCTION QSR_INT_MSG_SEND

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
qsr_int_msg_send (const msg_qsr_const_type * const_blk, uint32 isHash)
{
  msg_qsr_store_type *msg = NULL;
  const unsigned int num_args = 0;  /* # of message arguments */
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
        byte stream_id = 0;

        stream_id = msg_stream_check(&const_blk->desc);


#ifdef FEATURE_DIAG_STM
        if MSG_STM(stream_id,stm_enabled)
        {
            /* Need to handle as an STM message by creating full packet and transmitting immediately */
            uint32 pkt_length;
            void *stm_msg;

            /* Allocate memory for msg and populate non argument fields */
            stm_msg = diag_stm_qsr_msg_prep(const_blk, num_args, isHash, &pkt_length, 0, 0);

            if (stm_msg)
            {
                /* Send data to STM */
                diag_stm_write_pkt(stm_msg,pkt_length);

                /* Free data */
                diag_stm_msg_free(stm_msg);

        /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
        stream_id &= ~DIAG_STREAM_STM;
            }
        }
#endif /* FEATURE_DIAG_STM */

        if MSG_DIAGBUF(stream_id,stm_enabled)
        {
            /* Need to allocate from diagbuf and commit to buffer */
            msg = qsr_msg_send_prep (const_blk, num_args, isHash, stream_id);

      if (msg)
      {
        diagbuf_commit (msg);
      }
    }

    }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

   if (isHash)
   {
     msg_save_trace((const void *)const_blk, NULL, 0, (void *)msg, DIAG_F3_TRACE_QSHRINK, 0, NULL);
   }
   else
   {
     msg_save_trace((const void *)const_blk, NULL, 0, (void *)msg, DIAG_F3_TRACE_QSHRINK20, 0, NULL);
   }
#endif /* FEATURE_SAVE_DEBUG_TRACE */

} /* qsr_int_msg_send */

/*===========================================================================

FUNCTION QSR_INT_MSG_SEND_1

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 1 argument.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
qsr_int_msg_send_1 ( const msg_qsr_const_type * const_blk, uint32 isHash, uint32 xx_arg1)
{
  msg_qsr_store_type *msg = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 1;  /* # of message arguments */
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[1];
#endif /* FEATURE_SAVE_DEBUG_TRACE */
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
        byte stream_id = 0;

        stream_id = msg_stream_check(&const_blk->desc);


#ifdef FEATURE_DIAG_STM
        if MSG_STM(stream_id,stm_enabled)
        {
            uint32 pkt_length;
            void *stm_msg;

            /* Allocate memory for msg and populate non argument fields */
            stm_msg = diag_stm_qsr_msg_prep(const_blk, num_args, isHash, &pkt_length, 0, 0);

            if (stm_msg)
            {
                /* set up args in struct */
                {
                    char* arg_ptr = (char *)stm_msg;
                    msg_hdr_type *msg_hdr = (msg_hdr_type*)stm_msg;
                    arg_ptr += sizeof(msg_hdr_type) + sizeof(msg_desc_type);

                    if (msg_hdr->cmd_code == DIAG_QSR_EXT_MSG_TERSE_F)
                    {
                        /* Additional offset for QSR messages */
                        arg_ptr += sizeof(uint32);
                    }
                    *(uint32*)arg_ptr = xx_arg1;
                }

                /* Send data to STM */
                diag_stm_write_pkt(stm_msg,pkt_length);

                /* Free data */
                diag_stm_msg_free(stm_msg);

        /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
        stream_id &= ~DIAG_STREAM_STM;
            }
    }
#endif /* FEATURE_DIAG_STM */

        if MSG_DIAGBUF(stream_id,stm_enabled)
        {
            /* Need to allocate from diagbuf and commit to buffer */
            msg = qsr_msg_send_prep (const_blk, num_args, isHash, stream_id);

      if (msg)
      {
      /*-----------------------------------------------------------------------
        Store the arguments in the buffer.
      -----------------------------------------------------------------------*/
        args = msg->args;
        args[0] = xx_arg1;
        diagbuf_commit (msg);
      }
    }

  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

    f3_args[0] = xx_arg1;

    if (isHash)
    {
      msg_save_trace((const void *)const_blk, f3_args, 1, (void *)msg, DIAG_F3_TRACE_QSHRINK, 0, NULL);
    }
    else
    {
      msg_save_trace((const void *)const_blk, f3_args, 1, (void *)msg, DIAG_F3_TRACE_QSHRINK20, 0, NULL);
    }

#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* qsr_int_msg_send_1 */

/*===========================================================================

FUNCTION QSR_INT_MSG_SEND_2

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 2 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
qsr_int_msg_send_2 ( const msg_qsr_const_type * const_blk, uint32 isHash, uint32 xx_arg1, uint32 xx_arg2)
{
  msg_qsr_store_type *msg = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 2;  /* # of message arguments */
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[2];
#endif /* FEATURE_SAVE_DEBUG_TRACE */
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
      byte stream_id = 0;

      stream_id = msg_stream_check(&const_blk->desc);


#ifdef FEATURE_DIAG_STM
      if MSG_STM(stream_id,stm_enabled)
      {
          uint32 pkt_length;
          void *stm_msg;

          /* Allocate memory for msg and populate non argument fields */
          stm_msg = diag_stm_qsr_msg_prep(const_blk, num_args, isHash, &pkt_length, 0, 0);
          if (stm_msg)
          {
              /* set up args in struct */
              {
                  char* arg_ptr = (char *)stm_msg;
                  msg_hdr_type *msg_hdr = (msg_hdr_type*)stm_msg;
                  arg_ptr += sizeof(msg_hdr_type) + sizeof(msg_desc_type);

                  if (msg_hdr->cmd_code == DIAG_QSR_EXT_MSG_TERSE_F)
                  {
                      /* Additional offset for QSR messages */
                      arg_ptr += sizeof(uint32);
                  }
                  *(uint32*)arg_ptr = xx_arg1;
                  arg_ptr += sizeof(uint32);
                  *(uint32*)arg_ptr = xx_arg2;
              }

              /* Send data to STM */
              diag_stm_write_pkt(stm_msg,pkt_length);

              /* Free data */
              diag_stm_msg_free(stm_msg);

        /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
        stream_id &= ~DIAG_STREAM_STM;
          }
    }
#endif /* FEATURE_DIAG_STM */

      if MSG_DIAGBUF(stream_id,stm_enabled)
      {
          /* Need to allocate from diagbuf and commit to buffer */
          msg = qsr_msg_send_prep (const_blk, num_args, isHash, stream_id);

    if (msg)
    {
      /*-----------------------------------------------------------------------
        Store the arguments in the buffer.
      -----------------------------------------------------------------------*/
      args = MSG_LARGE_ARGS(msg->args);
      args[0] = xx_arg1;
      args[1] = xx_arg2;
              diagbuf_commit (msg);
          }
      }

  }

#if defined (FEATURE_SAVE_DEBUG_TRACE)

    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;

    if (isHash)
    {
      msg_save_trace((const void *)const_blk, f3_args, 2, (void *)msg, DIAG_F3_TRACE_QSHRINK, 0, NULL);
    }
    else
    {
      msg_save_trace((const void *)const_blk, f3_args, 2, (void *)msg, DIAG_F3_TRACE_QSHRINK20, 0, NULL);
    }

#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* qsr_int_msg_send_2 */

/*===========================================================================

FUNCTION QSR_INT_MSG_SEND_3

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 3 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
qsr_int_msg_send_3 ( const msg_qsr_const_type * const_blk, uint32 isHash, uint32 xx_arg1,
        uint32 xx_arg2, uint32 xx_arg3)
{
  msg_qsr_store_type *msg = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 3;  /* # of message arguments */
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[3];
#endif /* FEATURE_SAVE_DEBUG_TRACE */
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
      byte stream_id = 0;

      stream_id = msg_stream_check(&const_blk->desc);


#ifdef FEATURE_DIAG_STM
      if MSG_STM(stream_id,stm_enabled)
      {
          uint32 pkt_length;
          void *stm_msg;

          /* Allocate memory for msg and populate non argument fields */
          stm_msg = diag_stm_qsr_msg_prep(const_blk, num_args, isHash, &pkt_length, 0, 0);

          if (stm_msg)
          {
              /* set up args in struct */
              {
                  char* arg_ptr = (char *)stm_msg;
                  msg_hdr_type *msg_hdr = (msg_hdr_type*)stm_msg;
                  arg_ptr += sizeof(msg_hdr_type) + sizeof(msg_desc_type);

                  if (msg_hdr->cmd_code == DIAG_QSR_EXT_MSG_TERSE_F)
                  {
                      /* Additional offset for QSR messages */
                      arg_ptr += sizeof(uint32);
                  }
                  *(uint32*)arg_ptr = xx_arg1;
                  arg_ptr += sizeof(uint32);
                  *(uint32*)arg_ptr = xx_arg2;
                  arg_ptr += sizeof(uint32);
                  *(uint32*)arg_ptr = xx_arg3;
              }

              /* Send data to STM */
              diag_stm_write_pkt(stm_msg,pkt_length);

              /* Free data */
              diag_stm_msg_free(stm_msg);

        /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
        stream_id &= ~DIAG_STREAM_STM;
          }
    }
#endif /* FEATURE_DIAG_STM */

      if MSG_DIAGBUF(stream_id,stm_enabled)
      {
          /* Need to allocate from diagbuf and commit to buffer */
          msg = qsr_msg_send_prep (const_blk, num_args, isHash, stream_id);

    if (msg)
    {
      /*-----------------------------------------------------------------------
        Store the arguments in the buffer.
      -----------------------------------------------------------------------*/
      args = MSG_LARGE_ARGS(msg->args);
      args[0] = xx_arg1;
      args[1] = xx_arg2;
      args[2] = xx_arg3;
              diagbuf_commit (msg);
          }
      }

  }

#if defined (FEATURE_SAVE_DEBUG_TRACE)

    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;
    f3_args[2] = xx_arg3;

    if (isHash)
    {
      msg_save_trace((const void *)const_blk, f3_args, 3, (void *)msg, DIAG_F3_TRACE_QSHRINK, 0, NULL);
    }
    else
    {
      msg_save_trace((const void *)const_blk, f3_args, 3, (void *)msg, DIAG_F3_TRACE_QSHRINK20, 0, NULL);
    }

#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* qsr_int_msg_send_3 */


/*===========================================================================

FUNCTION qsr_v3_msg_send_prep

DESCRIPTION
  This message performs the common operations used by all messages.
  It allocates, fills in all data except arguments, and returns a pointer
  to the allocated message buffer.  It also handles message statisitics.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
static void *
qsr_v3_msg_send_prep (const msg_v3_const_hash * const_blk, unsigned int num_args, uint32 isHash, byte stream_id, uint32 version, uint32 id)
{

  msg_v3_multi_client_store_type *msg_ext = NULL;
  void * msg_ptr = NULL;
  unsigned int rsp_len = 0;
  msg_v3_store_type *msg = NULL;


  rsp_len = FPOS (msg_v3_store_type, args) + num_args * FSIZ (msg_v3_store_type,
                               args[0]);
  if (version == DIAGBUF_VERSION_1)
  {
    rsp_len += FPOS(msg_v3_multi_client_store_type, msg);
  }
  msg_ptr = (void *) diagbuf_alloc (DIAG_QSR_EXT_MSG_TERSE_F, rsp_len, 0, DIAGBUF_ALLOC_REQUEST_MSG, 0, 0);

  if (msg_ptr)
  {
    diagbuf_write_stream_id (msg_ptr,stream_id);
    msg = (msg_v3_store_type*) msg_ptr;
  }
  else
  {
    msg_drop_delta++;
    msg_dropped++;        /* count up one more ignored message */
    return NULL;
  }

  if (version == DIAGBUF_VERSION_1)
  {
    /*Fill out the header for DSDA msg pkts*/
    msg_ext = (msg_v3_multi_client_store_type*) msg_ptr;
    msg_ext->cmd_code = DIAG_CMD_EXT_F;
    msg_ext->version = DIAGBUF_VERSION_1;
    msg_ext->id = id;
    msg_ext->proc_id = DIAG_MY_PROC_ID;
    msg_ext->msg.hdr.cmd_code = DIAG_QSR_EXT_MSG_TERSE_F;
    msg = &(msg_ext->msg);
  }
  else
      {
    msg_ext = NULL;
  }

  /*-----------------------------------------------------------------------
      Get a pointer to a buffer.  If it's a NULL pointer, there aren't any
      free buffers.  No effort is made to save higher priority messages in
      favor of lower priority messages, as message flooding hides enough
      details to make the usable margin on such a feature too small for
      implementation.
    -----------------------------------------------------------------------*/

  if (msg)
  {
/*lint -save -e{545} Suspicious use of & */
    msg_get_time(&(msg->hdr.ts));
/*lint -restore */

    if(diag_wcdma_wl1_frame_cb != NULL)
      msg->hdr.ts_type = MSG_TS_TYPE_GW;
    else
      msg->hdr.ts_type = MSG_TS_TYPE_CDMA_FULL;

    msg->hdr.num_args = (uint8)num_args;

    msg->hdr.drop_cnt = (uint8) ((qsr_msg_drop_delta > 255) ? 255 : qsr_msg_drop_delta);

    qsr_msg_drop_delta = 0;   /* Reset delta drop count */

    /*-----------------------------------------------------------------------
        Set the pointer to the constant blk, to be expanded in DIAG context.
      -----------------------------------------------------------------------*/
    msg->qsr_const_data_ptr.v3_index = const_blk;
    msg->qsr_flag = QSR_BITFLAG_IS_V3|(QSR_BITFLAG_IS_HASH & isHash);
  }

  if (version==DIAGBUF_VERSION_1)
  {
    return (void *)msg_ext;
  }
  else
  {
    return (void *)msg;
  }

  return NULL;
}
              /* qsr_v3_msg_send_prep() */

/*===========================================================================

FUNCTION QSR_V3_INT_MSG_SEND

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
qsr_v3_int_msg_send ( const msg_v3_const_hash * const_blk, uint32 isHash, uint32 version, uint32 id)
{
  msg_v3_store_type *msg = NULL;
  msg_v3_multi_client_store_type *msg_ext = NULL;
  int num_args = 0;
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    byte stream_id = 0;

   stream_id = msg_v3_stream_check(const_blk);

#ifdef FEATURE_DIAG_STM
        if MSG_STM(stream_id,stm_enabled)
        {
            uint32 pkt_length;
            void *stm_msg;

            /* Allocate memory for msg and populate non argument fields */
            stm_msg = diag_stm_qsr_v3_msg_prep(const_blk, num_args, isHash, &pkt_length, version, id);

            if (stm_msg)
            {
                /* Send data to STM */
                diag_stm_write_pkt(stm_msg,pkt_length);

                /* Free data */
                diag_stm_msg_free(stm_msg);

        /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
        stream_id &= ~DIAG_STREAM_STM;
            }
        }
#endif /* FEATURE_DIAG_STM */

    if(version == DIAGBUF_VERSION_0)
    {
      if MSG_DIAGBUF(stream_id,stm_enabled)
      {
        msg = (msg_v3_store_type *)qsr_v3_msg_send_prep(const_blk, 0, isHash, stream_id, 0, 0);
      }

    if (msg)
    {
      diagbuf_commit (msg);
    }
  }
    else if(version == DIAGBUF_VERSION_1)
    {
      if MSG_DIAGBUF(stream_id,stm_enabled)
      {
        msg_ext = (msg_v3_multi_client_store_type *)qsr_v3_msg_send_prep(const_blk, 0, isHash, stream_id, version, id);
      }

      if(msg_ext)
      {
        diagbuf_commit(msg_ext);
      }
    }



  }

#if defined (FEATURE_SAVE_DEBUG_TRACE)
  if((version > DIAGBUF_VERSION_0) && (version <= DIAGBUF_MAX_VERSION_NUM))
  {
	  msg_save_trace((void *)const_blk, NULL, 0, (void *)msg_ext, DIAG_F3_TRACE_QSHRINK30, (uint8)version, (void *)&id);	  
  }
  else
  {
 	msg_save_trace((void *)const_blk, NULL, 0, (void *)msg, DIAG_F3_TRACE_QSHRINK30, 0, NULL ); 
  }
#endif /* FEATURE_SAVE_DEBUG_TRACE */

} /* qsr_v3_int_msg_send */

/*===========================================================================

FUNCTION QSR_V3_INT_MSG_SEND_1

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 1 argument.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
qsr_v3_int_msg_send_1(const msg_v3_const_hash *const_blk, uint32 isHash, uint32 version, uint32 id, uint32 xx_arg1)
{
  msg_v3_store_type *msg = NULL;
  msg_v3_multi_client_store_type *msg_ext = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 1;  /* # of message arguments */
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[1];
#endif /* FEATURE_SAVE_DEBUG_TRACE */
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
   byte stream_id = 0;

        stream_id = msg_v3_stream_check(const_blk);

#ifdef FEATURE_DIAG_STM
        if MSG_STM(stream_id,stm_enabled)
        {
            uint32 pkt_length;
            void *stm_msg;

            /* Allocate memory for msg and populate non argument fields */
            stm_msg = diag_stm_qsr_v3_msg_prep(const_blk, num_args, isHash, &pkt_length, version, id);

            if (stm_msg)
            {
                /* set up args in struct */
                {
                    char* arg_ptr = (char *)stm_msg;
                    msg_hdr_type *msg_hdr = (msg_hdr_type*)stm_msg;
                    arg_ptr += sizeof(msg_hdr_type) + sizeof(msg_desc_type);

          if(version == DIAGBUF_VERSION_1)
          {
            msg_v3_multi_client_store_type *msg_ext = (msg_v3_multi_client_store_type*) stm_msg;
            /* Add padding for multclient message header */
            arg_ptr += FPOS(msg_v3_multi_client_store_type, msg);

            /* Update msg pointer to correct location */
            msg_hdr = &(msg_ext->msg.hdr);
          }

                    if (msg_hdr->cmd_code == DIAG_QSR_EXT_MSG_TERSE_F)
                    {
                        /* Additional offset for QSR messages */
                        arg_ptr += sizeof(uint32);
                    }
                    *(uint32*)arg_ptr = xx_arg1;
                }

                /* Send data to STM */
                diag_stm_write_pkt(stm_msg,pkt_length);

                /* Free data */
                diag_stm_msg_free(stm_msg);

        /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
        stream_id &= ~DIAG_STREAM_STM;
            }
        }
#endif /* FEATURE_DIAG_STM */

   if(version == DIAGBUF_VERSION_0)
   {
     if MSG_DIAGBUF(stream_id,stm_enabled)
     {
       msg = (msg_v3_store_type *)qsr_v3_msg_send_prep(const_blk, num_args, isHash, stream_id, 0, 0);
     }

    if (msg)
    {
      /*-----------------------------------------------------------------------
        Store the arguments in the buffer.
      -----------------------------------------------------------------------*/
      args = msg->args;

      args[0] = xx_arg1;

      diagbuf_commit (msg);
    }
  }
    else if (version == DIAGBUF_VERSION_1)
   {
     if MSG_DIAGBUF(stream_id,stm_enabled)
     {
       msg_ext = (msg_v3_multi_client_store_type *)qsr_v3_msg_send_prep(const_blk, num_args, isHash, stream_id, version, id);
     }

      if (msg_ext)
      {
        /*-----------------------------------------------------------------------
          Store the arguments in the buffer.
        -----------------------------------------------------------------------*/
        args = msg_ext->msg.args;

        args[0] = xx_arg1;

        diagbuf_commit(msg_ext);
      }
    }

  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

  f3_args[0] = xx_arg1;

  if((version > DIAGBUF_VERSION_0) && (version <= DIAGBUF_MAX_VERSION_NUM))
  {
	msg_save_trace((void *)const_blk, f3_args, 1, (void *)msg_ext, DIAG_F3_TRACE_QSHRINK30, (uint8)version, (void *)&id);	  
  }
  else
  {
 	msg_save_trace((void *)const_blk, f3_args, 1, (void *)msg, DIAG_F3_TRACE_QSHRINK30, 0, NULL ); 
  }

#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* qsr_v3_int_msg_send_1 */

/*===========================================================================

FUNCTION QSR_V3_INT_MSG_SEND_2

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 2 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
qsr_v3_int_msg_send_2(const msg_v3_const_hash *const_blk, uint32 isHash, uint32 version, uint32 id, uint32 xx_arg1, uint32 xx_arg2)
{
  msg_v3_store_type *msg = NULL;
  msg_v3_multi_client_store_type *msg_ext = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 2;  /* # of message arguments */
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[2];
#endif /* FEATURE_SAVE_DEBUG_TRACE */
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
   byte stream_id = 0;

      stream_id = msg_v3_stream_check(const_blk);

#ifdef FEATURE_DIAG_STM
      if MSG_STM(stream_id,stm_enabled)
      {
          uint32 pkt_length;
          void *stm_msg;

          /* Allocate memory for msg and populate non argument fields */
          stm_msg = diag_stm_qsr_v3_msg_prep(const_blk, num_args, isHash, &pkt_length, version, id);
          if (stm_msg)
          {
              /* set up args in struct */
              {
                  char* arg_ptr = (char *)stm_msg;
                  msg_hdr_type *msg_hdr = (msg_hdr_type*)stm_msg;
                  arg_ptr += sizeof(msg_hdr_type) + sizeof(msg_desc_type);

          if(version == DIAGBUF_VERSION_1)
          {
            msg_v3_multi_client_store_type *msg_ext = (msg_v3_multi_client_store_type*) stm_msg;
            /* Add padding for multclient message header */
            arg_ptr += FPOS(msg_v3_multi_client_store_type, msg);

            /* Update msg pointer to correct location */
            msg_hdr = &(msg_ext->msg.hdr);
          }

          if (msg_hdr->cmd_code == DIAG_QSR_EXT_MSG_TERSE_F)
                  {
                      /* Additional offset for QSR messages */
                      arg_ptr += sizeof(uint32);
                  }
                  *(uint32*)arg_ptr = xx_arg1;
                  arg_ptr += sizeof(uint32);
                  *(uint32*)arg_ptr = xx_arg2;
              }

              /* Send data to STM */
              diag_stm_write_pkt(stm_msg,pkt_length);

              /* Free data */
              diag_stm_msg_free(stm_msg);

        /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
        stream_id &= ~DIAG_STREAM_STM;
      }
      }
#endif /* FEATURE_DIAG_STM */

    if(version == DIAGBUF_VERSION_0)
    {
      if MSG_DIAGBUF(stream_id,stm_enabled)
      {
        msg = (msg_v3_store_type *)qsr_v3_msg_send_prep(const_blk, num_args, isHash, stream_id, version, id);
      }

    if (msg)
    {
      /*-----------------------------------------------------------------------
        Store the arguments in the buffer.
      -----------------------------------------------------------------------*/
      args = MSG_LARGE_ARGS(msg->args);

      args[0] = xx_arg1;
      args[1] = xx_arg2;

      diagbuf_commit (msg);
    }
  }
    else if (version == DIAGBUF_VERSION_1)
    {
      if MSG_DIAGBUF(stream_id,stm_enabled)
      {
        msg_ext = (msg_v3_multi_client_store_type *)qsr_v3_msg_send_prep(const_blk, num_args, isHash, stream_id, version, id);
      }

      if (msg_ext)
      {
        /*-----------------------------------------------------------------------
          Store the arguments in the buffer.
        -----------------------------------------------------------------------*/
        args = MSG_LARGE_ARGS(msg_ext->msg.args);

        args[0] = xx_arg1;
        args[1] = xx_arg2;

        diagbuf_commit(msg_ext);
      }
    }

  }

#if defined (FEATURE_SAVE_DEBUG_TRACE)

    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;
	
  if((version > DIAGBUF_VERSION_0) && (version <= DIAGBUF_MAX_VERSION_NUM))
  {
	msg_save_trace((void *)const_blk, f3_args, 2, (void *)msg_ext, DIAG_F3_TRACE_QSHRINK30, (uint8)version, (void *)&id);	  
  }
  else
  {
 	msg_save_trace((void *)const_blk, f3_args, 2, (void *)msg, DIAG_F3_TRACE_QSHRINK30, 0, NULL ); 
  }

#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* qsr_v3_int_msg_send_2 */

/*===========================================================================

FUNCTION QSR_V3_INT_MSG_SEND_3

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 3 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
qsr_v3_int_msg_send_3(const msg_v3_const_hash *const_blk, uint32 isHash, uint32 version, uint32 id, uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3)
{
  msg_v3_store_type *msg = NULL;
  msg_v3_multi_client_store_type *msg_ext = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 3;  /* # of message arguments */
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[3];
#endif /* FEATURE_SAVE_DEBUG_TRACE */
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    byte stream_id = 0;

      stream_id = msg_v3_stream_check(const_blk);

#ifdef FEATURE_DIAG_STM
      if MSG_STM(stream_id,stm_enabled)
      {
          uint32 pkt_length;
          void *stm_msg;

          /* Allocate memory for msg and populate non argument fields */
          stm_msg = diag_stm_qsr_v3_msg_prep(const_blk, num_args, isHash, &pkt_length, version, id);

          if (stm_msg)
          {
              /* set up args in struct */
              {
                  char* arg_ptr = (char *)stm_msg;
                  msg_hdr_type *msg_hdr = (msg_hdr_type*)stm_msg;
                  arg_ptr += sizeof(msg_hdr_type) + sizeof(msg_desc_type);

          if(version == DIAGBUF_VERSION_1)
          {
            msg_v3_multi_client_store_type *msg_ext = (msg_v3_multi_client_store_type*) stm_msg;
            /* Add padding for multclient message header */
            arg_ptr += FPOS(msg_v3_multi_client_store_type, msg);

            /* Update msg pointer to correct location */
            msg_hdr = &(msg_ext->msg.hdr);
          }

          if (msg_hdr->cmd_code == DIAG_QSR_EXT_MSG_TERSE_F)
                  {
                      /* Additional offset for QSR messages */
                      arg_ptr += sizeof(uint32);
                  }
                  *(uint32*)arg_ptr = xx_arg1;
                  arg_ptr += sizeof(uint32);
                  *(uint32*)arg_ptr = xx_arg2;
                  arg_ptr += sizeof(uint32);
                  *(uint32*)arg_ptr = xx_arg3;
              }

              /* Send data to STM */
              diag_stm_write_pkt(stm_msg,pkt_length);

              /* Free data */
              diag_stm_msg_free(stm_msg);

        /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
        stream_id &= ~DIAG_STREAM_STM;
          }
      }
#endif /* FEATURE_DIAG_STM */

    if(version == DIAGBUF_VERSION_0)
    {
      if MSG_DIAGBUF(stream_id,stm_enabled)
        {
          msg = (msg_v3_store_type *)qsr_v3_msg_send_prep(const_blk, num_args, isHash, stream_id, 0, 0);
      }

    if (msg)
    {
      /*-----------------------------------------------------------------------
        Store the arguments in the buffer.
      -----------------------------------------------------------------------*/
      args = MSG_LARGE_ARGS(msg->args);

      args[0] = xx_arg1;
      args[1] = xx_arg2;
      args[2] = xx_arg3;
        diagbuf_commit(msg);
      }
    }
    else if (version == DIAGBUF_VERSION_1)
    {
      if MSG_DIAGBUF(stream_id,stm_enabled)
      {
        msg_ext = (msg_v3_multi_client_store_type *)qsr_v3_msg_send_prep(const_blk, num_args, isHash, stream_id, version, id);
      }

      if (msg_ext)
      {
        /*-----------------------------------------------------------------------
          Store the arguments in the buffer.
        -----------------------------------------------------------------------*/
        args = MSG_LARGE_ARGS(msg_ext->msg.args);

        args[0] = xx_arg1;
        args[1] = xx_arg2;
        args[2] = xx_arg3;
        diagbuf_commit(msg_ext);
    }
  }

  }

#if defined (FEATURE_SAVE_DEBUG_TRACE)

    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;
    f3_args[2] = xx_arg3;
	
  if((version > DIAGBUF_VERSION_0) && (version <= DIAGBUF_MAX_VERSION_NUM))
  {
	msg_save_trace((void *)const_blk, f3_args, 3, (void *)msg_ext, DIAG_F3_TRACE_QSHRINK30, (uint8)version, (void *)&id);	  
  }
  else
  {
 	msg_save_trace((void *)const_blk, f3_args, 3, (void *)msg, DIAG_F3_TRACE_QSHRINK30, 0, NULL ); 
  }

#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* qsr_v3_int_msg_send_3 */

/*--- Previous QShrink version stubs BEGIN ---*/
void qsr_msg_send ( const msg_qsr_const_type * xx_msg_const_ptr)
{
}
void qsr_msg_send_1 (const msg_qsr_const_type * xx_msg_const_ptr, uint32 xx_arg1)
{
}
void qsr_msg_send_2 ( const msg_qsr_const_type * xx_msg_const_ptr,uint32 xx_arg1,
    uint32 xx_arg2)
{
}
void qsr_msg_send_3 ( const msg_qsr_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2, uint32 xx_arg3)
{
}
void qsr_msg_send_var ( const msg_qsr_const_type * xx_msg_const_ptr, uint32 num_args, ...)
{
}


/*===========================================================================

FUNCTION MSG_V2_SEND

DESCRIPTION
  This function is similar to QSR_MSG_SEND function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v2_send ( const msg_v2_const_type * const_blk)
{
  qsr_int_msg_send((msg_qsr_const_type *)const_blk, MSG_CHECK_IS_HASH(const_blk->msg));

} /* msg_v2_send */

/*===========================================================================

FUNCTION MSG_V2_SEND_1

DESCRIPTION
  This function is similar to QSR_MSG_SEND_1 function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v2_send_1 ( const msg_v2_const_type * const_blk,uint32 xx_arg1)
{
  qsr_int_msg_send_1((msg_qsr_const_type *)const_blk,
    MSG_CHECK_IS_HASH(const_blk->msg),xx_arg1);

} /* msg_v2_send_1 */


/*===========================================================================

FUNCTION MSG_V2_SEND_2

DESCRIPTION
  This function is similar to QSR_MSG_SEND_2 function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v2_send_2 ( const msg_v2_const_type * const_blk, uint32 xx_arg1, uint32 xx_arg2)
{
  qsr_int_msg_send_2((msg_qsr_const_type *)const_blk,
    MSG_CHECK_IS_HASH(const_blk->msg),xx_arg1, xx_arg2);

} /* msg_v2_send_2 */

/*===========================================================================

FUNCTION MSG_V2_SEND_3

DESCRIPTION
  This function is similar to QSR_MSG_SEND_3 function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v2_send_3 ( const msg_v2_const_type * const_blk, uint32 xx_arg1,
        uint32 xx_arg2, uint32 xx_arg3)
{
  qsr_int_msg_send_3((msg_qsr_const_type *)const_blk,
    MSG_CHECK_IS_HASH(const_blk->msg),xx_arg1,xx_arg2,xx_arg3);

} /* msg_v2_send_3 */


/*===========================================================================

FUNCTION MSG_V2_SEND_VAR

DESCRIPTION
  This function is similar to QSR_MSG_SEND_VAR function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_v2_send_var (const msg_v2_const_type * const_blk,uint32 num_args, ...)
{
  msg_qsr_store_type *msg = NULL;
  uint32 *args = NULL;
  va_list arg_list;     /* ptr to the variable argument list */
  unsigned int i;
  msg_qsr_const_type *qsr_const_blk = (msg_qsr_const_type *)const_blk;
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[MAX_VAR_ARGS];  // Assuming at most MAX_VAR_ARGS arguments
#endif

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
      byte stream_id = 0;

      stream_id = msg_stream_check(&const_blk->desc);

#ifdef FEATURE_DIAG_STM
      if MSG_STM(stream_id,stm_enabled)
      {
          uint32 pkt_length;
          void *stm_msg;

          /* Allocate memory for msg and populate non argument fields */
          stm_msg = diag_stm_qsr_msg_prep(qsr_const_blk, num_args,
                                          MSG_CHECK_IS_HASH(const_blk->msg), &pkt_length, 0, 0);
          if (stm_msg)
          {
              /* set up args in struct */
              char* arg_ptr = (char *)stm_msg;
              msg_hdr_type *msg_hdr = (msg_hdr_type*)stm_msg;
              arg_ptr += sizeof(msg_hdr_type) + sizeof(msg_desc_type);

        if (msg_hdr->cmd_code == DIAG_QSR_EXT_MSG_TERSE_F)
              {
                  /* Additional offset for QSR messages */
                  arg_ptr += sizeof(uint32);
              }

              /*lint -save -e{64,10,718,746,628} */
              /* Initialize variable arguments */
              va_start (arg_list, num_args);

              /* Store arguments from variable list. */
              for (i = 0; i < MIN(num_args,MAX_VAR_ARGS); i++)
              {
                  *(uint32*)arg_ptr = va_arg (arg_list, uint32);
                  arg_ptr += sizeof(uint32);
              }               /* for */

              /* Reset variable arguments */
              va_end (arg_list);
              /*lint -restore */

              /* Send data to STM */
              diag_stm_write_pkt(stm_msg,pkt_length);

              /* Free data */
              diag_stm_msg_free(stm_msg);

        /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
        stream_id &= ~DIAG_STREAM_STM;
          }
      }
#endif /* FEATURE_DIAG_STM */

      if MSG_DIAGBUF(stream_id,stm_enabled)
      {
          /* Need to allocate from diagbuf and commit to buffer */
    msg = qsr_msg_send_prep (qsr_const_blk, num_args,
                                   MSG_CHECK_IS_HASH(const_blk->msg), stream_id);

    if (msg)
    {
      /*-----------------------------------------------------------------------
        Store the arguments in the buffer.
      -----------------------------------------------------------------------*/

      args = MSG_LARGE_ARGS(msg->args);

  /*lint -save -e{64,10,78,718,746,628,732,530} */
      /* Initialize variable arguments */
      va_start (arg_list, num_args);
  /*lint -restore */

      /* Store arguments from variable list. */
  /*lint -save -e{64,10,78,718,746,628,732,530} */
      for (i = 0; i < num_args; i++)
      {
        args[i] = va_arg (arg_list, uint32);
      }               /* for */
  /*lint -restore */
      /* Reset variable arguments */
      va_end (arg_list);

      diagbuf_commit (msg);
    }
  }


  }

#if defined (FEATURE_SAVE_DEBUG_TRACE)

/*lint -save -e{64,10,718,746,628} */
  /* Initialize variable arguments */
  va_start (arg_list, num_args);

  /* Store arguments from variable list. */
  for (i = 0; i < MIN(num_args,MAX_VAR_ARGS); i++)
  {
    f3_args[i] = va_arg (arg_list, uint32);
  }               /* for */

  /* Reset variable arguments */
  va_end (arg_list);
/*lint -restore */
  msg_save_trace((const void *)const_blk, f3_args, MIN(num_args,MAX_VAR_ARGS), (void *)msg, DIAG_F3_TRACE_QSHRINK20, 0, NULL);

#endif /* FEATURE_SAVE_DEBUG_TRACE */

} /* msg_send_var */




/*--- Previous QShrink version stubs END ---*/

/*===========================================================================

FUNCTION MSG_V3_SEND

DESCRIPTION
  This function is similar to QSR_MSG_SEND function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v3_send ( const msg_v3_const_type * const_blk)
{
  const msg_v3_const_hash *hash_const_blk = &(const_blk->qsr_const_blk);
  if (hash_const_blk->ssid_index >= msg_v3_ss_num_table_entries)
  {
    return;
  }
  qsr_v3_int_msg_send(hash_const_blk, MSG_CHECK_IS_HASH(hash_const_blk->msg), 0, 0);

} /* msg_v3_send */

/*===========================================================================

FUNCTION MSG_V3_SEND_EXT

DESCRIPTION
  This function is similar to MSG_V3_SEND function, expect that it takes
  additional parameters : version and void *. Based on the version value,
  void* can be interpreted as follows-
  Version      Pointer
     0          NULL
     1          uint32*

  In future, if there is a need for F3 macros to take additional parameters,
  a new version ID can be added and void* can be interpreted differently
  as per the requirements.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v3_send_ext ( const msg_v3_const_type * xx_msg_const_ptr, uint32 version, void* parameter)
{
  const msg_v3_const_hash *hash_const_blk = &(xx_msg_const_ptr->qsr_const_blk);
    uint32 *id = NULL;
  if (hash_const_blk->ssid_index >= msg_v3_ss_num_table_entries)
  {
    return;
  }

  switch (version)
  {
     case DIAGBUF_VERSION_0:
            qsr_v3_int_msg_send(hash_const_blk, MSG_CHECK_IS_HASH(hash_const_blk->msg),0, 0);
       break;
     case DIAGBUF_VERSION_1:
            if (parameter == NULL)
            {
              return;
            }
            id = (uint32 *) parameter;
            qsr_v3_int_msg_send(hash_const_blk, MSG_CHECK_IS_HASH(hash_const_blk->msg), version, *id);
       break;
     default: return;
  }
} /* msg_v3_send_ext */

/*===========================================================================

FUNCTION MSG_V3_SEND_1

DESCRIPTION
  This function is similar to QSR_MSG_SEND_1 function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v3_send_1 ( const msg_v3_const_type * const_blk,uint32 xx_arg1)
{
  const msg_v3_const_hash *hash_const_blk = &(const_blk->qsr_const_blk);
  if (hash_const_blk->ssid_index >= msg_v3_ss_num_table_entries)
  {
    return;
  }
  qsr_v3_int_msg_send_1(hash_const_blk, MSG_CHECK_IS_HASH(hash_const_blk->msg), 0, 0, xx_arg1);

} /* msg_v3_send_1 */

/*===========================================================================
FUNCTION MSG_V3_SEND_1_EXT

DESCRIPTION
  This function is similar to MSG_V3_SEND_1 function, expect that it takes
  additional parameters : version and void *. Based on the version value,
  void* can be interpreted as follows-
  Version      Pointer
     0          NULL
     1          uint32*

  In future, if there is a need for F3 macros to take additional parameters,
  a new version ID can be added and void* can be interpreted differently
  as per the requirements.

  The msg value is a pointer to a string, if its value is divisible by 4,
  else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).
===========================================================================*/
void msg_v3_send_1_ext ( const msg_v3_const_type * xx_msg_const_ptr, uint32 version, void* parameter,
                           uint32 xx_arg1)
{
  const msg_v3_const_hash *hash_const_blk = &(xx_msg_const_ptr->qsr_const_blk);
  uint32 *id = NULL;

  if (hash_const_blk->ssid_index >= msg_v3_ss_num_table_entries)
  {
    return;
  }

  switch (version)
  {
     case DIAGBUF_VERSION_0:
            qsr_v3_int_msg_send_1(hash_const_blk, MSG_CHECK_IS_HASH(hash_const_blk->msg),0, 0, xx_arg1);
       break;
     case DIAGBUF_VERSION_1:
            if (parameter == NULL)
            {
              return;
            }
            id =  (uint32 *) parameter;
            qsr_v3_int_msg_send_1(hash_const_blk, MSG_CHECK_IS_HASH(hash_const_blk->msg), version, *id, xx_arg1);
       break;
    default: return;
  }
} /* msg_v3_send_1_ext */

/*===========================================================================

FUNCTION MSG_V3_SEND_2

DESCRIPTION
  This function is similar to QSR_MSG_SEND_2 function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v3_send_2 ( const msg_v3_const_type * const_blk, uint32 xx_arg1, uint32 xx_arg2)
{
  const msg_v3_const_hash *hash_const_blk = &(const_blk->qsr_const_blk);
  if (hash_const_blk->ssid_index >= msg_v3_ss_num_table_entries)
  {
    return;
  }

  qsr_v3_int_msg_send_2(hash_const_blk, MSG_CHECK_IS_HASH(hash_const_blk->msg), 0, 0, xx_arg1, xx_arg2);

} /* msg_v3_send_2 */
/*===========================================================================
FUNCTION MSG_V3_SEND_2_EXT

DESCRIPTION
  This function is similar to MSG_V3_SEND_2 function, expect that it takes
  additional parameters : version and void *. Based on the version value,
  void* can be interpreted as follows-
  Version      Pointer
     0          NULL
     1          uint32*

  In future, if there is a need for F3 macros to take additional parameters,
  a new version ID can be added and void* can be interpreted differently
  as per the requirements.


  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v3_send_2_ext(const msg_v3_const_type *xx_msg_const_ptr, uint32 version, void *parameter,
                       uint32 xx_arg1, uint32 xx_arg2)
{
  const msg_v3_const_hash *hash_const_blk = &(xx_msg_const_ptr->qsr_const_blk);
  uint32 *id = NULL;
  if (hash_const_blk->ssid_index >= msg_v3_ss_num_table_entries)
  {
    return;
  }

  switch (version)
  {
     case DIAGBUF_VERSION_0:
            qsr_v3_int_msg_send_2(hash_const_blk, MSG_CHECK_IS_HASH(hash_const_blk->msg),0, 0, xx_arg1, xx_arg2);
       break;
     case DIAGBUF_VERSION_1:
            if (parameter == NULL)
            {
              return;
            }
            id = (uint32 *) parameter;
            qsr_v3_int_msg_send_2(hash_const_blk, MSG_CHECK_IS_HASH(hash_const_blk->msg), version, *id, xx_arg1, xx_arg2);
       break;
    default: return;
  }
} /* msg_v3_send_2_ext */

/*===========================================================================

FUNCTION MSG_V3_SEND_3

DESCRIPTION
  This function is similar to QSR_MSG_SEND_3 function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v3_send_3 ( const msg_v3_const_type * const_blk, uint32 xx_arg1,
        uint32 xx_arg2, uint32 xx_arg3)
{
  const msg_v3_const_hash *hash_const_blk = &(const_blk->qsr_const_blk);
  if (hash_const_blk->ssid_index >= msg_v3_ss_num_table_entries)
  {
    return;
  }

  qsr_v3_int_msg_send_3(hash_const_blk, MSG_CHECK_IS_HASH(hash_const_blk->msg),0,0,xx_arg1,xx_arg2,xx_arg3);

} /* msg_v3_send_3 */

/*===========================================================================

FUNCTION MSG_V3_SEND_3_EXT

DESCRIPTION
  This function is similar to MSG_V3_SEND_2 function, expect that it takes
  additional parameters : version and void *. Based on the version value,
  void* can be interpreted as follows-
  Version      Pointer
     0          NULL
     1          uint32*

  In future, if there is a need for F3 macros to take additional parameters,
  a new version ID can be added and void* can be interpreted differently
  as per the requirements.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v3_send_3_ext(const msg_v3_const_type *xx_msg_const_ptr, uint32 version, void *parameter,
                       uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3)
{
  const msg_v3_const_hash *hash_const_blk = &(xx_msg_const_ptr->qsr_const_blk);
  uint32 *id = NULL;

  if (hash_const_blk->ssid_index >= msg_v3_ss_num_table_entries)
  {
    return;
  }

  switch (version)
  {
     case DIAGBUF_VERSION_0:
            qsr_v3_int_msg_send_3(hash_const_blk, MSG_CHECK_IS_HASH(hash_const_blk->msg),0, 0, xx_arg1, xx_arg2, xx_arg3);
       break;
     case DIAGBUF_VERSION_1:
            if (parameter == NULL)
            {
               return;
            }
            id = (uint32 *) parameter;
            qsr_v3_int_msg_send_3(hash_const_blk, MSG_CHECK_IS_HASH(hash_const_blk->msg), version, *id, xx_arg1, xx_arg2, xx_arg3);
       break;
    default: return;
  }
} /* msg_v3_send_3_ext */

/*===========================================================================

FUNCTION MSG_V3_SEND_VAR

DESCRIPTION
  This function is similar to QSR_MSG_SEND_VAR function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_v3_send_var (const msg_v3_const_type * const_blk, uint32 num_args, ...)
{
  const msg_v3_const_hash *hash_const_blk = &(const_blk->qsr_const_blk);
  msg_v3_store_type *msg = NULL;
  uint32 *args = NULL;
  va_list arg_list;     /* ptr to the variable argument list */
  unsigned int i;
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[MAX_VAR_ARGS];  // Assuming at most MAX_VAR_ARGS arguments
#endif

  if (hash_const_blk->ssid_index >= msg_v3_ss_num_table_entries)
  {
    return;
  }

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    byte stream_id = 0;

    stream_id = msg_v3_stream_check(hash_const_blk);

#ifdef FEATURE_DIAG_STM
    if MSG_STM(stream_id,stm_enabled)
      {
        uint32 pkt_length;
        void *stm_msg;

        /* Allocate memory for msg and populate non argument fields */
        stm_msg = diag_stm_qsr_v3_msg_prep(hash_const_blk, num_args,
          MSG_CHECK_IS_HASH(hash_const_blk->msg), &pkt_length, 0, 0);
        if (stm_msg)
        {
          /* set up args in struct */
          char* arg_ptr = (char *)stm_msg;
          msg_hdr_type *msg_hdr = (msg_hdr_type*)stm_msg;
          arg_ptr += sizeof(msg_hdr_type) + sizeof(msg_desc_type);

          if (msg_hdr->cmd_code == DIAG_QSR_EXT_MSG_TERSE_F)
          {
            /* Additional offset for QSR messages */
            arg_ptr += sizeof(uint32);
          }

          /*lint -save -e{64,10,718,746,628} */
          /* Initialize variable arguments */
          va_start (arg_list, num_args);

          /* Store arguments from variable list. */
          for (i = 0; i < MIN(num_args,MAX_VAR_ARGS); i++)
          {
            *(uint32*)arg_ptr = va_arg (arg_list, uint32);
            arg_ptr += sizeof(uint32);
          }               /* for */

          /* Reset variable arguments */
          va_end (arg_list);
          /*lint -restore */

          /* Send data to STM */
          diag_stm_write_pkt(stm_msg,pkt_length);

          /* Free data */
          diag_stm_msg_free(stm_msg);

          /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
          stream_id &= ~DIAG_STREAM_STM;
        }
      }
#endif /* FEATURE_DIAG_STM */

    if MSG_DIAGBUF(stream_id,stm_enabled)
      {
    msg = qsr_v3_msg_send_prep (hash_const_blk, num_args,
                                 MSG_CHECK_IS_HASH(hash_const_blk->msg), stream_id, 0, 0);

    if (msg)
    {
          /*-------------------------------------------------------------------
        Store the arguments in the buffer.
           ------------------------------------------------------------------*/
      args = MSG_LARGE_ARGS(msg->args);

  /*lint -save -e{64,10,78,718,746,628,732,530} */
      /* Initialize variable arguments */
      va_start (arg_list, num_args);
  /*lint -restore */

      /* Store arguments from variable list. */
  /*lint -save -e{64,10,78,718,746,628,732,530} */
      for (i = 0; i < num_args; i++)
      {
        args[i] = va_arg (arg_list, uint32);
      }               /* for */
  /*lint -restore */
      /* Reset variable arguments */
      va_end (arg_list);

      diagbuf_commit (msg);
    }
  }


  }

#if defined (FEATURE_SAVE_DEBUG_TRACE)

/*lint -save -e{64,10,718,746,628} */
  /* Initialize variable arguments */
  va_start (arg_list, num_args);

  /* Store arguments from variable list. */
  for (i = 0; i < MIN(num_args,MAX_VAR_ARGS); i++)
  {
    f3_args[i] = va_arg (arg_list, uint32);
  }               /* for */

  /* Reset variable arguments */
  va_end (arg_list);
/*lint -restore */
  //XXX TODO: don't cast like this --rh
  msg_save_trace((const void *)hash_const_blk, f3_args, MIN(num_args,MAX_VAR_ARGS), (void *)msg, DIAG_F3_TRACE_QSHRINK30, 0, NULL);
#endif

} /* msg_send_var */


/*===========================================================================

FUNCTION MSG_V3_SEND_VAR_EXT

DESCRIPTION
  This function is similar to QSR_MSG_SEND_VAR function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_v3_send_var_ext(const msg_v3_const_type *const_blk, uint32 version, void *parameter, uint32 num_args, ...)
{
  va_list arg_list;     /* ptr to the variable argument list */
  uint32 args[9];
  int i = 0;
  uint8 args_count = MIN(num_args, MAX_VAR_ARGS);
  const msg_v3_const_hash *hash_const_blk = &(const_blk->qsr_const_blk);
  msg_v3_multi_client_store_type *msg_ext = NULL;
  uint32 *args_ext = NULL;
  uint32 *id = NULL;
  byte stream_id = 0;
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[MAX_VAR_ARGS];  // Assuming at most MAX_VAR_ARGS arguments
#endif

  if (msg_enabled_mask == 0 || msg_service_init != TRUE)
      return;

  if (hash_const_blk->ssid_index >= msg_v3_ss_num_table_entries)
  {
    return;
  }

    stream_id = msg_v3_stream_check(hash_const_blk);

  va_start(arg_list, num_args);
  /* Store arguments from variable list. */
  for (i = 0; i < args_count; i++)
  {
    args[i] = va_arg(arg_list, uint32);
  }
  switch (version)
  {
     case DIAGBUF_VERSION_0:
       switch (args_count)
       {
          case 4:
            msg_v3_send_var(const_blk, num_args, args[0], args[1], args[2], args[3]);
            break;
          case 5:
            msg_v3_send_var(const_blk, num_args, args[0], args[1], args[2], args[3], args[4]);
            break;
          case 6:
            msg_v3_send_var(const_blk, num_args, args[0], args[1], args[2], args[3], args[4], args[5]);
            break;
          case 7:
            msg_v3_send_var(const_blk, num_args, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
            break;
          case 8:
            msg_v3_send_var(const_blk, num_args, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
            break;
          case 9:
            msg_v3_send_var(const_blk, num_args, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8]);
            break;
       }

       va_end(arg_list);
       break;

     case DIAGBUF_VERSION_1:
       if (msg_enabled_mask != 0)
       {
         if (parameter == NULL)
         {
           return;
         }
         id = (uint32 *)parameter;


#ifdef FEATURE_DIAG_STM
         if MSG_STM(stream_id,stm_enabled)
           {
             uint32 pkt_length;
             void *stm_msg;

             /* Allocate memory for msg and populate non argument fields */
             stm_msg = diag_stm_qsr_v3_msg_prep(hash_const_blk, num_args,
                       MSG_CHECK_IS_HASH(hash_const_blk->msg), &pkt_length, version, *id);

             if (stm_msg)
             {
               /* set up args in struct */
               char* arg_ptr = (char *)stm_msg + FPOS(msg_v3_multi_client_store_type, msg);
               msg_hdr_type *msg_hdr = (msg_hdr_type*)(arg_ptr);

               arg_ptr += sizeof(msg_hdr_type) + sizeof(msg_desc_type);

               if (msg_hdr->cmd_code == DIAG_QSR_EXT_MSG_TERSE_F)
               {
                 /* Additional offset for QSR messages */
                 arg_ptr += sizeof(uint32);
               }

               /*lint -save -e{64,10,718,746,628} */
               /* Initialize variable arguments */
               va_start (arg_list, num_args);

               /* Store arguments from variable list. */
               for (i = 0; i < MIN(num_args,MAX_VAR_ARGS); i++)
               {
                 *(uint32*)arg_ptr = va_arg (arg_list, uint32);
                 arg_ptr += sizeof(uint32);
               }               /* for */

               /* Reset variable arguments */
               va_end (arg_list);
               /*lint -restore */

               /* Send data to STM */
               diag_stm_write_pkt(stm_msg,pkt_length);

               /* Free data */
               diag_stm_msg_free(stm_msg);

               /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
               stream_id &= ~DIAG_STREAM_STM;
             }
         }
    #endif /* FEATURE_DIAG_STM */

         if MSG_DIAGBUF(stream_id,stm_enabled)
         {
           msg_ext = (msg_v3_multi_client_store_type *)qsr_v3_msg_send_prep(hash_const_blk, num_args,
                                                                   MSG_CHECK_IS_HASH(hash_const_blk->msg), stream_id, version, *id);
         }

         if (msg_ext)
         {
           /*-----------------------------------------------------------------------
             Store the arguments in the buffer.
           -----------------------------------------------------------------------*/
           args_ext = MSG_LARGE_ARGS(msg_ext->msg.args);

           for (i = 0; i < args_count; i++)
           {
             args_ext[i] = args[i];
           }
           diagbuf_commit(msg_ext);
         }


       }

#if defined (FEATURE_SAVE_DEBUG_TRACE)


       /* Store arguments from variable list. */
       for (i = 0; i < args_count; i++)
       {
         f3_args[i] = args[i];
       }


       //XXX TODO: don't cast like this --rh
    msg_save_trace((const void *)hash_const_blk, f3_args, args_count, (void *)msg_ext, DIAG_F3_TRACE_QSHRINK30, (uint8)version, parameter);
#endif

       break;
     default:
       return;
  }

}
/*===========================================================================

FUNCTION       msg_errlog_

DESCRIPTION  These functions send the desired f3 message in addition to
        invoking err_put_log. This reduces it one function call for top
        level macro.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/*
void msg_errlog_3 (const msg_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3)
{
 msg_send_3 (const_blk, (uint32)code1, (uint32)code2, (uint32)code3);
 err_put_log (const_blk->desc.line, const_blk->fname);
}

void msg_errlog_2 (const msg_const_type* const_blk, uint32 code1, uint32 code2)
{
 msg_send_2 (const_blk, (uint32)code1, (uint32)code2);
 err_put_log (const_blk->desc.line, const_blk->fname);
}

void msg_errlog_1 (const msg_const_type* const_blk, uint32 code1)
{
 msg_send_1 (const_blk, (uint32)code1);
 err_put_log (const_blk->desc.line, const_blk->fname);
}

void msg_errlog_0 (const msg_const_type* const_blk)
{
 msg_send (const_blk);
 err_put_log (const_blk->desc.line, const_blk->fname);
}
*/

/*===========================================================================

FUNCTION       msg_qsrerrlog_

DESCRIPTION    These functions send the desired f3 message in addition to
        invoking err_put_log. This reduces it one function call for top
        level macro.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void msg_v2_errlog_3 (const err_msg_v2_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3)
{
 msg_v2_send_3 (&(const_blk->msg_v2_const_blk), (uint32)code1, (uint32)code2, (uint32)code3);
 err_put_log (const_blk->msg_v2_const_blk.desc.line, const_blk->fname);
}

void msg_v2_errlog_2 (const err_msg_v2_const_type* const_blk, uint32 code1, uint32 code2)
{
 msg_v2_send_2 (&(const_blk->msg_v2_const_blk), (uint32)code1, (uint32)code2);
 err_put_log (const_blk->msg_v2_const_blk.desc.line, const_blk->fname);
}

void msg_v2_errlog_1 (const err_msg_v2_const_type* const_blk, uint32 code1)
{
 msg_v2_send_1 (&(const_blk->msg_v2_const_blk), (uint32)code1);
 err_put_log (const_blk->msg_v2_const_blk.desc.line, const_blk->fname);
}

void msg_v2_errlog_0 (const err_msg_v2_const_type* const_blk)
{
 msg_v2_send (&(const_blk->msg_v2_const_blk));
 err_put_log (const_blk->msg_v2_const_blk.desc.line, const_blk->fname);
}


void msg_v3_errlog_3 (const err_msg_v3_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3)
{
 msg_v3_send_3 (&(const_blk->msg_v3_const_blk), (uint32)code1, (uint32)code2, (uint32)code3);
 err_put_log (const_blk->msg_v3_const_blk.qsr_const_blk.line, const_blk->fname);
}

void msg_v3_errlog_2 (const err_msg_v3_const_type* const_blk, uint32 code1, uint32 code2)
{
 msg_v3_send_2 (&(const_blk->msg_v3_const_blk), (uint32)code1, (uint32)code2);
 err_put_log (const_blk->msg_v3_const_blk.qsr_const_blk.line, const_blk->fname);
}

void msg_v3_errlog_1 (const err_msg_v3_const_type* const_blk, uint32 code1)
{
 msg_v3_send_1 (&(const_blk->msg_v3_const_blk), (uint32)code1);
 err_put_log (const_blk->msg_v3_const_blk.qsr_const_blk.line, const_blk->fname);
}

void msg_v3_errlog_0 (const err_msg_v3_const_type* const_blk)
{
 msg_v3_send (&(const_blk->msg_v3_const_blk));
 err_put_log (const_blk->msg_v3_const_blk.qsr_const_blk.line, const_blk->fname);
}

/*===========================================================================

FUNCTION       msg_qsrerrlog_

DESCRIPTION    These functions similar to msg_v3_errlog_x expect that it takes

DESCRIPTION
  These functions are similar to msg_v3_errlog_x functions, expect that it takes
  additional parameters : version and void *. Based on the version value,
  void* can be interpreted as follows-
  Version      Pointer
     0          NULL
     1          uint32*

  In future, if there is a need for F3 macros to take additional parameters,
  a new version ID can be added and void* can be interpreted differently
  as per the requirements.


DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v3_errlog_3_ext (const err_msg_v3_const_type* const_blk, uint32 version, void* parameter, uint32 code1, uint32 code2, uint32 code3)
{
 msg_v3_send_3_ext (&(const_blk->msg_v3_const_blk), version, parameter, (uint32)code1, (uint32)code2, (uint32)code3);
 err_put_log (const_blk->msg_v3_const_blk.qsr_const_blk.line, const_blk->fname);
}

void msg_v3_errlog_2_ext (const err_msg_v3_const_type* const_blk, uint32 version, void* parameter, uint32 code1, uint32 code2)
{
 msg_v3_send_2_ext (&(const_blk->msg_v3_const_blk), version, parameter,(uint32)code1, (uint32)code2);
 err_put_log (const_blk->msg_v3_const_blk.qsr_const_blk.line, const_blk->fname);
}

void msg_v3_errlog_1_ext (const err_msg_v3_const_type* const_blk, uint32 version, void* parameter, uint32 code1)
{
 msg_v3_send_1_ext (&(const_blk->msg_v3_const_blk), version, parameter, (uint32)code1);
 err_put_log (const_blk->msg_v3_const_blk.qsr_const_blk.line, const_blk->fname);
}

void msg_v3_errlog_0_ext (const err_msg_v3_const_type* const_blk, uint32 version, void* parameter)
{
 msg_v3_send_ext (&(const_blk->msg_v3_const_blk), version, parameter);
 err_put_log (const_blk->msg_v3_const_blk.qsr_const_blk.line, const_blk->fname);
}


/*===========================================================================

FUNCTION DIAG_F3_LISTENER_MATCH_AND_FREE

DESCRIPTION
  This routine is passed to and called by diag_searchlist_delete() for each
  node of the search until this routine returns TRUE or the search is
  exhausted.  This routine is responsible for marking the 'entry' with the
  remove flag which was allocated in diag_add_f3_listener().

===========================================================================*/
static boolean diag_f3_listener_match_and_free (diag_searchlist_node_type * match_node,
                                                diag_searchlist_node_type * list_node)
{
  boolean found = FALSE;
  diag_f3_listener_entry_type * match = NULL;
  diag_f3_listener_entry_type * entry = NULL;

  if (match_node && list_node)
  {
    match = (diag_f3_listener_entry_type *) match_node->goo;
    entry = (diag_f3_listener_entry_type *) list_node->goo;

    if (match && entry)
    {
      if (match->listener == entry->listener &&
          match->ss_mask == entry->ss_mask &&
          match->param == entry->param && FALSE == list_node->remove_flag)
      {
        found = TRUE;
        list_node->remove_flag = TRUE;

#if defined(FEATURE_DEBUG_DIAG_HEAP)
        diag_f3_listener_match_and_free_cnt++;
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "FREE diag_f3_listener_match_and_free = %d ",
                  diag_f3_listener_match_and_free_cnt);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
      }
    }
  }

  return found;

} /* diag_f3_listener_match_and_free */



/*===========================================================================

FUNCTION DIAG_ADD_F3_LISTENER

DESCRIPTION
  This routine registers a function to be called when the specified F3
  with matching mask is generated by DMSS/AMSS.

  'param' will be passed unmodified to the registered function when called.

  Duplicate entries are allowed.

RETURN VALUE
  TRUE - listener was successfully added to the listener table.
  FALSE - Listener was not added due to lack of resources.

===========================================================================*/
boolean diag_add_f3_listener (const unsigned int ssid,
                              const unsigned int ss_mask,
                              const diag_f3_listener listener, void *param)
{
  diag_f3_listener_entry_type *entry = NULL;
  boolean success = FALSE;
  int i = 0;
  boolean found = FALSE; /* Finding the ssid node to update the listener mask*/

  if( ssid == 0 )
  {
    MSG( MSG_SSID_LEGACY, MSG_LEGACY_ERROR, "diag_add_f3_listener: Listener for Legacy F3s not allowed." );
    return FALSE;
  }

  entry = (diag_f3_listener_entry_type *) diag_listeners_malloc (sizeof (diag_f3_listener_entry_type));

  if (entry)
  {
#if defined(FEATURE_DEBUG_DIAG_HEAP)
    diag_add_f3_listener_malloc_cnt++;
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "MALLOC diag_add_f3_listener_malloc = %d, bytes allocated = %d",
        diag_add_f3_listener_malloc_cnt, sizeof (diag_f3_listener_entry_type));
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    entry->listener = listener;
    entry->ss_mask = ss_mask;
    entry->param = param;

    success = diag_searchlist_add (&diag_f3_listeners, ssid, entry);

    if (success)
    {
      /* Assume the critical section is initialized (success, right?). */
      osal_enter_crit_sect (&diag_f3_listeners.crit_sect);
       if(NULL == diag_f3_listeners.listeners_list_to_delete)
      {
        diag_f3_listeners.listeners_list_to_delete = diag_f3_listeners_heap_to_delete;
      }

      if (diag_f3_listeners.count > 0)
      {
        DIAG_SET_MASK (diag_cx_state, DIAG_CX_F3_LISTEN_S);
        DIAG_SET_MASK (msg_enabled_mask, DIAG_MSG_LISTENER);
      }
          /* Update the listener msg mask for this ssid */
      for(i = 0 ; !found && i < LISTENER_MSG_MASK_TBL_CNT; i++)
      {
        if( (ssid >= listener_msg_mask_tbl[i].ssid_first) && (ssid <= listener_msg_mask_tbl[i].ssid_last))
        {
          found = TRUE;
          listener_msg_mask_tbl[i].f3_listener_mask_array[ssid - listener_msg_mask_tbl[i].ssid_first] |= ss_mask;
        }
      }

      osal_exit_crit_sect (&diag_f3_listeners.crit_sect);
    }
    else
    {
      /* unable to add, free 'entry' */
      diag_listeners_free ((void *) entry);
#if defined(FEATURE_DEBUG_DIAG_HEAP)
      diag_add_f3_listener_free_cnt++;
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            " Unable to add f3 listener, freeing it,  \
              diag_add_free_listener_free_cnt = %d",
             diag_add_f3_listener_free_cnt);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
    }
  }

  return success;

} /* diag_add_f3_listener */


/*===========================================================================

FUNCTION DIAG_REMOVE_F3_LISTENER

DESCRIPTION
  This routine unregisters a function from the F3 listener table.
  'listener' will no longer be called when the F3 SSID is
  'ssid', 'ss_mask', 'listener', and 'param' are used to match a registered
  listener.

  If duplicate entries exist, only one is removed.

RETURN VALUE
  Boolean indicating the success of the operation.  If FALSE, the entry was
  not found in the listener table.

===========================================================================*/
boolean diag_remove_f3_listener (const unsigned int ssid,
                                 const unsigned int ss_mask,
                                 const diag_f3_listener listener, void *param)
{
  diag_f3_listener_entry_type entry;
  diag_searchlist_node_type node;
  boolean success = FALSE;
    int i = 0;
  boolean found = FALSE; /* For finding the ssid node to update the listeners masks*/
  boolean status = FALSE; /* For other listeners registered with the ssid and ss_mask */

  entry.listener = listener;
  entry.ss_mask = ss_mask;
  entry.param = param;

  node.key = ssid;
  node.goo = (void *) &entry;

  success = diag_searchlist_delete (&diag_f3_listeners, &node,
                                    diag_f3_listener_match_and_free);

  if (success && diag_f3_listeners.initialized)
  {
    osal_enter_crit_sect (&diag_f3_listeners.crit_sect);


    /* Check if there is any other listener for this ssid and ss_mask*/
    status = diag_searchlist_search_all( ssid, &diag_f3_listeners ,NULL , NULL);
    if(FALSE == status)
    {
      /* Update the listeners mask */
      for(i = 0 ; !found && i < LISTENER_MSG_MASK_TBL_CNT; i++)
      {
        if( (ssid >= listener_msg_mask_tbl[i].ssid_first) && (ssid <= listener_msg_mask_tbl[i].ssid_last))
        {
          found = TRUE;
          listener_msg_mask_tbl[i].f3_listener_mask_array[ssid - listener_msg_mask_tbl[i].ssid_first] &= ~ss_mask ;
        }
      }
    }

    osal_exit_crit_sect (&diag_f3_listeners.crit_sect);
  }

  return success;

} /* diag_remove_f3_listener */


/*===========================================================================

FUNCTION DIAG_F3_LISTENER_NOTIFY

DESCRIPTION
  This routine is called by the searchlist when a matching listener node is
  found.  It is called once per entry.

  This routine is a wrapper to call the registered listeners.

===========================================================================*/
void diag_f3_listener_notify (uint32 seq_num, void * msg, diag_searchlist_node_type * node)
{
  diag_f3_listener_entry_type * entry;
  diag_msg_type *msg_notify = (diag_msg_type *) msg;
  msg_v3_store_type * msg_store_qsr = NULL;
  msg_ext_store_type * msg_store_ext = NULL;
  msg_ext_type       *ext_sprintf_msg = NULL;
  uint32 ss_mask = 0;

  if (node && msg)
  {
    entry = (diag_f3_listener_entry_type *) node->goo;

    if( msg_notify->cmd_code == DIAG_EXT_MSG_TERSE_F )
    {
      msg_store_ext = (msg_ext_store_type *)msg_notify->msg_store.ext_msg;
      if( msg_store_ext && msg_store_ext->const_data_ptr )
      {
        ss_mask = msg_store_ext->const_data_ptr->desc.ss_mask;
      }
    }
    else if( msg_notify->cmd_code == DIAG_QSR_EXT_MSG_TERSE_F )
    {
      msg_store_qsr = (msg_v3_store_type *)msg_notify->msg_store.qsr_msg;
      if( msg_store_qsr && msg_store_qsr->qsr_const_data_ptr.v3_index &&
        msg_store_qsr->qsr_const_data_ptr.v3_index->ssid_index < msg_v3_ss_num_table_entries )
      {
      /* qshrink3 */
        ss_mask = msg_v3_ss_table[msg_store_qsr->qsr_const_data_ptr.v3_index->ssid_index]->ss_mask;
      }
    }
    else if(msg_notify->cmd_code == DIAG_EXT_MSG_F)
    {
      ext_sprintf_msg = (msg_ext_type*)msg_notify->msg_store.ext_sprintf_msg;
      if(ext_sprintf_msg && ext_sprintf_msg->args)
      {
        ss_mask = ext_sprintf_msg->desc.ss_mask;
      }
    }

    //Further filter by subsystem mask
    if( (entry->ss_mask & ss_mask) > 0 )
    {
      if (entry->listener && (node->remove_flag == FALSE))
      {
        entry->listener (seq_num, msg_notify, entry->param);
      }
    }
  }
} /* diag_f3_listener_notify */


/*===========================================================================


FUNCTION DIAG_ADD_F3_LISTENER_EXT

DESCRIPTION
    This routine registers a function to be called by the DIAG Task when any of the SSID+Mask pair in the keys array
    matches with the F3 generated by DMSS/AMSS.

    'param' will be passed unmodified to the registered function when called.

    Duplicate entries are allowed.

RETURN VALUE
    TRUE - listener was successfully added to the listener table.
    FALSE - Listener was not added due to lack of resources.


===========================================================================*/

boolean diag_add_f3_listener_ext(f3_listener_ext_key_type *keys,
                                      const unsigned int num_keys,
                                      const diag_f3_listener_ext listener,
                                      void * param)
{
  f3_searchlist_ext_entry_type *ssid_node = NULL;
  f3_listener_ext_mask_entry *listener_node = NULL;
  unsigned int f3_listener_key_index = 0;
  boolean f3_listener_ext_ssid_node_created = FALSE; //True when a SSID node is created because it is not already present in the global list.
  diag_f3_listener_ext_entry_type listener_entry = {NULL, 0, NULL};
  boolean listener_ext_init = diag_f3_listeners_ext.initialized;
  int i = 0;
  boolean found = FALSE; /* For finding the ssid node to update the listener mask*/


  if(!listener)
  {
    return FALSE;
  }

   while (!listener_ext_init)
  {
    listener_ext_init = diag_f3_listeners_ext_init();
    /* Register the clean up function to be called in diag idle processing */
    diag_idle_processing_register(diag_clean_f3_listener_ext, NULL);
  }


/* For each entry in the  array of ssid+mask provided by the client */
for(f3_listener_key_index = 0; f3_listener_key_index < num_keys; f3_listener_key_index++)
{

  found = FALSE;
  /* Check for SSID and Mask*/
  if(keys[f3_listener_key_index].ssid == MSG_SSID_LEGACY || keys[f3_listener_key_index].ss_mask == 0)
  {
    continue;
  }
   /* We need to lock down the listener list from here to prevent it from being changed while we are using it. */
   osal_enter_crit_sect(&diag_f3_listeners_ext.lock);

  /* Check if a node is already present in the global list */
  ssid_node = (f3_searchlist_ext_entry_type*)ordered_list_linear_search(&diag_f3_listeners_ext.list, diag_f3_ssid_compare_func, &(keys[f3_listener_key_index].ssid));

  if(ssid_node == NULL)
  {

    /*Create a SSID node to be inserted in the global listeners list*/
    ssid_node = (f3_searchlist_ext_entry_type*)diag_listeners_malloc(sizeof(f3_searchlist_ext_entry_type));

    if(ssid_node)
    {
          f3_listener_ext_ssid_node_created = TRUE;
          memset(ssid_node, 0, sizeof(f3_searchlist_ext_entry_type));

          /*Update the SSID in the node created.*/
          ssid_node->entry.ssid = keys[f3_listener_key_index].ssid;

          /* Initialize the listeners List.*/
          list_init(&ssid_node->entry.listener_list);
    }
    else
    {
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Cannot allocate memory for ssid = %d and ss_mask = %d",
                                          keys[f3_listener_key_index].ssid, keys[f3_listener_key_index].ss_mask);
      osal_exit_crit_sect(&diag_f3_listeners_ext.lock);

      return FALSE;
    }
  }

  listener_entry.listener = listener;
  listener_entry.param = param;

  /* Check if this listener is already registered by the client */
  listener_node = list_linear_search(&ssid_node->entry.listener_list, diag_f3_listener_compare_func, (void*)&listener_entry);

  /* if this listener is already registered, Just update the mask */

  if(listener_node != NULL)
  {
      /* if the listener is marked to be freed, overwrite the old SS Mask with current value*/
    if(TRUE == listener_node->remove_flag)
    {
      listener_node->ss_mask = keys[f3_listener_key_index].ss_mask;
      /* Reset the remove flag of the listener */
      listener_node->remove_flag = FALSE;
    }
    else
    {
      /* else OR the old ss mask with the current value*/
    listener_node->ss_mask |= keys[f3_listener_key_index].ss_mask;
    }
  }
  else
  {
    /* Create a Listener node */
    listener_node = (f3_listener_ext_mask_entry*)diag_listeners_malloc(sizeof(f3_listener_ext_mask_entry));

    if(listener_node)
    {
      memset(listener_node, 0, sizeof(f3_listener_ext_mask_entry));
      /* Initialize the remove flag of the listener */
      listener_node->remove_flag = FALSE;

      /* Update the fields in the listener node */
      listener_node->ss_mask = keys[f3_listener_key_index].ss_mask;
      listener_node->listener = listener;
      listener_node->param = param;

      /* Push the listener node  to the list of listeners of this SSID */
      list_push_back(&ssid_node->entry.listener_list, &listener_node->link);

      /* Insert the SSID node (if created) in the global list */
      if(f3_listener_ext_ssid_node_created == TRUE)
      {
        f3_listener_ext_ssid_node_created = FALSE;
        ordered_list_push(&diag_f3_listeners_ext.list, &ssid_node->link, ssid_node->entry.ssid);

        /* Updating the count of SSID nodes in the global list */
        diag_f3_listeners_ext.count++;

        DIAG_SET_MASK (diag_cx_state, DIAG_CX_F3_LISTEN_EXT_S);
        DIAG_SET_MASK (msg_enabled_mask, DIAG_MSG_EXT_LISTENER);

      }

    }
    else
    {
      MSG_3(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Cannot allocate memory for listener: ssid = %d, ss_mask = %d, param = %d",
                                        keys[f3_listener_key_index].ssid, keys[f3_listener_key_index].ss_mask, param);
      if(f3_listener_ext_ssid_node_created == TRUE)
      {
        list_destroy(&ssid_node->entry.listener_list);
        diag_listeners_free(ssid_node);
      }
      osal_exit_crit_sect(&diag_f3_listeners_ext.lock);
      return FALSE;
    }
  }
  /* Update the listener masks for this ssid node  */
  for(i = 0 ; !found && i < LISTENER_MSG_MASK_TBL_CNT; i++)
  {
    if( (keys[f3_listener_key_index].ssid >= listener_msg_mask_tbl[i].ssid_first) && (keys[f3_listener_key_index].ssid <= listener_msg_mask_tbl[i].ssid_last))
    {
      found = TRUE;
      listener_msg_mask_tbl[i].f3_listener_ext_mask_array[keys[f3_listener_key_index].ssid - listener_msg_mask_tbl[i].ssid_first] |= keys[f3_listener_key_index].ss_mask;
    }
  }
   osal_exit_crit_sect(&diag_f3_listeners_ext.lock);
}

  return TRUE;
}

/*===========================================================================

FUNCTION DIAG_REMOVE_F3_LISTENER_EXT

DESCRIPTION
  This routine marks a function to be deleted from the F3 listener table.
  'listener' will no longer be called for any F3 generated by DMSS/AMSS.

  If duplicate entries exist, all will be marked to be deleted.

RETURN VALUE
  Boolean indicating the success of the operation.  If FALSE, the entry was
  not found in the listener table.

===========================================================================*/
boolean diag_remove_f3_listener_ext (const diag_f3_listener_ext listener, void * param)
{
  f3_searchlist_ext_entry_type* ssid_node = NULL;
  f3_listener_ext_mask_entry* listener_node = NULL;
  f3_listener_ext_mask_entry* first_node = NULL;
  diag_f3_listener_ext_entry_type listener_entry;
  int i = 0;
  boolean found = FALSE; /* For finding the ssid node to update the listeners mask */
  boolean listener_found = FALSE;
  int ssid_index = 0; /* ssid index in the listeners mask array */

  /* If Listener is NULL or the global list is not yet initialized */
  if(!listener || !diag_f3_listeners_ext.initialized)
  {
    return FALSE;
  }

  /* We need to lock down the listener list from here to prevent it from being changed while we are using it. */
  osal_enter_crit_sect(&diag_f3_listeners_ext.lock);
  ssid_node = (f3_searchlist_ext_entry_type*)ordered_list_peek_front(&diag_f3_listeners_ext.list);

  while(ssid_node)
  {
    found = FALSE;
    listener_entry.listener= listener;
    listener_entry.param= param;

    listener_node = (f3_listener_ext_mask_entry*)list_linear_search(&ssid_node->entry.listener_list, diag_f3_listener_compare_func, (void*)&listener_entry);

    if( listener_node && (FALSE == listener_node->remove_flag) )
    {
      /* Found the listener with matching listener call back and param */
      /* Update the remove flag to TRUE*/
      listener_node->remove_flag = TRUE;
      /* Zero the mask value for this node */
      listener_node->ss_mask = 0;
      /* Increment the number of listeners to be deleted */
      diag_f3_listeners_ext.free_count++;
      /* Find the SSID node in the listener mask array to update the mask*/
      for(i = 0 ; !found && i < LISTENER_MSG_MASK_TBL_CNT; i++)
      {
        if( (ssid_node->entry.ssid >= listener_msg_mask_tbl[i].ssid_first) && (ssid_node->entry.ssid <= listener_msg_mask_tbl[i].ssid_last))
        {
          found = TRUE;
        }
      }
      ssid_index = i-1;
      first_node = (f3_listener_ext_mask_entry*)list_peek_front(&ssid_node->entry.listener_list);
      /* Parse through all the listener nodes under this SSID node to update the listener mask*/
      while(first_node)
      {
        listener_msg_mask_tbl[ssid_index].f3_listener_ext_mask_array[ssid_node->entry.ssid - listener_msg_mask_tbl[ssid_index].ssid_first] |= first_node->ss_mask;
        first_node = (f3_listener_ext_mask_entry*)list_peek_next(&ssid_node->entry.listener_list, (list_link_type*)first_node );
      }
      listener_found = TRUE;
    }
    ssid_node = (f3_searchlist_ext_entry_type*)ordered_list_peek_next(&diag_f3_listeners_ext.list, (ordered_list_link_type*)ssid_node);
  }
  osal_exit_crit_sect(&diag_f3_listeners_ext.lock);
  return listener_found;
}
/*===========================================================================
FUNCTION DIAG_CLEAN_F3_LISTENER_EXT
DESCRIPTION
  This routine unregisters a function from the F3 listener table which are marked to be removed.
  'listener' will no longer be called for any F3 generated by DMSS/AMSS.

  If duplicate entries exist, all will be removed.
RETURN VALUE
void
===========================================================================*/
void diag_clean_f3_listener_ext (void *param)
{
  f3_searchlist_ext_entry_type* ssid_node = NULL;
  f3_searchlist_ext_entry_type* next_ssid_node = NULL;
  f3_listener_ext_mask_entry* listener_node = NULL;
  f3_listener_ext_mask_entry* next_listener_node = NULL;
  if(!diag_f3_listeners_ext.initialized)
  {
    return;
  }
  if(diag_f3_listeners_ext.free_count > 0)
  {
    osal_enter_crit_sect(&diag_f3_listeners_ext.lock);
    ssid_node = (f3_searchlist_ext_entry_type*)ordered_list_peek_front(&diag_f3_listeners_ext.list);
    while(ssid_node)
    {
        next_ssid_node = (f3_searchlist_ext_entry_type*)ordered_list_peek_next(&diag_f3_listeners_ext.list,(ordered_list_link_type *)ssid_node);
        listener_node = (f3_listener_ext_mask_entry*)list_peek_front(&ssid_node->entry.listener_list);
        while(listener_node)
        {
            /* Get this here because we might soon be freeing ssid_node */
            next_listener_node = (f3_listener_ext_mask_entry*)list_peek_next(&ssid_node->entry.listener_list, (list_link_type*)listener_node);

            if( TRUE == listener_node->remove_flag)
            {
                /* Found the listener with matching listener call back and param */
                list_pop_item(&ssid_node->entry.listener_list, (list_link_type*)listener_node);
                /* Free the listeners node */
                diag_listeners_free(listener_node);

                /* Decrement the count of listeners to be deleted */
                diag_f3_listeners_ext.free_count--;
                /* If there are no listeners in the list, remove the SSID node from the Global List */
                if(ssid_node->entry.listener_list.size == 0)
                {
                    /* Destroy the list of listenes added for this SSID */
                    list_destroy(&ssid_node->entry.listener_list);
                    /* Remove the SSID node from the global listeners list */
                    ordered_list_pop_item(&diag_f3_listeners_ext.list, (ordered_list_link_type*)ssid_node);
                    /* Decrement the number of SSID nodes in diag_f3_listeners_ext list */
                    diag_f3_listeners_ext.count--;

                    if(diag_f3_listeners_ext.count == 0)
                    {
                        DIAG_CLR_MASK (diag_cx_state, DIAG_CX_F3_LISTEN_EXT_S);
                        DIAG_CLR_MASK (msg_enabled_mask, DIAG_MSG_EXT_LISTENER);
                    }
                    /* Free the ssid node */
                    diag_listeners_free(ssid_node);
                    break;
                }
            }
            listener_node = next_listener_node;
        }
        /* Return from the function if the free count becomes 0*/
        if(diag_f3_listeners_ext.free_count == 0)
        {
            osal_exit_crit_sect(&diag_f3_listeners_ext.lock);
            return;
        }
        ssid_node = next_ssid_node;
    }
    osal_exit_crit_sect(&diag_f3_listeners_ext.lock);
  }
}


/*===========================================================================

FUNCTION DIAG_F3_SEARCHLIST_EXT

DESCRIPTION
  This routine calls the listeners from the F3 listener table which is
  registered with the SSID ssid and mask ss_mask.

.. It calls all the listeners (with different params) registered for this
  SSID and mask

RETURN VALUE
  Boolean indicating the success of the operation.  If FALSE, the entry was
  not found in the listener table.

===========================================================================*/

boolean diag_f3_searchlist_ext(unsigned int ssid, unsigned int ss_mask, diag_msg_type *msg)
{
  f3_searchlist_ext_entry_type *ssid_node = NULL;
  f3_listener_ext_mask_entry *listener_node = NULL;
  boolean listener_registered = FALSE;

  /* Check if the list is initialized */
  if(!diag_f3_listeners_ext.initialized)
  {
    return FALSE;
  }

   /* We need to lock down the listener list from here to prevent it from being changed
        while we are using it. */
  osal_enter_crit_sect(&diag_f3_listeners_ext.lock);

  /* Search for the node with the SSID in the argument */
  ssid_node = ordered_list_linear_search(&diag_f3_listeners_ext.list, diag_f3_ssid_compare_func, &ssid);

  if(ssid_node)
  {
    listener_node = (f3_listener_ext_mask_entry*)list_peek_front(&ssid_node->entry.listener_list);
    while(listener_node)
    {

      if(listener_node->ss_mask & ss_mask)
      {
        if(msg && (FALSE == listener_node->remove_flag)  )
        {
          listener_node->listener(msg, listener_node->param);
        }

        listener_registered = TRUE;
      }
            /* Search through all the listeners in this SSID node and call all the listeners with matching mask*/
      listener_node = list_peek_next(&ssid_node->entry.listener_list, (list_link_type*)listener_node);
    }

  }
  osal_exit_crit_sect(&diag_f3_listeners_ext.lock);

  return listener_registered;
}

/* Compare functions needed for searching in the ordered list */

static int diag_f3_ssid_compare_func(void* item_ptr, void* compare_val)
{
  f3_searchlist_ext_entry_type *ssid_node = (f3_searchlist_ext_entry_type*)item_ptr;
  unsigned int *compare_ssid_val = (unsigned int*)compare_val;

  if(ssid_node == NULL || compare_val == NULL)
  {
    return 0;
  }

/* Return 1 if SSID matches. */
  if(ssid_node->entry.ssid == *compare_ssid_val)
    return 1;
  else
    return 0;
}


static int diag_f3_listener_compare_func(void* item_ptr, void* compare_val)
{
  f3_listener_ext_mask_entry *listener_node = (f3_listener_ext_mask_entry*)item_ptr;
  diag_f3_listener_ext_entry_type *listener_entry = (diag_f3_listener_ext_entry_type*)compare_val;

  if(listener_node == NULL || compare_val == NULL)
  {
    return 0;
  }

/* Check if there is a listener entry already with this listener and param */
  if(listener_node->listener == listener_entry->listener && listener_node->param == listener_entry->param)
    return 1;
  else
    return 0;
}

/* ==========================================================================
FUNCTION diag_f3_listeners_ext_init

DESCRIPTION
Initializes the diag_f3_listeners_ext list and also the critical section for
diag_f3_listeners_ext.

RETURN VALUE
  Boolean indicating the success of the operation.

============================================================================= */
static boolean diag_f3_listeners_ext_init()
{
    if(!osal_atomic_compare_and_set((osal_atomic_word_t *)&diag_f3_ext_init_start, FALSE, TRUE))
        return FALSE;

    ordered_list_init(&diag_f3_listeners_ext.list,ORDERED_LIST_ASCENDING,ORDERED_LIST_PUSH_SLT);
    osal_init_crit_sect(&diag_f3_listeners_ext.lock);
    diag_f3_listeners_ext.initialized = 1;

    return TRUE;
}

