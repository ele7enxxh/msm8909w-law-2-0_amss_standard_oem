
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Mobile Diagnostic Message Services

General Description
  All the declarations, definitions, and routines necessary to support
  reporting messages for errors and debugging.

Initialzation and Sequencing Requirements
  msg_init() must be called prior to msg_put().

Copyright (c) 1992-2015 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                           Edit History 


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/16/15   ph      Corrected binary search logic in msg_update_ss_table().
07/20/15   ph      Fixed compiler warnings.
03/12/15   ph      Handle the update of rt mask when SSID entries are missing
                   in qshrink table.
01/23/15   ph      Fixed msg_update_ss_table() such that negative index of
                   qshrink table is not accessed.
12/17/14   ph      Removed diag_strlcpy() and replaced with strlcpy().
01/08/15   rs      Added a flag for msg_init(). 
07/30/14   ph      Avoid additional mask check while draining.
07/06/14   ph      Fixed KW errors.
05/30/14   ph      Fixed the exit condition in msg_update_ss_table().
04/11/14   ph      Compiler warning fixes.
04/01/14   rs      Added support for build mask centralization.
02/06/14   xy      Replace memcpy() with memscpy()   
10/14/13   sr      Added support for extended event, log and F3 pkts  
09/25/13   ph      Fixed the issue with memscpy call where dest_length is improper.
09/18/13   sa      Support for listener masks
09/13/13   xy      Fixed KW P1 issues 
07/29/13   rh      Fixed calls to msg_update_ss_table to actually give full mask
06/08/13   sa      Fixed Issues responding to V1 and V2 Diag Test Commands
05/15/13   sr      Fixed Klockwork warnings
05/02/13   rh      QShrink 3.0
04/09/13   sr      Fixed the compiler warnings 
04/03/13   sr      Replace memcpy() and memmove() with memscpy() and memsmove()
03/28/13   sr      Fixed Klockwork warnings  
12/19/12   rh      Fixed Klockwork errors 
11/29/12   is      Fix array access issue in msg_masks_enabled_check()
11/15/12   is      Support for preset masks
08/14/12   ra      Implemented Binary Search to determine the msg_mask_tbl 
                   index for a given SSID 
08/10/12   sa      Mainlined DIAG_CENTRAL_ROUTING
07/09/12   sa      F3 messages sent to communication layer only if
                   the RT Mask is enabled.
07/10/12   ra      F3 Mask Check Optimization  
07/09/12   sa      F3 messages sent to communication layer only if
                   the RT Mask is enabled.
06/13/12   sa      Support for Extended F3 Listeners
05/16/12   rh      Support for F3 listeners 
03/07/12   rh      Fixed Klockwork Errors
11/07/11   is      Fixed compiler warnings
10/07/11   is      Master Diag saves and sends master msg mask to slaves
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/15/11   is      Modifed "set mask" via CTRL msgs fot EA support
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
07/22/11   is      Do not send partial DIAG_QSR_EXT_MSG_TERSE_F packets, which
                   causes QCAT crash on invalid pkt format
07/20/11   is      Support for QShrink 2.0
03/18/11   hm      Diag sleep vote bug fix 
03/01/11   is      Migrate to diag_strlcpy()
12/20/10   mad     Changes for dual mask diag
12/21/10   hm      Dual mask changes  
09/17/10   is      Migrate from deprecated clk driver APIs
08/25/10   sg      Fixed compiler warnings
08/24/10   sg      Featurized registrations for msg_qsr_expand_and_send_pkt()
                   and msg_expand_and_send_pkt() on WM since we expand them in LSM layer
08/09/10   sg      Removed std_strlcpy() wrapper function calls and used std lib calls
                   since these wrappers not available on adsp
06/19/10   sg      Merging of DCM rtos and WM files
06/20/10   sg      check whether diag is initialized before allocating space from
                   diagbuf when clients call diag message apis
05/11/10   is      Removed FEATURE_ERR_EXTENDED_STORE.
04/05/10   sg      Removed inclusion of msg_qsr.h
04/26/10   vs      Update msg_store if current state machine is not correct
03/23/10   vs      Added optimizations required for ERR macro 
02/22/10   sg      Updated dspdiag changes
02/12/10   vs      Ported diag_time_get from depot
01/29/10   JV      Fixing compile error
01/27/10   ps      Decouple Error functions from diag.
12/28/09   ps      Fixed Klocwork errors & compiler warnings.
11/30/09   ps      Fixed compiler warnings & added F3trace support(SW).
11/18/09   ps      Added qsr feature support.
11/04/09   cahn    DIAG Health
09/30/09   as      Ported and mainlined msg_send_ts() api
09/29/09   mad     Included msg_arrays_i.h, removed MSG_TBL_GEN feature
09/22/09   JV      Saving to trace buffer or not, now depends on just the features
                   and not the do_sav variable.
09/22/09   vs      Use OSAL for all rex calls on qdsp6.
09/09/09   as      Removed feature FEATURE_MSG_IFACE_VIOLATION   
09/09/2009 JV      Changed signature of osal mutex calls.
07/31/09   JV      Merged Q6 diag code back to mainline
07/10/09   as      Decoupled faeture FEATURE_DIAG_NON_STREAMING
05/12/09   JV      Introduced the OS abstraction layer for rex.
05/07/09   vk      changed includes from external to internal headers
02/01/09   vg      Made trace buffer mutex consistent between event's and F3 msg
01/08/09   as      Fixed deadlock and thread protection issues
10/03/08   vg      Updated code to use PACK() vs. PACKED
12/15/06   as      Fixed critical lint errors.
08/29/06   as      Added DSP Diag support.
10/31/05   as      Fixed lint errors.
08/15/05   eav     Checked to see if trace buffer contents have been corrupted 
02/18/04   as      Fixed critical lint errors.
11/04/04   as      Added code to set ts_type filed in WCDMA Phones
09/20/04   eav     Rearranged code so that interrupts are locked for a shorter
                   amount of time when saving a trace record to buffer
07/14/04   eav     Fixed LINT errors 
06/09/04   eav     Added support for "savevars" and "savetime" of the 
                   msg_const_type.
05/14/04   eav     Renamed functions and variables to conform to naming
                   convention.
03/23/04   eav     The function msg_save_trace() will save the
                   msg_ext_store_type sent to the msg_send_3 to the EFS.
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
02/03/02   as      Added msg_sprintf function to support strings
04/19/02   igt     Added new msg_send() and support functions for enhanced
                   message services
04/30/01   lad     Added msg_format_filename() under T_WINNT for use in off
                   target testing.
04/06/01   lad     Changed use of FEATURE_DIAG_MSP_BUILD to FEATURE_ZREX_TIME.
02/23/01   lad     Made use of diagpkt API and performed cleanup for
10/16/00   jal     Turned on logging for off-target debug targets
06/09/00   lad     Added flags for MSM5000_IRAM_FWD
05/15/00   lad     Changed msg_nice default to 0.
05/10/00   lad     Added load balancing support.
04/11/00   lad     Removed LOCAL from msg_dropped for externalization.
02/28/00   lad     Added support for streaming diag services.
01/26/00   lad     Removed INTLOCK from uint32 ops.  uint32 is atomic in ARM.
03/31/99   lad     Changed dropped msg handling to be correctly noted.
03/25/99    ms     Incorporated optimized MSG routines.
02/14/97   rdh     Used Pascal directive on msg_put to save ROM.
05/19/92   jah     Changed to allow dynamic message filtering
03/03/92   jah     created file, drawn from brassboard err services

===========================================================================*/
#include "core_variation.h"
#include "comdef.h"

#include <string.h>     /* Prototype for memcpy() */
#include <stdarg.h>     /* support for variable argument lists */
#include <stdio.h>

#include "msg.h"        /* Message typedefs and prototypes */
#include "msg_qsr.h"

#include "diagcmd.h"        /* diag command codes */
#include "diagbuf_v.h"        /* diag outpout buffer allocation manager. */
#include "diagdiag_v.h"
#include "diagi_v.h"      /* Needed for sleep vote. */
#include "diagpkt.h"
#include "err.h"
#include "msg_pkt_defs.h"
#include "qw.h"         /* definition of qword */
#include "diagcomm_v.h"
#include "diagcomm_ctrl.h" /* For diag_ctrl_msg_type */
#include "msg_arrays_i.h"  /* For msg_mask_tbl etc */
#include "msgi.h"
#include "stringl.h"

#ifdef FEATURE_WINCE
#error code not present
#endif 
#if defined (FEATURE_DIAG_ADSP_CALLBACK)
#include "dspdiag_dispatch.h" /* For adsp_dspdiag125_callback */
#endif /* FEATURE_DIAG_ADSP_CALLBACK */


#include "diagtarget.h"

#if defined (DIAG_QDSP6_APPS_PROC)
  #include "diagstub.h"
#else
  #include "assert.h"
#endif

#include "diag_cfg.h"
#include <stdlib.h> 

osal_mutex_arg_t         diag_debug_record_buf_cs;

extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */
extern uint8 diag_cur_preset_id; /* from diag.c */

/* If Qshrink3 did not run, the msg_mask is set so that everything gets filtered. */
uint32 msg_v3_ss_num_table_entries = 0;
msg_v3_ssid_table_type *msg_v3_ss_table;
/* This is a lookup table to calculate what bit(s) we want to examine or set in
   an entry of msg_v3_ss_table based on what preset mask we are interested in.
   If some preset/mask combinations are not used, they are assigned to 0. */
uint16 msg_stream_ssid_tbl_mask[DIAG_MAX_STREAM_ID][DIAG_MAX_PRESET_ID] =
{
  {0x0001, 0x0002},
  {0x8000, 0x0}
};
uint16 diag_qsr_rt_mask = 0x0001|0x0002;
/*Global variables to track whether the current F3 being drained is 
  in extended format or not*/
boolean diag_msg_ext = FALSE;
boolean diag_msg_sprintf_ext = FALSE;
extern uint8 diag_max_presets[DIAG_MAX_STREAM_ID]; /* from Diag.c */

/* Error (Warning) 628: no argument information provided for function 
  '__va_start() and __va_arg() */
/*lint -save -e{628} */

/* --------------------------------------------------------------------------
** Definitions and Declarations
** ----------------------------------------------------------------------- */

uint8 msg_enabled_mask = DIAG_MSG_NONE; /* Bit mask to check if F3 masks, listeners and Extended Listeners are set */

typedef struct
{
  msg_ssid_range_type ssid_ranges[MSG_MASK_TBL_CNT];
}
msg_large_ssid_ranges_type;

#define MSG_LARGE_SSID_RANGE(X) \
   (((msg_large_ssid_ranges_type *) X)->ssid_ranges)

/* Message operating mode.  If a legacy message request packet is used, the message service will build legacy packet formats for MSG_SSID_LEGACY and ignore all other SSIDs. */

static enum
{
  MSG_MODE_STD = 0,         /* Standard operating mode */
  MSG_MODE_TERSE,           /* Terse mode - send the store packet as is
                               (let the tool expand the pointers). */
  MSG_MODE_LEGACY           /* Legacy packet mode */
} msg_mode;

/* Statistics */
static uint32 msg_total;    /* total number of messages logged */
uint32 msg_dropped;  /* number of dropped messages */
uint32 msg_drop_delta;   /* number of dropped messages */

/* Legacy values for level are used to index this array. Legacy values were:
   #define MSG_LVL_LOW     0
   #define MSG_LVL_MED     1
   #define MSG_LVL_HIGH    2
   #define MSG_LVL_ERROR   3
   #define MSG_LVL_FATAL   4
   #define MSG_LVL_NONE    255 */

static const uint32 msg_legacy_masks[] = {
  MSG_LEGACY_LOW, MSG_LEGACY_MED, MSG_LEGACY_HIGH, MSG_LEGACY_ERROR,
  MSG_LEGACY_FATAL
};

/*lint -save -e{570} */
static const uint32 msg_legacy_rt_masks[] = {
  MSG_LVL_LOW, MSG_LVL_MED, MSG_LVL_HIGH, MSG_LVL_ERROR, MSG_LVL_FATAL
};
/*lint -restore */

#if defined(DIAG_MP_MASTER)
/* Defines and variables for saving to master event mask */
#define MSTR_MSG_MASK_SIZE 200
typedef struct
{
  uint16 ssid_first;      /* Start of range of supported SSIDs */
  uint16 ssid_last;       /* Last SSID in range */
  uint16 ssid_last_max;   /* Max SSID in range */
  uint8 * status[DIAG_MAX_STREAM_ID];

  /* Pointer to array of (ssid_last - ssid_first + 1) masks */
  uint32 * rt_mask_array[DIAG_MAX_STREAM_ID];
} diag_msg_mask_mstr_type;

/* Array of (ssid_last - ssid_first + 1) masks */
uint32 diag_mstr_rt_preset_mask[DIAG_MAX_PRESET_ID][MSG_MASK_TBL_CNT][MSTR_MSG_MASK_SIZE];     /* Stream 1 QXDM Master masks. */
uint32 diag_mstr_rt_dci_mask[DIAG_MAX_DCI_ID][MSG_MASK_TBL_CNT][MSTR_MSG_MASK_SIZE];           /* Stream 2 DCI Master masks. */

uint8 diag_mstr_rt_preset_status[DIAG_MAX_PRESET_ID][MSG_MASK_TBL_CNT];
uint8 diag_mstr_rt_dci_status[DIAG_MAX_DCI_ID][MSG_MASK_TBL_CNT];

diag_msg_mask_mstr_type diag_msg_mask_mstr_tbl[MSG_MASK_TBL_CNT];
#endif /* DIAG_MP_MASTER */

#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
static diag_ctrl_info_fwd_status bt_msg_mask_fwd_status[MSG_MASK_TBL_CNT];
static diag_ctrl_info_fwd_status bt_msg_ssid_range_fwd_status;
#endif

boolean msg_service_init = FALSE;
unsigned int msg_ssid_count (void);
static boolean msg_set_rt_mask_adv (uint16 ssid, uint32 mask, byte stream_id, uint8 preset_id);
static boolean msg_masks_enabled_check(byte stream_id);
static boolean msg_sleep_vote (void);

static void msg_expand_and_send_pkt (diag_send_desc_type *send, byte stream_id);

static void msg_expand_and_send_legacy_pkt (diag_send_desc_type *desc, byte stream_id);
static void msg_qsr_expand_and_send_pkt (diag_send_desc_type *desc, byte stream_id);
static void msg_qsr_ext_expand_and_send_pkt(diag_send_desc_type *desc, byte stream_id);
static void msg_sprintf_expand_and_send_pkt(diag_send_desc_type *desc, byte stream_id);
   
#if defined(T_WINNT)
#error code not present
#endif
#if !defined(FEATURE_WINCE)
extern diag_searchlist_type diag_f3_listeners;
extern void diag_f3_listener_notify (uint32 seq_num, void * msg, diag_searchlist_node_type * node);
#endif

#ifndef MSG_FMT_STR_ARG_SIZE
#define MSG_FMT_STR_ARG_SIZE 100 /* 100 is guess */
#endif

/*===========================================================================

FUNCTION MSG_INIT

DESCRIPTION
  Initialize the message reporting services.

DEPENDENCIES
  diagbuf_init() must be called prior to this call.

RETURN VALUE
  None

SIDE EFFECTS
  Sets the counts to 0, and throws away any buffered messages.

===========================================================================*/
void
msg_init (void)
{
#if defined(DIAG_MP)
  uint8 index = 0;
#endif
   
#if defined(DIAG_MP_MASTER)
  uint8 stream_index, port_num = 0;
#endif
   
#ifndef FEATURE_WINCE
  (void) diagbuf_register_outbound_pkt_handler (DIAG_EXT_MSG_TERSE_F,   /*126*/
                     msg_expand_and_send_pkt);
  (void) diagbuf_register_outbound_pkt_handler (DIAG_QSR_EXT_MSG_TERSE_F,   /*146 = 0x92*/
                     msg_qsr_expand_and_send_pkt);
  (void)diagbuf_register_outbound_pkt_handler(DIAG_EXT_MSG_F,    /*121 = 0x79*/
                                       msg_sprintf_expand_and_send_pkt);
  (void) diagbuf_register_outbound_pkt_handler (DIAG_CMD_EXT_F,    
                     msg_qsr_ext_expand_and_send_pkt);
    
#endif 


  msg_total = 0;        /* total number of errors reported */
  msg_dropped = 0;      /* total number dropped */
  msg_drop_delta = 0;       /* number dropped since last success */
  msg_mode = MSG_MODE_STD;

#if defined(DIAG_MP_MASTER)
  /* Initialize master msg mask table */
  for( index = 0; index < MSG_MASK_TBL_CNT; index++ )
  {
    /* Point Master masks to initial Master preset masks. */
    diag_msg_mask_mstr_tbl[index].rt_mask_array[DIAG_STREAM_1-1] = &diag_mstr_rt_preset_mask[diag_cur_preset_id-1][index][0];
    diag_msg_mask_mstr_tbl[index].status[DIAG_STREAM_1-1] = &diag_mstr_rt_preset_status[diag_cur_preset_id-1][index];
    
    #if defined(DIAG_STREAM_2_MASK_CHECK)
      diag_msg_mask_mstr_tbl[index].rt_mask_array[DIAG_STREAM_2-1] = &diag_mstr_rt_dci_mask[DIAG_DCI_MASK_1-1][index][0];
      diag_msg_mask_mstr_tbl[index].status[DIAG_STREAM_2-1] = &diag_mstr_rt_preset_status[DIAG_DCI_MASK_1-1][index];
    #endif
    
    diag_msg_mask_mstr_tbl[index].ssid_first = msg_mask_tbl[index].ssid_first;
    diag_msg_mask_mstr_tbl[index].ssid_last = msg_mask_tbl[index].ssid_first;
    diag_msg_mask_mstr_tbl[index].ssid_last_max = (uint16)(msg_mask_tbl[index].ssid_first + MSTR_MSG_MASK_SIZE - 1);
    
    if( index + 1 < MSG_MASK_TBL_CNT )
    {
      /* Ensure ssid_last_max doesn't overlap with next table's ssid_first */
      diag_msg_mask_mstr_tbl[index].ssid_last_max = (uint16)(MIN(diag_msg_mask_mstr_tbl[index].ssid_last_max,
                                                                 msg_mask_tbl[index+1].ssid_first - 1));
    }
    
    for( stream_index = 0; stream_index < DIAG_MAX_STREAM_ID; stream_index++ )
    {
      diag_msg_mask_mstr_tbl[index].status[stream_index] = DIAG_CTRL_MASK_INVALID;
    }
  }
  
  /* Initialize keeping track of sending msg masks to Slaves */
  for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
  {
    diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].msg_mask_update[DIAG_STREAM_1-1] = 
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].msg_mask_preset_update[diag_cur_preset_id-1];
        
  #if defined(DIAG_STREAM_2_MASK_CHECK)      
    diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].msg_mask_update[DIAG_STREAM_2-1] = 
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].msg_mask_dci_update[DIAG_DCI_MASK_1-1];
  #endif
  
    for( index = 0; index < MSG_MASK_TBL_CNT; index++ )
    {
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].msg_mask_preset_update[DIAG_PRESET_MASK_1-1][index] = FALSE;
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].msg_mask_preset_update[DIAG_PRESET_MASK_2-1][index] = FALSE;
      #if defined(DIAG_STREAM_2_MASK_CHECK)
        diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].msg_mask_dci_update[DIAG_DCI_MASK_1-1][index] = FALSE;
      #endif
    }
  }
#endif /* DIAG_MP_MASTER */

#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
  for(index = 0; index < MSG_MASK_TBL_CNT; index++ )
  {
    bt_msg_mask_fwd_status[index] = DIAG_CTRL_INFO_FWD_STATUS_NOT_SENT;
  }
  bt_msg_ssid_range_fwd_status = DIAG_CTRL_INFO_FWD_STATUS_NOT_SENT;
#endif
  msg_set_all_listener_masks(0);  //Set the listener masks
  msg_set_all_rt_masks_adv(0, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);

  msg_service_init = TRUE;

  (void) msg_sleep_vote ();
}               /* end of msg_init */



/*===========================================================================

FUNCTION MSG_EXPAND_AND_SEND_PKT

DESCRIPTION
  This routine expands the intermediate storage format into the full
  packet format, copying data from the constant block in 'stored_msg'.

DEPENDENCIES
  msg_init() must have been called previously.

RETURN VALUE

===========================================================================*/
typedef enum
{
  MSG_EXPAND_STATE_HDR = 0,
  MSG_EXPAND_STATE_DESC,
  MSG_EXPAND_STATE_ARGS,
  MSG_EXPAND_STATE_FMT,
  MSG_EXPAND_STATE_FMT_NULL,
  MSG_EXPAND_STATE_FNAME,
  MSG_EXPAND_STATE_FNAME_NULL,
  MSG_EXPAND_STATE_COMPLETE
}
msg_expand_enum_type;

static void
msg_expand_and_send_pkt (diag_send_desc_type *desc, byte stream_id)
{
  msg_ext_store_type *msg_store = NULL;
  diagbuf_status_enum_type status = DIAGBUF_READY_S;
  diag_msg_type msg_notify;
   
  static diag_send_desc_type send = {NULL, NULL, DIAG_SEND_STATE_START, FALSE};
  static msg_expand_enum_type state_sav = MSG_EXPAND_STATE_HDR; /* Used to store state of process. */

  /* Local copies of static variables for optimized access.  
     This makes this routine persistent but not re-entrant. */
  msg_expand_enum_type state = state_sav;

  int send_pkt_len;
  
  //TODO: Move legacy mode support to diagbuf registration, not here.
  if (msg_mode == MSG_MODE_LEGACY)
  {
     msg_expand_and_send_legacy_pkt (desc,stream_id);
  }
  else if (desc)
  {
    /* 'decs' is not moved until the entire message is complete.  Therefore,
       it always points to the stored message, in terse format. */
    msg_store = (msg_ext_store_type *) desc->pkt;

    if (desc->state == DIAG_SEND_STATE_START)
    {
      /* Increment state to indicate the packet is now in progress. */
      desc->state++;

      /* Keep stats of total messages */
      msg_total++;

      if (msg_store)
      {
        /* Add dropped messages to the total. */
        msg_total += msg_store->hdr.drop_cnt;
        
#if !defined(FEATURE_WINCE)
        msg_notify.cmd_code = DIAG_EXT_MSG_TERSE_F;
        msg_notify.msg_store.ext_msg = msg_store;

      diag_f3_searchlist_ext(msg_store->const_data_ptr->desc.ss_id,
                        msg_store->const_data_ptr->desc.ss_mask,
                        (void*)&msg_notify);
    
        (void) diag_searchlist_search_all( msg_store->const_data_ptr->desc.ss_id, 
                                           &diag_f3_listeners, 
                                          (void *)&msg_notify, 
                                           diag_f3_listener_notify );
#endif        
      }
      
      if (state != MSG_EXPAND_STATE_HDR)
      {
        /* This should never happen.  However, if we start over, just kill the
           last message in progress */
        ERR ("New message while in progress", 0, 0, 0);

        desc->pkt = (void *) ((byte *) desc->last + 1);
        desc->state = DIAG_SEND_STATE_COMPLETE;
        state = MSG_EXPAND_STATE_HDR;   /* reset state */
        msg_store = (msg_ext_store_type *) desc->pkt;
      }
    }
    
    if(msg_store)
    {
      /* check if a valid stream bit is set. Listeners should already be
         handled by this point */
      if( !(stream_id&(DIAG_STREAM_1|DIAG_STREAM_2)))
      {
        /* Mark Complete - Dont send it to the communication layer */
        desc->state = DIAG_SEND_STATE_COMPLETE;
        /* Skip this packet - no tool asked for it, or already sent */
        desc->pkt = (void *) ((byte *) desc->last + 1);
        state = MSG_EXPAND_STATE_HDR;
        send.pkt = NULL;
      }
      else
      {
        /* The packet is expanded in fragments.  'state' marks each stage of
           the process. */
        if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_HDR)
        {
          if (send.pkt == NULL && msg_store)
          {
            /* Set the command code since it was stored as MSG_EXT_TERSE_F. */
            msg_store->hdr.cmd_code = DIAG_EXT_MSG_F;

            /* Set up the send descriptor for this fragment. */
            send.pkt = (void *) &msg_store->hdr;
            send.last = (void *) ((byte *) send.pkt + sizeof (msg_store->hdr) - 1);
            send.state = DIAG_SEND_STATE_START;
            send.terminate = FALSE;
          }

          (void) diagbuf_send_pkt (&send,stream_id);

          if (send.state == DIAG_SEND_STATE_COMPLETE)
          {
            state++;
            send.pkt = NULL;
          }
        }

        if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_DESC)
        {
          if (send.pkt == NULL && msg_store)
          {
            /* Set up the send descriptor for this fragment. */
            send.pkt = (void *) &msg_store->const_data_ptr->desc;
            send.last = (void *) ((byte *) send.pkt + 
            sizeof (msg_store->const_data_ptr->desc) - 1);
            
            send.state = DIAG_SEND_STATE_BUSY;
          }

          (void) diagbuf_send_pkt (&send,stream_id);

          if (send.state == DIAG_SEND_STATE_COMPLETE)
          {
            state++;
            send.pkt = NULL;
          }
        }

        if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_ARGS)
        {
          if (send.pkt == NULL && msg_store)
          {
            /* Set up the send descriptor for this fragment. */
            send.pkt = (void *) msg_store->args;
            send.last = (void *) ((byte *) send.pkt + 
            sizeof (msg_store->args[0]) * msg_store->hdr.num_args - 1);
            
            send.state = DIAG_SEND_STATE_BUSY;
          }

          (void) diagbuf_send_pkt (&send,stream_id);

          if (send.state == DIAG_SEND_STATE_COMPLETE)
          {
            state++;
            send.pkt = NULL;
          }
        }

        if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_FMT)
        {
          if (send.pkt == NULL && msg_store)
          {
            /* Set up the send descriptor for this fragment. */
            send.pkt = (void *) msg_store->const_data_ptr->fmt;

            send_pkt_len = (int)strlen ((char *) send.pkt);

            /* Do not sub 1 to include NULL char */
            send.last = (void *) ((byte *) send.pkt + 
              MIN (MSG_MAX_STRLEN, send_pkt_len));
            
            send.state = DIAG_SEND_STATE_BUSY;
          }

          (void) diagbuf_send_pkt (&send,stream_id);

          if (send.state == DIAG_SEND_STATE_COMPLETE)
          {
            state++;
            send.pkt = NULL;
            
            if (*((byte *) send.last) == 0)
            {
              state++; /* skip the NULL term state */
            }
          }
        }

        if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_FMT_NULL)
        {
          /* There was a string length overrun in the FNAME state, so terminate
             with a NULL char.  Use the 'terminate' (which is otherwise ignored)
             field in the desc terminator as the NULL and set it to 0. */
          send.last = send.pkt = &desc->terminate;
          desc->terminate = 0;
          
          send.state = DIAG_SEND_STATE_BUSY;
      
          (void) diagbuf_send_pkt (&send,stream_id);

          if (send.state == DIAG_SEND_STATE_COMPLETE)
          {
            state++;
            send.pkt = NULL;
          }
        }

        if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_FNAME)
        {
          if (send.pkt == NULL && msg_store)
          {
            /* Set up the send descriptor for this fragment. */
  #if !defined (T_WINNT)
            send.pkt = (void *) msg_store->const_data_ptr->fname;
  #else
            #error code not present
#endif
            send_pkt_len = (int)strlen ((char *) send.pkt);
            /* Do not sub 1 to include NULL char */
            send.last = (void *) ((byte *) send.pkt + 
              MIN (MSG_MAX_STRLEN, send_pkt_len));
            
            send.state = DIAG_SEND_STATE_BUSY;
            
            if (*((byte *) send.last) == 0)
            {
              send.terminate = TRUE;
            }
          }

          (void) diagbuf_send_pkt (&send,stream_id);

          if (send.state == DIAG_SEND_STATE_COMPLETE)
          {
            state++;
            send.pkt = NULL;
            
            if (send.terminate == TRUE)
            {
              state++; /* skip the NULL term state */
            }
          }
        }

        if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_FNAME_NULL)
        {
          /* There was a string length overrun in the FNAME state, so terminate
             with a NULL char.  Use the 'terminate' field in the desc tyerminator
             as the NULL and set it to 0. */
          send.last = send.pkt = &desc->terminate;
          desc->terminate = 0;
          
          send.state = DIAG_SEND_STATE_BUSY;
          send.terminate = TRUE;

          (void) diagbuf_send_pkt (&send,stream_id);

          if (send.state == DIAG_SEND_STATE_COMPLETE)
          {
            state = MSG_EXPAND_STATE_HDR;
            desc->pkt = (void *) ((byte *) desc->last + 1);
            send.pkt = NULL;
          }
        }

        if (state == MSG_EXPAND_STATE_COMPLETE)
        {
          state = MSG_EXPAND_STATE_HDR;
          desc->pkt = (void *) ((byte *) desc->last + 1);
          desc->state = DIAG_SEND_STATE_COMPLETE;
          send.pkt = NULL;
        }
      }
    }
  }
  /* Commit local copies of persistent data back to static variables. */
  state_sav = state;
  
  return;
}

/*===========================================================================

FUNCTION MSG_EXPAND_AND_SEND_LEGACY_PKT

DESCRIPTION
  This routine expands the intermediate storage format into the legacy
  message format.

DEPENDENCIES
  msg_init() must have been called previously.

RETURN VALUE

===========================================================================*/
static void
msg_expand_and_send_legacy_pkt (diag_send_desc_type *desc,byte stream_id)
{
  const msg_ext_store_type *msg_store;
  const uint32 *args = NULL;
  byte num_args = 0;
  unsigned int i;
  
  static msg_legacy_rsp_type msg;   /* Intermediate storage buffer */
  static diag_send_desc_type send = 
  {
    (void *) &msg,
    (void *) ((byte *) &msg + sizeof (msg) - 1),
    DIAG_SEND_STATE_START,
    TRUE
  };

  if (desc)
  {
    msg_store = (const msg_ext_store_type *) desc->pkt;
    
    if(msg_store)
    {
      /* check if a valid stream bit is set. Listeners should already be
      handled by this point */
      if( !(stream_id&(DIAG_STREAM_1|DIAG_STREAM_2)))
      {
        /* Mark Complete - Dont send it to the communication layer */
        desc->state = DIAG_SEND_STATE_COMPLETE;
      /*Skip the packet - no tool asked for it */
        desc->pkt = (void *) ((byte *) desc->last + 1);
        send.pkt = NULL;
      }
      else
      {
        if (desc->state == DIAG_SEND_STATE_START && msg_store)
        {
            /* Increment state to indicate the packet is now in progress. */
            desc->state++;

          /* Keep stats of total messages */
          msg_total++;

          /* Add dropped messages to the total. */
          msg_total += msg_store->hdr.drop_cnt;

          
          /* Set up the send descriptor. */
          send.pkt = (void *) &msg;
          send.state = DIAG_SEND_STATE_START;

          /* Fill in the legacy descriptor. */
          msg.cmd_code = DIAG_MSG_F;
          msg.qty = 1;

          msg.drop_cnt = msg_dropped;
          msg.total_msgs = msg_total;

          /* init to low message */
          msg.level = 0;

          for (i = 0; i < sizeof (msg_legacy_masks) / sizeof (msg_legacy_masks[0]);
               i++)
          {
            if (msg_legacy_masks[i] & msg_store->const_data_ptr->desc.ss_mask)
            {
              msg.level = (byte)i;
              break;
            }
          }

            /* Copy file name and null terminate it. */

          strlcpy ((char *) msg.file, msg_store->const_data_ptr->fname,
                          MSG_FILE_LEN);

          /* Copy source file line number */
          msg.line = msg_store->const_data_ptr->desc.line;

          /* Copy format string and null terminate it. */

          strlcpy ((char *) msg.fmt, msg_store->const_data_ptr->fmt,
                          MSG_FMT_LEN);

          /* Copy parameters for format string */
          args = MSG_LARGE_ARGS(msg_store->args);

          num_args = msg_store->hdr.num_args;

          /* Copy up to 3 args, depending on what is available */
          msg.code1 = (num_args >= 1) ? args[0] : 0;
          msg.code2 = (num_args >= 2) ? args[1] : 0;
          msg.code3 = (num_args >= 3) ? args[2] : 0;

          /* Get timestamp for this message ASAP */
          qw_equ (msg.time, (unsigned long *) msg_store->hdr.ts);
        }

        (void) diagbuf_send_pkt (&send,stream_id);

        if (send.state == DIAG_SEND_STATE_COMPLETE)
        {
          send.pkt = NULL;
          desc->pkt = (void *) ((byte *) desc->last + 1);
          desc->state = DIAG_SEND_STATE_COMPLETE;
        }
      }
    }
  }

  return;
}

boolean msg_get_qsr_table_entry(uint16 ssid_index, uint16 *msg_ssid, uint32 *msg_mask)
{
  if (ssid_index < msg_v3_ss_num_table_entries)
  {
    if (msg_ssid)
    {
      *msg_ssid = msg_v3_ss_table[ssid_index]->ss_id;
    }
    if (msg_mask)
    {
      *msg_mask = msg_v3_ss_table[ssid_index]->ss_mask;
    }
    return TRUE;
  }
  else
    return FALSE;
}



/*===========================================================================

FUNCTION MSG_UPDATE_SS_Table

DESCRIPTION
  This function will update the msg_v3_ss_table flags for ssid, and ss_mask
  Its possible that ssid entries are missing from this table

DEPENDENCIES 

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
static void msg_update_ss_table (uint16 curr_ssid, uint32 ss_mask, uint8 stream_id, uint8 preset_id)
{
  int mid_index; /*need to be signed for binary search*/
  int start_index = 0;   /*need to be signed for binary search*/
  int end_index = msg_v3_ss_num_table_entries - 1;  /*need to be signed for binary search*/
  uint32 table_index = 0;
  int ss_index; /* needs to be signed; not enormously concerned about range */
  uint16 rt_mask = 0;
  boolean match_found=FALSE;

  if (msg_v3_ss_num_table_entries == 0)
  {
   return;
  }

  if ((curr_ssid < msg_v3_ss_table[start_index]->ss_id) || (curr_ssid > msg_v3_ss_table[end_index]->ss_id))
  {
    return;
  }

  if (stream_id > DIAG_MAX_STREAM_ID || stream_id < DIAG_MIN_STREAM_ID || preset_id > diag_max_presets[stream_id-1])
  {
    return;
  }

  /* the binary search modified*/
  while( (start_index <= end_index) &&
        (start_index >=0) && (end_index <=(msg_v3_ss_num_table_entries -1)) )
  {
    mid_index = ( start_index + end_index ) >> 1;

    if( msg_v3_ss_table[mid_index]->ss_id > curr_ssid )
    {
      end_index = mid_index - 1;
    }
    else if(curr_ssid > msg_v3_ss_table[mid_index]->ss_id)
    {
      start_index = mid_index + 1;
    }
    else
    {
      table_index = mid_index;
      match_found=TRUE;
      break;
    }
  }
  
  if(!match_found)
  {
    return;
  }
  /* came to an area where ssid match occurred */ 

  /* Set ss_mask for above/below values */
  if (preset_id == DIAG_PRESET_MASK_ALL)
  {
    int i;
    for (i = 0; i < DIAG_MAX_PRESET_ID; i += 1)
    {
      rt_mask |= msg_stream_ssid_tbl_mask[stream_id-1][i];
    }
  }
  else
  {
    rt_mask = msg_stream_ssid_tbl_mask[stream_id-1][preset_id-1];
  }

  for (ss_index = table_index;
        ss_index >= 0 && msg_v3_ss_table[ss_index]->ss_id == curr_ssid;
        ss_index--)
  {
    if (msg_v3_ss_table[ss_index]->ss_mask & ss_mask)
    {
      msg_v3_ss_table[ss_index]->rt_mask |= rt_mask;
    }
    else {
      msg_v3_ss_table[ss_index]->rt_mask &= ~rt_mask;
    }
  }

  for (ss_index = table_index+1;
        ss_index < msg_v3_ss_num_table_entries && msg_v3_ss_table[ss_index]->ss_id == curr_ssid;
        ss_index++)
  {
    if (msg_v3_ss_table[ss_index]->ss_mask & ss_mask){
      msg_v3_ss_table[ss_index]->rt_mask |= rt_mask;
    }
    else {
      msg_v3_ss_table[ss_index]->rt_mask &= ~rt_mask;
    }
  }

} /* msg_update_ss_table () */



/*===========================================================================

FUNCTION MSG_MASK_BINARY_SEARCH

DESCRIPTION
  This function will return the msg_mask_tbl index using a binary search technique
  for a given SSID. Every index returned corresponds to the ssid_first in each structure.

DEPENDENCIES 
  Depends on the msg_mask_tbl defined in msg_arrays_i.c

RETURN VALUE
  If the entry was found, the table index between 0 and MSG_MASK_TBL_CNT-1
  into msg_mask_tbl will be returned. Else, MSG_MASK_TBL_CNT will be returned.

SIDE EFFECTS
  None.
===========================================================================*/

uint8 msg_mask_binary_search (uint16 curr_ssid)
{
  uint8 mid_index;
  uint8 start_index = 1;  // starting at 1, since 0 is for legacy
  uint8 end_index = MSG_MASK_TBL_CNT - 1;
  uint8 table_index = MSG_MASK_TBL_CNT;

  // Skips the search for the legacy messages.
  if(curr_ssid <= msg_mask_tbl[0].ssid_last) 
  {
    return 0;
  }

  /* the binary search modified to work with structures holding the start and end ssid values */
  while( start_index <= end_index )
  {
    mid_index = ( start_index + end_index ) >> 1;

    if( msg_mask_tbl[mid_index].ssid_first > curr_ssid )
    {
      end_index = mid_index - 1;
    }
    else if(curr_ssid > msg_mask_tbl[mid_index].ssid_last)
    {
      start_index = mid_index + 1;
    }
    else
    {
      table_index = mid_index;
      break;
    }
    /* Adjusting for the index returned by the binary search to make it correspond to the ssid_first in that structure */
    if( (start_index - end_index) == 1 )
    {
      if (curr_ssid < msg_mask_tbl[start_index].ssid_first) 
      {
        table_index = start_index - 1;
        break;
      } 
      else if (curr_ssid < msg_mask_tbl[end_index].ssid_first) 
      {
        table_index = start_index;
        break;
      } 
      else 
      { 
        table_index = end_index;
        break;
      }
    }
  }
  // the ssid is outside the range of SSIDs defined in the msg_mask_tbl.
  if (curr_ssid > msg_mask_tbl[table_index].ssid_last)
  {
    return MSG_MASK_TBL_CNT;
  }
  return table_index; 
} /* msg_mask_binary_search () */

/*===========================================================================

FUNCTION MSG_GET_SSID_MASKS_ADV

DESCRIPTION
  This function will get an SSID as input and return its corresponding
  build time mask or real time mask by accessing the appropriate data
  structure. It returns True/False based on whether the SSID is valid or not.

DEPENDENCIES

RETURN VALUE
  True/False

SIDE EFFECTS
  None.
===========================================================================*/
boolean
msg_get_ssid_masks_adv (msg_mask_type mask_type, uint16 ssid_start,
            uint16 ssid_end, uint32 * mask, byte stream_id, uint8 preset_id)
{
  unsigned int ssid_index;
  boolean success = FALSE;
  uint8 table_index = 0;

  const msg_mask_tbl_type *tbl = msg_mask_tbl;
  const uint32 *mask_array = NULL;  /* First pass forces a search */

/*-------------------------------------------------------------------------*/

  if (mask && ssid_start <= ssid_end)
  {
    success = TRUE;
    for (ssid_index = ssid_start; ssid_index <= ssid_end; ssid_index++)
    {
      /* If not currently matched to a correct table, reset table search. */
      if ( ssid_index < tbl->ssid_first || ssid_index > tbl->ssid_last)
      {
        tbl = msg_mask_tbl;
        mask_array = NULL;
      }

      /* Table search: if mask_array is not set */
      if ( !mask_array )
      {
        table_index = msg_mask_binary_search(ssid_index);
        if( table_index == MSG_MASK_TBL_CNT )
        {
          mask_array = NULL;
        }
        else 
        {
          tbl = &msg_mask_tbl[table_index];
          if (mask_type == MSG_MASK_BT)
          {
             mask_array = tbl->bt_mask_array;
          }
          else
          {
            mask_array = (const uint32 *)&tbl->rt_base_ptr[stream_id - 1][(preset_id-1)*(tbl->ssid_last - tbl->ssid_first + 1)];
          }  
        }
      }

      if (mask_array)
      {
        /* Valid SSID found.  Write mask to caller's mask. */
        mask[ssid_index - ssid_start] = mask_array[ssid_index - tbl->ssid_first];
      }
      else
      {
        /* Invalid SSID.  Indicate failure and continue. */
        mask[ssid_index - ssid_start] = 0;
        success = FALSE;    /* At least 1 entry is invalid */
      }
    }
  }

  return (success);
}               /* msg_get_ssid_masks_adv() */

/*===========================================================================

FUNCTION MSG_SET_RT_MASK_ADV

DESCRIPTION
  This function will get an SSID and a real time mask as input and set the
  real time mask that corresponds to that ssid to the requested value.
  It returns True/False based on whether the SSID is valid or not.

PARAMETERS 
  ssid - SSID to set
  mask - Mask to set for ssid specified
  stream_id - SSID for this stream ID will be set. Valid stream IDs are 
              DIAG_MIN_STREAM_ID to DIAG_MAX_STREAM_ID.
  preset_id - SSID for this preset ID will be set. Valid preset IDs are 
              DIAG_MIN_PRESET_ID to DIAG_MAX_PRESET_ID.
  
DEPENDENCIES

RETURN VALUE
  True/False

SIDE EFFECTS
  None.
===========================================================================*/
static boolean
msg_set_rt_mask_adv (uint16 ssid, uint32 mask, byte stream_id, uint8 preset_id)
{
  boolean found = FALSE;
  uint32 i;
  uint16 count = 0;
  
#if defined(DIAG_MP_MASTER)
  boolean mstr_found = FALSE;
  uint8 port_num = 0;
  uint8 * mstr_status = NULL;
  uint32 * rt_mask_tbl_ptr;
#endif

  /* Sanity checks */
  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) || 
      (preset_id == 0) || (preset_id > DIAG_MAX_PRESET_ID) )
    return FALSE;
  
#if defined(DIAG_MP_MASTER)  
  if( stream_id == DIAG_STREAM_1 )
  {
    rt_mask_tbl_ptr = &diag_mstr_rt_preset_mask[preset_id-1][0][0];
    mstr_status = &diag_mstr_rt_preset_status[preset_id-1][0];
  }
  #if defined(DIAG_STREAM_2_MASK_CHECK)
  else if( stream_id == DIAG_STREAM_2 )
  {
    rt_mask_tbl_ptr = &diag_mstr_rt_dci_mask[DIAG_DCI_MASK_1-1][0][0];
    mstr_status = &diag_mstr_rt_dci_status[DIAG_DCI_MASK_1-1][0];
  }
  #endif
  else
    return FALSE;
  
  
  /* Save to master msg mask tbl */
  for (i = 0; !found && i < MSG_MASK_TBL_CNT; i++)
  {
    if( ssid >= diag_msg_mask_mstr_tbl[i].ssid_first && ssid <= diag_msg_mask_mstr_tbl[i].ssid_last_max )
    {
      found = TRUE;
      mstr_status[i] = DIAG_CTRL_MASK_VALID;
      rt_mask_tbl_ptr[(i * MSTR_MSG_MASK_SIZE) + (ssid - diag_msg_mask_mstr_tbl[i].ssid_first)] = mask;
      
      if( diag_msg_mask_mstr_tbl[i].ssid_last < ssid )
        diag_msg_mask_mstr_tbl[i].ssid_last = ssid;
      
      /* Master needs to keep track to send msg masks to Slaves */
      for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
      {
        if( stream_id == DIAG_STREAM_1 )
          diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].msg_mask_preset_update[preset_id-1][i] = TRUE;
        #if defined(DIAG_STREAM_2_MASK_CHECK)
        else if( stream_id == DIAG_STREAM_2)
          diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].msg_mask_dci_update[DIAG_DCI_MASK_1-1][i] = TRUE;
        #endif
      }
    }
  }
#endif

  found = FALSE;
  for (i = 0; !found && i < MSG_MASK_TBL_CNT; i++)
  {
    if (ssid >= msg_mask_tbl[i].ssid_first && ssid <= msg_mask_tbl[i].ssid_last)
    {
      found = TRUE;
      count = msg_mask_tbl[i].ssid_last - msg_mask_tbl[i].ssid_first + 1;
      msg_mask_tbl[i].rt_base_ptr[stream_id-1][(preset_id-1)*count + (ssid - msg_mask_tbl[i].ssid_first)] = mask;
      /* qshrink3 update msg_v3_ss_table */
      msg_update_ss_table(ssid, mask, stream_id, preset_id);
    }
  }

#if defined(DIAG_MP_MASTER)  
  return (mstr_found);
#else
  return (found);
#endif
}               /* msg_set_rt_mask_adv() */

/*===========================================================================

FUNCTION MSG_SET_ALL_RT_MASKS_ADV

DESCRIPTION
  This function will get a real time mask as input and set ALL
  real time masks to the requested value.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void
msg_set_all_rt_masks_adv (uint32 mask, byte stream_id, uint8 preset_id)
{
  uint32 tbl_index, j, count;
  uint8 preset_index = 0;
  uint32 ssid_local;

#if defined(DIAG_MP_MASTER)
  uint8 status = mask ? DIAG_CTRL_MASK_ALL_ENABLED : DIAG_CTRL_MASK_ALL_DISABLED;
#endif

  if( (stream_id == DIAG_STREAM_1) || (stream_id == DIAG_STREAM_ALL) )
  {
    if( preset_id == DIAG_PRESET_MASK_ALL )
    {
      for( preset_index=0; preset_index < DIAG_MAX_PRESET_ID; preset_index++ )
      {
        for( tbl_index = 0; tbl_index < MSG_MASK_TBL_CNT; tbl_index++ )
        {
          count = (uint32)(msg_mask_tbl[tbl_index].ssid_last - msg_mask_tbl[tbl_index].ssid_first + 1);
          for (j = 0; j < count; j++)
          {
            msg_mask_tbl[tbl_index].rt_base_ptr[DIAG_STREAM_1-1][(preset_index)*count + j] = mask;
            /* qshrink3 update msg_v3_ss_table */
            ssid_local = msg_mask_tbl[tbl_index].ssid_first +j;
            msg_update_ss_table(ssid_local, mask, DIAG_STREAM_1, preset_index);
          }
        
#if defined(DIAG_MP_MASTER)
          /* Save to master msg mask table */
          count = (uint32)(diag_msg_mask_mstr_tbl[tbl_index].ssid_last_max - diag_msg_mask_mstr_tbl[tbl_index].ssid_first + 1);
          for( j = 0; j < count; j++ )
          {
            diag_mstr_rt_preset_mask[preset_index][tbl_index][j] = mask;
          }
          diag_mstr_rt_preset_status[preset_index][tbl_index] = status;
#endif /* DIAG_MP_MASTER */
        }
      }
    }
    else
    {
      for( tbl_index = 0; tbl_index < MSG_MASK_TBL_CNT; tbl_index++ )
      {
        count = (uint32)(msg_mask_tbl[tbl_index].ssid_last - msg_mask_tbl[tbl_index].ssid_first + 1);
        for (j = 0; j < count; j++)
        {
          msg_mask_tbl[tbl_index].rt_base_ptr[DIAG_STREAM_1-1][(preset_id-1)*count + j] = mask;
        }
        
#if defined(DIAG_MP_MASTER)
        /* Save to master msg mask table */
        count = (uint32)(diag_msg_mask_mstr_tbl[tbl_index].ssid_last_max - diag_msg_mask_mstr_tbl[tbl_index].ssid_first + 1);
        for( j = 0; j < count; j++ )
        {
          diag_mstr_rt_preset_mask[preset_id-1][tbl_index][j] = mask;
        }
        diag_mstr_rt_preset_status[preset_id-1][tbl_index] = status;
#endif /* DIAG_MP_MASTER */
      }
    }
  } //end set stream 1 all
    
#if defined(DIAG_STREAM_2_MASK_CHECK)
  if( (stream_id == DIAG_STREAM_2) || (stream_id == DIAG_STREAM_ALL) )
  {
    for( tbl_index = 0; tbl_index < MSG_MASK_TBL_CNT; tbl_index++ )
    {
      count = (uint32)(msg_mask_tbl[tbl_index].ssid_last - msg_mask_tbl[tbl_index].ssid_first + 1);
      for (j = 0; j < count; j++)
      {
        msg_mask_tbl[tbl_index].rt_base_ptr[DIAG_STREAM_2-1][(DIAG_DCI_MASK_1-1)*count + j] = mask;
        /* qshrink3 update msg_v3_ss_table */        
        ssid_local = msg_mask_tbl[tbl_index].ssid_first +j;
        msg_update_ss_table(ssid_local, mask, DIAG_STREAM_2, DIAG_DCI_MASK_1-1);
      }
        
#if defined(DIAG_MP_MASTER)
      /* Save to master msg mask table */
      count = (uint32)(diag_msg_mask_mstr_tbl[tbl_index].ssid_last_max - diag_msg_mask_mstr_tbl[tbl_index].ssid_first + 1);
      for( j = 0; j < count; j++ )
      {
        diag_mstr_rt_dci_mask[DIAG_DCI_MASK_1-1][tbl_index][j] = mask;
      }
      diag_mstr_rt_dci_status[DIAG_DCI_MASK_1-1][tbl_index] = status;
#endif /* DIAG_MP_MASTER */
    }
  } //end set stream 2 all
#endif /* DIAG_STREAM_2_MASK_CHECK */

#if defined(DIAG_MP_MASTER)
  /* Propagate masks to Slaves. */  
  diagcomm_mask_update( DIAG_UPDATE_MASK_F3, stream_id, preset_id, TRUE );
#endif

  /* Set the masks of all clients registered with DCM.*/
  (void)diag_client_maskchange(DIAG_MSGSVC_MASK_CHANGE);
   
 /* Vote for sleep */
  if (mask == 0 ) // Vote for sleep only when stream 1 is off or all the streams are off.
  {
    switch(stream_id)
    {
    case 0: 
      diag_sleep_vote (DIAG_SLEEP_MSG, TRUE);
      DIAG_CLR_MASK(msg_enabled_mask, DIAG_MSG_STREAM_1);
      DIAG_CLR_MASK(msg_enabled_mask, DIAG_MSG_STREAM_2);
      break;
    case DIAG_STREAM_1:
      diag_sleep_vote (DIAG_SLEEP_MSG, TRUE);
      DIAG_CLR_MASK(msg_enabled_mask, DIAG_MSG_STREAM_1);
      break;
    case DIAG_STREAM_2:
      DIAG_CLR_MASK(msg_enabled_mask, DIAG_MSG_STREAM_2);
      break;
    }
  }
  else
  {
    diag_sleep_vote (DIAG_SLEEP_MSG, FALSE);
    switch(stream_id)
    {
    case 0:
      DIAG_SET_MASK(msg_enabled_mask, DIAG_MSG_STREAM_1);
      DIAG_SET_MASK(msg_enabled_mask, DIAG_MSG_STREAM_2);
      break;
    case DIAG_STREAM_1:
      DIAG_SET_MASK(msg_enabled_mask, DIAG_MSG_STREAM_1);
      break;
    case DIAG_STREAM_2:
      DIAG_SET_MASK(msg_enabled_mask, DIAG_MSG_STREAM_2);
      break;
    }
  }

}               /* msg_set_all_rt_mask_adv() */


/*===========================================================================

FUNCTION MSG_SSID_COUNT

DESCRIPTION
  This function will calculate the number of valid SubSystem IDs for this
  target and return it.

DEPENDENCIES

RETURN VALUE
  The number of valid SSIDs.

SIDE EFFECTS
  None.
===========================================================================*/
unsigned int
msg_ssid_count (void)
{
  unsigned int i, ssid_num = 0;

/*-------------------------------------------------------------------------*/

  for (i = 0; i < MSG_MASK_TBL_CNT; i++)
  {
    ssid_num += msg_mask_tbl[i].ssid_last - msg_mask_tbl[i].ssid_last + 1;
  }

  return (ssid_num);

}               /* msg_ssid_count() */

/*===========================================================================

             Debug Message Service Packet Handler Routines

===========================================================================*/

/* Extended Message Config Request */

/*===========================================================================

FUNCTION MSG_PKT_GET_SSID_RANGES

DESCRIPTION
  This function will return the valid SSID range.

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
static void *
msg_pkt_get_ssid_ranges (void *req_pkt, word req_len)
{
  msg_get_ssid_ranges_req_type *req =
    (msg_get_ssid_ranges_req_type *) req_pkt;
  msg_get_ssid_ranges_rsp_type *rsp = NULL;
  unsigned int i = 0, rsp_len = 0;

/*-------------------------------------------------------------------------*/

  if (req && req->sub_cmd == (uint8) MSG_EXT_SUBCMD_GET_RANGES)
  {
    /*-------------------------------------------------------------------------
      calculate number of valid ssids and the length of the response packet.
    -------------------------------------------------------------------------*/
    rsp_len = FPOS (msg_get_ssid_ranges_rsp_type, ssid_ranges) +
      MSG_MASK_TBL_CNT * FSIZ (msg_get_ssid_ranges_rsp_type, ssid_ranges);

    /*-------------------------------------------------------------------------
      allocate space for the response packet
    -------------------------------------------------------------------------*/
    rsp = (msg_get_ssid_ranges_rsp_type *)
      diagpkt_alloc (DIAG_EXT_MSG_CONFIG_F, rsp_len);

    if (rsp)
    {
      rsp->sub_cmd = req->sub_cmd;
      rsp->status = TRUE;
      rsp->range_cnt = MSG_MASK_TBL_CNT;
      rsp->rsvd = 0;

      /*-------------------------------------------------------------------------
        make aux_access_ptr point to the end of the msg_get_ssid_ranges_rsp_type
        which is where the SSIDd should start.
      -------------------------------------------------------------------------*/

      for (i = 0; i < MSG_MASK_TBL_CNT; i++)
      {
        rsp->ssid_ranges[i].ssid_first = msg_mask_tbl[i].ssid_first;
        rsp->ssid_ranges[i].ssid_last = msg_mask_tbl[i].ssid_last;
      }
    }
  }                

  else
  {
    rsp = (void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len);
  }

  return (rsp);
}               /* msg_pkt_get_ssid_ranges () */


/*===========================================================================

FUNCTION MSG PKT GET LOCAL SSID RANGE

DESCRIPTION
  This function will return the valid SSID range.
  This function queries the local Diag message mask table, as opposed to msg_pkt_get_ssid_ranges(),
  which queries the target master message mask table

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACK(void *) msg_pkt_get_local_ssid_range(PACK(void *) req_pkt, uint16 pkt_len)
{
  msg_get_local_ssid_range_req_type *req = (msg_get_local_ssid_range_req_type *) req_pkt;
  msg_get_local_ssid_range_rsp_type *rsp = NULL;
  unsigned int rsp_len = 0;
  int i = 0;
  diagpkt_subsys_cmd_code_type cmd_code;
  
   
  if(req == NULL)
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "msg_pkt_get_local_ssid_range(): Request pointer is NULL \n",1);
   return NULL;
  }

   cmd_code = diagpkt_subsys_get_cmd_code (req);

   if ((pkt_len != sizeof(msg_get_local_ssid_range_req_type)) || 
       ((cmd_code != DIAGDIAG_RETRIEVE_SSID_RANGE_MODEM) && (cmd_code != DIAGDIAG_RETRIEVE_SSID_RANGE_APP)
         && (cmd_code != DIAGDIAG_RETRIEVE_SSID_RANGE_Q6) && (cmd_code != DIAGDIAG_RETRIEVE_SSID_RANGE_RIVA)))
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "MSG_1 cmd_code %d \n",cmd_code);
     return ((void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, pkt_len));
  }

   if(diagcomm_status(DIAGCOMM_PORT_1))
   {
   rsp_len = FPOS (msg_get_local_ssid_range_rsp_type, ssid_ranges) +
      MSG_MASK_TBL_CNT * FSIZ (msg_get_local_ssid_range_rsp_type, ssid_ranges);

    rsp = (msg_get_local_ssid_range_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, 
                                              cmd_code, rsp_len);

    if (rsp)
    {
      rsp->status = SUCCESSFUL;     
      rsp->range_cnt = MSG_MASK_TBL_CNT;
      
      for (i = 0; i < MSG_MASK_TBL_CNT; i++)
      {
        rsp->ssid_ranges[i].ssid_first = msg_mask_tbl[i].ssid_first;
        rsp->ssid_ranges[i].ssid_last = msg_mask_tbl[i].ssid_last;
      }
    }
   }
   return ((void *) rsp);
}

static void *
msg_pkt_get_ssid_ranges_adv (void *req_pkt, word req_len)
{
  msg_get_ssid_ranges_adv_req_type *req =
    (msg_get_ssid_ranges_adv_req_type *) req_pkt;
  msg_get_ssid_ranges_adv_rsp_type *rsp = NULL;
  unsigned int i = 0, rsp_len = 0;

/*-------------------------------------------------------------------------*/
  
  if (req && (req_len == sizeof(msg_get_ssid_ranges_adv_req_type)) && (req->opcode == (uint8)MSG_EXT_SUBCMD_GET_RANGES) )
  {
    /*-------------------------------------------------------------------------
      calculate number of valid ssids and the length of the responce packet.
    -------------------------------------------------------------------------*/
    rsp_len = FPOS (msg_get_ssid_ranges_adv_rsp_type, ssid_ranges) +
      MSG_MASK_TBL_CNT * FSIZ (msg_get_ssid_ranges_adv_rsp_type, ssid_ranges);

    /*-------------------------------------------------------------------------
      allocate space for the responce packet
    -------------------------------------------------------------------------*/
    rsp = (msg_get_ssid_ranges_adv_rsp_type *)
      diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_EXT_MSG_CONFIG_ADV, rsp_len);

    if (rsp)
    {
      rsp->header.cmd_code = req->cmd_code;
      rsp->header.cmd_version = req->cmd_version;
      rsp->header.opcode = req->opcode;
      rsp->status = TRUE;
      rsp->range_cnt = MSG_MASK_TBL_CNT;
      rsp->rsvd = 0;

      /*-------------------------------------------------------------------------
        make aux_access_ptr point to the end of the msg_get_ssid_ranges_rsp_type
        which is where the SSIDd should start.
      -------------------------------------------------------------------------*/

      for (i = 0; i < MSG_MASK_TBL_CNT; i++)
      {
        rsp->ssid_ranges[i].ssid_first = msg_mask_tbl[i].ssid_first;
        rsp->ssid_ranges[i].ssid_last = msg_mask_tbl[i].ssid_last;
      }
    }
  }                
  else
  {
    rsp = (void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len);
  }

  return (rsp);
}               /* msg_pkt_get_ssid_ranges_adv () */



/*===========================================================================

FUNCTION MSG_PKT_GET_MASKS

DESCRIPTION
  This function will retrieve an array of masks of specified type for the
  range of SSIDs provided.

DEPENDENCIES
  req is not NULL and points to a valid  request packet

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
static void *
msg_pkt_get_masks (void *req_pkt, word req_len, msg_mask_type mask_type)
{
  msg_get_mask_req_type *req = (msg_get_mask_req_type *) req_pkt;
  msg_get_mask_rsp_type *rsp = NULL;
  unsigned int rsp_len = 0;

/*-------------------------------------------------------------------------*/

  if (req && req->ssid_start <= req->ssid_end &&
      (req->sub_cmd == (uint8) MSG_EXT_SUBCMD_GET_BUILD_MASK ||
       req->sub_cmd == (uint8) MSG_EXT_SUBCMD_GET_RT_MASK))
  {
    rsp_len = FPOS (msg_get_mask_rsp_type, bld_mask) +
      ((req->ssid_end - req->ssid_start + 1) *
       FSIZ (msg_get_mask_rsp_type, bld_mask));

    rsp = (msg_get_mask_rsp_type *)
      diagpkt_alloc (DIAG_EXT_MSG_CONFIG_F, rsp_len);

    if (rsp)
    {
      rsp->sub_cmd = req->sub_cmd;
      rsp->ssid_start = req->ssid_start;
      rsp->ssid_end = req->ssid_end;
      rsp->pad = 0;

      rsp->status = msg_get_ssid_masks_adv (mask_type, req->ssid_start,
                    req->ssid_end, rsp->bld_mask, DIAG_STREAM_1, diag_cur_preset_id);
    }
  }
  else
  {
    rsp = (msg_get_mask_rsp_type *)
      diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len);
  }

  return (rsp);

}               /* msg_pkt_get_masks() */

static void *
msg_pkt_get_masks_adv (void *req_pkt, word req_len, msg_mask_type mask_type)
{
  msg_get_mask_adv_req_type *req = (msg_get_mask_adv_req_type *) req_pkt;
  msg_get_mask_adv_rsp_type *rsp = NULL;
  msg_get_rt_mask_adv_req_type *req_rt = (msg_get_rt_mask_adv_req_type *) req_pkt;
  msg_get_rt_mask_adv_rsp_type *rsp_rt = NULL;
  
  unsigned int rsp_len = 0;

/*-------------------------------------------------------------------------*/

  if (req && 
      (req->header.opcode == (uint8) MSG_EXT_SUBCMD_GET_BUILD_MASK ||
       req->header.opcode == (uint8) MSG_EXT_SUBCMD_GET_RT_MASK))
  {

    if(req->header.opcode == (uint8) MSG_EXT_SUBCMD_GET_BUILD_MASK)
    {
      if(req->ssid_first > req->ssid_last)
      {
        return ((void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len));
      }
         rsp_len = FPOS (msg_get_mask_adv_rsp_type, bld_mask) +
        ((req->ssid_last - req->ssid_first + 1) *
         FSIZ (msg_get_mask_adv_rsp_type, bld_mask));
  
      rsp = (msg_get_mask_adv_rsp_type *)
            diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_EXT_MSG_CONFIG_ADV, rsp_len);
  
      if (rsp)
      {
        rsp->header = req->header;
        rsp->ssid_first = req->ssid_first;
        rsp->ssid_last = req->ssid_last;
        rsp->rsvd = 0;
  
        rsp->status = msg_get_ssid_masks_adv (mask_type, req->ssid_first,
                      req->ssid_last, (uint32 *)(rsp->bld_mask), 
                      DIAG_STREAM_1, diag_cur_preset_id);  // stream_id and preset_id doesn't matter here.
      }

      return (void *)rsp;
    }
    else 
    {
      if( (req_rt->header.cmd_version == 1) && (req_rt->id.stream_id >=DIAG_MIN_STREAM_ID) && 
          (req_rt->id.stream_id <= DIAG_MAX_STREAM_ID) && (req_rt->ssid_last >= req_rt->ssid_first) )
      {
        rsp_len = FPOS (msg_get_rt_mask_adv_rsp_type, rt_mask) +
        ((req_rt->ssid_last - req_rt->ssid_first + 1) *
         FSIZ (msg_get_rt_mask_adv_rsp_type, rt_mask));
  
        rsp_rt = (msg_get_rt_mask_adv_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_EXT_MSG_CONFIG_ADV, rsp_len);
                  
        if( rsp_rt )
        {
          rsp_rt->header = req_rt->header;
          rsp_rt->id.stream_id = req_rt->id.stream_id;
          rsp_rt->ssid_first = req_rt->ssid_first;
          rsp_rt->ssid_last = req_rt->ssid_last;

          rsp_rt->status = msg_get_ssid_masks_adv( mask_type, req_rt->ssid_first,
                                                   req_rt->ssid_last, (uint32 *)(rsp_rt->rt_mask), 
                                                   req_rt->id.stream_id, diag_cur_preset_id );
        }
        return (void *)rsp_rt;
      }
      else if( (req_rt->header.cmd_version == 2) && (req_rt->id.preset_id >=DIAG_MIN_PRESET_ID) && 
               (req_rt->id.preset_id <= DIAG_MAX_PRESET_ID) && (req_rt->ssid_last >= req_rt->ssid_last) )
      {
        rsp_len = FPOS (msg_get_rt_mask_adv_rsp_type, rt_mask) +
        ((req_rt->ssid_last - req_rt->ssid_first + 1) *
         FSIZ (msg_get_rt_mask_adv_rsp_type, rt_mask));
  
        rsp_rt = (msg_get_rt_mask_adv_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_EXT_MSG_CONFIG_ADV, rsp_len);
                  
        if( rsp_rt )
        {
          rsp_rt->header = req_rt->header;
          rsp_rt->id.preset_id = req_rt->id.preset_id;
          rsp_rt->ssid_first = req_rt->ssid_first;
          rsp_rt->ssid_last = req_rt->ssid_last;

          rsp_rt->status = msg_get_ssid_masks_adv( mask_type, req_rt->ssid_first,
                                                   req_rt->ssid_last, (uint32 *)(rsp_rt->rt_mask), 
                                                   DIAG_STREAM_1, req_rt->id.preset_id );
        }
        return (void *)rsp_rt;
      }
      else
      {
        return ( (void *)diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len) );
      }
    }
  }
  else
  {    
     return ((void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len));
  }
}               /* msg_pkt_get_masks_adv() */

/*===========================================================================
FUNCTION MSG PKT GET LOCAL MASKS

DESCRIPTION
  This function will return the
  message mask values for a valid ssid range specified in the request packet.
  This function queries the local Diag message mask table, as opposed to msg_pkt_get_masks(),
  which queries the target master message mask table

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/ 
  PACK(void *)
msg_pkt_get_local_masks(PACK(void *) req_pkt, uint16 pkt_len)
{ 
  msg_get_local_mask_req_type *req = (msg_get_local_mask_req_type *) req_pkt;
  msg_get_local_mask_rsp_type *rsp = NULL;
  unsigned int rsp_len = 0;
  diagpkt_subsys_cmd_code_type cmd_code = 0;
  uint8 preset_id;
  
  if(req == NULL) 
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "msg_pkt_get_local_masks(): Request pointer is NULL \n",1);
   return NULL; 
  }

   cmd_code = diagpkt_subsys_get_cmd_code (req_pkt);

  if ((pkt_len != sizeof(msg_get_local_mask_req_type)) || 
       ((cmd_code != DIAGDIAG_MSG_MASK_RETRIEVAL_MODEM) && (cmd_code != DIAGDIAG_MSG_MASK_RETRIEVAL_APP)
         && (cmd_code != DIAGDIAG_MSG_MASK_RETRIEVAL_Q6) && (cmd_code != DIAGDIAG_MSG_MASK_RETRIEVAL_RIVA)) || 
       (req->stream_id > DIAG_MAX_STREAM_ID) || (req->stream_id < DIAG_MIN_STREAM_ID))
  {
     MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "MSG_2 cmd_code %d stream id %d \n",cmd_code,req->stream_id);
     return ((void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, pkt_len));
  }

  if( req->stream_id == DIAG_STREAM_1 )
  {
    preset_id = diag_cur_preset_id;
  }
  else if( req->stream_id == DIAG_STREAM_2 )
  {
    preset_id = DIAG_DCI_MASK_1;
  }
  else
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Invalid stream_id = %d ", req->stream_id);
    return ((void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, pkt_len));
  }
  
/*-------------------------------------------------------------------------*/

  if (req && req->ssid_start <= req->ssid_end && diagcomm_status(DIAGCOMM_PORT_1))     
  {
    rsp_len = FPOS (msg_get_local_mask_rsp_type, msg_mask) +
        ((req->ssid_end - req->ssid_start + 1) *
         FSIZ (msg_get_local_mask_rsp_type, msg_mask));

     rsp = (msg_get_local_mask_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, 
                                              cmd_code, rsp_len);

    if (rsp)
    {
      rsp->ssid_start = req->ssid_start;
      rsp->ssid_end = req->ssid_end;
      rsp->stream_id = req->stream_id;
      rsp->status = msg_get_ssid_masks_adv (MSG_MASK_RT, req->ssid_start,
                    req->ssid_end, rsp->msg_mask, req->stream_id, preset_id);
      rsp->padding = 0;
    }
  }

  return ((void *) rsp);

} /* msg_pkt_get_local_masks */

/*===========================================================================

FUNCTION  MSG_PKT_SET_RT_MASKS

DESCRIPTION
  This function will SET the  value of the "tool" mask for the
  specific SubSystem ID and prepare a responce packet. If the
  SSID is not valid it will return Status = false.

DEPENDENCIES
  req is not NULL and points to a valid  request packet

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
static void *
msg_pkt_set_rt_masks (void *req_pkt, word req_len)
{
  msg_set_rt_mask_req_type *req = (msg_set_rt_mask_req_type *) req_pkt;
  msg_set_rt_mask_rsp_type *rsp = NULL;
  uint32 *mask = NULL;
  unsigned int rsp_len = req_len;   /* Same as request */
  int i;
  boolean success = TRUE;

/*-------------------------------------------------------------------------*/

  if (req && req->sub_cmd == MSG_EXT_SUBCMD_SET_RT_MASK &&
      req->ssid_start <= req->ssid_end)
  {
    rsp = (msg_set_rt_mask_rsp_type *)
      diagpkt_alloc (DIAG_EXT_MSG_CONFIG_F, rsp_len);

    if (rsp)
    {
      memscpy ((void *) rsp, ((FPOS(msg_set_rt_mask_req_type,rt_mask)) + (4 * ((req->ssid_end - req->ssid_start)+1))), (void *) req, rsp_len);

      rsp->status = TRUE;   /* Initialize */

      mask = req->rt_mask;

      for (i = req->ssid_start; i <= req->ssid_end; i++)
      {
        success = msg_set_rt_mask_adv ((uint16) i, *mask, DIAG_STREAM_1, diag_cur_preset_id);

        if (success == FALSE)
        {
          rsp->status = FALSE;
        }

        mask++;         /* Advance to next entry in array */
      }

#if defined(DIAG_MP_MASTER)
      /* Propagate masks to Slaves. */
      diagcomm_mask_update( DIAG_UPDATE_MASK_PARTIAL_F3, DIAG_STREAM_1, diag_cur_preset_id, TRUE );
#endif

      /* Send the updated mask to every LSM registered with DCM. */
      (void)diag_client_maskchange(DIAG_MSGSVC_MASK_CHANGE);

      (void) msg_sleep_vote ();
    }
  }
  else
  {
    rsp = (msg_set_rt_mask_rsp_type *)
      diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len);
  }

  return (rsp);

}               /* msg_pkt_set_rt_masks() */

/* Function to set run-time masks based on stream_id */

static void *
msg_pkt_set_rt_masks_adv (void *req_pkt, word req_len)
{
  msg_set_rt_mask_adv_req_type *req = (msg_set_rt_mask_adv_req_type *) req_pkt;
  msg_set_rt_mask_adv_rsp_type *rsp = NULL;
  uint32 *mask = NULL;
  unsigned int rsp_len = req_len;   /* Same as request */
  int i;
  boolean success = TRUE;
  boolean send_error = TRUE;
  byte stream_id;
  uint8 preset_id;
  
/*-------------------------------------------------------------------------*/

  if (req && (req->header.opcode == MSG_EXT_SUBCMD_SET_RT_MASK) && (req->ssid_first <= req->ssid_last) )
  {
    if( req->header.cmd_version == 1 )
    {
      /* stream_id = 0 is not valid here */
      if( (req->id.stream_id >= DIAG_MIN_STREAM_ID) && (req->id.stream_id <= DIAG_MAX_STREAM_ID) )
      {
        send_error = FALSE;
        stream_id = req->id.stream_id;
        preset_id = diag_cur_preset_id;
      }
    }
    else if( req->header.cmd_version == 2 )
    {
      /* preset_id = 0 is not valid here */
      if( (req->id.preset_id >= DIAG_MIN_PRESET_ID) && (req->id.preset_id <= DIAG_MAX_PRESET_ID) )
      {
        send_error = FALSE;
        stream_id = DIAG_STREAM_1;
        preset_id = req->id.preset_id;
      }
    }
  }
  
  if( send_error )
  {
    rsp = (msg_set_rt_mask_adv_rsp_type *)
      diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len);
  }
  else
  {
    rsp = (msg_set_rt_mask_adv_rsp_type *)
          diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_EXT_MSG_CONFIG_ADV, rsp_len);

    if (rsp)
    {
      memscpy ((void *) rsp, ((FPOS(msg_set_rt_mask_adv_req_type,rt_mask)) + (4 * ((req->ssid_last - req->ssid_first)+1))), (void *) req, rsp_len);

      rsp->status = TRUE;   /* Initialize */

      mask = (uint32 *)(req->rt_mask);

      for (i = req->ssid_first; i <= req->ssid_last; i++)
      {
        success = msg_set_rt_mask_adv ((uint16) i, *mask, stream_id, preset_id);

        if (success == FALSE)
        {
          rsp->status = FALSE;
        }

        mask++;         /* Advance to next entry in array */
      }

#if defined(DIAG_MP_MASTER)
      /* Propagate masks to Slaves. */
      diagcomm_mask_update( DIAG_UPDATE_MASK_PARTIAL_F3, stream_id, preset_id, TRUE );
#endif

      /* Send the updated mask to every LSM registered with DCM. */
      (void)diag_client_maskchange(DIAG_MSGSVC_MASK_CHANGE);

      (void) msg_sleep_vote ();
    }
  }

  return (rsp);

} /* msg_pkt_set_rt_masks_adv */


/*===========================================================================

FUNCTION   MSG_PKT_SET_ALL_MASKS

DESCRIPTION
  This function will SET the  values of the "tool" masks for all
  the valid SubSystem IDs and prepare a responce packet.

DEPENDENCIES
  req is not NULL and points to a valid  request packet

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
static void *
msg_pkt_set_all_masks (void *req_pkt, word req_len)
{
  msg_set_all_masks_req_type *req = (msg_set_all_masks_req_type *) req_pkt;
  msg_set_all_masks_rsp_type *rsp = NULL;
  const unsigned int rsp_len = sizeof (msg_set_all_masks_rsp_type);

/*-------------------------------------------------------------------------*/

  if (req && req->sub_cmd == MSG_EXT_SUBCMD_SET_ALL_RT_MASKS)
  {
    rsp = (msg_set_all_masks_rsp_type *)
      diagpkt_alloc (DIAG_EXT_MSG_CONFIG_F, rsp_len);

    /*-------------------------------------------------------------------------
      fill in the fields of the responce packet
    -------------------------------------------------------------------------*/
    if(rsp)
    {
        rsp->sub_cmd = req->sub_cmd;
        rsp->status = TRUE;
        rsp->rsvd = 0;
        rsp->rt_mask = req->rt_mask;
    }
    /*-------------------------------------------------------------------------
      set all the real time masks to the new requested value.
    -------------------------------------------------------------------------*/
    msg_set_all_rt_masks_adv (req->rt_mask, DIAG_STREAM_1, diag_cur_preset_id);
  }
  else
  {
    rsp = (msg_set_all_masks_rsp_type *)
      diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len);
  }

  return (rsp);

}               /* msg_pkt_set_all_masks() */

static void *
msg_pkt_set_all_masks_adv (void *req_pkt, word req_len)
{
  byte stream_id;
  uint8 preset_id;
  boolean send_error = TRUE;
  unsigned int rsp_len=0;
  msg_set_all_masks_adv_rsp_type *rsp = NULL;
  msg_set_all_masks_adv_req_type *req = (msg_set_all_masks_adv_req_type *) req_pkt;
  
  rsp_len = sizeof (msg_set_all_masks_adv_rsp_type);
  
  /*-------------------------------------------------------------------------*/

  if (req && (req->header.opcode == MSG_EXT_SUBCMD_SET_ALL_RT_MASKS) )
  {
    if( req->header.cmd_version == 1 )
    {
      if(req->id.stream_id <= DIAG_MAX_STREAM_ID)
      {
        send_error = FALSE;
        stream_id = req->id.stream_id;
        preset_id = diag_cur_preset_id;
      }
    }
    else if( req->header.cmd_version == 2 )
    {
      if(req->id.preset_id <= DIAG_MAX_PRESET_ID)
      {
        send_error = FALSE;
        stream_id = DIAG_STREAM_1;
        preset_id = req->id.preset_id;
      }
    }
  }
  
  if( send_error )
  {
    rsp = (msg_set_all_masks_adv_rsp_type *)
      diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len);
  }
  else
  {
    rsp = (msg_set_all_masks_adv_rsp_type *)
          diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_EXT_MSG_CONFIG_ADV, rsp_len);

    /*-------------------------------------------------------------------------
      fill in the fields of the responce packet
    -------------------------------------------------------------------------*/
    if(rsp)
    {
      rsp->header = req->header;
      rsp->status = TRUE;
      rsp->rt_mask = req->rt_mask;
      
      if( req->header.cmd_version == 1 )
        rsp->id.stream_id = req->id.stream_id;
      else if( req->header.cmd_version == 2 )
        rsp->id.preset_id = req->id.preset_id;
    }
    /*-------------------------------------------------------------------------
      set all the real time masks to the new requested value.
    -------------------------------------------------------------------------*/
    msg_set_all_rt_masks_adv (req->rt_mask, stream_id, preset_id);
  }

  return (rsp);

} /* msg_pkt_set_all_masks_adv() */


/*===========================================================================

FUNCTION MSG_PKT_PROCESS_CONFIG

DESCRIPTION
  This function will figure out the type of extended message request
  by examining the message command and then call the appropriate
  function to handle the request.

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACK(void *)
msg_pkt_process_config (PACK(void *) req_pkt, word req_len)
{
  msg_get_ssid_ranges_req_type *req =
    (msg_get_ssid_ranges_req_type *) req_pkt;
  void *rsp = NULL;
  boolean valid_cmd = TRUE;
  boolean send_rsp = TRUE;
  #if defined(DIAG_MP_MASTER)
  boolean set_mask_req = FALSE;
  #endif
    
  /*-------------------------------------------------------------------------*/
  
  if (req && req->cmd_code == DIAG_EXT_MSG_CONFIG_F)
  {
    /* dispatch acording to subcommand */
    switch (req->sub_cmd)
    {
    case MSG_EXT_SUBCMD_GET_RANGES:

    /*---------------------------------------------------------------------
          This will prepare a packet with all valid SubSystem IDs.
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_get_ssid_ranges (req, req_len);
      break;

    case MSG_EXT_SUBCMD_GET_BUILD_MASK:

    /*---------------------------------------------------------------------
          Will return a packet with the "build" mask for a specific SSID
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_get_masks (req, req_len, MSG_MASK_BT);
      break;

    case MSG_EXT_SUBCMD_GET_RT_MASK:

    /*---------------------------------------------------------------------
          Returns a packet with the "tool" mask for a specific SSID
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_get_masks (req, req_len, MSG_MASK_RT);

      break;

    case MSG_EXT_SUBCMD_SET_RT_MASK:

    /*---------------------------------------------------------------------
          forces mobile to set the "tool" mask for a specific SSID
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_set_rt_masks (req, req_len);
      #if defined(DIAG_MP_MASTER)
      set_mask_req = TRUE;
      #endif
      break;

    case MSG_EXT_SUBCMD_SET_ALL_RT_MASKS:

    /*---------------------------------------------------------------------
          set "tool" masks for all valid SSIDs to a given value.
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_set_all_masks (req, req_len);
      #if defined(DIAG_MP_MASTER)
      set_mask_req = TRUE;
      #endif
      break;

    default:            /* nothing to do */
      rsp = (void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req, req_len);
      valid_cmd = TRUE;
      break;
    }

    if (valid_cmd)
    {
      msg_mode = MSG_MODE_STD;
    }
  }
  else
  {
    rsp = (void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req, req_len);
  }
  
 /* if master processor on multi processor build, don't send response */
  #if !defined(DIAG_RSP_SEND)
  if (!diagcomm_apps_only_config)
    send_rsp = FALSE;
  #endif

  #if defined(DIAG_MP_MASTER)
    /* MP Master now responds to "set mask" requests */
    if( set_mask_req )
      send_rsp = TRUE;
  #endif
  
 /* if diag is not connected, don't send response */
  if (!diagcomm_status(DIAGCOMM_PORT_1))
  {
    send_rsp = FALSE;
  }

  #if defined (FEATURE_DIAG_ADSP_CALLBACK)
      adsp_dspdiag125_callback(req_pkt, req_len);
  #endif /* FEATURE_DIAG_ADSP_CALLBACK */
 
  /* return response */
  if (!send_rsp)
  {
    diagpkt_free(rsp);
    return NULL;
  }
  else
  {
    return ((void *) rsp);
  }

} /* msg_pkt_process_config */


/*===========================================================================

FUNCTION EXT_MSG_PKT_CONFIG_ADV

DESCRIPTION
  Function to handle dual mask msg config requests.

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACK(void *)
ext_msg_pkt_config_adv (PACK(void *) req_pkt, word req_len)
{
  ext_config_header *req = (ext_config_header *) req_pkt;
  void *rsp = NULL;
  boolean valid_cmd = TRUE;
  boolean send_rsp = TRUE;
  
  if ( req && (req->subsys_cmd_code == DIAG_EXT_MSG_CONFIG_ADV) 
           && ((req->cmd_version == 1) || (req->cmd_version == 2)) )
  {
    
    /* dispatch acording to subcommand */
    switch (req->opcode)
    {
    case MSG_EXT_SUBCMD_GET_RANGES:

    /*---------------------------------------------------------------------
          This will prepare a packet with all valid SubSystem IDs.
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_get_ssid_ranges_adv (req, req_len);
      break;

    case MSG_EXT_SUBCMD_GET_BUILD_MASK:

    /*---------------------------------------------------------------------
          Will return a packet with the "build" mask for a specific SSID
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_get_masks_adv (req, req_len, MSG_MASK_BT);
      break;

    case MSG_EXT_SUBCMD_GET_RT_MASK:

    /*---------------------------------------------------------------------
          Returns a packet with the "tool" mask for a specific SSID
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_get_masks_adv (req, req_len, MSG_MASK_RT);

      break;

    case MSG_EXT_SUBCMD_SET_RT_MASK:

    /*---------------------------------------------------------------------
          forces mobile to set the "tool" mask for a specific SSID
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_set_rt_masks_adv (req, req_len);
      break;

    case MSG_EXT_SUBCMD_SET_ALL_RT_MASKS:

    /*---------------------------------------------------------------------
          set "tool" masks for all valid SSIDs to a given value.
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_set_all_masks_adv (req, req_len);
      break;

    default:            /* nothing to do */
      rsp = (void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req, req_len);
      valid_cmd = TRUE;
      break;
    }

    if (valid_cmd)
    {
      msg_mode = MSG_MODE_STD;
    }
  }
  else
  {
    rsp = (void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req, req_len);
  }

  /* if master processor on multi processor build, don't send response */
  #if !defined(DIAG_RSP_SEND)
  if (!diagcomm_apps_only_config)
    send_rsp = FALSE;
  #endif
  
  #if defined(DIAG_MP_MASTER)
    /* MP Master now responds to "set mask" requests.
       If we're in buffering mode, Master needs to send out the responses.
       If Modem is not up, Master needs to send out responses as well. */
    send_rsp = TRUE;
  #endif
  
 /* if diag is not connected, don't send response */
  if (!diagcomm_status(DIAGCOMM_PORT_1))
  {
    send_rsp = FALSE;
  }

  #if defined (FEATURE_DIAG_ADSP_CALLBACK)
      adsp_dspdiag125_callback(req_pkt, req_len);
  #endif /* FEATURE_DIAG_ADSP_CALLBACK */
 
  /* return response */
  if (!send_rsp)
  {
    diagpkt_free(rsp);
    return NULL;
  }
  else
  {
    return ((void *) rsp);
  }
} /* ext_msg_pkt_config_adv */

/*===========================================================================
 
FUNCTION MSG_MASKS_ENABLED_CHECK

DESCRIPTION
  This function checks the runtime masks for all SSIDs for a particular stream
  to determine if the message service is "off".  It will return TRUE if the 
  service is "off", false otherwise. 

===========================================================================*/
static boolean            
msg_masks_enabled_check(byte stream_id)
{
   boolean masks_enabled = FALSE;
   uint32 msg_mask_tbl_index, ssid_index, ssid_count;
   
   if ( (stream_id > DIAG_MAX_STREAM_ID) || (stream_id < DIAG_MIN_STREAM_ID) )
   {
     MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "msg_masks_enabled_check Sanity check failed");
     return FALSE;
   }
   
   for (msg_mask_tbl_index = 0; !masks_enabled && msg_mask_tbl_index < MSG_MASK_TBL_CNT; msg_mask_tbl_index++)
   {
    ssid_count = (uint32) (msg_mask_tbl[msg_mask_tbl_index].ssid_last - msg_mask_tbl[msg_mask_tbl_index].ssid_first + 1);
    for(ssid_index = 0; !masks_enabled && ssid_index < ssid_count; ssid_index++)
    {
      /* Set the masks_enabled flag to TRUE when a mask is set in msg_mask_tbl */
      if(msg_mask_tbl[msg_mask_tbl_index].rt_mask_array[stream_id-1][ssid_index])
      {
        masks_enabled = TRUE;
      }
    }
  }
  
  /* Sets/Reset the corresponding bit in F3 Enabled Mask */
  if(masks_enabled)
  {
    if (stream_id == DIAG_STREAM_1)
    {
      DIAG_SET_MASK(msg_enabled_mask, DIAG_MSG_STREAM_1);
    }
    #ifdef DIAG_STREAM_2_MASK_CHECK
    else if (stream_id == DIAG_STREAM_2)
    {
      DIAG_SET_MASK(msg_enabled_mask, DIAG_MSG_STREAM_2);
    }
    #endif
  }
  else
  {
    if (stream_id == DIAG_STREAM_1)
    {
      DIAG_CLR_MASK(msg_enabled_mask, DIAG_MSG_STREAM_1);
    }
    #ifdef DIAG_STREAM_2_MASK_CHECK
    else if (stream_id == DIAG_STREAM_2)
    {
      DIAG_CLR_MASK(msg_enabled_mask, DIAG_MSG_STREAM_2);
    }
    #endif
  }
  
  return masks_enabled;
  
} /* msg_masks_enabled_check */


/*===========================================================================
 
FUNCTION MSG_SLEEP_VOTE

DESCRIPTION
  This function checks the runtime masks for all SSIDs to determine if the
  message service is "off".  If "off", it will clear the corresponding bit
  in the msg_enabled_mask bitmask; else will set the corresponding bit
  and it will vote to the diagnostic task that it is OK to sleep.

===========================================================================*/
static boolean
msg_sleep_vote (void)
{
  boolean masks_enabled_stream_1 =  FALSE;
  #ifdef DIAG_STREAM_2_MASK_CHECK
  boolean masks_enabled_stream_2 =  FALSE;
  #endif

  masks_enabled_stream_1 = msg_masks_enabled_check(DIAG_STREAM_1);
  #ifdef DIAG_STREAM_2_MASK_CHECK
  masks_enabled_stream_2 = msg_masks_enabled_check(DIAG_STREAM_2);
  #endif
  
  /* Vote for sleep only if Stream 1 Masks are not enabled */
  if( masks_enabled_stream_1 )
  {
    diag_sleep_vote (DIAG_SLEEP_MSG, FALSE);
  }
  else
  {
    diag_sleep_vote (DIAG_SLEEP_MSG, TRUE);
  }
  
  return !masks_enabled_stream_1;  /* If masks are disabled, vote for sleep */
  
} /* msg_sleep_vote */


/* Legacy MSG service packet handler */

/*===========================================================================

FUNCTION MSG_PKT_LEGACY_REQ

DESCRIPTION
  This procedure processes a request to retreive the accumulated msgs in
  the mobile's msg buffer.

===========================================================================*/
PACK(void *)
msg_pkt_legacy_req (PACK(void *) req_ptr, word pkt_len)
{
  msg_legacy_req_type *req = (msg_legacy_req_type *) req_ptr;
  msg_legacy_rsp_type *rsp = NULL;
  const unsigned int rsp_len = sizeof (msg_legacy_rsp_type);
  boolean send_rsp = TRUE;

  (void) pkt_len; /* suppress compiler warning */


#if defined (MSG_LEVEL) && (MSG_LEVEL != MSG_LVL_NONE)
  /* Suppress msg_file compiler warning. ** This compiler optimizes this to
     nothing, ** but msg_file is now referenced. */
#endif

  /* Legacy values for level are used to index this array. Legacy values were:
     #define MSG_LVL_LOW     0 #define MSG_LVL_MED     1 #define MSG_LVL_HIGH 2
     #define MSG_LVL_ERROR   3 #define MSG_LVL_FATAL   4 #define MSG_LVL_NONE
     255 */

  if (req->msg_level <
      sizeof (msg_legacy_rt_masks) / sizeof (msg_legacy_rt_masks[0]))
  {
    msg_set_all_rt_masks_adv (msg_legacy_rt_masks[req->msg_level], DIAG_STREAM_1, diag_cur_preset_id);
  }
  else
  {
    msg_set_all_rt_masks_adv (0, DIAG_STREAM_1, diag_cur_preset_id);
  }

  (void) msg_sleep_vote ();

  /* Set mode to legacy mode. */
  msg_mode = MSG_MODE_LEGACY;

  /* if master processor on multi processor build, don't send response */
  #if !defined(DIAG_RSP_SEND)
  if (!diagcomm_apps_only_config)
    send_rsp = FALSE;
  #endif

  #if defined(DIAG_MP_MASTER)
    /* MP Master now responds to "set mask" requests */
    send_rsp = TRUE;
  #endif
  
/* if diag is not connected, don't send response */
  if (!diagcomm_status(DIAGCOMM_PORT_1))
  {
    send_rsp = FALSE;
  }


  /*----------------------------------------------------------------------
    Format the response packet.
  ----------------------------------------------------------------------*/
  if (send_rsp)
  {
    rsp = (msg_legacy_rsp_type *) diagpkt_alloc (DIAG_MSG_F, rsp_len);

    if (rsp)
    {
      rsp->total_msgs = msg_total;

      /*----------------------------------------------------
      Since we are streaming, this response is an ACK.
      Set qty to 0.
      ----------------------------------------------------*/
      rsp->qty = 0;

      rsp->drop_cnt = msg_dropped;

    }
  }
  
  return ((void *) rsp);
  

}               /* msg_pkt_legacy_req */

#if defined(T_WINNT)
#error code not present
#endif /* T_WINNT */




/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====

        Mobile Diagnostic Message Services for QSHRINK feature
        
====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* Statistics */
static uint32 qsr_msg_total;    /* total number of messages logged */


typedef enum
{
  QSR_MSG_EXPAND_STATE_HDR = 0,
  QSR_MSG_EXPAND_STATE_DESC,
  QSR_MSG_EXPAND_STATE_HASH_VALUE,
  QSR_MSG_EXPAND_STATE_ARGS,
  QSR_MSG_EXPAND_STATE_COMPLETE
}
qsr_msg_expand_enum_type;

/*===========================================================================

FUNCTION MSG_QSR_EXT_EXPAND_AND_SEND_PKT

DESCRIPTION
  This routine sends out the extended pkt header and calls the appropriate
  function to expands the intermediate storage format into the full packet
  format and send out the F3.

DEPENDENCIES
  msg_init() must have been called previously.

RETURN VALUE

===========================================================================*/
static void
msg_qsr_ext_expand_and_send_pkt(diag_send_desc_type *desc, byte stream_id)
{
  msg_v3_multi_client_store_type *msg_store = NULL;
  msg_store = (msg_v3_multi_client_store_type *)desc->pkt;
  diag_send_desc_type send = { NULL, NULL, DIAG_SEND_STATE_START, FALSE };
  msg_send_multi_client_type *msg_ext = NULL;
  msg_ext = (msg_send_multi_client_type *)desc->pkt;

  if (msg_store)
  { 
     if (msg_ext->msg.hdr.cmd_code == DIAG_EXT_MSG_F)
    {
      send.pkt = (void *)&msg_ext->cmd_code;
      send.last = (void *)((uint32)send.pkt + FPOS(msg_send_multi_client_type, msg) - 1);
      send.state = DIAG_SEND_STATE_START;
      send.terminate = FALSE;
      desc->pkt = (void *)&msg_ext->msg.hdr;
      //desc->state = send.state;
      if (stream_id == DIAG_STREAM_1)
      {
        /*Send the extended message only on Stream 1 since 
          DCI does not have support for extended packets*/
        (void)diagbuf_send_pkt(&send, stream_id);
        if (send.state != DIAG_SEND_STATE_COMPLETE)
        {
          return;
        }
      }
      send.pkt = NULL;
      send.last = NULL;
      send.state = DIAG_SEND_STATE_BUSY;
      send.terminate = FALSE;
      diag_msg_sprintf_ext = TRUE;
     msg_sprintf_expand_and_send_pkt (desc, stream_id);
    }
    else if (msg_store->msg.hdr.cmd_code == DIAG_QSR_EXT_MSG_TERSE_F)
    {
      send.pkt = (void *)&msg_store->cmd_code;
      send.last = (void *)((uint32)send.pkt + FPOS(msg_v3_multi_client_store_type, msg) - 1);
      send.state = DIAG_SEND_STATE_START;
      send.terminate = FALSE;
      desc->pkt = (void *)&msg_store->msg.hdr;
      diag_msg_ext = TRUE;
      if (stream_id == DIAG_STREAM_1)
      {
        /*Send the extended message only on Stream 1 since 
          DCI does not have support for extended packets*/
        (void)diagbuf_send_pkt(&send, stream_id);
        if (send.state != DIAG_SEND_STATE_COMPLETE)
        {
          return;
        }
      }
      send.pkt = NULL;
      send.last = NULL;
      send.state = DIAG_SEND_STATE_BUSY;
      send.terminate = FALSE;
      msg_qsr_expand_and_send_pkt(desc, stream_id);
    }
    else
    {
      /*Unknown case- move the last pointer and continue 
        draining the other pkts*/
      desc->pkt = (void *)((byte *)desc->last + 1);
      desc->state = DIAG_SEND_STATE_COMPLETE;
      
      return;
    }
  }
}

/*===========================================================================

FUNCTION MSG_QSR_EXPAND_AND_SEND_PKT

DESCRIPTION
  This routine expands the intermediate storage format into the full
  packet format, copying data from the constant block in 'stored_msg'.

DEPENDENCIES
  msg_init() must have been called previously.

RETURN VALUE

===========================================================================*/
static void
msg_qsr_expand_and_send_pkt(diag_send_desc_type *desc, byte stream_id)
{
  msg_v3_store_type *msg_store = NULL;
  diagbuf_status_enum_type status = DIAGBUF_READY_S;
  diag_msg_type msg_notify;
  msg_desc_type ldesc;

  static diag_send_desc_type send = { NULL, NULL, DIAG_SEND_STATE_START, FALSE };
  static qsr_msg_expand_enum_type state_sav = QSR_MSG_EXPAND_STATE_HDR; /* Used to store state of process. */
  int filename_len;
  char *str_buf;

  /* Local copies of static variables for optimized access.  
     This makes this routine persistent but not re-entrant. */
  qsr_msg_expand_enum_type state = state_sav;


  if (desc)
  {
    uint16 msg_line = 0;
    uint16 msg_ssid = 0;
    uint32 msg_mask = 0;
    uint16 ssid_index = ~0; /* make this a thoroughly invalid number unless it gets set to something correct */
    /* 'decs' is not moved until the entire message is complete.  Therefore,
       it always points to the stored message, in terse format. */
    msg_store = (msg_v3_store_type *)desc->pkt;

    /* pre-qshrink 3.0 compatibility system */
    if (msg_store == NULL) {
       return;
    }
       if ((msg_store->qsr_flag & QSR_BITFLAG_IS_V3) &&
           msg_store->qsr_const_data_ptr.v3_index->ssid_index < msg_v3_ss_num_table_entries)
       {
         ssid_index = msg_store->qsr_const_data_ptr.v3_index->ssid_index;
         msg_ssid = msg_v3_ss_table[ssid_index]->ss_id;
         msg_mask = msg_v3_ss_table[ssid_index]->ss_mask;
         msg_line = msg_store->qsr_const_data_ptr.v3_index->line;
       }
       else
       {
         msg_line = msg_store->qsr_const_data_ptr.msg_desc->desc.line;
         msg_ssid = msg_store->qsr_const_data_ptr.msg_desc->desc.ss_id;
         msg_mask = msg_store->qsr_const_data_ptr.msg_desc->desc.ss_mask;
       }
    
    if (desc->state == DIAG_SEND_STATE_START)
    {
      /* Increment state to indicate the packet is now in progress. */
      desc->state++;

      /* Keep stats of total messages */
      qsr_msg_total++;

      if (msg_store)
      {
        /* Add dropped messages to the total. */
        qsr_msg_total += msg_store->hdr.drop_cnt;

#if !defined(FEATURE_WINCE)
        msg_notify.cmd_code = DIAG_QSR_EXT_MSG_TERSE_F;
        msg_notify.msg_store.qsr_v3_msg = msg_store;

        diag_f3_searchlist_ext(msg_ssid,
                           msg_mask,
                           (void*)&msg_notify);
        
        (void) diag_searchlist_search_all(msg_ssid,
                                        &diag_f3_listeners,
                                        (void *)&msg_notify,
                                        diag_f3_listener_notify );
#endif
      }

      if (state != QSR_MSG_EXPAND_STATE_HDR)
      {
        /* This should never happen.  However, if we start over, just kill the
           last message in progress */
        ERR("New message while in progress", 0, 0, 0);

        desc->pkt = (void *)((byte *)desc->last + 1);
        desc->state = DIAG_SEND_STATE_COMPLETE;
        state = QSR_MSG_EXPAND_STATE_HDR;   /* reset state */
      }
    }

    if (msg_store)
    {
      /* check if a valid stream bit is set. Listeners should already be
      handled by this point */
      if (!( stream_id & (DIAG_STREAM_1|DIAG_STREAM_2)))
      {
        /* Mark Complete - Don't send to the communication layer */
        desc->state = DIAG_SEND_STATE_COMPLETE;

        /* Skip this packet - no tool asked for it */
        desc->pkt = (void *)((byte *)desc->last + 1);
        state = QSR_MSG_EXPAND_STATE_HDR;
        send.pkt = NULL;
      }
      else
      {
        /* The packet is expanded in fragments.  'state' marks each stage of
           the process. */
        if ((msg_store) && ((msg_store->qsr_flag & QSR_BITFLAG_IS_HASH) &&
                            (status == DIAGBUF_READY_S && state == QSR_MSG_EXPAND_STATE_HDR)))
        {
          if (send.pkt == NULL && msg_store)
          {
            /* Set the command code since it was stored as MSG_EXT_TERSE_F. */
            msg_store->hdr.cmd_code = DIAG_QSR_EXT_MSG_TERSE_F;

            /* Set up the send descriptor for this fragment. */
            send.pkt = (void *)&msg_store->hdr;
            send.last = (void *)((byte *)send.pkt + sizeof(msg_store->hdr) - 1);

            /*Set the state to 'busy' if the extended header pkt has been sent 
              so that we HDLC encode the data including the extended header pkt*/
            if (diag_msg_ext)
            {
              send.state = DIAG_SEND_STATE_BUSY;
              diag_msg_ext = FALSE;
            }
            else
            {
            send.state = DIAG_SEND_STATE_START;
            }
            send.terminate = FALSE;
          }

          (void)diagbuf_send_pkt(&send, stream_id);

          if (send.state == DIAG_SEND_STATE_COMPLETE)
          {
            state++;
            send.pkt = NULL;
          }
        }

        if ((msg_store) && (!(msg_store->qsr_flag & QSR_BITFLAG_IS_HASH)) &&
            status == DIAGBUF_READY_S && state == QSR_MSG_EXPAND_STATE_HDR)
        {
          /*  
           *  Branch on general behavior between actual hashes and addresses for 
           *  QShrink30
           */
          if (send.pkt == NULL && msg_store)
          {
            /*
                    * if last 2 bits of hash == 00, then treat as address to string
                    * mimic msg_expand_and_send_pkt
                    * build/ms/Qshrink.py replacement mode
                
                    * have "New message while in progress" error still work
                    */
            state++;

            /* send header */
            msg_store->hdr.cmd_code = DIAG_EXT_MSG_F;

            send.pkt = (void *)&msg_store->hdr;
            send.last = (void *)((byte *)send.pkt + sizeof(msg_store->hdr) - 1);

            /*Set the state to 'busy' if the extended header pkt has been sent 
              so that we HDLC encode the data including the extended header pkt*/
            if (diag_msg_ext)
            {
              send.state = DIAG_SEND_STATE_BUSY;
              diag_msg_ext = FALSE;
            }
            else
            {
            send.state = DIAG_SEND_STATE_START;
            }
            send.terminate = FALSE;

            (void)diagbuf_send_pkt(&send, stream_id);

            if (send.state != DIAG_SEND_STATE_COMPLETE)
            {
              send.pkt = NULL;
              desc->pkt = (void *)((byte *)desc->last + 1);
              desc->state = DIAG_SEND_STATE_COMPLETE;
              return;
            }

            /* send description  - qshrink3 */
            ldesc.ss_id = msg_ssid;
            ldesc.ss_mask = msg_mask;
            ldesc.line = msg_line;
            send.pkt = (void *)&ldesc;
            send.last = (void *)((byte *)send.pkt +
                                 sizeof(ldesc) - 1);

            send.state = DIAG_SEND_STATE_BUSY;
            send.terminate = FALSE;

            (void)diagbuf_send_pkt(&send, stream_id);

            if (send.state != DIAG_SEND_STATE_COMPLETE)
            {
              send.pkt = NULL;
              desc->pkt = (void *)((byte *)desc->last + 1);
              desc->state = DIAG_SEND_STATE_COMPLETE;
              return;
            }

            /* send args */
            send.pkt = (void *)msg_store->args;
            send.last = (void *)((byte *)send.pkt +
                                 sizeof(msg_store->args[0]) * msg_store->hdr.num_args - 1);

            send.state = DIAG_SEND_STATE_BUSY;

            (void)diagbuf_send_pkt(&send, stream_id);

            if (send.state != DIAG_SEND_STATE_COMPLETE)
            {
              send.pkt = NULL;
              desc->pkt = (void *)((byte *)desc->last + 1);
              desc->state = DIAG_SEND_STATE_COMPLETE;
              return;
            }

            // send fmt
            // current == msg_hash -> "filename:string"
            // want    == send.pkt -> "string"
            if (msg_store->qsr_flag & QSR_BITFLAG_IS_V3)
            {
              for (str_buf = (char *)msg_store->qsr_const_data_ptr.v3_index->msg;
                    *str_buf != ':'; str_buf++) ;

              filename_len = str_buf - (char *)msg_store->qsr_const_data_ptr.v3_index->msg;
              send.pkt = (void *)(msg_store->qsr_const_data_ptr.v3_index->msg + filename_len + 1);
            }
            else
            {
              for (str_buf = (char *) msg_store->qsr_const_data_ptr.msg_desc->msg_hash;
                    *str_buf != ':'; str_buf++) ;
        
              filename_len = str_buf - (char *)msg_store->qsr_const_data_ptr.msg_desc-> msg_hash;
              send.pkt = (void *) (msg_store->qsr_const_data_ptr.msg_desc->msg_hash + filename_len + 1);
            }


            send.last = (void *)((byte *)send.pkt +
                                 MIN(MSG_MAX_STRLEN, (int)strlen((char *)send.pkt)));

            send.state = DIAG_SEND_STATE_BUSY;

            (void)diagbuf_send_pkt(&send, stream_id);

            if (send.state != DIAG_SEND_STATE_COMPLETE)
            {
              send.pkt = NULL;
              desc->pkt = (void *)((byte *)desc->last + 1);
              desc->state = DIAG_SEND_STATE_COMPLETE;
              return;
            }

            // send fmt_null
            if (*((byte *)send.last) != 0)
            {
              send.last = send.pkt = &desc->terminate;
              desc->terminate = 0;

              send.state = DIAG_SEND_STATE_BUSY;

              (void)diagbuf_send_pkt(&send, stream_id);

              if (send.state != DIAG_SEND_STATE_COMPLETE)
              {
                send.pkt = NULL;
                desc->pkt = (void *)((byte *)desc->last + 1);
                desc->state = DIAG_SEND_STATE_COMPLETE;
                return;
              }
            }

            // send file name
            if (msg_store->qsr_flag & QSR_BITFLAG_IS_V3)
              send.pkt = (void *)(msg_store->qsr_const_data_ptr.v3_index->msg);
            else
              send.pkt = (void *)(msg_store->qsr_const_data_ptr.msg_desc->msg_hash);
            send.last = (void *)((byte *)send.pkt +
                                 MIN(MSG_MAX_STRLEN, filename_len - 1));

            send.state = DIAG_SEND_STATE_BUSY;

            if (*((byte *)send.last) == 0)
            {
              send.terminate = TRUE;
            }
            else
            {
              send.terminate = FALSE;
            }

            (void)diagbuf_send_pkt(&send, stream_id);

            if (send.state != DIAG_SEND_STATE_COMPLETE)
            {
              send.pkt = NULL;
              desc->pkt = (void *)((byte *)desc->last + 1);
              desc->state = DIAG_SEND_STATE_COMPLETE;
              return;
            }

            // send file name null
            if (send.terminate != TRUE)
            {
              send.last = send.pkt = &desc->terminate;
              desc->terminate = 0;

              send.state = DIAG_SEND_STATE_BUSY;
              send.terminate = TRUE;

              (void)diagbuf_send_pkt(&send, stream_id);

              if (send.state != DIAG_SEND_STATE_COMPLETE)
              {
                send.pkt = NULL;
                desc->pkt = (void *)((byte *)desc->last + 1);
                desc->state = DIAG_SEND_STATE_COMPLETE;
                return;
              }
            }
          }

          // send complete
          state = QSR_MSG_EXPAND_STATE_HDR;
          desc->pkt = (void *)((byte *)desc->last + 1);
          desc->state = DIAG_SEND_STATE_COMPLETE;
          send.pkt = NULL;

          state_sav = state;

          return;
        }

        if (status == DIAGBUF_READY_S && state == QSR_MSG_EXPAND_STATE_DESC)
        {
          if (send.pkt == NULL && msg_store)
          {
            /* Set up the send descriptor for this fragment. */
            /* qshrink3 */
            ldesc.ss_id = msg_ssid;
            ldesc.ss_mask = msg_mask;
            ldesc.line = msg_line;
            send.pkt = (void *)&ldesc;
            send.last = (void *)((byte *)send.pkt +
                                 sizeof(ldesc) - 1);

            send.state = DIAG_SEND_STATE_BUSY;
            send.terminate = FALSE;
          }

          (void)diagbuf_send_pkt(&send, stream_id);

          if (send.state == DIAG_SEND_STATE_COMPLETE)
          {
            state++;
            send.pkt = NULL;
          }
        }

        if (status == DIAGBUF_READY_S && state == QSR_MSG_EXPAND_STATE_HASH_VALUE)
        {
          if (send.pkt == NULL && msg_store)
          {
            /* Set up the send descriptor for this fragment. */
            if (msg_store->qsr_flag & QSR_BITFLAG_IS_V3)
            {
              send.pkt = (void *)&msg_store->qsr_const_data_ptr.v3_index->msg;
              send.last = (void *)((byte *)send.pkt +
                                   sizeof(msg_store->qsr_const_data_ptr.v3_index->msg) - 1);
            }
            else
            {
              send.pkt = (void *)&msg_store->qsr_const_data_ptr.msg_desc->msg_hash;
              send.last = (void *)((byte *)send.pkt +
                                   sizeof(msg_store->qsr_const_data_ptr.msg_desc->msg_hash) - 1);
            }

            send.state = DIAG_SEND_STATE_BUSY;
            send.terminate = FALSE;
          }

          (void)diagbuf_send_pkt(&send, stream_id);

          if (send.state == DIAG_SEND_STATE_COMPLETE)
          {
            state++;
            send.pkt = NULL;
          }
        }

        if (status == DIAGBUF_READY_S && state == QSR_MSG_EXPAND_STATE_ARGS)
        {
          if (send.pkt == NULL && msg_store)
          {
            /* Set up the send descriptor for this fragment. */
            send.pkt = (void *)msg_store->args;
            send.last = (void *)((byte *)send.pkt +
                                 sizeof(msg_store->args[0]) * msg_store->hdr.num_args - 1);

            send.state = DIAG_SEND_STATE_BUSY;
            send.terminate = TRUE;
          }

          (void)diagbuf_send_pkt(&send, stream_id);

          if (send.state == DIAG_SEND_STATE_COMPLETE)
          {
            state = QSR_MSG_EXPAND_STATE_HDR;
            desc->pkt = (void *)((byte *)desc->last + 1);
            desc->state = DIAG_SEND_STATE_COMPLETE;
            send.pkt = NULL;
          }
        }
      }
    }
  }

  /* Commit local copies of persistent data back to static variables. */
  state_sav = state;

  return;
} /* msg_qsr_expand_and_send_pkt */

/*===========================================================================

FUNCTION MSG_SPRINTF_EXPAND_AND_SEND_PKT

DESCRIPTION
  This routine notifies the listeners (registered) whenever a sprintf msg is
  generated and also drains out the msg.

DEPENDENCIES
  msg_init() must have been called previously.

RETURN VALUE

===========================================================================*/

static void msg_sprintf_expand_and_send_pkt(diag_send_desc_type *desc, byte stream_id)
{

  msg_ext_type *ext_sprintf_msg = NULL;
  diag_msg_type msg_notify;
  
  if (desc)
  {
    ext_sprintf_msg = (msg_ext_type*)desc->pkt;

    if(ext_sprintf_msg)
    {
      #if !defined(FEATURE_WINCE)

        msg_notify.cmd_code = DIAG_EXT_MSG_F;
        msg_notify.msg_store.ext_sprintf_msg = ext_sprintf_msg;

        diag_f3_searchlist_ext(ext_sprintf_msg->desc.ss_id,
                                ext_sprintf_msg->desc.ss_mask,
                                &msg_notify);

       (void) diag_searchlist_search_all(ext_sprintf_msg->desc.ss_id,
                                      &diag_f3_listeners,
                                      (void*)&msg_notify,
                                      diag_f3_listener_notify);

      #endif    

       if ((stream_id & (DIAG_STREAM_1|DIAG_STREAM_2)))
       {
      /*Set the state to 'busy' if the extended header pkt has been sent 
        so that we HDLC encode the data including the extended header pkt*/    
       if (diag_msg_sprintf_ext)
       {
         desc->state = DIAG_SEND_STATE_BUSY;
         diag_msg_sprintf_ext = FALSE;
       }
      (void)diagbuf_send_pkt(desc, stream_id);
       }
       else
       {
         /* Mark Complete - Don't send to the communication layer */
         desc->pkt = (void *)((byte *)desc->last + 1);
         desc->state = DIAG_SEND_STATE_COMPLETE;
       }
    }
  }

  return;
}


/*===========================================================================

FUNCTION DIAG_SWITCH_MSG_PRESET_MASK

DESCRIPTION
  Whenever 'diag_cur_preset_id' is updated via command/response or control 
  packet to switch the current Stream 1 preset mask id, we need to call this
  function to actually switch the current preset msg mask in use.
  
PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
void diag_switch_msg_preset_mask()
{
  uint8 index;
  uint32 count;
  
  for( index = 0; index < MSG_MASK_TBL_CNT; index++ )
  {
    count = msg_mask_tbl[index].ssid_last - msg_mask_tbl[index].ssid_first + 1;
    msg_mask_tbl[index].rt_mask_array[DIAG_STREAM_1-1] = (uint32 *)&msg_mask_tbl[index].rt_base_ptr[DIAG_STREAM_1-1][(diag_cur_preset_id-1)*count];
    
#if defined(DIAG_MP_MASTER)
    diag_msg_mask_mstr_tbl[index].rt_mask_array[DIAG_STREAM_1-1] = &diag_mstr_rt_preset_mask[diag_cur_preset_id-1][index][0];
    diag_msg_mask_mstr_tbl[index].status[DIAG_STREAM_1-1] = &diag_mstr_rt_preset_status[diag_cur_preset_id-1][index];
#endif
  }
       
  diag_qsr_rt_mask = msg_stream_ssid_tbl_mask[DIAG_STREAM_1-1][diag_cur_preset_id] |
                     msg_stream_ssid_tbl_mask[DIAG_STREAM_2-1][DIAG_DCI_MASK_1-1];

  (void)diag_client_maskchange(DIAG_MSGSVC_MASK_CHANGE);
  (void)msg_sleep_vote();
  
} /* diag_switch_msg_preset_mask */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_SEND_MSG_MASK

DESCRIPTION
  Composes a CTRL msg to send an F3 mask update to specified Slave Diag.

PARAMETERS
  port_type - Port type (DIAGCOMM_PORT_SIO/DIAGCOMM_PORT_SMD)
  port_num - Port num (DIAGCOMM_PORT_1, DIAGCOMM_PORT_2, ...)
  stream_id - F3 stream to send
  preset_id
  send_now - If True, send CTRL msg immediately. 
             If False, aggregate until DSM is full before sending.
  
RETURN VALUE
  None
  
===========================================================================*/
/* On MP Master only */
#if defined(DIAG_MP_MASTER)
void diagcomm_ctrl_send_msg_mask( diagcomm_port_type port_type, 
                                  diagcomm_enum_port_type port_num, 
                                  uint8 stream_id,
                                  uint8 preset_id,
                                  boolean send_now )
{
  diag_ctrl_msg_type ctrl_msg;
  boolean is_sent = FALSE;
  boolean update_all = FALSE;
  uint8 i = 0;
  diagcomm_io_conn_type * conn = NULL;
  
  conn = &diagcomm_io_ctrl_conn[port_type][port_num];
  
  if( (stream_id > DIAG_MAX_STREAM_ID) || (stream_id < DIAG_MIN_STREAM_ID) ||
      (preset_id > DIAG_MAX_PRESET_ID) || (preset_id < DIAG_MIN_PRESET_ID) )
    return;
  
  #if defined(DIAG_STREAM_2_MASK_CHECK)
  /* Stream 2 mask for msgs currently is not in use, so skip. */
  if( stream_id != DIAG_STREAM_1 )
    return;
  #endif
  
  /* Check if port has been opened */
  if( !conn->connected || !conn->channel_connected )
    return;
  
  MSG_4(MSG_SSID_DIAG, MSG_LEGACY_LOW, "send_msg_mask port_num=%d stream=%d preset_id=%d send=%d", 
        port_num, stream_id, preset_id, send_now);
        
  if( diagcomm_ctrl_mask_status(DIAGCOMM_PORT_SMD, port_num, DIAG_UPDATE_MASK_F3, stream_id, preset_id) )
  {
    update_all = TRUE;
    diagcomm_ctrl_clear_mask_status( port_type, port_num, DIAG_UPDATE_MASK_F3, stream_id, preset_id );
    diagcomm_ctrl_clear_mask_status( port_type, port_num, DIAG_UPDATE_MASK_PARTIAL_F3, stream_id, preset_id );
  }
  else if( diagcomm_ctrl_mask_status(DIAGCOMM_PORT_SMD, port_num, DIAG_UPDATE_MASK_PARTIAL_F3, stream_id, preset_id) )
  {
    update_all = FALSE;
    diagcomm_ctrl_clear_mask_status( port_type, port_num, DIAG_UPDATE_MASK_PARTIAL_F3, stream_id, preset_id );
  }
  else
    return;
  
  for( i=0; i < MSG_MASK_TBL_CNT; i++ )
  {
    if( update_all || diagcomm_ctrl_msg_mask_status(port_type, port_num, stream_id, preset_id, i) )
    {
      /* Compose DIAG_CTRL_MSG_F3_MASK_V3 packet */
      ctrl_msg.cmd_type = DIAG_CTRL_MSG_F3_MASK_V3;
      ctrl_msg.data.ctrl_msg_f3_mask_v3.stream_id = stream_id;
      ctrl_msg.data.ctrl_msg_f3_mask_v3.preset_id = preset_id;
      ctrl_msg.data.ctrl_msg_f3_mask_v3.msg_mode = (uint8)msg_mode;
      ctrl_msg.data.ctrl_msg_f3_mask_v3.ssid_first = diag_msg_mask_mstr_tbl[i].ssid_first;
      ctrl_msg.data.ctrl_msg_f3_mask_v3.ssid_last = diag_msg_mask_mstr_tbl[i].ssid_last;
      
      if( stream_id == DIAG_STREAM_1 )
        ctrl_msg.data.ctrl_msg_f3_mask_v3.status = diag_mstr_rt_preset_status[preset_id-1][i];
      #if defined(DIAG_STREAM_2_MASK_CHECK)
      else if( stream_id == DIAG_STREAM_2 )
        ctrl_msg.data.ctrl_msg_f3_mask_v3.status = diag_mstr_rt_dci_status[DIAG_DCI_MASK_1-1][i];
      #endif
      
      if( ctrl_msg.data.ctrl_msg_f3_mask_v3.status == DIAG_CTRL_MASK_ALL_DISABLED )
      {
        ctrl_msg.data.ctrl_msg_f3_mask_v3.msg_mask_size = 0;
        ctrl_msg.data_len = sizeof(diag_ctrl_msg_f3_mask_v3_type);
        
        (void)diagcomm_ctrl_send_mult_pkt( (PACK(void *))&ctrl_msg, 
                                           DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_f3_mask_v3_type), 
                                           NULL, 0,
                                           port_type, port_num, send_now, &is_sent );
      }
      else if( ctrl_msg.data.ctrl_msg_f3_mask_v3.status == DIAG_CTRL_MASK_ALL_ENABLED )
      {
        ctrl_msg.data.ctrl_msg_f3_mask_v3.msg_mask_size = 1;
        ctrl_msg.data_len = sizeof(diag_ctrl_msg_f3_mask_v3_type) + sizeof(uint32);
        
        if( stream_id == DIAG_STREAM_1 )
        {
          (void)diagcomm_ctrl_send_mult_pkt( (PACK(void *))&ctrl_msg, 
                                         DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_f3_mask_v3_type), 
                                         (PACK(void *))&diag_mstr_rt_preset_mask[preset_id-1][i][0],
                                         ctrl_msg.data.ctrl_msg_f3_mask_v3.msg_mask_size * sizeof(uint32),
                                         port_type, port_num, send_now, &is_sent );
        }
        #if defined(DIAG_STREAM_2_MASK_CHECK)
        else if( stream_id == DIAG_STREAM_2 )
        {
          (void)diagcomm_ctrl_send_mult_pkt( (PACK(void *))&ctrl_msg, 
                                         DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_f3_mask_v3_type), 
                                         (PACK(void *))&diag_mstr_rt_dci_mask[DIAG_DCI_MASK_1-1][i][0],
                                         ctrl_msg.data.ctrl_msg_f3_mask_v3.msg_mask_size * sizeof(uint32),
                                         port_type, port_num, send_now, &is_sent );
        }
        #endif
      }
      else if( ctrl_msg.data.ctrl_msg_f3_mask_v3.status == DIAG_CTRL_MASK_VALID )
      {
        ctrl_msg.data.ctrl_msg_f3_mask_v3.msg_mask_size = diag_msg_mask_mstr_tbl[i].ssid_last - 
                                                       diag_msg_mask_mstr_tbl[i].ssid_first + 1;
        ctrl_msg.data_len = sizeof(diag_ctrl_msg_f3_mask_v3_type) + 
                            (ctrl_msg.data.ctrl_msg_f3_mask_v3.msg_mask_size * sizeof(uint32));
        
        if( stream_id == DIAG_STREAM_1 )
        {
          (void)diagcomm_ctrl_send_mult_pkt( (PACK(void *))&ctrl_msg, 
                                         DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_f3_mask_v3_type), 
                                         (PACK(void *))&diag_mstr_rt_preset_mask[preset_id-1][i][0],
                                         ctrl_msg.data.ctrl_msg_f3_mask_v3.msg_mask_size * sizeof(uint32),
                                         port_type, port_num, send_now, &is_sent );
        }
        #if defined(DIAG_STREAM_2_MASK_CHECK)
        else if( stream_id == DIAG_STREAM_2 )
        {
          (void)diagcomm_ctrl_send_mult_pkt( (PACK(void *))&ctrl_msg, 
                                         DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_f3_mask_v3_type), 
                                         (PACK(void *))&diag_mstr_rt_dci_mask[DIAG_DCI_MASK_1-1][i][0],
                                         ctrl_msg.data.ctrl_msg_f3_mask_v3.msg_mask_size * sizeof(uint32),
                                         port_type, port_num, send_now, &is_sent );
        }
        #endif
      }
    }
  }
                                      
  diagcomm_ctrl_clear_msg_mask_status( port_type, port_num, stream_id, preset_id );
  
  /* No more msg mask tables to fwd; Send the current DSM item immediately and then quit */
  diagcomm_ctrl_send_mult_pkt( NULL, 0, NULL, 0, port_type, port_num, send_now, &is_sent );
                                   
} /* diagcomm_ctrl_send_msg_mask */
#endif /* DIAG_MP_MASTER */


/*===========================================================================

FUNCTION DIAG_CTRL_UPDATE_MSG_MASK

DESCRIPTION
  Given a new F3 mask, updates the local copy of the F3 mask with the
  new mask.

PARAMETERS
  ctrl_msg - Ptr to CTRL msg
  
RETURN VALUE
  None
  
===========================================================================*/
/* On MP Slave only */
#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
void diag_ctrl_update_msg_mask( diag_ctrl_msg_type * ctrl_msg )
{
  uint8 * mask_ptr = NULL;
  uint32 * rt_mask_ptr = NULL;
  uint8 index, stream_id, status = 0;
  uint16 ssid_first, ssid_last, ssid_local = 0;
  uint32 j, count, mask, msg_mask_size = 0;
  boolean found = FALSE;
  
  if( (ctrl_msg == NULL) || (ctrl_msg->cmd_type != DIAG_CTRL_MSG_F3_MASK_V2) )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_msg_mask Sanity check failed");
    return;
  }
  
  status = ctrl_msg->data.ctrl_msg_f3_mask.status;
  msg_mode = ctrl_msg->data.ctrl_msg_f3_mask.msg_mode;
  stream_id = ctrl_msg->data.ctrl_msg_f3_mask.stream_id;
  ssid_first = ctrl_msg->data.ctrl_msg_f3_mask.ssid_first;
  ssid_last = ctrl_msg->data.ctrl_msg_f3_mask.ssid_last;
  msg_mask_size = ctrl_msg->data.ctrl_msg_f3_mask.msg_mask_size;
  
  if( (stream_id > DIAG_MAX_STREAM_ID) || (stream_id < DIAG_MIN_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_msg_mask: Invalid stream_id %d", stream_id);
    return;
  }
 
  if( ctrl_msg->data_len != (sizeof(diag_ctrl_msg_f3_mask_type) + (msg_mask_size * sizeof(uint32))) )
  {
    MSG_2( MSG_SSID_DIAG, MSG_LEGACY_ERROR, 
             "diag_ctrl_update_msg_mask: Invalid lengths %d != %d", 
             ctrl_msg->data_len, sizeof(diag_ctrl_msg_f3_mask_type) + (msg_mask_size * sizeof(uint32)) );
    return;
  }
    
  if( status == DIAG_CTRL_MASK_ALL_DISABLED )
  {
    for( index=0; !found && (index < MSG_MASK_TBL_CNT); index++ )
    {
      if( ssid_first == msg_mask_tbl[index].ssid_first )
      {
        found = TRUE;
        count = (uint32)(msg_mask_tbl[index].ssid_last - msg_mask_tbl[index].ssid_first + 1);
        for( j=0; j < count; j++ )
        {
          msg_mask_tbl[index].rt_mask_array[stream_id-1][j] = 0;
          /* qshrink3 update msg_v3_ss_table */
          ssid_local = msg_mask_tbl[index].ssid_first +j;
          msg_update_ss_table(ssid_local, 0, stream_id, diag_cur_preset_id);
        }
      }
    }
  }
  else if( status == DIAG_CTRL_MASK_ALL_ENABLED )
  {
    if( msg_mask_size > 0 )
    {
      mask_ptr = ((uint8 *)ctrl_msg) + DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_f3_mask_type);
      memscpy( (void *)&mask,sizeof(uint32), mask_ptr, sizeof(uint32) );
    }
    else
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_msg_mask: No mask to update");
      return;
    }
    
    for( index=0; !found && (index < MSG_MASK_TBL_CNT); index++ )
    {
      if( ssid_first == msg_mask_tbl[index].ssid_first )
      {
        found = TRUE;
        count = (uint32)(msg_mask_tbl[index].ssid_last - msg_mask_tbl[index].ssid_first + 1);
        for( j=0; j < count; j++ )
        {
          /* Update all with same mask */
          msg_mask_tbl[index].rt_mask_array[stream_id-1][j] = mask;
          /* qshrink3 update msg_v3_ss_table */
          ssid_local = msg_mask_tbl[index].ssid_first +j;
          msg_update_ss_table(ssid_local, mask, stream_id, diag_cur_preset_id);
        }
      }
    }
  }
  else if( status == DIAG_CTRL_MASK_VALID )
  {
    for( index=0; !found && (index < MSG_MASK_TBL_CNT); index++ )
    {
      if( ssid_first == msg_mask_tbl[index].ssid_first )
      {
        found = TRUE;
        mask_ptr = ((uint8 *)ctrl_msg) + DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_f3_mask_type);
        rt_mask_ptr = (uint32 *)&(msg_mask_tbl[index].rt_mask_array[stream_id-1][0]);
        count = msg_mask_tbl[index].ssid_last - msg_mask_tbl[index].ssid_first + 1;
        msg_mask_size = MIN(msg_mask_size, count);
       if(  (memscpy( (void *)rt_mask_ptr, count * sizeof(uint32),mask_ptr, msg_mask_size * sizeof(uint32))) < (msg_mask_size * sizeof(uint32)))
       {
         MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_msg_mask: Not enough space for copy");
         return;
       }
        /* qshrink3 update msg_v3_ss_table */
        for( j=0; j < msg_mask_size; j++ )
        {
          ssid_local = msg_mask_tbl[index].ssid_first +j;
          msg_update_ss_table(ssid_local, rt_mask_ptr[j], stream_id, diag_cur_preset_id);
          mask_ptr+=4;
        }
      }
    }
  }
  
  if( !found )
  {
    MSG_3( MSG_SSID_DIAG, MSG_LEGACY_ERROR, 
           "update_msg_mask not found: stream_id=%d, ssid_first=%d, ssid_last=%d", 
           stream_id, ssid_first, ssid_last );
    return;
  }
  
  /* Set the masks of all clients registered with DCM.*/
  (void)diag_client_maskchange(DIAG_MSGSVC_MASK_CHANGE);
  (void)msg_sleep_vote();
    
} /* diag_ctrl_update_msg_mask */
#endif /* DIAG_MP && !DIAG_MP_MASTER */


/*===========================================================================

FUNCTION DIAG_CTRL_UPDATE_MSG_PRESET_MASK

DESCRIPTION
  Given a new F3 mask, updates the local copy of the Stream 1 preset F3 mask 
  with the new mask.

PARAMETERS
  ctrl_msg - Ptr to CTRL msg
  
RETURN VALUE
  None
  
===========================================================================*/
/* On MP Slave only */
#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
void diag_ctrl_update_msg_preset_mask( diag_ctrl_msg_type * ctrl_msg )
{
  uint8 * mask_ptr = NULL;
  uint32 * rt_mask_ptr = NULL;
  uint8 index, stream_id, preset_id, status = 0;
  uint16 ssid_first, ssid_last, ssid_local = 0;
  uint32 j, count, mask, msg_mask_size = 0;
  boolean found = FALSE;
  
  if( (ctrl_msg == NULL) || (ctrl_msg->cmd_type != DIAG_CTRL_MSG_F3_MASK_V3) )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_msg_preset_mask Sanity check failed");
    return;
  }
  
  status = ctrl_msg->data.ctrl_msg_f3_mask_v3.status;
  msg_mode = ctrl_msg->data.ctrl_msg_f3_mask_v3.msg_mode;
  stream_id = ctrl_msg->data.ctrl_msg_f3_mask_v3.stream_id;
  preset_id = ctrl_msg->data.ctrl_msg_f3_mask_v3.preset_id;
  ssid_first = ctrl_msg->data.ctrl_msg_f3_mask_v3.ssid_first;
  ssid_last = ctrl_msg->data.ctrl_msg_f3_mask_v3.ssid_last;
  msg_mask_size = ctrl_msg->data.ctrl_msg_f3_mask_v3.msg_mask_size;
  
  if( (stream_id > DIAG_MAX_STREAM_ID) || (preset_id > DIAG_MAX_PRESET_ID) || (stream_id == 0) \
      || (preset_id == 0))
  {
    MSG_2( MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_msg_preset_mask: Invalid stream_id %d or preset_id %d", 
           stream_id, preset_id );
    return;
  }
   
  if( stream_id == DIAG_STREAM_2 )
  {
    preset_id = DIAG_DCI_MASK_1;
  }
  
  if( ctrl_msg->data_len != (sizeof(diag_ctrl_msg_f3_mask_v3_type) + (msg_mask_size * sizeof(uint32))) )
  {
    MSG_2( MSG_SSID_DIAG, MSG_LEGACY_ERROR, 
             "diag_ctrl_update_msg_preset_mask: Invalid lengths %d != %d", 
             ctrl_msg->data_len, sizeof(diag_ctrl_msg_f3_mask_v3_type) + (msg_mask_size * sizeof(uint32)) );
    return;
  }
    
  if( status == DIAG_CTRL_MASK_ALL_DISABLED )
  {
    for( index=0; !found && (index < MSG_MASK_TBL_CNT); index++ )
    {
      if( ssid_first == msg_mask_tbl[index].ssid_first )
      {
        found = TRUE;
        count = (uint32)(msg_mask_tbl[index].ssid_last - msg_mask_tbl[index].ssid_first + 1);
        for( j=0; j < count; j++ )
        {
          msg_mask_tbl[index].rt_base_ptr[stream_id-1][(preset_id-1)*count + j] = 0;
          /* qshrink3 update msg_v3_ss_table */
          ssid_local = msg_mask_tbl[index].ssid_first +j;
          msg_update_ss_table(ssid_local, 0, stream_id, preset_id);
        }
      }
    }
  }
  else if( status == DIAG_CTRL_MASK_ALL_ENABLED )
  {
    if( msg_mask_size > 0 )
    {
      mask_ptr = ((uint8 *)ctrl_msg) + DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_f3_mask_v3_type);
      memscpy( (void *)&mask, sizeof(uint32), mask_ptr, sizeof(uint32) );
    }
    else
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_msg_preset_mask: No mask to update");
      return;
    }
    
    for( index=0; !found && (index < MSG_MASK_TBL_CNT); index++ )
    {
      if( ssid_first == msg_mask_tbl[index].ssid_first )
      {
        found = TRUE;
        count = (uint32)(msg_mask_tbl[index].ssid_last - msg_mask_tbl[index].ssid_first + 1);
        for( j=0; j < count; j++ )
        {
          /* Update all with same mask */
          msg_mask_tbl[index].rt_base_ptr[stream_id-1][(preset_id-1)*count + j] = mask;
          /* qshrink3 update msg_v3_ss_table */
          ssid_local = msg_mask_tbl[index].ssid_first +j;
          msg_update_ss_table(ssid_local, mask, stream_id, preset_id);
        }
      }
    }
  }
  else if( status == DIAG_CTRL_MASK_VALID )
  {
    for( index=0; !found && (index < MSG_MASK_TBL_CNT); index++ )
    {
      if( ssid_first == msg_mask_tbl[index].ssid_first )
      {
        found = TRUE;
        mask_ptr = ((uint8 *)ctrl_msg) + DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_f3_mask_v3_type);
        count = msg_mask_tbl[index].ssid_last - msg_mask_tbl[index].ssid_first + 1;
        rt_mask_ptr = (uint32 *)&(msg_mask_tbl[index].rt_base_ptr[stream_id-1][(preset_id-1)*count]);
        msg_mask_size = MIN(msg_mask_size, count);
      if(  (memscpy( (void *)rt_mask_ptr, count * sizeof(uint32),mask_ptr, msg_mask_size * sizeof(uint32))) < (msg_mask_size * sizeof(uint32)))
      {
         MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_msg_preset_mask: Not enough space for copy");
         return;
      }
        /* qshrink3 update msg_v3_ss_table */
        for( j=0; j < msg_mask_size; j++ )
        {
          ssid_local = msg_mask_tbl[index].ssid_first +j;
          msg_update_ss_table(ssid_local, rt_mask_ptr[j], stream_id, diag_cur_preset_id);
          mask_ptr+=4;
        }
      }
    }
  }
  
  if( !found )
  {
    MSG_4( MSG_SSID_DIAG, MSG_LEGACY_ERROR, 
           "diag_ctrl_update_msg_preset_mask not found: stream_id=%d preset_id=%d, ssid_first=%d, ssid_last=%d", 
           stream_id, preset_id, ssid_first, ssid_last );
    return;
  }
  
  /* Set the masks of all clients registered with DCM.*/
  (void)diag_client_maskchange(DIAG_MSGSVC_MASK_CHANGE);
  (void)msg_sleep_vote();
    
} /* diag_ctrl_update_msg_preset_mask */

/*===========================================================================

FUNCTION DIAG_CTRL_SEND_MSG_SSID_RANGE_CTRL_PKT

DESCRIPTION
  Sends build time msg ssid range information to Master Diag for build mask
  centralization support.
  
PARAMETERS
  None
 
RETURN VALUE
  None
  
===========================================================================*/
void diag_ctrl_send_msg_ssid_range_ctrl_pkt(void)
{
     diag_ctrl_msg_type ctrl_msg;
     uint8 i = 0;
     boolean is_sent = FALSE;
     uint32 bytes_written = 0;

     if(bt_msg_ssid_range_fwd_status == DIAG_CTRL_INFO_FWD_STATUS_NOT_SENT)
     {
          /* Compose DIAG_CTRL_BT_MSG_SSID_RANGE packet */
          ctrl_msg.cmd_type = DIAG_CTRL_BT_MSG_RANGE;
          ctrl_msg.data_len = sizeof(diag_ctrl_bt_msg_ssid_range_type);
          ctrl_msg.data.ctrl_bt_msg_ssid_range.version = DIAG_CTRL_MSG_BT_MASK_VER;
          ctrl_msg.data.ctrl_bt_msg_ssid_range.num_ssid_ranges = MSG_MASK_TBL_CNT;

          for (i = 0; i < MSG_MASK_TBL_CNT; i++) 
          {
                ctrl_msg.data.ctrl_bt_msg_ssid_range.ssid_ranges[i].ssid_first = msg_mask_tbl[i].ssid_first;
                ctrl_msg.data.ctrl_bt_msg_ssid_range.ssid_ranges[i].ssid_last = msg_mask_tbl[i].ssid_last;
          }

          bytes_written = diagcomm_ctrl_send_mult_pkt( (void *)&ctrl_msg, DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_bt_msg_ssid_range_type), 
                                                          NULL, 0, DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1, TRUE, &is_sent );

          if(bytes_written == 0)
                return;
          else if(is_sent == TRUE)
                bt_msg_ssid_range_fwd_status = DIAG_CTRL_INFO_FWD_STATUS_SENT;
          else /* bytes_written > 0 && is_sent == FALSE */
                bt_msg_ssid_range_fwd_status = DIAG_CTRL_INFO_FWD_STATUS_QUEUED;
     }
}

/*===========================================================================

FUNCTION DIAG_CTRL_SEND_MSG_BT_MASK_CTRL_PKT

DESCRIPTION
  Sends F3 build time mask for all SSID's to Master Diag for build mask
  centralization support.
  
PARAMETERS
  None
 
RETURN VALUE
  None
  
===========================================================================*/
void diag_ctrl_send_msg_bt_mask_ctrl_pkt(void)
{

     diag_ctrl_msg_type ctrl_msg;
     uint8 i = 0;
     boolean is_sent = FALSE;
     const msg_mask_tbl_type *tbl_ptr = NULL;
     uint32 userpkt_len = 0, bytes_written = 0;

     for (i = 0; i < MSG_MASK_TBL_CNT; i++) 
     {
          tbl_ptr = &msg_mask_tbl[i];
          if((tbl_ptr != NULL) && (bt_msg_mask_fwd_status[i] == DIAG_CTRL_INFO_FWD_STATUS_NOT_SENT))
          {
                /* Compose DIAG_CTRL_BT_MSG_MASK packet */
                userpkt_len = ((msg_mask_tbl[i].ssid_last - msg_mask_tbl[i].ssid_first + 1) * sizeof(uint32));

                ctrl_msg.cmd_type = DIAG_CTRL_BT_MSG_MASK;
                ctrl_msg.data_len = sizeof(diag_ctrl_bt_msg_mask_type) + userpkt_len;
                ctrl_msg.data.ctrl_bt_msg_mask.version = DIAG_CTRL_MSG_BT_MASK_VER;
                ctrl_msg.data.ctrl_bt_msg_mask.range_cnt = 1;
                ctrl_msg.data.ctrl_bt_msg_mask.ssid_range.ssid_first = msg_mask_tbl[i].ssid_first;
                ctrl_msg.data.ctrl_bt_msg_mask.ssid_range.ssid_last = msg_mask_tbl[i].ssid_last;

                bytes_written = diagcomm_ctrl_send_mult_pkt((void *)&ctrl_msg, DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_bt_msg_mask_type), 
                                                                (const void *)tbl_ptr->bt_mask_array, userpkt_len, DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1, FALSE, &is_sent );

                if(bytes_written == 0)
                     return;
                else if(is_sent == TRUE) /* bytes_written > 0 */
                     bt_msg_mask_fwd_status[i] = DIAG_CTRL_INFO_FWD_STATUS_SENT;
                else /* bytes_written > 0 && is_sent == FALSE */
                     bt_msg_mask_fwd_status[i] = DIAG_CTRL_INFO_FWD_STATUS_QUEUED;

          }
     }

     /* No more msg mask tables to fwd; Send the current DSM item immediately and then quit */
     (void)diagcomm_ctrl_send_mult_pkt( NULL, 0, NULL, 0, DIAGCOMM_PORT_SIO, 
                                            DIAGCOMM_PORT_1, TRUE, &is_sent );
     if( is_sent )
     {
          /* Updates the status */
          for(i = 0; i < MSG_MASK_TBL_CNT; i++)
          {
                bt_msg_mask_fwd_status[i] = DIAG_CTRL_INFO_FWD_STATUS_SENT;
          }
     }

}
#endif /* DIAG_MP && !DIAG_MP_MASTER */

/*===========================================================================
FUNCTION MSG_LISTENERS_MASKS_ENABLED
DESCRIPTION
  This function will check if there is any clients registered with the
  specified ssid and ss_mask.
  type - single or extended listener and searches the associated listeners mask.

DEPENDENCIES

RETURN VALUE
  TRUE if a listener mask is set for the specified ssid and ss_mask.

SIDE EFFECTS
  None.
===========================================================================*/
boolean msg_listeners_masks_enabled(uint32 ssid, uint32 ss_mask, f3_listener_type type )
{
  int i = 0;
  boolean found = FALSE;
  uint32 mask = 0;
  for(i = 0 ; !found && i < LISTENER_MSG_MASK_TBL_CNT ; i++)
  {
    if( (ssid >= listener_msg_mask_tbl[i].ssid_first) && (ssid <= listener_msg_mask_tbl[i].ssid_last))
    {
      found = TRUE;
      if(type == F3_LISTENER)
      {
        mask = listener_msg_mask_tbl[i].f3_listener_mask_array[ssid - listener_msg_mask_tbl[i].ssid_first];
      }
      else if(type == F3_LISTENER_EXT)
      {
        /* Check for extended listeners */
        mask = listener_msg_mask_tbl[i].f3_listener_ext_mask_array[ssid - listener_msg_mask_tbl[i].ssid_first];
      }
      if( mask & ss_mask)
        return TRUE;
    }
  }
  return FALSE;
}
/*===========================================================================
FUNCTION MSG_SET_ALL_LISTENER_MASKS
DESCRIPTION
  This function will set all the listener masks to the
  specified mask value.
DEPENDENCIES

RETURN VALUE
  None
SIDE EFFECTS
  None.
===========================================================================*/
void msg_set_all_listener_masks(uint32 mask)
{
  int i = 0;
  int count = 0;
  int j = 0;
  for( i = 0 ; i < LISTENER_MSG_MASK_TBL_CNT ; i++)
  {
    count = listener_msg_mask_tbl[i].ssid_last - listener_msg_mask_tbl[i].ssid_first + 1;
    for( j = 0 ; j < count ; j++)
    {
      listener_msg_mask_tbl[i].f3_listener_mask_array[j] = 0;
      listener_msg_mask_tbl[i].f3_listener_ext_mask_array[j] = 0;
    }
  }
}
