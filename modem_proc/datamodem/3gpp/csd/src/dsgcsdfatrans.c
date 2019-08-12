/*===========================================================================

                Transparent Fax Adaptation

Description:
  This source file contains the implementation of GSM Fax Adaptation for
  the transparent mode of the Bearer Services Quality of Service attribute.
  It implements the following specification:

  ETSI GSM 03.45 Version 7.0.1 Release 1998
  Technical realization of facsimile group 3 transparent

Externalized Functions:
  dsgcsdfatrans_call_init()         - Perform inits on call setup.
  dsgcsdfatrans_process_uplink()    - Process uplink fax data.
  dsgcsdfatrans_process_downlink()  - Process downlink fax data.
  dsgcsdfatrans_return_to_idle()    - Return Fax Adaptor state to idle.
  dsgcsdfatrans_process_bcs_frame() - Process fax HDLC BCS frame.
  dsgcsdfatrans_timer_expired()     - Handle expiration of timed event.
  dsgcsdfa_tx_ready()               - Indicates if FA TX state is ok to xmit.
  dsgcsdfa_is_idle()                - Indicates if FA is in IDLE state or not.

Initialization and Sequencing Requirements:
  dsgcsdfatrans_call_init() must be called at the beginning of every fax call.

 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Copyright (c) 2002 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved
                     Qualcomm Proprietary

  This software is controlled by the United States Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsgcsdfatrans.c#1 $

when      who   what, where, why
--------  ---   ----------------------------------------------------------
07/15/11  RP    QShrink2 changes: MSG_SPRINTF is used instead of MSG buffers.
12/12/08  SA    Fixing OFF_TARGET lint errors.
07/11/08  SA    Replaced __inline with INLINE macro for all targets.
03/13/08  SA    Fixing Klocwork Critical issues.
11/15/07  AR    Use safety macros for watermark operations.
03/28/07  AR    More Lint error corrections.
02/26/07  AR    Replace banned APIs with safer routines.
02/26/07  DA    Fixed high lint errors
02/09/07  DA    Fixed critical lint errors
10/04/06  RLP   Changed fa_trans_downlink__bcs_tra to terminate its loop
                  if it fails to synchronize after a specified number
                  of iterations.
08/16/06  AR    Changed SYNC_FRAME_BIT_ERRORS_TOLERATED to 6 per GSM spec.
02/09/06  AR    Lint corrections.
10/26/05  AR    Add support for multiple call instances.
10/10/05  AR    Code review issue correction.
07/12/05  AR    Replace REX calls to clear timer with DSGCSDFA_TIMER_CLR.
06/01/04  RLP   Modified the FA layer when it is in IDLE to ensure that it 
                  can service the DL watermark to keep it from filling up.
                Changed some debug messages from MSG_LOW to MSG_HIGH to
                  aid in field testing.
                LINT cleanup.
04/28/04  RLP   Modified octet alignment to make sure it skip a byte if
                  data is split across 2 dsm items.
05/30/04  AR    Lint corrections.
O6/04/03  RLP   Wrapped fa_state_trans_names inside TREX ifdef as these
                state names are only used during TREX builds
04/25/03  TMR   Changes for new UMTS CS data architecture
03/19/03  RLP   GCF certification changes to fix MT fax issue.
02/10/03  RLP   Spain IOT changes.
                  - Added support to limit amount of data processed in
                    BCS_TRA per 20ms interval.
                  - Changed unnecessary ASSERT to MSG_HIGH in 
                    dsgcsdfatrans_process_downlink().
01/14/03  RLP   Changed a MSG_HIGH to MSG_LOW in fa_perform_octet_alignment()
12/20/02  RLP   Added new interface function dsgcsdfa_is_idle().
                Changed IDLE state to not send SYNC frames if a disconnect
                  is pending.
                Changed FA_FRAME_DUMP macro to not loop when dumping frame.
                Changed debug messages.
12/06/02  RLP   Changed debug message from FA_MSG_HIGH to FA_MSG_LOW
11/15/02  RLP   Changes for GCF certification support.
11/04/02  RLP   Nortel IOT updates from Stage 2 testing.
09/11/02  RLP   MMO2 IOT changes.
                  - Added support for non-octet aligned data.
                  - Added support to minimize amount of sync frame
                    data queued to the uplink watermark which was 
                    causing delays in getting responses to the remote 
                    FA thereby causing MT calls to fail.
                  - Added additional debug.
07/18/02  RLP   Additional changes to maintain TREX compatibilty.
                  - removed sync_frame_mask variable
                  - change num_sync_frame counts
                  - change SYNC_FRAME define to compile under TREX
                    without a warning
                  - changed/added some debug messages
                  - fixed a couple of compiler warnings
07/12/02  RLP   Changes to maintain TREX compatibilty.
06/25/02  RLP   look_for_sync_or_status_frame() was passing back zeros to
                  T31 even though it may have found actual data when 
                  synchronizing on a status frame.  It now passes back the
                  status frame data if resynchronization occurs.
06/21/02  RLP   First pass changes to get fax working with Surf/Racal.
03/14/02  RLP   Initial release.

===========================================================================*/

/*===========================================================================
                INCLUDE FILES
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds_3gppi_utils.h"
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_GCSD_FAX
#include "stdio.h"
#include "dog.h"
#include "err.h"
#include "dsgcsdfa.h"
#include "dsgcsdfatrans.h"
#include "dsgcsdi.h"
#include "amssassert.h"
#include "comdef.h"
#include "data_msg.h"
#include "dsm.h"

/*===========================================================================
                DEFINITIONS AND DECLARATIONS
===========================================================================*/
#if 0 /* Debug Use Only */
#define DSGCSDFA_RACAL_OCTET_ALIGN_TEST 1
#endif
#if 0 /* Debug Use Only */
#define DS_GCSD_DEBUG_RACAL_TEST
#endif

#if 0 /* Debug Use Only */
#define DSGCSDFA_ENABLE_FAKE_TRAINING
#define DSGCSDFA_ENABLE_TRAINING_TIMER
#endif

/* For compiling in extra debug code. */
#define FA_TRANS_DEBUG

/* All transparent Fax Adaptor frames (SYNC, STATUS, and DATA) are 8 bytes. */
#define FRAME_SIZE  8

/* The number of consecutive good SYNC frames that must be received at
** the start of a call in order for the CONNECT to be sent to the local TE
*/
#define FA_SYNC_FRAMES_HYSTERISIS 10

#define FA_SYNC_FRAME_LOSS_HYSTERISIS 10

/* The number of resynchronization retries allowed before FA gives up */
#define FA_RESYNC_ATTEMPTS 10

/* The minimum amount of data allowed in the uplink watermark when 
** sending SYNC frames.  If the watermark has at least this much data in
** it, any attempt to queue additional SYNC frames will be disallowed.
** This is used to limit how much data can be pending for transmission.
*/
#define FA_UL_WM_SYNC_THRESHOLD 192

/* After the FA syncs up with the remote FA (at the start of a call) we
** have to allow time for the CED tonal aspect of the fax protocol which
** must be presented for a minimum of 2.6s prior to the start of phase A
*/
#define CED_TONE_DELAY 4500

/* The 8-byte SYNC frame exchanged between Fax Adaptors.
** From GSM 03.45, 5.2.2.1:  "SYNC frames are obtained by concatenating
** two 32 bit sync codewords, the second one being the 1's complement notation
** of the first one." */
#ifndef ENABLE_GSM_TREX_NETWORK_INTERFACE

/* Specify unsigned long long (ULL) values */
#define SYNC_FRAME_1_OFS0 0x3e375096c1c8af69ULL
#define SYNC_FRAME_2_OFS0 0x69afc8c19650373eULL
#define SYNC_FRAME_1_OFS1 0x7cd25e91832da16eULL
#define SYNC_FRAME_1_OFS2 0xf8a4bd22075b42ddULL
#define SYNC_FRAME_1_OFS3 0xf1497b450eb684baULL
#define SYNC_FRAME_1_OFS4 0xe393f68a1c6c0975ULL
#define SYNC_FRAME_1_OFS5 0xc627ed1539d812eaULL
#define SYNC_FRAME_1_OFS6 0x8d4fda2b72b025d4ULL
#define SYNC_FRAME_1_OFS7 0x1b9fb457e3604ba8ULL

#else

#define SYNC_FRAME_1_OFS0 0x3e375096c1c8af69
#define SYNC_FRAME_2_OFS0 0x69afc8c19650373e
#define SYNC_FRAME_1_OFS1 0x7cd25e91832da16e
#define SYNC_FRAME_1_OFS2 0xf8a4bd22075b42dd
#define SYNC_FRAME_1_OFS3 0xf1497b450eb684ba
#define SYNC_FRAME_1_OFS4 0xe393f68a1c6c0975
#define SYNC_FRAME_1_OFS5 0xc627ed1539d812ea
#define SYNC_FRAME_1_OFS6 0x8d4fda2b72b025d4
#define SYNC_FRAME_1_OFS7 0x1b9fb457e4604ba8

#endif

/* Need to send the sync frame msb first */
static const byte  uplink_sync_frame[] = {0x7c, 0xec, 0x0a, 0x69,
                                          0x83, 0x13, 0xf5, 0x96};

typedef struct sync_frame_offset_s 
{
  uint64 sync_value;
  uint16 sync_ofs;
  uint8  sync_value_id;
} sync_frame_ofs_struct_type;

static sync_frame_ofs_struct_type sync_frames[] =
{
  /*
  *********************************************************************** 
  ** WARNING --- WARNING --- WARNING --- WARNING --- WARNING --- WARNING
  **
  **   CHANGING THE ORDER OF THE ENTRIES IN THIS TABLE CAN LEAD TO VERY 
  **   BAD CONSEQUENCES RESULTING IN FAX DATA CALL FAILURES.  
  ** 
  **   DO NOT CHANGE THIS TABLE UNLESS YOU ARE ABSOLUTELY SURE BOTH MT 
  **   AND MO FAX DATA CALLS HAVE NOT BEEN ADVERSELY AFFECTED FOR ALL
  **   CASES.
  *********************************************************************** 
  */
  {SYNC_FRAME_2_OFS0, 0, 0x3e},
  {SYNC_FRAME_1_OFS0, 0, 0x69},
  {SYNC_FRAME_1_OFS5, 5, 0xea},
  {SYNC_FRAME_1_OFS6, 6, 0xd4},
  {SYNC_FRAME_1_OFS7, 7, 0xa8},
  {SYNC_FRAME_1_OFS1, 1, 0x6e},
  {SYNC_FRAME_1_OFS2, 2, 0xdd},
  {SYNC_FRAME_1_OFS3, 3, 0xba},
  {SYNC_FRAME_1_OFS4, 4, 0x75},
};

static boolean FA_fake_training_active = FALSE;
static uint32  TPS_byte_cnt = 0;
static uint32  T31_byte_cnt = 0;
static uint32  T31_fake_train_cnt = 0;
static uint32  T31_real_train_cnt = 0;

/* GSM 03.45, 5.2.2.1 says "up to 3 errors are tolerate in each single
** sync codeword."  So let's make a define for it.  Two codewords are
** concatenated for the test. */
#define SYNC_FRAME_BIT_ERRORS_TOLERATED  6

/* STATUS frames are 8 bytes, with two 1-byte fields alternating as follows:
**   -------------------------------------------------------------
**   | IDENT | INFO | IDENT | INFO | IDENT | INFO | IDENT | INFO |
**   -------------------------------------------------------------
**  MSB                                                         LSB
**
** The IDENT and INFO fields are set as follows:
** (Each IDENT field consists of 2 identical 4-bit codes.)
**
**           MSG_REC  MSG_TRA  BCS_REC
**           -------  -------  -------
**   IDENT:   0x33     0x44     0x11
**   INFO:    0x0f     0x0f     Actual BCS data byte from attached DTE.
*/

/* This goes into the INFO field of a STATUS frame if the local FA state
** is NOT BCS_REC. */
#define INFO_FIELD_IDLE  0x0f

/* GSM 03.45, 5.2.2.1 says up to 3 corrupted codes are tolerate in the
** total 8 instances repeated within a single STATUS frame."  That means if
** we got 5 good codes, we got a STATUS frame. */
#define STATUS_FRAME_GOOD_CODES_NEEDED  5

/* At the goofy fax rates (12,000 and 7,200 bps), there are SYNC frames mixed
** in with the DATA frames.  Here are number of DATA frames for each SYNC.
** At non-goofy rates, there are DATA frames only.  If for some reason the
** fax rate is unknown (should never happen), -1 is used. */
#define DATA_PER_SYNC_AT_12000  5
#define DATA_PER_SYNC_AT_7200   3
#define DATA_FRAMES_ONLY        0
#define UNKNOWN_DATA_PER_SYNC  -1

/* GSM 03.45, 5.2.3.3 says if we lose sync while in the BCS_TRA state for
** 2.5 seconds, we go give up on this state and go IDLE.  Doesn't really say
** to kill the call, so we'll let T31 and/or the attached DTE decide. */
#define SYNC_LOSS_DURATION  2500

/* Fairly arbirary number limiting the number of bytes we process per call to
** dsgcsdfatrans_process_downlink().  Should never hit, but just in case. */
#define MAX_DOWNLINK_BYTES_TO_PROCESS  100

#define FA_FRAME_DUMP(frame, frame_debug_enable) \
if (frame_debug_enable == TRUE) \
{ \
  FA_MSG_HIGH("f[0]=0x%x f[1]=0x%x f[2]=0x%x", \
    frame[0] & 0xff,frame[1] & 0xff,frame[2] & 0xff); \
  FA_MSG_HIGH("f[3]=0x%x f[4]=0x%x f[5]=0x%x",\
    frame[3] & 0xff,frame[4] & 0xff,frame[5] & 0xff); \
  FA_MSG_HIGH("f[6]=0x%x f[7]=0x%x", \
    frame[6] & 0xff,frame[7] & 0xff,0); \
}

#ifdef ENABLE_GSM_TREX_NETWORK_INTERFACE

/* Transparent Fax Adaptation state names for debugging. */
static const char *fa_state_trans_names[FA_TRANS_NUM_STATES] =
{ "IDLE", "BCS_REC", "BCS_TRA", "MSG_REC", "MSG_TRA" };

#endif

/* Transparent Fax Adaptation state machine function handler types for
** uplink and downlink. */
typedef ds_fa_status_enum_type (*fa_trans_uplink_handler_func_type)
                           (dsm_item_type                 *fax_uplink_data,
                            fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr);

typedef ds_fa_status_enum_type (*fa_trans_downlink_handler_func_type)
                           (dsm_item_type                **fax_downlink_data,
                            ds_fa_data_enum_type          *fax_type_of_data,
                            fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr);

/* Gotta prototype all these here for state machine declaration below. */
static ds_fa_status_enum_type fa_trans_uplink__invalid_data_for_state
                          (dsm_item_type                 *fax_uplink_data,
                           fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr);

static ds_fa_status_enum_type fa_trans_uplink__idle__no_data
                          (dsm_item_type                 *fax_uplink_data,
                           fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr);

static ds_fa_status_enum_type fa_trans_uplink__bcs_rec__no_data
                          (dsm_item_type                 *fax_uplink_data,
                           fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr);

static ds_fa_status_enum_type fa_trans_uplink__bcs_tra__no_data
                          (dsm_item_type                 *fax_uplink_data,
                           fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr);

static ds_fa_status_enum_type fa_trans_uplink__msg_tra__no_data
                          (dsm_item_type                 *fax_uplink_data,
                           fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr);

static ds_fa_status_enum_type fa_trans_uplink__idle__bcs_data
                          (dsm_item_type                 *fax_uplink_data,
                           fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr);

static ds_fa_status_enum_type fa_trans_uplink__bcs_rec__bcs_data
                          (dsm_item_type                 *fax_uplink_data,
                           fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr);

static ds_fa_status_enum_type fa_trans_uplink__idle__msg_data
                          (dsm_item_type                 *fax_uplink_data,
                           fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr);

static ds_fa_status_enum_type fa_trans_uplink__msg_rec__no_or_msg_data
                          (dsm_item_type                 *fax_uplink_data,
                           fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr);

static ds_fa_status_enum_type fa_trans_downlink__idle
                          (dsm_item_type                **fax_downlink_data,
                           ds_fa_data_enum_type          *fax_type_of_data,
                           fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr);

static ds_fa_status_enum_type fa_trans_downlink__bcs_rec
                          (dsm_item_type                **fax_downlink_data,
                           ds_fa_data_enum_type          *fax_type_of_data,
                           fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr);

static ds_fa_status_enum_type fa_trans_downlink__bcs_tra
                          (dsm_item_type                **fax_downlink_data,
                           ds_fa_data_enum_type          *fax_type_of_data,
                           fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr);

static ds_fa_status_enum_type fa_trans_downlink__msg_rec
                          (dsm_item_type                **fax_downlink_data,
                           ds_fa_data_enum_type          *fax_type_of_data,
                           fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr);

static ds_fa_status_enum_type fa_trans_downlink__msg_tra
                          (dsm_item_type                **fax_downlink_data,
                           ds_fa_data_enum_type          *fax_type_of_data,
                           fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr);

/* Transparent FA state machine handlers for uplink processing. */
static const fa_trans_uplink_handler_func_type
             fa_trans_uplink_state_handler[DS_FA_DATA_NUM_TYPES]
                                          [FA_TRANS_NUM_STATES] =
{
  /* DS_FA_DATA_NONE received from T31 layer.  No data to process. */
  {
    fa_trans_uplink__idle__no_data,            /* FA_STATE_TRANS_IDLE    */
    fa_trans_uplink__bcs_rec__no_data,         /* FA_STATE_TRANS_BCS_REC */
    fa_trans_uplink__bcs_tra__no_data,         /* FA_STATE_TRANS_BCS_TRA */
    fa_trans_uplink__msg_rec__no_or_msg_data,  /* FA_STATE_TRANS_MSG_REC */
    fa_trans_uplink__msg_tra__no_data          /* FA_STATE_TRANS_MSG_TRA */
  },
  /* DS_FA_DATA_BCS received from T31 layer.  BCS data to process. */
  {
    fa_trans_uplink__idle__bcs_data,           /* FA_STATE_TRANS_IDLE    */
    fa_trans_uplink__bcs_rec__bcs_data,        /* FA_STATE_TRANS_BCS_REC */
    fa_trans_uplink__invalid_data_for_state,   /* FA_STATE_TRANS_BCS_TRA */
    fa_trans_uplink__invalid_data_for_state,   /* FA_STATE_TRANS_MSG_REC */
    fa_trans_uplink__invalid_data_for_state    /* FA_STATE_TRANS_MSG_TRA */
  },
  /* DS_FA_DATA_MSG received from T31 layer.  MSG data to process. */
  {
    fa_trans_uplink__idle__msg_data,           /* FA_STATE_TRANS_IDLE    */
    fa_trans_uplink__invalid_data_for_state,   /* FA_STATE_TRANS_BCS_REC */
    fa_trans_uplink__invalid_data_for_state,   /* FA_STATE_TRANS_BCS_TRA */
    fa_trans_uplink__msg_rec__no_or_msg_data,  /* FA_STATE_TRANS_MSG_REC */
    fa_trans_uplink__invalid_data_for_state    /* FA_STATE_TRANS_MSG_TRA */
  }
};

/* Transparent FA state machine handlers for downlink processing. */
static const fa_trans_downlink_handler_func_type
       fa_trans_downlink_state_handler[FA_TRANS_NUM_STATES] =
{
  fa_trans_downlink__idle,        /* FA_STATE_TRANS_IDLE    */
  fa_trans_downlink__bcs_rec,     /* FA_STATE_TRANS_BCS_REC */
  fa_trans_downlink__bcs_tra,     /* FA_STATE_TRANS_BCS_TRA */
  fa_trans_downlink__msg_rec,     /* FA_STATE_TRANS_MSG_REC */
  fa_trans_downlink__msg_tra      /* FA_STATE_TRANS_MSG_TRA */
};

/*===========================================================================
                FUNCTION DEFINITIONS
===========================================================================*/

/*========================================================================
  Function: fa_perform_octet_alignment

  Description:
    This function is used to align the incoming data stream on octet
    boundaries if it has been determined that the data has been shifted 
    by some number of bits (1-7).  The is_it_sync_frame function will
    detect if the data has been shifted when it attempts to synchronize
    with the it.  If the data is shifted, it will save off the number
    of bits it has been shifted by so it can then be shifted back into
    alignment by this function before any further processing is done
    on the data.

  Dependencies:
    None.

  Parameters:
    dsm_ptr  - DSM item packet chain of interest.
    ctrl_blk_ptr - Transparent Fax Adaptor control block pointer.

  Return Value:
    void

  Side Effects:
    None.
========================================================================*/
INLINE void fa_perform_octet_alignment
(
  dsm_item_type  **dsm_downlink_data,
  fa_trans_ctrl_blk_struct_type  *ctrl_blk_ptr
)
{
  register uint8  hi_bits,
                  lo_bits;
  register sint15 pull_byte;
  dsm_item_type  *new_dsm_item_ptr = NULL;
  uint8           new_byte;   
  uint8           hi_bit_shift,
                  lo_bit_shift;
  uint16          push_count;
 

  /* Pull first byte from DSM downlink item chain. */
  pull_byte = dsm_pull8(dsm_downlink_data);

  if (pull_byte == -1)
  {
    /* It's empty.  Just leave. */
    return;
  }

  if (!ctrl_blk_ptr->have_leftover_byte)
  {
    /* We don't have a byte leftover from last time.  Pretend that we do. */
    ctrl_blk_ptr->have_leftover_byte = TRUE;
    ctrl_blk_ptr->leftover_byte = (uint8)pull_byte;

    pull_byte = dsm_pull8(dsm_downlink_data);

    if (pull_byte == -1)
    {
      /* It's empty.  Just leave. */
      return;
    }
  }

  /* 1st byte is the one leftover from before.  2nd byte is the new one. */
  hi_bits = ctrl_blk_ptr->leftover_byte;
  lo_bits = (uint8)pull_byte;
  hi_bit_shift = (uint8)(8 - ctrl_blk_ptr->bits_to_shift);
  lo_bit_shift = (uint8)(ctrl_blk_ptr->bits_to_shift);

  while (TRUE) /*lint !e716*/
  {
 
    /* Create the new octet aligned byte by appropriate bits which have
    ** been spread across the octet boundary.
    */
    new_byte = (uint8)((hi_bits << hi_bit_shift) | (lo_bits >> lo_bit_shift));

    FA_MSG_LOW("B1=0x%x B2=0x%x NewByte=0x%x", 
      hi_bits & 0xff, lo_bits & 0xff, new_byte & 0xff);

    /* Store the new byte away. */
    push_count = dsm_pushdown_tail(&new_dsm_item_ptr, &new_byte,
                                   sizeof(new_byte), DSM_DS_SMALL_ITEM_POOL);
    ASSERT(push_count == sizeof(new_byte));

    /* Get another byte. */
    pull_byte = dsm_pull8(dsm_downlink_data);

    if (pull_byte != -1)
    {
      /* The unused lo_bits will become the hi_bits for the next octet */
      hi_bits = lo_bits;
      /* This byte will contain the lo_bits of the next octet */
      lo_bits = (uint8)pull_byte;
    }
    else
    {
      ctrl_blk_ptr->leftover_byte = lo_bits;
      ctrl_blk_ptr->have_leftover_byte = TRUE;
      break;
    }
  } /* while */

 
  /* Return new DSM item chain to caller. */
  *dsm_downlink_data = new_dsm_item_ptr;
} /* fa_perform_octet_alignment */

/*========================================================================
  Function: better_dsm_extract

  Description:
    This is basically a wrapper for dsm_extract().  dsm_extract() always
    does a copy when you call it.  This function checks to see if the desired
    number of bytes are all in the 1st DSM item in the chain.  If so, no
    copy is done because the bytes are already contiguous in memory.

    This function only works from the beginning of a DSM packet chain, not
    from an arbitrary offset within the chain.

  Dependencies:
    None.

  Parameters:
    dsm_ptr  - DSM item packet chain of interest.
    buf_ptr  - If necessary, pointer to buffer to extract into.  If no copy
               needed, returns pointer to requested data in DSM item.
    ui16_len - Number of bytes to extract.

  Return Value:
    uint16 - Number of bytes actually extracted.

  Side Effects:
    None.
========================================================================*/
static uint16 better_dsm_extract
(
  dsm_item_type * dsm_ptr, 
  byte **         buf_ptr,
  uint16          ui16_len
)
{
  uint16  ui16_num_bytes;
  uint16  ui16_used;


  if (dsm_ptr == NULL)
  {
    /* Can't exactly get any data if we got no DSM item packet chain. */
    ui16_num_bytes = 0;
  }
  else
  {
    ui16_used = dsm_ptr->used;

    if (ui16_len <= ui16_used)
    {
      /* All desired bytes are in 1st DSM item in chain.  No copy needed. */
      *buf_ptr  = dsm_ptr->data_ptr;
      ui16_num_bytes = ui16_len;
    }
    else if (dsm_ptr->pkt_ptr == NULL)
    {
      /* Chain consists of only 1 DSM item.  Return whatever's in the 1 item. */
      *buf_ptr  = dsm_ptr->data_ptr;
      ui16_num_bytes = ui16_used;
    }
    else
    {
      /* Desired bytes reside in more than 1 DSM item.  Gotta do the copy. */
      ui16_num_bytes = dsm_extract(dsm_ptr, 0, (void*)*buf_ptr, ui16_len);
    }
  }

  return ui16_num_bytes;
} /* better_dsm_extract */

/*========================================================================
  Function: send_sync_frames

  Description:
    Send SYNC frame(s) to the remote Fax Adaptor.

  Dependencies:
    None.

  Parameters:
    ui16_num_frames - Number of duplicate SYNC frames to send.
    ctrl_blk_ptr    - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS - Function completed successfully.
      DS_FA_FAILURE - Fatal error.  Call should be aborted.

  Side Effects:
    Send SYNC frame(s) to the remote Fax Adaptor.
========================================================================*/
static ds_fa_status_enum_type send_sync_frames
(
  uint16                               ui16_num_frames,
  const fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr,
  boolean                              override
)
{
  dsm_item_type          *dsm_item_ptr = NULL;
  uint16                  ui16_count;
  ds_fa_status_enum_type  ret_val = DS_FA_SUCCESS;

  if( ctrl_blk_ptr->uplink_wm_ptr && 
      ((ctrl_blk_ptr->uplink_wm_ptr->current_cnt < FA_UL_WM_SYNC_THRESHOLD) ||
       (ctrl_blk_ptr->fa_state_trans == FA_STATE_TRANS_MSG_REC) || override))
  {
    if (override == TRUE)
    {
      DATA_MSG2_HIGH("OVERRIDE: Send %d SYNC frames, cnt=%d", 
                  ui16_num_frames, ctrl_blk_ptr->uplink_wm_ptr->current_cnt);
    }

    DATA_MSG2_LOW("Sending %d SYNC frame(s) cnt=%d", 
               ui16_num_frames, ctrl_blk_ptr->uplink_wm_ptr->current_cnt);

    /* Loop on desired number of SYNC frames. */
    for (ui16_count = 0; ui16_count < ui16_num_frames; ui16_count++)
    {
      if (dsm_pushdown_tail(&dsm_item_ptr, (void*)&uplink_sync_frame[0],
                            FRAME_SIZE, DSM_DS_SMALL_ITEM_POOL) != FRAME_SIZE)
      {
        /* This is bad.  Function couldn't allocate another DSM item. */
        ret_val = DS_FA_FAILURE;
        break;
      }
    }

    /* Now give it to the GCSD Transport Protocol Stack's watermark. */
    if (dsm_item_ptr)
    {
      DSM_ENQUEUE(ctrl_blk_ptr->uplink_wm_ptr, &dsm_item_ptr);
      FA_MSG_LOW("TPS-UL:Qcnt=%d Qhi=%d Qlo=%d", 
                 ctrl_blk_ptr->uplink_wm_ptr->current_cnt,
                 ctrl_blk_ptr->uplink_wm_ptr->hi_watermark,
                 ctrl_blk_ptr->uplink_wm_ptr->lo_watermark);
    }
  }

  return ret_val;
} /* send_sync_frames */

/*========================================================================
  Function: send_status_frames

  Description:
    Send a number of STATUS frames to the remote Fax Adaptor.

  Dependencies:
    Transparent Fax Adaptor state.

  Parameters:
    ui8_transmit_info_byte - In BCS_REC state, this has BCS byte received from
                             DTE.  In all other states, it's INFO_FIELD_IDLE.
                             Passed in by caller in both cases.
    ui16_num_frames        - Number of duplicate STATUS frames to send.
    curr_state             - Current local Fax Adaptor State.
    f_sync_first           - Whether or not to send SYNC frame first.
    ctrl_blk_ptr           - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS - Function completed successfully.
      DS_FA_FAILURE - Fatal error.  Call should be aborted.

  Side Effects:
    Queues a number of DSM items on GCSD Transport Protocol Stack watermark.
========================================================================*/
INLINE ds_fa_status_enum_type send_status_frames 
(
  uint8                                ui8_transmit_info_byte,
  uint16                               ui16_num_frames,
  fa_state_trans_enum_type             curr_state,
  boolean                              f_sync_first,
  const fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  dsm_item_type          *dsm_item_ptr;
  uint16                  ui16_count = 0;
  uint8                   ui8_ident_field, status_frame[FRAME_SIZE];
  ds_fa_status_enum_type  ret_val = DS_FA_FAILURE;


  dsm_item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);

  if (dsm_item_ptr)
  {
    if (f_sync_first)
    {
      /* If we're sending a SYNC first, in order to alternate SYNC and
      ** STATUS, there should only be one STATUS behind it. */
      DATA_MSG1_LOW("Sending %d SYNC+STATUS frame(s)", ui16_num_frames);
    }
    else
    {
      DATA_MSG1_LOW("Sending %d STATUS frame(s)", ui16_num_frames);
    }

    /* Goofy method of sending the local FA state to the remote FA.  Two
    ** 4-bit fields containing the same ID value. */
    ui8_ident_field = (uint8)curr_state;
    ui8_ident_field |= (uint8)(ui8_ident_field << 4);

    /* More goofy stuff described in GSM 03.45, 5.2.2.1.  STATUS frame is
    ** 8 bytes of alternating IDENT and INFO fields.  IDENT byte was
    ** generated above to tell remote FA what our state is.  INFO byte was
    ** passed in by caller and is actual BCS data received from DTE
    ** if we're in BCS_REC.  It's INFO_FIELD_IDLE in the other states. */
    do
    {
      status_frame[ui16_count++] = GCSD_REFLECT_BYTE(ui8_ident_field);
      if(ui16_count >= FRAME_SIZE)
      {
        DATA_MSG1_HIGH("Exiting loop, array overrun %d", ui16_count);
        break;
      }
      status_frame[ui16_count++] = GCSD_REFLECT_BYTE(ui8_transmit_info_byte);    
    }
    while(ui16_count < FRAME_SIZE);

    /* Loop on desired number of STATUS frames. */
    for (ui16_count = 0; ui16_count < ui16_num_frames; ui16_count++)
    {
      /* Queue up the status frame */
      if (dsm_pushdown(&dsm_item_ptr, (void*)&status_frame[0],
                            FRAME_SIZE, DSM_DS_SMALL_ITEM_POOL) != FRAME_SIZE)
      {
        /* This is bad.  Function couldn't allocate another DSM item. */
        break;
      }

      if (f_sync_first)
      {
        /* Throw a SYNC frame in first. */
        if (dsm_pushdown(&dsm_item_ptr, (void*)&uplink_sync_frame[0],
                         FRAME_SIZE, DSM_DS_SMALL_ITEM_POOL) != FRAME_SIZE)
        {
          /* This is bad.  Function couldn't allocate another DSM item. */
          break;
        }
      }
    }

    /* Now give it to the GCSD Transport Protocol Stack's watermark. */
    DSM_ENQUEUE(ctrl_blk_ptr->uplink_wm_ptr, &dsm_item_ptr);

    /* We succeeded in queuing up the data */
    ret_val = DS_FA_SUCCESS;

    if( ctrl_blk_ptr->uplink_wm_ptr )
    {
      FA_MSG_LOW("TPS-UL:Qcnt=%d Qhi=%d Qlo=%d", 
                 ctrl_blk_ptr->uplink_wm_ptr->current_cnt,
                 ctrl_blk_ptr->uplink_wm_ptr->hi_watermark,
                 ctrl_blk_ptr->uplink_wm_ptr->lo_watermark);
    }
  }

  return ret_val;
} /* send_status_frames */

/*========================================================================
  Function: is_it_sync_frame

  Description:
    Takes a pointer to the first byte of an 8-byte frame as input and
    determines if it is a SYNC frame or not.  GSM 03.45, 5.2.2.1 says
    "up to 3 errors are tolerate in each single sync codeword."  So we
    gotta do a bit comparison.  If only 3 or fewer differ from the SYNC
    frame bits, we still call it a SYNC frame.  The algorithm below doing
    the bit checking will loop a maximum of 4 times, no matter what
    "unknown_frame" looks like.

  Dependencies:
    None.

  Parameters:
    frame_ptr - Pointer to first byte of unknown 8-byte frame.
    ctrl_blk_ptr   - Transparent Fax Adaptor control block pointer.

  Return Value:
    TRUE  - Passed-in frame IS a SYNC frame.
    FALSE - Passed-in frame is NOT a SYNC frame.

  Side Effects:
    None.
========================================================================*/
INLINE boolean is_it_sync_frame
(
  const uint8 *frame_ptr,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  boolean sync_found = FALSE;
  uint8   i, 
          idx;
  uint8   max_tries;
  uint8   bit_err_cnt;
  uint64  ui64_frame_value;
  uint64  ui64_bit_differences;


  /* XOR all the bits of the unknown frame with the bits of a SYNC frame.
  ** Any position with a 1 bit means that bit doesn't match.  I realize it's
  ** totally lame to copy all FRAME_SIZE bytes every bloody time, but the
  ** ARM requires 32-bit alignment to do the fancy math below.  If we get
  ** ridda the fancy math, we can get ridda the copy. */

  /* The first 2 entries in the SYNC frame table are the octet aligned
  ** SYNC frames (MSB first / LSB first).  If the offset has already
  ** been found then we know that to match a SYNC frame it must be one
  ** of the 2 octet aligned values so we can go ahead and limit the
  ** match process to those 2.  Otherwise, we don't know what the offset
  ** is so we have to search the table to find it.  In all case, we start
  ** the search by always looking for the normal expected octet aligned
  ** SYNC frame which is in table entry 0.
  */
  idx = 0;
  if (ctrl_blk_ptr->bits_to_shift != -1)
  {
    /* We have already found the offset so the data coming in should
    ** be octet aligned so we only need to look at the first 2 table
    ** entries.
    */
    max_tries = 2;
  }
  else
  {
    /* We have NOT found the offset yet so we need to search the table
    ** to find it!
    */
    max_tries = ARR_SIZE(sync_frames);
  }

  /* We are banking on the fact that the sync frame pattern once found
  ** will not change during the call.  So if we know the pattern and the
  ** first byte of the frame is not the same as the expected pattern it 
  ** can't be a sync frame so we can skip the rest of the processing.
  */
  if (ctrl_blk_ptr->f_synchronized == TRUE)
  {
    if (ctrl_blk_ptr->bits_to_shift != -1)
    {
      if ((*frame_ptr != sync_frames[0].sync_value_id) &&
          (*frame_ptr != sync_frames[1].sync_value_id))
      {
        FA_MSG_LOW("SYNC[0] wrong: fp[0]=0x%x sync[0]=0x%x idx=%d",
          *frame_ptr, sync_frames[idx].sync_value_id, idx);

        return (FALSE);
      }
    }
  }

  memscpy((byte*)&ui64_frame_value,sizeof(ui64_frame_value), frame_ptr, FRAME_SIZE);

  for (i = 0; i < max_tries; i++)
  {
    /* Assume we will find a SYNC frame */
    sync_found = TRUE;
    bit_err_cnt = 0;

    ui64_bit_differences = ui64_frame_value;
    ui64_bit_differences ^= sync_frames[idx].sync_value;

    /* This loop counts the number of bit differences, but will never loop
    ** more than SYNC_FRAME_BIT_ERRORS_TOLERATED times. */
    while (ui64_bit_differences != 0)
    {
      /* Increment bit delta count. */
      bit_err_cnt++;

      if (bit_err_cnt > SYNC_FRAME_BIT_ERRORS_TOLERATED)
      {
        /* Too many bits don't match.  It must not be a SYNC frame. */
        /* Check the next sync frame pattern */
        sync_found = FALSE;
        break;
      }

      /* This magic code turns the least significant 1 bit into a 0.  Cool. */
      ui64_bit_differences &= (ui64_bit_differences - 1);
    } /* while */

    /* Now, if we think we have a SYNC frame we need to check the wrap
    ** around byte with the beginning of the packet...they should be the
    ** same.  If not, we won't count the frame as a SYNC and will continue
    ** looking.
    */
    if (sync_found == TRUE)
    {
      /* We only need to set the shift offset one time, banking on the
      ** fact that it will stay constant throughout the duration of the
      ** call.
      */
      if (ctrl_blk_ptr->bits_to_shift == -1)
      {
        /* Since we will now be aligning the data on octet boundaries
        ** we can start looking for the normal octet aligned SYNC frame
        ** so we start the sync table index at 0 which is where this
        ** frame format is located.
        */
        ctrl_blk_ptr->last_sync_frame_index = (int8)idx; 
        ctrl_blk_ptr->bits_to_shift = sync_frames[idx].sync_ofs; 

        FA_MSG_HIGH("SYNC Found, idx=%d bits=%d val=0x%x",
          idx, sync_frames[idx].sync_ofs, sync_frames[idx].sync_value);
      }
      break;
    }

    idx = (idx+1) % max_tries;
  } /* for */

  /* If we got here, it must be a SYNC frame. */
  return (sync_found);
} /* is_it_sync_frame */


/*========================================================================
  Function: determine_bcs_byte

  Description:
    Takes a pointer to the first byte of an 8-byte STATUS frame as input
    and determines what the 8 bits worth of BCS data from the frame are.
    GSM 03.45, 5.2.3.3 says the technique for getting the desired byte from
    the 8 bytes is "likely a voting algorithm, implementation dependent".

    There are 4 copies of the 1 BCS byte in the 8-byte frame.  In an ideal
    radio channel, all 4 values are the same.  But the world ain't ideal.
    The technique here is to simply use the value of the first
    two found that match.  If 3 or all 4 match, the algorithm will end up
    using that value.  If there are 2 different pairs, it just uses the first
    one found.  If all 4 are different, it ends up using the 3rd value.

  Dependencies:
    None.

  Parameters:
    status_frame - 64-bit STATUS frame to inspect.

  Return Value:
    uint8 - Contains one byte of BCS data from the frame.

  Side Effects:
    None.
========================================================================*/
static uint8 determine_bcs_byte
(
  const uint8 *frame_ptr
)
{
  uint8  ui8_val1, ui8_val2, ui8_val3, ui8_val4, ui8_ret_val;


  /* The STATUS frame format is shown earlier in this file.
  ** We want to grab the 4 INFO bytes outta the STATUS frame. */
  ui8_val1 = *(frame_ptr + 1);
  ui8_val2 = *(frame_ptr + 3);
  ui8_val3 = *(frame_ptr + 5);
  ui8_val4 = *(frame_ptr + 7);

  if ((ui8_val1 == ui8_val2) || (ui8_val1 == ui8_val3) ||
      (ui8_val1 == ui8_val4))
  {
    /* 1st equals 2nd, 3rd, or 4th.  So use it. */
    ui8_ret_val = ui8_val1;
  }
  else if ((ui8_val2 == ui8_val3) || (ui8_val2 == ui8_val4))
  {
    /* 2nd equals 3rd or 4th.  We'll take it. */
    ui8_ret_val = ui8_val2;
  }
  else
  {
    /* Either 3rd equals 4th, or they're all different.  Take the 3rd. */
    ui8_ret_val = ui8_val3;
  }

  return ui8_ret_val;
} /* determine_bcs_byte */


/*========================================================================
  Function: is_it_status_frame

  Description:
    Takes a pointer to the first byte of an 8-byte frame as input and
    determines if it is a STATUS frame or not.  The STATUS frame format
    is shown earlier in this file.  In the frame are 4 IDENT fields with
    2 codes each, for a total of 8 codes.  According to GSM 03.45, 5.2.2.1,
    if 5 of the 8 codes correspond to one of the 3 valid IDs, then we have
    a STATUS frame.

    The ID received represents the state of the remote Fax Adaptor state
    machine.  If it corresponds to the BCS_REC state, then there is a BCS
    data byte in the frame too that we must get back to the attached DTE.

  Dependencies:
    None.

  Parameters:
    frame_ptr    - Pointer to 1st of 8 bytes that might be a STATUS frame.
    frame_type   - If it's a STATUS frame, this returns the state of
                   the remote Fax Adaptor.
    ui8_bcs_data - If it's a STATUS frame, this returns the one BCS byte
                   sent in the frame by the remote Fax Adaptor.

  Return Value:
    TRUE  - Passed-in frame IS a STATUS frame.
    FALSE - Passed-in frame is NOT a STATUS frame.

  Side Effects:
    None.
========================================================================*/
static boolean is_it_status_frame
(
  const uint8              *frame_ptr,
  fa_state_trans_enum_type *frame_type,
  uint8                    *ui8_bcs_data
)
{
  uint16   ui16_count, ui16_inner_count;
  uint8    ui8_ident_code;
  uint16   ui16_bcs_rec_count = 0;
  uint16   ui16_msg_rec_count = 0;
  uint16   ui16_msg_tra_count = 0;
  boolean  f_ret_val = TRUE;       /* Assume it is a STATUS frame at first. */


  /* Every other byte in frame is IDENT field, so loop half the byte count. */
  for (ui16_count = 0; ui16_count < FRAME_SIZE; ui16_count += 2)
  {
    /* Grab the 1-byte IDENT field from the frame. */
    ui8_ident_code = *(frame_ptr + ui16_count);

    /* But each IDENT field has 2 codes.  Gotta look at each separately. */
    for (ui16_inner_count = 0; ui16_inner_count < 2; ui16_inner_count++)
    {
      /* Depending on the 4-bit code found, increment a counter. */
      switch (ui8_ident_code & 0x0f)
      {
        case FA_STATE_TRANS_BCS_REC:
          ui16_bcs_rec_count++;
          break;

        case FA_STATE_TRANS_MSG_REC:
          ui16_msg_rec_count++;
          break;

        case FA_STATE_TRANS_MSG_TRA:
          ui16_msg_tra_count++;
          break;

        default:
          /* Ain't gotta do nothing here.  We don't recognize it. */
          break;
      }

      /* Do shift to get next 4-bit code. */
      ui8_ident_code >>= 4;
    }
  }

  /* We're done looping.  Now check the counts.  If one is big enough,
  ** we've found ourselves a STATUS frame. */

  if (ui16_bcs_rec_count >= STATUS_FRAME_GOOD_CODES_NEEDED)
  {
    *frame_type = FA_STATE_TRANS_BCS_REC;
  }
  else if (ui16_msg_rec_count >= STATUS_FRAME_GOOD_CODES_NEEDED)
  {
    *frame_type = FA_STATE_TRANS_MSG_REC;
  }
  else if (ui16_msg_tra_count >= STATUS_FRAME_GOOD_CODES_NEEDED)
  {
    *frame_type = FA_STATE_TRANS_MSG_TRA;
  }
  else
  {
    /* This ain't no bloody STATUS frame. */
    f_ret_val = FALSE;
  }

  /* Determine BCS byte that came with the STATUS frame.  This is really only
  ** interesting for state BCS_REC.  It's supposed to be 0x0f otherwise.
  ** And when it's not a STATUS frame, take a shot in the dark for fun. */
  *ui8_bcs_data = determine_bcs_byte(frame_ptr);

  return f_ret_val;
} /* is_it_status_frame */


/*========================================================================
  Function: look_for_sync_frame

  Description:
    This function attempts to get synchronized with the incoming data
    stream from the remote FA.  It scans the data for a SYNC frame.  If it
    finds one, it removes the data up to and including the SYNC frame to
    leave the remaining data on a frame boundary.  It removes data as it
    looks, so if it never finds one, that data goes in the bit bucket.

  Dependencies:
    None.

  Parameters:
    remote_fa_data - DSM item with data to scan for SYNC frame.
    ctrl_blk_ptr   - Transparent Fax Adaptor control block pointer.

  Return Value:
    TRUE  - Found a SYNC frame.
    FALSE - Did NOT find a SYNC frame.

  Side Effects:
    Removes data from passed-in DSM item, leaving it on frame boundary.
========================================================================*/
/* ARGSUSED (lint) */
static boolean look_for_sync_frame
(
  dsm_item_type                **remote_fa_data,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  uint16   bytes_available;
  uint8    sync_search_buf[FRAME_SIZE], *frame_ptr;


  DATA_MSG0_LOW("Look for SYNC frame");

  /* Actually loops until we find SYNC or run out of data. */
  while (TRUE) /*lint !e716*/
  {
    /* Grab a frame's worth of data to check out. */
    frame_ptr = &sync_search_buf[0];
    bytes_available = better_dsm_extract(*remote_fa_data, &frame_ptr,
                                         FRAME_SIZE);

    if (bytes_available < FRAME_SIZE)
    {
      /* We ran out of data, and still no SYNC frame. */
      DATA_MSG1_LOW("Did not find SYNC! bytes=%d",bytes_available);
      return(FALSE);
    }

    FA_FRAME_DUMP(frame_ptr, FALSE);  /*lint !e506 !e774*/ 

    if (is_it_sync_frame(frame_ptr, ctrl_blk_ptr))
    {
      FA_FRAME_DUMP(frame_ptr, FALSE);  /*lint !e506 !e774*/ 

      /* Yippee!  We finally found a SYNC frame.  Toss the SYNC frame, so
      ** now we are synchronized on a frame boundary. */
      (void)dsm_pullup(remote_fa_data, NULL, FRAME_SIZE);

      if (ctrl_blk_ptr->sync_loss_hysterisis_cnt > 0)
      {
        ctrl_blk_ptr->sync_loss_hysterisis_cnt = 0;
      } 

      return(TRUE);
    }
    else
    {
      /* Still not synchronized.  Toss a byte and look at next frame. */
      (void)dsm_pull8(remote_fa_data);

      /* Keep hysterisis on number of consecutive bad SYNC frames */
      if ((ctrl_blk_ptr->sync_loss_hysterisis_cnt++) == FA_SYNC_FRAME_LOSS_HYSTERISIS)
      {
        FA_MSG_HIGH("SYNC officially lost, hyst=%d",
          FA_SYNC_FRAME_LOSS_HYSTERISIS,0,0);
        FA_FRAME_DUMP(frame_ptr, TRUE);  /*lint !e506 !e774*/
        ctrl_blk_ptr->sync_loss_hysterisis_cnt = 0;
        return(FALSE);
      }
    }
  } 
} /* look_for_sync_frame */


/*========================================================================
  Function: look_for_sync_or_status_frame

  Description:
    This function attempts to get synchronized with the incoming data
    stream from the remote FA.  It scans the data for a SYNC or STATUS
    frame.  If it finds one, it removes the data up to the frame to
    leave the remaining data on a frame boundary.  It removes data as it
    looks, so if it never finds one, that data goes in the bit bucket.

    In addition, if syncronization loss last for some time, this function
    "makes up" some data to return to T31 and the attached DTE.

    This function is only used in the BCS_TRA state.

  Dependencies:
    None.

  Parameters:
    remote_fa_data - DSM item with data to scan for SYNC or STATUS frame.
    data_for_t31   - DSM item returned to caller that may have fabricated
                     BCS data to return to the attached DTE.  Only happens
                     if we're out of sync for awhile.
    f_synchronized - Return to caller if we got synchronized or not.
    ctrl_blk_ptr   - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS - Function completed successfully.
      DS_FA_FAILURE - Fatal error.  Call should be aborted.

  Side Effects:
    Removes data from passed-in DSM item, leaving it on frame boundary.
========================================================================*/
static ds_fa_status_enum_type look_for_sync_or_status_frame
(
  dsm_item_type                **remote_fa_data,
  dsm_item_type                **data_for_t31,
  boolean                       *f_synchronized,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  boolean                   found_sync = FALSE, 
                            found_status = FALSE;
  uint16                    bytes_available, ui16_bytes_per_bcs_byte;
  uint8                     frame_buf[FRAME_SIZE], *frame_ptr;
  fa_state_trans_enum_type  frame_type = FA_STATE_TRANS_IDLE;
  uint8                     ui8_bcs_data = 0;


  DATA_MSG0_HIGH("In BCS_TRA trying to get SYNC'ed");

  if ((rex_get_timer(ctrl_blk_ptr->timer_ptr) == 0) &&
      !ctrl_blk_ptr->f_goto_idle)
  {
    /* The sync loss timer isn't running, AND it hasn't been running, so we
    ** just lost sync.  Start up the sync loss timer. */
    DATA_MSG1_HIGH("Start %dmsec BCS_TRA sync loss timer", 
      SYNC_LOSS_DURATION);

    ctrl_blk_ptr->timer_type = FA_SYNC_LOSS_TIMER_TYPE;
    (void)rex_set_timer(ctrl_blk_ptr->timer_ptr, SYNC_LOSS_DURATION);
  }

  /* Given the OTA rate and the number of bytes in a frame, calculate
  ** the number of OTA bytes we're gonna need to have just one BCS byte. */
  ui16_bytes_per_bcs_byte = (uint16)(ctrl_blk_ptr->ui16_frames_per_bcs_byte *
                                     FRAME_SIZE);

  /* Loops until we find SYNC or STATUS frame or run out of data. */
  while (TRUE)  /*lint !e716*/
  {
    /* Grab a frame's worth of data to check out. */
    frame_ptr = &frame_buf[0];
    bytes_available = better_dsm_extract(*remote_fa_data, &frame_ptr,
                                              FRAME_SIZE);

    if (bytes_available < FRAME_SIZE)
    {
      /* We ran out of data, and still no SYNC STATUS frame. */
      FA_FRAME_DUMP(frame_ptr, FALSE);  /*lint !e506 !e774*/
      DATA_MSG1_LOW("Incomplete frame, bytes=%d!", bytes_available);
      break;
    }

    found_sync = is_it_sync_frame(frame_ptr, ctrl_blk_ptr);

    if (found_sync == FALSE)
    {
      found_status = is_it_status_frame(frame_ptr, &frame_type, &ui8_bcs_data);
    }

    FA_MSG_LOW("bcs_data=0%x sync?=%d status?=%d",
      ui8_bcs_data, found_sync, found_status);

    if (found_sync || found_status)
    {
      /* Yippee!  We finally got synchronized. */
      *f_synchronized = TRUE;

      if (found_sync)
      {
        DATA_MSG0_HIGH("Found SYNC frame!");
      }
      else
      {
        DATA_MSG2_HIGH("Found STATUS frame! typ=%d data=%x", 
          frame_type, ui8_bcs_data);
      }

      /* Kill loss of sync timer now that we're sync'ed up. */
      if ((rex_get_timer(ctrl_blk_ptr->timer_ptr) != 0) &&
          (ctrl_blk_ptr->timer_type == FA_SYNC_LOSS_TIMER_TYPE))
      {
        DATA_MSG0_HIGH("Stopping SYNC loss timer");
        DSGCSDFA_TIMER_CLR( ctrl_blk_ptr->timer_ptr, ctrl_blk_ptr->timer_sig );
      }

      /* Restart bad byte counter because now we're sync'ed up.  This doesn't
      ** exactly follow the spec, in that we should return some guess data to
      ** T31.  Hopefully we got real data now, so skip the guessing part. */
      ctrl_blk_ptr->ui16_bytes_tossed = 0;

      return DS_FA_SUCCESS;
    }

    /* Still not synchronized.  Toss a byte and look at next frame offset. */
    (void)dsm_pull8(remote_fa_data);
    ctrl_blk_ptr->ui16_bytes_tossed++;
  }

  /* If we've been outta sync for awhile, we won't have any received BCS
  ** data for the attached DTE, even though we should.  GSM 03.45, 5.2.3.3
  ** says to return "an estimate of the INFO octet value in the received
  ** STATUS frames" to the attached DTE even if we can't identify any STATUS
  ** frames.  Let's just return zeros to T31 at the appropriate rate.  The
  ** received frames are crap, and zeros seem as benign as anything. */
  ui8_bcs_data = 0x00;

  while (ctrl_blk_ptr->ui16_bytes_tossed >= ui16_bytes_per_bcs_byte)
  {
    if (dsm_pushdown_tail(data_for_t31, &ui8_bcs_data, sizeof(ui8_bcs_data),
                          DSM_DS_SMALL_ITEM_POOL) != sizeof(ui8_bcs_data))
    {
      /* The DSM item was full.  Attempt to chain new one failed. */
      return DS_FA_FAILURE;
    }

    ctrl_blk_ptr->ui16_bytes_tossed -= ui16_bytes_per_bcs_byte;
  }

  DATA_MSG1_HIGH("Giving %d fake BCS bytes to T31", 
    dsm_length_packet(*data_for_t31));

  return DS_FA_SUCCESS;
} /* look_for_sync_or_status_frame */


/*========================================================================
  Function: set_num_repeated_status_frames

  Description:
    Checks the Bearer Services Access Rate and determines the number of
    STATUS frames that correspond to that rate.  This applies to the BCS phase
    of a fax call, which always transfers data at 300 bps.  But the OTA rate
    is greater than that, so we duplicate data to fill the channel.

    This function is called whenever we enter the BCS_REC or BCS_TRA
    states because those are the states we handle STATUS frames in.  That
    way we get an up-to-date OTA rate value to work with.

  Dependencies:
    Bearer Services Access Rate.

  Parameters:
    ctrl_blk_ptr - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS - Function completed successfully.
      DS_FA_FAILURE - Fatal error.  Call should be aborted.

  Side Effects:
    Sets ctrl_blk_ptr->ui16_frames_per_bcs_byte.
========================================================================*/
static ds_fa_status_enum_type set_num_repeated_status_frames
(
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  ds_fa_status_enum_type  ret_val = DS_FA_SUCCESS;


  /* Given the OTA rate, determine the number of STATUS frames to send for
  ** each byte of 300 bps BCS data.  Done as follows:
  ** ui16_frames_per_bcs_byte = OTA Rate / 300 bps / 8 (FRAME_SIZE) bytes */
  switch (ctrl_blk_ptr->data_rate)
  {
    case DS_UCSD_RATE_2400:
      /* 2400 bps / 300 bps / 8 bytes per frame = 1 frame per byte */
      ctrl_blk_ptr->ui16_frames_per_bcs_byte = 1;
      break;

    case DS_UCSD_RATE_4800:
      /* 4800 bps / 300 bps / 8 bytes per frame = 2 frames per byte */
      ctrl_blk_ptr->ui16_frames_per_bcs_byte = 2;
      break;

    case DS_UCSD_RATE_9600:
      /* 9600 bps / 300 bps / 8 bytes per frame = 4 frames per byte */
      ctrl_blk_ptr->ui16_frames_per_bcs_byte = 4;
      break;

    case DS_UCSD_RATE_14400:
      /* 14400 bps / 300 bps / 8 bytes per frame = 6 frames per byte */
      ctrl_blk_ptr->ui16_frames_per_bcs_byte = 6;
      break;

      /* GSM 03.45 doesn't say what to do with these three rates. */
    default:
      /* Landing here is bad.  We don't know how to handle whatever the OTA
      ** rate is.  Just say 1 STATUS frame for the hell of it, and return
      ** an error meaning T31 should abort the call. */
      DATA_MSG1_ERROR("Bad OTA rate: %d", ctrl_blk_ptr->data_rate);

      ret_val = DS_FA_FAILURE;
  }

  return ret_val;
} /* set_num_repeated_status_frames */


/*========================================================================
  Function: set_num_repeated_sync_frames

  Description:
    Checks the Bearer Services Access Rate and determines the number of
    SYNC frames that correspond to that rate.  We need to keep the channel
    full of data.

  Dependencies:
    Bearer Services Access Rate.

  Parameters:
    ctrl_blk_ptr - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS - Function completed successfully.
      DS_FA_FAILURE - Fatal error.  Call should be aborted.

  Side Effects:
    None
========================================================================*/
static ds_fa_status_enum_type set_num_repeated_sync_frames
(
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  ds_fa_status_enum_type  ret_val = DS_FA_SUCCESS;
  

  /* Given the OTA rate, determine the number of STATUS frames to send for
  ** each byte of 300 bps BCS data.  Done as follows:
  ** ui16_frames_per_bcs_byte = OTA Rate / 300 bps / 8 (FRAME_SIZE) bytes */
  switch (ctrl_blk_ptr->data_rate)
  {
    case DS_UCSD_RATE_2400:
      ctrl_blk_ptr->num_sync_frames = 1;
      break;

    case DS_UCSD_RATE_4800:
      ctrl_blk_ptr->num_sync_frames = 2;
      break;

    case DS_UCSD_RATE_9600:
      ctrl_blk_ptr->num_sync_frames = 4;
      break;

    case DS_UCSD_RATE_14400:
      ctrl_blk_ptr->num_sync_frames = 6;
      break;


      /* GSM 03.45 doesn't say what to do with these three rates. */
    default:
      /* Landing here is bad.  We don't know how to handle whatever the OTA
      ** rate is.  Just say 1 STATUS frame for the hell of it, and return
      ** an error meaning T31 should abort the call. */
      DATA_MSG1_ERROR("Bad OTA rate: %d", ctrl_blk_ptr->data_rate);

      ctrl_blk_ptr->num_sync_frames = 1;

      ret_val = DS_FA_FAILURE;
  }

  return (ret_val);
} /* set_num_repeated_sync_frames */

/*========================================================================
  Function: change_state_to

  Description:
    Changes state to the new, passed-in state.  If the old state requires
    and exit processing, or if the new state requires any entry processing,
    do that too.

  Dependencies:
    Current state, new state, and Bearer Services Access Rate.

  Parameters:
    new_state    - State to transition to.
    ctrl_blk_ptr - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS           - Function completed successfully.
      DS_FA_SWITCHING_TO_IDLE - Success, and state machine going IDLE.
      DS_FA_FAILURE           - Fatal error.  Call should be aborted.

  Side Effects:
    Changes the state.
========================================================================*/
static ds_fa_status_enum_type change_state_to
(
  fa_state_trans_enum_type       new_state,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  ds_fa_status_enum_type  ret_val = DS_FA_SUCCESS;


  /* New state should always differ from the current state. */
  if (new_state == ctrl_blk_ptr->fa_state_trans)
  {
    DATA_MSG1_HIGH("Same state transition, state=%d", new_state);
    return(DS_FA_SUCCESS);
  }

  FA_MSG_STATE(ctrl_blk_ptr->fa_state_trans, new_state,
               ctrl_blk_ptr->f_synchronized);

  if ((ctrl_blk_ptr->fa_state_trans == FA_STATE_TRANS_MSG_REC) ||
      (ctrl_blk_ptr->fa_state_trans == FA_STATE_TRANS_MSG_TRA))
  {
    /* If we're leaving a MSG state, we ain't training no more. */
    if (ctrl_blk_ptr->f_training_phase == TRUE)
    {
      DATA_MSG0_HIGH("End TRAINING phase");
      ctrl_blk_ptr->f_training_phase = FALSE;
    }

    /* Page data processing complete so dump byte count. */
    if (ctrl_blk_ptr->fa_state_trans == FA_STATE_TRANS_MSG_TRA)
    {
      DATA_MSG1_HIGH("Page Complete: Total Rx MSG bytes: %d", 
                  ctrl_blk_ptr->ui32_msg_byte_count);
      if (T31_fake_train_cnt > 0)
      {
        DATA_MSG1_HIGH("Page Complete: Fake Train bytes: %d", 
                    T31_fake_train_cnt);
      }
      if (T31_real_train_cnt > 0)
      {
        DATA_MSG1_HIGH("Page Complete: Real Train bytes: %d", 
                    T31_real_train_cnt);
      }
      if (TPS_byte_cnt > 0)
      {
        DATA_MSG1_HIGH("Page Complete: TPS Rx MSG bytes: %d", 
                    TPS_byte_cnt);
      }
      if (T31_byte_cnt > 0)
      {
        DATA_MSG1_HIGH("Page Complete: T31 Rx MSG bytes: %d", 
                    T31_byte_cnt);
      }
    }
    else
    {
      DATA_MSG1_HIGH("Page Complete: Total Tx MSG bytes: %d", 
                  ctrl_blk_ptr->ui32_msg_byte_count);
      if (T31_fake_train_cnt > 0)
      {
        DATA_MSG1_HIGH("Page Complete: Fake Train bytes: %d", 
                    T31_fake_train_cnt);
      }
      if (T31_real_train_cnt > 0)
      {
        DATA_MSG1_HIGH("Page Complete: Real Train bytes: %d", 
                    T31_real_train_cnt);
      }
      if (TPS_byte_cnt > 0)
      {
        DATA_MSG1_HIGH("Page Complete: TPS Tx MSG bytes: %d", 
                    TPS_byte_cnt);
      }
      if (T31_byte_cnt > 0)
      {
        DATA_MSG1_HIGH("Page Complete: T31 Tx MSG bytes: %d", 
                    T31_byte_cnt);
      }
    }
  }

  if (new_state != FA_STATE_TRANS_IDLE)
  {
    /* Using OTA rate, determine number of STATUS frames per BCS byte. */
    ret_val = set_num_repeated_status_frames(ctrl_blk_ptr);

    /* Using OTA rate, determine number of SYNC frames per 20ms slot. */
    if (ret_val == DS_FA_SUCCESS)
    {
      ret_val = set_num_repeated_sync_frames(ctrl_blk_ptr);
    }
  }

  /* Handle special processing upon state entry, if there is any. */
  switch (new_state)
  {
    case FA_STATE_TRANS_IDLE:
      ret_val = DS_FA_SWITCHING_TO_IDLE;
      break;

    case FA_STATE_TRANS_BCS_TRA:
      /* Just entering state, so we haven't tossed any bytes yet. */
      ctrl_blk_ptr->ui16_bytes_tossed = 0;
      break;

    case FA_STATE_TRANS_BCS_REC:
      /* Start off thinking remote FA is not ready for our frames yet. */
      ctrl_blk_ptr->f_remote_fa_in_bcs_tra = FALSE;
      break;

    case FA_STATE_TRANS_MSG_REC:
      if (ctrl_blk_ptr->i16_bytes_between_sync_frames == UNKNOWN_DATA_PER_SYNC)
      {
        /* Either we got a corrupt DCS, or we never saw a DCS.  Now we're
        ** entering the MSG phase, and we don't know the fax rate.  This could
        ** be considered fatal.  Let's just assume it's not a goofy rate. */
        DATA_MSG0_HIGH("Unknown fax rate! Assume standard!");
        ctrl_blk_ptr->i16_bytes_between_sync_frames = DATA_FRAMES_ONLY;
      }

      /* Reset counter for each transmitted fax page. */
      ctrl_blk_ptr->i16_bytes_to_next_sync =
                    ctrl_blk_ptr->i16_bytes_between_sync_frames;

      if (ctrl_blk_ptr->f_training_phase == FALSE) 
      {
        DATA_MSG0_HIGH("Transmitting fax PAGE Data");
      }
      else
      {
        DATA_MSG0_HIGH("Transmitting fax TRAINING Data");
      }
      /* Tell the remote FA we changed states */
      (void) send_status_frames(INFO_FIELD_IDLE, 
                                ctrl_blk_ptr->ui16_frames_per_bcs_byte, 
                                FA_STATE_TRANS_MSG_REC, TRUE, ctrl_blk_ptr);

      /* Start off thinking remote FA is not ready for our DATA frames yet. */
      ctrl_blk_ptr->f_remote_fa_in_msg_tra = FALSE;

      /* Initialize debug byte counter. */
      ctrl_blk_ptr->ui32_msg_byte_count = 0;
      TPS_byte_cnt = 0;
      T31_byte_cnt = 0;
      T31_fake_train_cnt = 0;
      T31_real_train_cnt = 0;
      break;

    case FA_STATE_TRANS_MSG_TRA:
      if (ctrl_blk_ptr->i16_bytes_between_sync_frames == UNKNOWN_DATA_PER_SYNC)
      {
        /* Either we got a corrupt DCS, or we never saw a DCS.  Now we're
        ** entering the MSG phase, and we don't know the fax rate.  This could
        ** be considered fatal.  Let's just assume it's not a goofy rate. */
        DATA_MSG0_HIGH("Unknown fax rate!! Assume standard!");
        ctrl_blk_ptr->i16_bytes_between_sync_frames = DATA_FRAMES_ONLY;
      }

#ifdef DSGCSDFA_ENABLE_TRAINING_TIMER
      if (ctrl_blk_ptr->f_training_phase)
      {
        DATA_MSG1_HIGH("Start %dmsec TCF timer", TRAINING_DURATION);

        /* T.30 says training should last 1.5 +/- 10% seconds. */
        ctrl_blk_ptr->timer_type = FA_TRAINING_TIMER_TYPE;
        (void)rex_set_timer(ctrl_blk_ptr->timer_ptr, TRAINING_DURATION);
      }
#endif

      /* Tell the remote FA we changed states */
      (void) send_status_frames(INFO_FIELD_IDLE, 
                                ctrl_blk_ptr->ui16_frames_per_bcs_byte, 
                                FA_STATE_TRANS_MSG_TRA, TRUE, ctrl_blk_ptr);

      /* Just starting to receive higher speed page or training data.
      ** Can't get back-to-back SYNC yet. */
      ctrl_blk_ptr->f_got_sync_last_time = FALSE;

      /* Initialize debug byte counter. */
      ctrl_blk_ptr->ui32_msg_byte_count = 0;
      TPS_byte_cnt = 0;
      T31_byte_cnt = 0;
      T31_fake_train_cnt = 0;
      T31_real_train_cnt = 0;
      break;

    default:
      /* Corrupt state!  Very, very bad. */
      DATA_MSG0_ERROR("Bad fax adapter state!");
      break;
  }

  /* Finally change to new state. */
  ctrl_blk_ptr->fa_state_trans = new_state;

  return ret_val;
} /* change_state_to */

/*========================================================================
  Function: fa_trans_uplink__invalid_data_for_state

  Description:
    This function is called if we get uplink data (BSC or MSG) from T31
    that is incompatible with the current transparent Fax Adaptor state.
    In theory this function will never run.  If it does, it means T31 and
    Fax Adaptation state machines are out of sync.

  Dependencies:
    None.

  Parameters:
    fax_uplink_data - Pointer to DSM item containing fax data, BCS or MSG.
    ctrl_blk_ptr    - Transparent FA control block pointer.  Unused.

  Return Value:
    ds_fa_status_enum_type - Always returns DS_FA_INVALID_DATA_TYPE because
                             it's bad when this function gets called.

  Side Effects:
    Frees up passed-in DSM item(s).
========================================================================*/
/* ARGSUSED (lint) */
static ds_fa_status_enum_type fa_trans_uplink__invalid_data_for_state
(
  dsm_item_type                 *fax_uplink_data,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  DATA_MSG1_HIGH("Invalid T31 data in state: state_num=%d", 
              ctrl_blk_ptr->fa_state_trans);

  dsm_free_packet(&fax_uplink_data);

  return DS_FA_INVALID_DATA_TYPE;
} /* fa_trans_uplink__invalid_data_for_state */


/*===========================================================================
** The following 5 functions handle periodic uplink processing when there
** is no new data received from T31 to transmit on the uplink.  Even though
** there is no new data, each state has unique processing to be performed.
===========================================================================*/

/*========================================================================
  Function: fa_trans_uplink__idle__no_data

  Description:
    fa_state_trans is FA_STATE_TRANS_IDLE.
    We have been called to do uplink processing.
    There is no new data from T31 (or the DTE) to process.

    Our only goal in this situation is to fill up the reverse link with
    SYNC frames.  So generate one and give it to the GCSD Transport Protocol
    State to go to the remote Fax Adaptor.  T31 will keep calling us if
    we need to generate more.

  Dependencies:
    None.

  Parameters:
    fax_uplink_data - This should always be NULL and is not used.
    ctrl_blk_ptr    - Transparent Fax Adaptor control block pointer.  Unused.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS - Function completed successfully.
      DS_FA_FAILURE - Fatal error.  Call should be aborted.

  Side Effects:
    Sends a SYNC frame to the remote Fax Adaptor.
========================================================================*/
/* ARGSUSED (lint) */
static ds_fa_status_enum_type fa_trans_uplink__idle__no_data
(
  dsm_item_type                 *fax_uplink_data,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  ds_fa_status_enum_type  ret_val = DS_FA_SUCCESS;


  /* If a disconnect is pending we don't need to send any more SYNC frames
  ** since the call is in the process of terminating.
  */
  if (ctrl_blk_ptr->dcn_pending == FALSE)
  {
    ret_val = send_sync_frames(ctrl_blk_ptr->num_sync_frames, ctrl_blk_ptr, FALSE);
  }

  return(ret_val);
} /* fa_trans_uplink__idle__no_data */


/*========================================================================
  Function: fa_trans_uplink__bcs_rec__no_data

  Description:
    fa_state_trans is FA_STATE_TRANS_BCS_REC.
    We have been called to do uplink processing.
    There is no new data from T31 (or the DTE) to process.

    In the BCS_REC state, we receive BCS data from T31 and send it to
    the remote FA.  However, T31 didn't give us anything, so there is
    nothing to do.

  Dependencies:
    None.

  Parameters:
    fax_uplink_data - This should always be NULL and is not used.
    ctrl_blk_ptr    - Transparent Fax Adaptor control block pointer.  Unused.

  Return Value:
    ds_fa_status_enum_type - Always returns DS_FA_UPLINK_COMPLETE because we
                             need BCS data in order to generate any uplink
                             data.  But we didn't get any, so we're hosed.

  Side Effects:
    None.
========================================================================*/
/* ARGSUSED (lint) */
static ds_fa_status_enum_type fa_trans_uplink__bcs_rec__no_data
(
  dsm_item_type                 *fax_uplink_data,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  return DS_FA_UPLINK_COMPLETE;
} /* fa_trans_uplink__bcs_rec__no_data */


/*========================================================================
  Function: fa_trans_uplink__bcs_tra__no_data

  Description:
    fa_state_trans is FA_STATE_TRANS_BCS_TRA.
    We have been called to do uplink processing.
    There is no new data from T31 (or the DTE) to process.

    Our only goal in this situation is to fill up the reverse link with
    SYNC frames.  So generate one and give it to the GCSD Transport Protocol
    State to go to the remote Fax Adaptor.  T31 will keep calling us if
    we need to generate more.

  Dependencies:
    None.

  Parameters:
    fax_uplink_data - This should always be NULL and is not used.
    ctrl_blk_ptr    - Transparent Fax Adaptor control block pointer.  Unused.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS - Function completed successfully.
      DS_FA_FAILURE - Fatal error.  Call should be aborted.

  Side Effects:
    Sends a SYNC frame to the remote Fax Adaptor.
========================================================================*/
/* ARGSUSED (lint) */
static ds_fa_status_enum_type fa_trans_uplink__bcs_tra__no_data
(
  dsm_item_type                 *fax_uplink_data,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  return(send_sync_frames(ctrl_blk_ptr->num_sync_frames, ctrl_blk_ptr, FALSE));
} /* fa_trans_uplink__bcs_tra__no_data */


/*========================================================================
  Function: fa_trans_uplink__msg_tra__no_data

  Description:
    fa_state_trans is FA_STATE_TRANS_MSG_TRA.
    We have been called to do uplink processing.
    There is no new data from T31 (or the DTE) to process.

    Depending upon where we're at in the fax protocol, we either want to
    fill up the reverse link with all SYNC frames or with alternating
    SYNC and STATUS frames.

    GSM 03.45, 5.2.3.5 describes this state in the fax reception process.
    Upon entry into this state, we start off alternating SYNC and STATUS
    frames.  Then once we the first DATA frame (meaning loss of
    synchronization) from the remote FA, we switch to all SYNC frames.
    EXCEPT if this is the TCF (training) phase in the protocol.  Then we
    skip that first stuff and go straight to all SYNC frames.

  Dependencies:
    None.

  Parameters:
    fax_uplink_data - This should always be NULL and is not used.
    ctrl_blk_ptr    - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS - Function completed successfully.
      DS_FA_FAILURE - Fatal error.  Call should be aborted.

  Side Effects:
    Sends a SYNC or STATUS frame to the remote Fax Adaptor.
========================================================================*/
/* ARGSUSED (lint) */
static ds_fa_status_enum_type fa_trans_uplink__msg_tra__no_data
(
  dsm_item_type                 *fax_uplink_data,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  ds_fa_status_enum_type  ret_val = DS_FA_SUCCESS;


  if (!ctrl_blk_ptr->f_synchronized || ctrl_blk_ptr->f_training_phase)
  {
    /* One of two things gets us to this point:
    ** 1) We are in page data mode of the fax protocol, and we have received
    **    at least one DATA frame with page data from the remote FA,
    **    meaning we are NOT synchronized.
    ** 2) We are in training mode, in which case we don't need to receive at
    **    least one DATA frame before entering this phase of the protocol.
    **    (If previous state was BCS_TRA, we just received the DCS frame,
    **     meaning now we're receiving training.)
    **
    ** Just sending SYNC frames to remote FA. */

    ret_val = send_sync_frames(ctrl_blk_ptr->num_sync_frames, ctrl_blk_ptr, FALSE);
  }
  else
  {
    DATA_MSG0_LOW("FA: now in MSG_TRA, notify remote FA");

    /* Send alternating SYNC and STATUS frames to the remote FA. */
    ret_val = send_status_frames(INFO_FIELD_IDLE, 
                                ctrl_blk_ptr->ui16_frames_per_bcs_byte, 
                                FA_STATE_TRANS_MSG_TRA, TRUE, ctrl_blk_ptr);
  }

  return ret_val;
} /* fa_trans_uplink__msg_tra__no_data */


/*===========================================================================
** The following 2 functions handle the processing of BCS data received
** from T31, in the FA_STATE_TRANS_IDLE and FA_STATE_TRANS_BCS_REC states.
===========================================================================*/

/*========================================================================
  Function: fa_trans_uplink__idle__bcs_data

  Description:
    fa_state_trans is FA_STATE_TRANS_IDLE.
    We have been called to do uplink processing.
    We have new BCS data from T31 (or the DTE) to process.

    We are idle and just got some uplink BCS data from the attached DTE.
    Time for a state change to FA_STATE_TRANS_BCS_REC to process the data.

  Dependencies:
    None.

  Parameters:
    fax_uplink_data - DSM item with new BCS uplink data.
    ctrl_blk_ptr    - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS - Function completed successfully.
      DS_FA_FAILURE - Fatal error.  Call should be aborted.

  Side Effects:
    State change.
========================================================================*/
static ds_fa_status_enum_type fa_trans_uplink__idle__bcs_data
(
  dsm_item_type                 *fax_uplink_data,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  /* Make sure T31 is not pulling a nasty. */
  ASSERT(fax_uplink_data != NULL);

  return change_state_to(FA_STATE_TRANS_BCS_REC, ctrl_blk_ptr);
} /* fa_trans_uplink__idle__bcs_data */


/*========================================================================
  Function: fa_trans_uplink__bcs_rec__bcs_data

  Description:
    fa_state_trans is FA_STATE_TRANS_BCS_REC.
    We have been called to do uplink processing.
    We have new BCS data from T31 (or the DTE) to process.

    In the BCS_REC state, we receive BCS data from T31 and send it to
    the remote FA.  We must do lots of data duplication to get a 300 bps
    stream to fill up the RF.

  Dependencies:
    Bearer Services Access Rate.

  Parameters:
    fax_uplink_data - DSM item with new BCS uplink data.
    ctrl_blk_ptr    - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS - Function completed successfully.
      DS_FA_FAILURE - Fatal error.  Call should be aborted.

  Side Effects:
    Sends BCS data down to GCSD Transport Protocol Stack.
========================================================================*/
static ds_fa_status_enum_type fa_trans_uplink__bcs_rec__bcs_data
(
  dsm_item_type                       *fax_uplink_data,
  fa_trans_ctrl_blk_struct_type       *ctrl_blk_ptr
)
{
  int16                   i16_bcs_byte;
  uint16                  ui16_num_frames =
                          ctrl_blk_ptr->ui16_frames_per_bcs_byte;
  ds_fa_status_enum_type  ret_val = DS_FA_SUCCESS;


  /* Make sure T31 is not pulling a nasty. */
  ASSERT(fax_uplink_data != NULL);

  DATA_MSG1_LOW("Sending %d BCS bytes", dsm_length_packet(fax_uplink_data));
  if( ctrl_blk_ptr->uplink_wm_ptr )
  {
    FA_MSG_LOW("TPS-UL:Qcnt=%d Qhi=%d Qlo=%d", 
               ctrl_blk_ptr->uplink_wm_ptr->current_cnt,
               ctrl_blk_ptr->uplink_wm_ptr->hi_watermark,
               ctrl_blk_ptr->uplink_wm_ptr->lo_watermark);
  }
  
#ifdef FA_TRANS_DEBUG
  /* Total, hacky debug code.  If we get BCS data with a certain string,
  ** generate lots of bad data to force loss of synchronization. */
  if (memcmp(fax_uplink_data->data_ptr, "Lose Sync Now", 13) == 0)
  {
    dsm_free_packet(&fax_uplink_data);

    for (i16_bcs_byte = 0; i16_bcs_byte < 5; i16_bcs_byte++)
    {
      fax_uplink_data = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
      if (fax_uplink_data == NULL)
      {
        DS_3GPP_MSG0_ERROR("DSM item pool exhausted");
      }
      else
      {
        memset(fax_uplink_data->data_ptr, 0x00, fax_uplink_data->size);
        fax_uplink_data->used = fax_uplink_data->size;
        DSM_ENQUEUE(ctrl_blk_ptr->uplink_wm_ptr, &fax_uplink_data);
      }
    }
  }
#endif

  while ((ret_val == DS_FA_SUCCESS) &&
         ((i16_bcs_byte = dsm_pull8(&fax_uplink_data)) != -1))
  {
    DATA_MSG1_LOW("FA-BCS: data=0x%x", i16_bcs_byte);

    /* OK, for each byte in the DSM item received from T31, send the
    ** calculated number of 8-byte STATUS frames. */
    ret_val = send_status_frames((byte)i16_bcs_byte, ui16_num_frames,
                                 FA_STATE_TRANS_BCS_REC, FALSE, ctrl_blk_ptr);
  }

  /* Don't need original DSM item anymore.  If no errors occurred, DSM
  ** packet has already been freed.  If there were errors, toss the data. */
  dsm_free_packet(&fax_uplink_data);

  return ret_val;
} /* fa_trans_uplink__bcs_rec__bcs_data */


/*===========================================================================
** The following 2 functions handle the processing of MSG data received
** from T31, in the FA_STATE_TRANS_IDLE and FA_STATE_TRANS_MSG_REC states.
===========================================================================*/

/*========================================================================
  Function: fa_trans_uplink__idle__msg_data

  Description:
    fa_state_trans is FA_STATE_TRANS_IDLE.
    We have been called to do uplink processing.
    We have new MSG data from T31 (or the DTE) to process.

    We are idle and just got some uplink MSG data from the attached DTE.
    Time for a state change to FA_STATE_TRANS_MSG_REC to process the data.

  Dependencies:
    None.

  Parameters:
    fax_uplink_data - DSM item with new MSG uplink data.
    ctrl_blk_ptr    - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS - Function never fails.

  Side Effects:
    State change.
========================================================================*/
static ds_fa_status_enum_type fa_trans_uplink__idle__msg_data
(
  dsm_item_type                 *fax_uplink_data,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  /* Make sure T31 is not pulling a nasty. */
  ASSERT(fax_uplink_data != NULL);

  /* Send alternating SYNC and STATUS instead of DATA frames for now. */
  (void) send_status_frames(INFO_FIELD_IDLE, 
                            ctrl_blk_ptr->ui16_frames_per_bcs_byte, 
                            FA_STATE_TRANS_MSG_REC, TRUE, ctrl_blk_ptr);

  return change_state_to(FA_STATE_TRANS_MSG_REC, ctrl_blk_ptr);
} /* fa_trans_uplink__idle__msg_data */


/*========================================================================
  Function: fa_trans_uplink__msg_rec__no_or_msg_data

  Description:
    fa_state_trans is FA_STATE_TRANS_MSG_REC.
    We have been called to do uplink processing.
    We may or may not have new MSG data from T31 (or the DTE) to process.

    We are currently receiving higher speed MSG (training or page) data from
    T31 for uplink transmission to the remote FA.  If it's training data, or
    if the remote FA is ready, we can pass it down to GCSD Transport Protocol
    Stack during this function call.  If not, we gotta queue it up for later.
    All queued data is discarded if T31 calls return_to_idle() before we get
    to transmit the data.  T31 should only do this during error situations,
    like radio link failure, etc.

    Also for the more standard fax data rates (14400, 9600, 4800, and 2400
    bps), we can just shoot the DSM item on through untouched to the GCSD
    Transport Protocol Stack.  7200 and 12000 bps require special handling
    because we gotta send a SYNC frame every so often with the DATA frames.
    GSM 03.45, Table 1a states:
      12 kbps  - 1 SYNC frame for every 5 DATA frames.
      7.2 kbps - 1 SYNC frame for every 3 DATA frames.

  Dependencies:
    Whether or not we're allowed to send DATA frames to remote FA yet.

  Parameters:
    fax_uplink_data - DSM item with new MSG uplink data.  NULL is none.
    ctrl_blk_ptr    - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_UPLINK_COMPLETE    - Function completed successfully, AND there's
                                 no use calling us again with no data to
                                 generate more uplink data.  We are in a
                                 state where we send DATA frames, and we
                                 need data to do that.
      DS_FA_SUCCESS            - Function completed successfully, AND T31 may
                                 call us again to generate more uplink data
                                 to fill the channel, if necessary.
      DS_FA_UPLINK_DATA_QUEUED - Same as DS_FA_SUCCESS, AND we were unable
                                 to send all uplink data to the remote FA.
                                 Remote FA is not accepting DATA frames
                                 yet.  If T31 calls return_to_idle() at this
                                 point, all queued data is freed and is never
                                 transmitted to the remote FA.
      DS_FA_FAILURE            - Fatal error.  Call should be aborted.

  Side Effects:
    Possibly passes data down stack to GCSD Transport Protocol Stack.
========================================================================*/
static ds_fa_status_enum_type fa_trans_uplink__msg_rec__no_or_msg_data
(
  dsm_item_type                 *fax_uplink_data,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  uint16                  ui16_num_bytes;
  uint16                  ui16_dsm_length = 0;
  dsm_item_type          *new_dsm_ptr = NULL;
  ds_fa_status_enum_type  ret_val;


  if (ctrl_blk_ptr->f_remote_fa_in_msg_tra || ctrl_blk_ptr->f_training_phase)
  {
    /* Remote FA is in MSG_TRA state, OR we are receiving training from
    ** the attached DTE.  We want to send DATA frames to remote FA now. */

    /* Start off assuming success. */
    ret_val = DS_FA_UPLINK_COMPLETE;

    if (ctrl_blk_ptr->uplink_buf_dsm_ptr != NULL)
    {
      /* This kinda sucks.  We are finally allowed to send DATA frames, but
      ** we have some data buffered up that we must process before we can
      ** process the data we just got.  So throw the new stuff onto the end
      ** of the old stuff. */
      dsm_append(&ctrl_blk_ptr->uplink_buf_dsm_ptr, &fax_uplink_data);
      fax_uplink_data = ctrl_blk_ptr->uplink_buf_dsm_ptr;
      ctrl_blk_ptr->uplink_buf_dsm_ptr = NULL;
    }

    /* If we got no data, we got nothing to do. */
    if (fax_uplink_data != NULL)
    {
      if (ctrl_blk_ptr->i16_bytes_between_sync_frames == DATA_FRAMES_ONLY)
      {
        /* This means the fax rate is neither 7,200 nor 12,000 bps.  So we
        ** don't add SYNC frames to the DATA.  Just send to GCSD-TPS. */

        /* Keeping byte count, just for fun. */
        ui16_dsm_length = (uint16)dsm_length_packet(fax_uplink_data);
        ctrl_blk_ptr->ui32_msg_byte_count += ui16_dsm_length;
        T31_byte_cnt += ui16_dsm_length;
        if (ctrl_blk_ptr->f_training_phase)
        {
          T31_real_train_cnt += ui16_dsm_length;
        }

        DATA_MSG1_LOW("%d DATA bytes to TPS", ui16_dsm_length);

        DSM_ENQUEUE(ctrl_blk_ptr->uplink_wm_ptr, &fax_uplink_data);
        if( ctrl_blk_ptr->uplink_wm_ptr )
        {
          FA_MSG_LOW("TPS-UL:Qcnt=%d Qhi=%d Qlo=%d", 
                     ctrl_blk_ptr->uplink_wm_ptr->current_cnt,
                     ctrl_blk_ptr->uplink_wm_ptr->hi_watermark,
                     ctrl_blk_ptr->uplink_wm_ptr->lo_watermark);
        }
      }
      else
      {
        /* This means we are using a goofy fax rate.  At these messed-up rates,
        ** we intersperse SYNC frames with the DATA frames.  At 7,200 bps,
        ** 1 SYNC per 3 DATA.  At 12,000 bps, 1 SYNC per 5 DATA. */

        /* Keep spinning while we have no errors.  We also leave when
        ** we run out of data to mess with. */
        while (ret_val == DS_FA_UPLINK_COMPLETE)
        {
          /* How much uplink MSG data do we have to deal with? */
          ui16_num_bytes = (uint16)dsm_length_packet(fax_uplink_data);
          T31_byte_cnt += ui16_dsm_length;
          if (ctrl_blk_ptr->f_training_phase)
          {
            T31_real_train_cnt += ui16_dsm_length;
          }

          if (ui16_num_bytes < ctrl_blk_ptr->i16_bytes_to_next_sync)
          {
            /* This is the easiest case.  It's not time to send SYNC yet.
            ** Just send what we got from T31 right down protocol stack. */

            /* Keeping byte count, just for fun. */
            ctrl_blk_ptr->ui32_msg_byte_count += (uint32)ui16_num_bytes;

            DATA_MSG1_LOW("2:: %d DATA bytes to TPS", ui16_num_bytes);

            /* Decrement byte count to SYNC because we're getting closer. */
            ctrl_blk_ptr->i16_bytes_to_next_sync -= (int16)ui16_num_bytes;

            /* No SYNC yet.  Just give it to GCSD-TPS and leave. */
            DSM_ENQUEUE(ctrl_blk_ptr->uplink_wm_ptr, &fax_uplink_data);
            if( ctrl_blk_ptr->uplink_wm_ptr )
            {
              FA_MSG_LOW("TPS-UL:Qcnt=%d Qhi=%d Qlo=%d", 
                         ctrl_blk_ptr->uplink_wm_ptr->current_cnt,
                         ctrl_blk_ptr->uplink_wm_ptr->hi_watermark,
                         ctrl_blk_ptr->uplink_wm_ptr->lo_watermark);
            }
            break;
          }
          else if (ui16_num_bytes ==
                   (uint16)ctrl_blk_ptr->i16_bytes_to_next_sync)
          {
            /* This case is pretty easy too.  We got just the right number to
            ** necessitate a SYNC frame.  So send the T31 data followed by a
            ** SYNC frame right down the protocol stack. */

            /* Keeping byte count, just for fun. */
            ctrl_blk_ptr->ui32_msg_byte_count += (uint32)ui16_num_bytes;

            DATA_MSG1_LOW("%d DATA bytes AND SYNC to TPS", ui16_num_bytes);

            /* Reset count to SYNC because we are sending a SYNC. */
            ctrl_blk_ptr->i16_bytes_to_next_sync =
                          ctrl_blk_ptr->i16_bytes_between_sync_frames;

            /* Put SYNC frame in DSM item after the data. */
            if (dsm_pushdown_tail(&fax_uplink_data, 
                                  (void*)&uplink_sync_frame[0],
                                  FRAME_SIZE, DSM_DS_LARGE_ITEM_POOL) !=
                                                                    FRAME_SIZE)
            {
              /* SYNC frame didn't fit, and DSM item allocation failed. */
              ret_val = DS_FA_FAILURE;
            }

            /* Give DATA followed by SYNC to GCSD-TPS and leave. */
            DSM_ENQUEUE(ctrl_blk_ptr->uplink_wm_ptr, &fax_uplink_data);
            if( ctrl_blk_ptr->uplink_wm_ptr )
            {
              FA_MSG_LOW("TPS-UL:Qcnt=%d Qhi=%d Qlo=%d", 
                         ctrl_blk_ptr->uplink_wm_ptr->current_cnt,
                         ctrl_blk_ptr->uplink_wm_ptr->hi_watermark,
                         ctrl_blk_ptr->uplink_wm_ptr->lo_watermark);
            }
            break;
          }
          else
          {
            /* This case sucks, and we are going to jump through some hoops to
            ** avoid copying data.  We need to send some data, then a SYNC,
            ** then some more data, then maybe a SYNC, then maybe DATA, and
            ** so one... */

            /* Keeping byte count, just for fun. */
            ctrl_blk_ptr->ui32_msg_byte_count +=
                     (uint16)ctrl_blk_ptr->i16_bytes_to_next_sync;

            DATA_MSG1_LOW("%d DATA bytes AND SYNC to TPS.",
                       ctrl_blk_ptr->i16_bytes_to_next_sync);

            /* Here we get a new DSM item and copy the bytes that need to go
            ** out _before_ the SYNC frame into this DSM item. */
            if (dsm_pushdown(&new_dsm_ptr, fax_uplink_data->data_ptr,
                             (word)ctrl_blk_ptr->i16_bytes_to_next_sync,
                             DSM_DS_LARGE_ITEM_POOL))
            {
              /* Now put SYNC frame in DSM item after first chunk of data. */
              if (dsm_pushdown_tail(&new_dsm_ptr, (void*)&uplink_sync_frame[0],
                                    FRAME_SIZE, DSM_DS_LARGE_ITEM_POOL) !=
                                                                  FRAME_SIZE)
              {
                /* SYNC frame didn't fit, and DSM item allocation failed. */
                ret_val = DS_FA_FAILURE;
              }

              /* Now remove those bytes from the original DSM item. */
              ui16_num_bytes = dsm_pullup(&fax_uplink_data, NULL,
                                 (word)ctrl_blk_ptr->i16_bytes_to_next_sync);
              ASSERT(ui16_num_bytes ==
                     (word)ctrl_blk_ptr->i16_bytes_to_next_sync);

              /* Reset count to SYNC because we are sending a SYNC. */
              ctrl_blk_ptr->i16_bytes_to_next_sync =
                            ctrl_blk_ptr->i16_bytes_between_sync_frames;

              /* Send DATA and SYNC on their merry way down the stack. */
              DSM_ENQUEUE(ctrl_blk_ptr->uplink_wm_ptr, &new_dsm_ptr);
              if( ctrl_blk_ptr->uplink_wm_ptr )
              {
                FA_MSG_LOW("TPS-UL:Qcnt=%d Qhi=%d Qlo=%d", 
                           ctrl_blk_ptr->uplink_wm_ptr->current_cnt,
                           ctrl_blk_ptr->uplink_wm_ptr->hi_watermark,
                           ctrl_blk_ptr->uplink_wm_ptr->lo_watermark);
              }
            }
            else
            {
              /* DSM item allocation failure. */
              ret_val = DS_FA_FAILURE;
            }
          }
        }
      }
    }
  }
  else
  {
    uint8 pkt_buf[500];
    uint16 pkt_len = (uint16)dsm_length_packet(fax_uplink_data);

    (void)dsm_pullup(&fax_uplink_data, pkt_buf, pkt_len);

    if (dsm_pushdown_tail(&ctrl_blk_ptr->uplink_buf_dsm_ptr, pkt_buf, 
          pkt_len, DSM_DS_LARGE_ITEM_POOL) != pkt_len)
    {
      ret_val = DS_FA_FAILURE;
    }
    else
    {
      DATA_MSG1_HIGH("remote not ready, %d UL bytes pending", 
        dsm_length_packet(ctrl_blk_ptr->uplink_buf_dsm_ptr));

      /* Send alternating SYNC and STATUS instead of DATA frames for now. */
      ret_val = send_status_frames(INFO_FIELD_IDLE, 
                                   ctrl_blk_ptr->ui16_frames_per_bcs_byte, 
                                   FA_STATE_TRANS_MSG_REC,
                                   TRUE, ctrl_blk_ptr);

      /* Let T31 know if we have MSG data queued to go out or not.  This is
      ** so he can avoid calling return_to_idle() if possible.  If he does
      ** call return_to_idle() at this point, queued data is freed and lost. */
      if ((ctrl_blk_ptr->uplink_buf_dsm_ptr != NULL) &&
          (ret_val == DS_FA_SUCCESS))
      {
        ret_val = DS_FA_UPLINK_DATA_QUEUED;
      }
    }
  }

  return ret_val;
} /* fa_trans_uplink__msg_rec__no_or_msg_data */

/*===========================================================================
** The following 5 functions handle periodic downlink processing for each
** of the 5 states.
===========================================================================*/

/*========================================================================
  Function: fa_trans_downlink__idle

  Description:
    fa_state_trans is FA_STATE_TRANS_IDLE.
    We have been called to do downlink processing.

    Since we're idle, we're just looking for SYNC frames or STATUS frames.
    SYNC frames contain no useful information, so we just toss them.
    STATUS frames require additional processing, which may result in a
    state change.

  Dependencies:
    None.

  Parameters:
    fax_downlink_data - Not used by this function.
    fax_type_of_data  - Not used by this function.
    ctrl_blk_ptr      - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS - Function completed successfully.
      DS_FA_FAILURE - Fatal error.  Call should be aborted.

  Side Effects:
    May result in state change.
========================================================================*/
/* ARGSUSED (lint) */
static ds_fa_status_enum_type fa_trans_downlink__idle
(
  dsm_item_type                **fax_downlink_data,
  ds_fa_data_enum_type          *fax_type_of_data,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  uint8                     frame_buf[FRAME_SIZE], *frame_ptr;
  fa_state_trans_enum_type  frame_type;
  uint8                     ui8_bcs_data;
  ds_fa_status_enum_type    ret_val        = DS_FA_SUCCESS;
  dsm_item_type           **dsm_ptr        =
                            &ctrl_blk_ptr->downlink_buf_dsm_ptr;
  boolean                  *f_synchronized = &ctrl_blk_ptr->f_synchronized;


  /* Just a sanity check.  We shouldn't be here if this is NULL. */
  ASSERT(*dsm_ptr != NULL);

  /* This is the big outter processing loop.  We leave once we no longer have
  ** a frame's worth of data to look at, or if we leave the IDLE state. */
  while (ctrl_blk_ptr->fa_state_trans == FA_STATE_TRANS_IDLE)
  {
    if (!*f_synchronized)
    {
      /* We ain't synchronized yet, so give it a shot. */
      *f_synchronized = look_for_sync_frame(dsm_ptr, ctrl_blk_ptr);
      if (*f_synchronized)
      {
        DATA_MSG0_HIGH("NOW synchronized in IDLE");
      }
    }

    /* If the above look_for_sync_frame() call failed, we will not have a
    ** frame's worth of data to look at, and this extract will also fail. */
    frame_ptr = &frame_buf[0];
    if (better_dsm_extract(*dsm_ptr, &frame_ptr, FRAME_SIZE) == FRAME_SIZE)
    {
      if (is_it_sync_frame(frame_ptr, ctrl_blk_ptr))
      {
        if ((ctrl_blk_ptr->f_send_connect == TRUE) &&
            (ctrl_blk_ptr->f_call_is_MO == FALSE) &&
            (ctrl_blk_ptr->sync_hysterisis_cnt++ == FA_SYNC_FRAMES_HYSTERISIS))
        {
          DATA_MSG0_HIGH("FA in SYNC with REMOTE FA");
          DATA_MSG1_HIGH("delay %dms for CED processing",CED_TONE_DELAY);

          ctrl_blk_ptr->f_send_connect = FALSE;
          ctrl_blk_ptr->timer_type = FA_CED_TIMER_TYPE;
          (void)rex_set_timer(ctrl_blk_ptr->timer_ptr, CED_TONE_DELAY);
          ctrl_blk_ptr->sync_hysterisis_cnt = 0;
        }
      }
      else if (is_it_status_frame(frame_ptr, &frame_type, &ui8_bcs_data))
      {
        /* We found a STATUS frame.  Dig deeper for more info. */
	DATA_MSG1_HIGH("Remote FA in %d",frame_type);
		
        switch (frame_type)
        {
          case FA_STATE_TRANS_BCS_TRA:
            /* Remote FA is in BCS_TRA state.  That is our signal to go to
            ** BCS_REC.  We can now send data to the remote FA */
            FA_FRAME_DUMP(frame_ptr, FALSE);  /*lint !e506 !e774*/

            ctrl_blk_ptr->f_remote_fa_in_bcs_tra = TRUE;
            ret_val = change_state_to(FA_STATE_TRANS_BCS_REC, ctrl_blk_ptr);
            break;

          case FA_STATE_TRANS_BCS_REC:
            /* Remote FA is in BCS_REC state.  That is our signal to go to
            ** BCS_TRA.  We are now receiving BCS data from remote FA. */
            FA_FRAME_DUMP(frame_ptr, FALSE);  /*lint !e506 !e774*/

            /* Do we need to send a CONNECT? */
            if (ctrl_blk_ptr->f_send_connect == TRUE)
            {
              DATA_MSG0_HIGH("Remote FA in BCS_REC: send CONNECT");
              ds_gcsd_report_fax_connect( ctrl_blk_ptr->call_id );
              ctrl_blk_ptr->f_send_connect = FALSE;
            }
            ret_val = change_state_to(FA_STATE_TRANS_BCS_TRA, ctrl_blk_ptr);
            break;

          case FA_STATE_TRANS_MSG_REC:
            /* Remote FA is in MSG_REC state.  That is our signal to go to
            ** MSG_TRA.  We will soon receive MSG data from remote FA. */
            ret_val = change_state_to(FA_STATE_TRANS_MSG_TRA, ctrl_blk_ptr);
            break;

          case FA_STATE_TRANS_MSG_TRA:
            /* This could happen if the remote FA has waited the 75 msec
            ** between DCS and TCF and has transitioned to MSG_TRA without
            ** waiting for local FA to enter MSG_REC.  The attached DTE
            ** should give us TCF soon, so we should be making the
            ** transition soon.  At any rate, no action necessary. */
            break;

          default:
            /* This ain't great, but nothing really can be done about it. */
            DATA_MSG1_HIGH("In IDLE got bad remote FA STATUS: frame_type=%d",
                        frame_type);
            break;
        } /* switch */
      }
      else
      {
        /* Crap!  It's neither SYNC nor STATUS frame.  Gotta start over. */
        DATA_MSG0_HIGH("SYNC loss in IDLE");
        FA_FRAME_DUMP(frame_ptr, TRUE);  /*lint !e506 !e774*/
        *f_synchronized = FALSE;

        if ((ctrl_blk_ptr->f_send_connect == TRUE) &&
            (ctrl_blk_ptr->f_call_is_MO == FALSE))
        {
          DATA_MSG0_HIGH("Resetting SYNC frame hysterisis");
          ctrl_blk_ptr->sync_hysterisis_cnt = 0;
        }
      }

      if (*f_synchronized &&
          (ctrl_blk_ptr->fa_state_trans != FA_STATE_TRANS_BCS_TRA))
      {
        /* Only toss frame if we're still synch'ed up, AND we didn't change
        ** states to BCS_TRA.  If we're not synch'ed, keep data around to
        ** search for SYNC frame again.  If we're in BCS_TRA, keep data
        ** around because that state will pull a BCS data byte from frame. */
        (void)dsm_pullup(dsm_ptr, NULL, FRAME_SIZE);
      }
    }
    else
    {
      /* We may or may not be synchronized at this point, but we don't have
      ** enough data to play with either way.  So get outta here. */
      break;
    }
  }

  return ret_val;
} /* fa_trans_downlink__idle */


/*========================================================================
  Function: fa_trans_downlink__bcs_rec

  Description:
    fa_state_trans is FA_STATE_TRANS_BCS_REC.
    We have been called to do downlink processing.

    In the BCS_REC state, we are receiving BCS data from the attached DTE
    and sending it to the remote FA.  So we don't expect anything on the
    downlink FROM the remote FA, just SYNC frames.  So let's go ahead and
    look for the SYNC frames, and throw them away as we find them.  This
    function really need not do anything at all, but let's try to remain
    synchronized with the incoming data stream.

  Dependencies:
    None.

  Parameters:
    fax_downlink_data - Not used by this function.
    fax_type_of_data  - Not used by this function.
    ctrl_blk_ptr      - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type - Always returns DS_FA_SUCCESS.

  Side Effects:
    May result in synchronization loss.
========================================================================*/
/* ARGSUSED (lint) */
static ds_fa_status_enum_type fa_trans_downlink__bcs_rec
(
  dsm_item_type                **fax_downlink_data,
  ds_fa_data_enum_type          *fax_type_of_data,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  uint8            frame_buf[FRAME_SIZE], *frame_ptr;
  uint16           cnt;
  dsm_item_type  **dsm_ptr        = &ctrl_blk_ptr->downlink_buf_dsm_ptr;
  boolean         *f_synchronized = &ctrl_blk_ptr->f_synchronized;


  /* Just a sanity check.  We shouldn't be here if this is NULL. */
  ASSERT(*dsm_ptr != NULL);

  /* This is the big outter processing loop.  We will break out once we no
  ** longer have a frame's worth of data to look at. */
  while (TRUE)  /*lint !e716*/
  {
    if (!*f_synchronized)
    {
      /* We ain't synchronized yet, so give it a shot. */
      *f_synchronized = look_for_sync_frame(dsm_ptr, ctrl_blk_ptr);
    }

    /* If the above look_for_sync_frame() call failed, we will not have a
    ** frame's worth of data to look at, and this extract will also fail. */
    frame_ptr = &frame_buf[0];
    cnt = better_dsm_extract(*dsm_ptr, &frame_ptr, FRAME_SIZE);
    if (cnt  < FRAME_SIZE)
    {
      /* We may or may not be synchronized at this point, but we don't have
      ** enough data to play with either way.  So get outta here. */
      FA_FRAME_DUMP(frame_ptr, FALSE);  /*lint !e506 !e774*/
      DATA_MSG1_LOW("Incomplete frame, bytes=%d!", cnt);
      break;
    }

    if (is_it_sync_frame(frame_ptr, ctrl_blk_ptr))
    {
      /* It's a SYNC frame.  It has no valuable info, so toss it. */
      (void)dsm_pullup(dsm_ptr, NULL, FRAME_SIZE);
    }
    else
    {
      /* Bummer!  We're only expecting SYNC frames at this point. */
      DATA_MSG0_HIGH("SYNC lost in BCS_REC downlink");
      FA_FRAME_DUMP(frame_ptr, TRUE);  /*lint !e506 !e774*/
      *f_synchronized = FALSE;
    }
  }

  /* This baby never fails and never returns any data. */
  return DS_FA_SUCCESS;
} /* fa_trans_downlink__bcs_rec */


/*========================================================================
  Function: fa_trans_downlink__bcs_tra

  Description:
    fa_state_trans is FA_STATE_TRANS_BCS_TRA.
    We have been called to do downlink processing.

    In the BCS_TRA state, we are receiving BCS data from the remote FA and
    sending it to the attached DTE.  The BCS data comes in STATUS frames,
    and there may be multiple STATUS frames per BCS byte.  So do all the
    math and stuff necessary to regenerate the BCS bytes and pass them
    back to T31.

  Dependencies:
    Bearer Services Access Rate.

  Parameters:
    fax_downlink_data - Return BCS data to T31, if we got some.
    fax_type_of_data with values:
      - DS_FA_DATA_NONE if no data to return.
      - DS_FA_DATA_BCS if we got some BCS data from the remote FA.
    ctrl_blk_ptr      - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS - Function completed successfully.
      DS_FA_FAILURE - Fatal error.  Call should be aborted.

  Side Effects:
    May result in synchronization loss.
========================================================================*/
static ds_fa_status_enum_type fa_trans_downlink__bcs_tra
(
  dsm_item_type                **fax_downlink_data,
  ds_fa_data_enum_type          *fax_type_of_data,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  uint16                    ui16_bytes_per_bcs_byte;
  uint16                    cnt;
  uint16                    resync_cnt = 0;
  uint8                     frame_buf[FRAME_SIZE], *frame_ptr, ui8_bcs_data;
  fa_state_trans_enum_type  frame_type;
  ds_fa_status_enum_type    ret_val        = DS_FA_SUCCESS;
  dsm_item_type           **dsm_ptr        =
                            &ctrl_blk_ptr->downlink_buf_dsm_ptr;
  boolean                  *f_synchronized = &ctrl_blk_ptr->f_synchronized;


  /* Just a sanity check.  We shouldn't be here if this is NULL. */
  ASSERT(*dsm_ptr != NULL);

  /* Given the OTA rate and the number of bytes in a frame, calculate
  ** the number of OTA bytes we're gonna need to have just one BCS byte. */
  ui16_bytes_per_bcs_byte = (uint16)(ctrl_blk_ptr->ui16_frames_per_bcs_byte *
                                     FRAME_SIZE);

  /* Stay in this loop while we haven't changed state, AND no errors so far.
  ** Also leaves if we run out of data. */
  while (TRUE) /*lint !e716*/
  {
    /* Use local buffer to synchronize with */
    frame_ptr = &frame_buf[0];

    if (!*f_synchronized)
    {
      /* If we lost SYNC and the DCS timer is set, then we might already
      ** be receiving training data so allow a return to IDLE and wait 
      ** for T31 to invoke downlink processing again to complete the state
      ** change.
      */
      if ((ctrl_blk_ptr->timer_type == FA_DCS_TIMER_TYPE) ||
          (ctrl_blk_ptr->f_start_dcs_timer == TRUE))
      {
        if (ctrl_blk_ptr->f_start_dcs_timer == TRUE)
        {
          /* Here we are supposed to start the DCS timer but we are no longer
          ** synchronized.  This is likely because we are already receiving
          ** training data early.  To deal with this condition we will simply
          ** start a shorted DCS timer which when it expires will transition
          ** us to the MSG_TRA state to start receiving the training data.
          */
          DATA_MSG1_HIGH("Start short %dmsec DCS->TCF timer",
            DCS_TO_TRAIN_SHORT_DELAY);
  
          ctrl_blk_ptr->f_start_dcs_timer = FALSE;
          ctrl_blk_ptr->timer_type = FA_DCS_TIMER_TYPE;
          (void)rex_set_timer(ctrl_blk_ptr->timer_ptr, DCS_TO_TRAIN_SHORT_DELAY);
        }

        /* Goto the IDLE state and let the DCS timer expire */
        ctrl_blk_ptr->f_goto_idle = TRUE;
        return (DS_FA_SUCCESS);
      }

      /* We ain't synchronized yet, so give it a shot. */
      ret_val = look_for_sync_or_status_frame(dsm_ptr, fax_downlink_data,
                                              f_synchronized, ctrl_blk_ptr);
      if ((ret_val != DS_FA_SUCCESS) || (!*f_synchronized))
      {
        break;
      }
    }

    /* We shouldn't be synchronized yet unless the above search happened to be
    ** successful.  However, if it was successful, we don't need to extract any
    ** data from the buffer.  We use what was alread found to determine how to
    ** proceeed.
    */
    cnt = better_dsm_extract(*dsm_ptr, &frame_ptr, FRAME_SIZE);
    if (cnt  < FRAME_SIZE)
    {
      /* We may or may not be synchronized at this point, but we don't have
      ** enough data to play with either way.  So get outta here.
      */
      FA_FRAME_DUMP(frame_ptr, FALSE);  /*lint !e506 !e774*/
      DATA_MSG1_LOW("Incomplete frame, bytes=%d!", cnt);
      break;
    }

    if (is_it_sync_frame(frame_ptr, ctrl_blk_ptr))
    {
      /* SYNC frame means remote FA is done sending us BCS data.  Go back
      ** to IDLE and throw away the frame. */
      DATA_MSG0_HIGH("going IDLE in BCS_TRA");

      /* If we are currently in the training phase we need to start the
      ** DCS->TCF timer first.
      */
      if (ctrl_blk_ptr->f_start_dcs_timer == TRUE)
      {
        DATA_MSG1_HIGH("Start %dmsec DCS->TCF timer", DCS_TO_TRAIN_DELAY);

        ctrl_blk_ptr->f_start_dcs_timer = FALSE;
        ctrl_blk_ptr->timer_type = FA_DCS_TIMER_TYPE;
        (void)rex_set_timer(ctrl_blk_ptr->timer_ptr, DCS_TO_TRAIN_DELAY);
      }

      ret_val = change_state_to(FA_STATE_TRANS_IDLE, ctrl_blk_ptr);
      (void)dsm_pullup(dsm_ptr, NULL, FRAME_SIZE);
      break;
    }

    if (is_it_status_frame(frame_ptr, &frame_type, &ui8_bcs_data))
    {
      FA_MSG_LOW("Found STATUS frame=0x%x in bcs_tra, dsmLen=%d bytes_per_bcs=%d",
                 frame_type,dsm_length_packet(*dsm_ptr), ui16_bytes_per_bcs_byte);

      if (dsm_length_packet(*dsm_ptr) >= ui16_bytes_per_bcs_byte)
      {
        /* There may be duplicate STATUS frames for just 1 BCS byte,
        ** depending upon the OTA rate.  But we don't care.  We just look
        ** at the first and toss the rest.  GSM 03.45, 4.2.2 calls this
        ** process "implementation dependent". */
        (void)dsm_pullup(dsm_ptr, NULL, ui16_bytes_per_bcs_byte);

        /* What kind of STATUS frame is this? */
        if (frame_type != FA_STATE_TRANS_BCS_REC)
        {
          /* This STATUS frame isn't the kind we expected.  We're gonna
          ** treat this similar to a loss of synchronization. */
          *f_synchronized = FALSE;

         DATA_MSG1_HIGH("Remote FA: frame_type=%d, should be: BCS_REC",
                      frame_type);

          /* Notice that since this isn't the right kind of STATUS frame,
          ** the ui8_bcs_data byte is pretty much bogus.  GSM 03.45, 5.2.3.3
          ** says to return "an estimate of the INFO octet value in the
          ** received STATUS frames" to the attached DTE.  We will do that,
          ** but trust me.  This byte is crap. */
        }

        DATA_MSG1_LOW("Putting bcs_data=0x%x into dsm_item", ui8_bcs_data);

        /* Put the one BCS byte into the DSM item. */
        if (dsm_pushdown_tail(fax_downlink_data, &ui8_bcs_data,
                              sizeof(ui8_bcs_data), DSM_DS_SMALL_ITEM_POOL) !=
            sizeof(ui8_bcs_data))
        {
          /* The DSM item was full.  Attempt to chain new one failed. */
          ret_val = DS_FA_FAILURE;
          break;
        }
        else
        {
          break;
        }
      }
      else
      {
        /* We're synchronized, which is good.  The frame we just looked at
        ** is a STATUS frame, which is also good.  But we haven't
        ** accumulated enough STATUS frames for 1 BCS byte yet.  So leave
        ** here, and try again another day. */
        break;
      }
    }
    else
    {
      /* Crap!  It's neither SYNC nor STATUS frame.  Gotta start over. */
      *f_synchronized = FALSE;

      if (resync_cnt++ != FA_RESYNC_ATTEMPTS)
      {
        DATA_MSG0_HIGH("SYNC lost in BCS_TRA downlink");
      }
      else
      {
        DATA_MSG0_HIGH("Cannot find SYNC in BCS_TRA downlink");
        ret_val = DS_FA_FAILURE;
        break;
      }

    }
  }

  /* Set return value if we're returning data to T31; */
  if (*fax_downlink_data != NULL)
  {
    *fax_type_of_data = DS_FA_DATA_BCS;

    DATA_MSG1_LOW("Returning %d BCS bytes to T31", 
      dsm_length_packet(*fax_downlink_data));
  }
 
  return ret_val;
} /* fa_trans_downlink__bcs_tra */


/*========================================================================
  Function: fa_trans_downlink__msg_rec

  Description:
    fa_state_trans is FA_STATE_TRANS_MSG_REC.
    We have been called to do downlink processing.

    In the MSG_REC state, we are receiving MSG data from the attached DTE
    and sending it to the remote FA.  So we're not really expecting to be
    receiving much on the downlink.  A received SYNC frame means the remote
    FA is currently receiving our DATA frames, and we just toss it.  A
    STATUS frame we need to look at to check the state of the remote FA.

  Dependencies:
    None.

  Parameters:
    fax_downlink_data - Not used by this function.
    fax_type_of_data  - Not used by this function.
    ctrl_blk_ptr      - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type - Always returns DS_FA_SUCCESS.

  Side Effects:
    May result in synchronization loss.
========================================================================*/
/* ARGSUSED (lint) */
static ds_fa_status_enum_type fa_trans_downlink__msg_rec
(
  dsm_item_type                **fax_downlink_data,
  ds_fa_data_enum_type          *fax_type_of_data,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  uint8                     frame_buf[FRAME_SIZE], *frame_ptr, ui8_bcs_data;
  fa_state_trans_enum_type  frame_type;
  dsm_item_type           **dsm_ptr        =
                            &ctrl_blk_ptr->downlink_buf_dsm_ptr;
  boolean                  *f_synchronized = &ctrl_blk_ptr->f_synchronized;


  /* Just a sanity check.  We shouldn't be here if this is NULL. */
  ASSERT(*dsm_ptr != NULL);

  /* We really don't stay in this loop forever.  We leave once we no longer
  ** have a frame's worth of data to inspect. */
  while (TRUE)  /*lint !e716*/
  {
    if (!*f_synchronized)
    {
      /* We ain't synchronized yet, so give it a shot. */
      *f_synchronized = look_for_sync_frame(dsm_ptr, ctrl_blk_ptr);
      if (*f_synchronized == TRUE)
      {
        DATA_MSG0_HIGH("SYNC found in MSG_REC");
      }
    }

    /* If the above look_for_sync_frame() call failed, we will not have a
    ** frame's worth of data to look at, and this extract will also fail. */
    frame_ptr = &frame_buf[0];
    if (better_dsm_extract(*dsm_ptr, &frame_ptr, FRAME_SIZE) == FRAME_SIZE)
    {
      if (!is_it_sync_frame(frame_ptr, ctrl_blk_ptr))
      {
        if (is_it_status_frame(frame_ptr, &frame_type, &ui8_bcs_data))
        {
          /* We found a STATUS frame.  Dig deeper for more info. */
          switch (frame_type)
          {
            case FA_STATE_TRANS_IDLE:
              /* Remote FA is IDLE.  No action necessary. */
              break;

            case FA_STATE_TRANS_MSG_TRA:
              /* Remote FA is in MSG_TRA state.  That means we can start
              ** sending him DATA frames on the uplink.  Set flag. */
 
              if (ctrl_blk_ptr->f_remote_fa_in_msg_tra == FALSE)
              {
                DATA_MSG0_HIGH("remote FA now in MSG_TRA");
              }
              ctrl_blk_ptr->f_remote_fa_in_msg_tra = TRUE;

              break;

            case FA_STATE_TRANS_BCS_REC:
              /* just a trailing BCS frame from the watermark, just toss it */
              DATA_MSG1_HIGH("MSG_TRA: trailing BCS frame: type=%d", frame_type);
              break;

            default:
              /* This is bad.  The received frame type doesn't correspond
              ** to what we're expecting.  Not sure what to do about it,
              ** so do nothing. */
              DATA_MSG1_HIGH("MSG_TRA: Bad remote STATUS frame: st=%d", frame_type);
              break;
          }
        }
        else
        {
          /* Crap!  It's neither SYNC nor STATUS frame.  Gotta start over. */
          DATA_MSG0_HIGH("SYNC lost in MSG_REC downlink");
          FA_FRAME_DUMP(frame_ptr, TRUE);  /*lint !e506 !e774*/
          *f_synchronized = FALSE;
        }
      }

      if (*f_synchronized)
      {
        /* Only toss frame if we're still synch'ed up.  If we're not
        ** synch'ed, keep data around to search for SYNC frame again. */
        (void)dsm_pullup(dsm_ptr, NULL, FRAME_SIZE);
      }
    }
    else
    {
      /* We may or may not be synchronized at this point, but we don't have
      ** a frame's worth of data anyway.  So get outta here. */
      break;
    }
  }

  /* This baby never returns any data and never fails. */
  return DS_FA_SUCCESS;
} /* fa_trans_downlink__msg_rec */


/*========================================================================
  Function: fa_trans_downlink__msg_tra

  Description:
    fa_state_trans is FA_STATE_TRANS_MSG_TRA.
    We have been called to do downlink processing.

    In the MSG_TRA state, we are receiving or are about to receive MSG data
    from the remote FA.  That is fax data we return to T31, destined for
    the attached DTE.  How we process incoming data in this state depends
    on if we are synchronized or not.

    If we are synchronized, meaning the last time around we successfully
    identified SYNC and/or STATUS frames from the remote FA, we keep looking
    for more SYNC and/or STATUS frames.  This means we are not getting DATA
    frames yet.

    Once we fail to recognized SYNC and/or STATUS frames in the data stream,
    we assume we're getting DATA frames.  At this point we are actually
    receiving a fax.  This data we return up to T31 for the attached DTE.

    This tricky part here is we have to inspect the data for SYNC frames.
    We need to look at every string of 8 bytes and see if it's a SYNC
    frame.  GSM 03.45 just seems to assume a SYNC frame could not appear
    normally within the fax data.  Actually it could, but I guess they felt
    those odds were negligible.

    Once we see a SYNC frame, we assume the remote FA is done sending fax
    data, and we go IDLE.  Unless the fax rate is 7,200 or 12,000 bps.  Then
    we need to see 2 SYNC frames in a row before going IDLE, because SYNC
    frames are mixed in with the DATA frames at those rates.

  Dependencies:
    None.

  Parameters:
    fax_downlink_data - Return MSG data to T31, if we got some.
    fax_type_of_data with values:
      - DS_FA_DATA_NONE if no data to return.
      - DS_FA_DATA_MSG if we got some MSG data from the remote FA.
    ctrl_blk_ptr      - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS - Function completed successfully.
      DS_FA_FAILURE - Fatal error.  Call should be aborted.

  Side Effects:
    May result in state change.
========================================================================*/
static ds_fa_status_enum_type fa_trans_downlink__msg_tra
(
  dsm_item_type                **fax_downlink_data,
  ds_fa_data_enum_type          *fax_type_of_data,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  uint8                     data_buf[FRAME_SIZE], *data_ptr, ui8_data;
  fa_state_trans_enum_type  frame_type;
  uint16                    bytes_available;
#ifdef DSGCSDFA_ENABLE_FAKE_TRAINING
  uint16                    ui16_data_per_frame;
#endif
  uint16                    ui16_dsm_length;
  ds_fa_status_enum_type    ret_val        = DS_FA_SUCCESS;
  dsm_item_type           **dsm_ptr        =
                            &ctrl_blk_ptr->downlink_buf_dsm_ptr;
  boolean                   go_idle = FALSE;
  boolean                  *f_synchronized = &ctrl_blk_ptr->f_synchronized;


  /* Just a sanity check.  We shouldn't be here if this is NULL. */
  ASSERT(*dsm_ptr != NULL);

  /* Assume no data is received at the start */
  *fax_downlink_data = NULL;
  *fax_type_of_data  = DS_FA_DATA_NONE;

  /* Only do this stuff if we're not receiving fax MSG data yet. */
  if (*f_synchronized)
  {
#ifdef DSGCSDFA_ENABLE_FAKE_TRAINING
    /* This is a goofy value only used to compensate for the goofy fax
    ** rates of 7,200 and 14,400 bps.  At normal rates, the average number of
    ** useful data bytes per frame is just the frame size.  At goofy rates,
    ** SYNC frames are periodically thrown in, pulling the average down. */
    ui16_data_per_frame = ctrl_blk_ptr->ui16_data_per_frame;
#endif

    /* In this loop, we are waiting for fax DATA frames to start coming in
    ** from the remote FA.  So we leave the loop once we lose synchronization,
    ** meaning we have DATA frames.  We also leave if we run out of data
    ** or change state. */
    while (TRUE)  /*lint !e716*/
    {
      /* Grab a frame's worth of data to check out. */
      data_ptr = &data_buf[0];
      bytes_available = better_dsm_extract(*dsm_ptr, &data_ptr, FRAME_SIZE);

      if (bytes_available < FRAME_SIZE)
      {
        /* We don't have a frame's worth of data.  Get outta here. */
        break;
      }

      if (!is_it_sync_frame(data_ptr, ctrl_blk_ptr))
      {
        if (is_it_status_frame(data_ptr, &frame_type, &ui8_data))
        {
          /* We found a STATUS frame.  Let's give it a peek. */
          if ((frame_type != FA_STATE_TRANS_MSG_REC) &&
              (frame_type != FA_STATE_TRANS_BCS_REC))
          {
            /* Remote FA says he's in a state that really doesn't jive with the
            ** state we're in.  Let's just go IDLE and hope for the best. */
            DATA_MSG1_HIGH("MSG_TRA: bad remote FA STATUS: ft=%d",
                        frame_type);

            /* Gotta change states to IDLE when leaving */
            go_idle = TRUE;

            /* Kill training duration timer, if it's running. */
            DSGCSDFA_TIMER_CLR( ctrl_blk_ptr->timer_ptr, \
                                ctrl_blk_ptr->timer_sig );

            break;
          }
        }
        else
        {
          if (ctrl_blk_ptr->f_training_phase == TRUE) 
          {
            uint64 frame_value;

            memscpy((byte*)&frame_value, sizeof(frame_value), data_ptr, FRAME_SIZE);
            if (frame_value == 0)
            {
              FA_FRAME_DUMP(data_ptr, TRUE);  /*lint !e506 !e774*/
              DATA_MSG0_HIGH("SYNC lost, RX TRAINING data");

              *f_synchronized = FALSE;
              FA_fake_training_active = FALSE;
              break;
            } 
            else
            {
              /* Toss the frame.  It has no useful unformation. */
              (void)dsm_pullup(dsm_ptr, NULL, FRAME_SIZE);
            }
          }
          else
          {
            FA_FRAME_DUMP(data_ptr, TRUE);  /*lint !e506 !e774*/
            DATA_MSG0_HIGH("SYNC lost, RX PAGE data");

            *f_synchronized = FALSE;
            break;
          }
        }
      }

      /* Toss the SYNC or STATUS frame.  It has no useful unformation. */
      (void)dsm_pullup(dsm_ptr, NULL, FRAME_SIZE);

#ifdef DSGCSDFA_ENABLE_FAKE_TRAINING
      /* If we are in the training phase, we are supposed to be receiving
      ** TCF from the remote FA.  But we're not yet.  GSM 03.45, 5.2.4 says
      ** to generate fake, good TCF until we get real TCF.  So do that at
      ** a rate equivalent to the incoming data rate. */
      if (ctrl_blk_ptr->f_training_phase)
      {
        static uint8 good_training[FRAME_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};
        static uint8 cnt = sizeof(good_training);
               uint8 i, frame_cnt = ctrl_blk_ptr->num_sync_frames;

        for (i = 0 ; i < frame_cnt; i++)
        {
          if (dsm_pushdown_tail(fax_downlink_data, (void*)good_training,
                                cnt, DSM_DS_LARGE_ITEM_POOL) == cnt)
          {
            DATA_MSG1_LOW("Generating %d train bytes for DTE", cnt);
            FA_fake_training_active = TRUE;
            T31_fake_train_cnt += cnt;
          }
          else
          {
            /* DSM item allocation failed. */
            ret_val = DS_FA_FAILURE;
            break;
          }
        }

        if (ret_val == DS_FA_FAILURE)
        {
          break;
        }
      }
#endif
    }

#ifdef DSGCSDFA_ENABLE_FAKE_TRAINING
    /* Dump debug message if we generated some training. */
    ui16_data_per_frame = dsm_length_packet(*fax_downlink_data);

    if (ui16_data_per_frame != 0)
    {
      DATA_MSG1_LOW("Generating %d train bytes for DTE", 
        ui16_data_per_frame);
    }
#endif
  }

  /* Only do this stuff if we're receiving fax MSG data. */
  if (!*f_synchronized)
  {
    /* In this loop we process fax MSG data received from the remote FA.
    ** Basically we continually try to get synchronized.  Once we do, assume
    ** the fax page has ended.  So we stick around while we still have a
    ** frame's worth of data to inspect.  Also leave if we state change back
    ** to IDLE or have a DSM item allocation failure. */
    while (TRUE)  /*lint !e716*/
    {
      /* Grab a frame's worth of data to check out. */
      data_ptr = &data_buf[0];
      bytes_available = better_dsm_extract(*dsm_ptr, &data_ptr, FRAME_SIZE);

      if (bytes_available < FRAME_SIZE)
      {
        /* We don't have a frame's worth of data.  Get outta here. */
        break;
      }

      if (is_it_sync_frame(data_ptr, ctrl_blk_ptr))
      {
        /* Throw away that darned SYNC frame.  It's worthless. */
        bytes_available = dsm_pullup(dsm_ptr, NULL, FRAME_SIZE);
        ASSERT(bytes_available == FRAME_SIZE);

        DATA_MSG0_LOW("MSG_TRA found SYNC_FRAME");

        if ((ctrl_blk_ptr->i16_bytes_between_sync_frames ==
             DATA_FRAMES_ONLY) || ctrl_blk_ptr->f_got_sync_last_time)
        {
          /* Either it is NOT a goofy fax rate (7200 or 1200 bps), OR
          ** it IS a goofy fax rate, AND we got two SYNC frames in a row.
          ** That's the remote FA telling us he's done sending DATA frames.
          ** Fax data reception has officially ended.  :( */
          DATA_MSG0_HIGH("going IDLE in MSG_TRA");

          /* Gotta change states to IDLE when leaving */
          go_idle = TRUE;

          /* Kill training duration timer, if it's running.  If it is
          ** running, that means training didn't last a full 1.5 seconds,
          ** but the remote FA is no longer sending us training data.  This
          ** isn't great but probably isn't life threatening either.  Let's
          ** assume remote FA knows what he's doing and just go IDLE. */
          DSGCSDFA_TIMER_CLR( ctrl_blk_ptr->timer_ptr, ctrl_blk_ptr->timer_sig );

          break;
        }
        else
        {
          /* It IS a goofy fax rate, but we just got one SYNC so far.  That
          ** is normal because they are mixed in with DATA frames to fill
          ** the channel. */
          ctrl_blk_ptr->f_got_sync_last_time = TRUE;
        }
      }
      else
      {
        /* The first 8 bytes in the downlink DSM chain are not a SYNC frame.
        ** So we can treat the first byte as DATA and keep the next 7 around
        ** to look for a SYNC frame again. */
        ctrl_blk_ptr->f_got_sync_last_time = FALSE;

        /* Remove one byte from the received downlink data. */
        ui8_data = (byte)dsm_pull8(dsm_ptr);

        /* Now put the byte in another DSM chain destined for T31 layer. */
        if (dsm_pushdown_tail(fax_downlink_data, 
                              (void*)&ui8_data, sizeof(ui8_data),
                              DSM_DS_SMALL_ITEM_POOL) != sizeof(ui8_data))
        {
          /* This is bad.  Function couldn't allocate another DSM item. */
          ret_val = DS_FA_FAILURE;
          break;
        }

        /* Keep separate count of data received up from the TPS */
        TPS_byte_cnt++;
      }
    }
  }

  /* Set return value if we're returning data to T31. */
  if (*fax_downlink_data != NULL)
  {
    /* Tell T31 what kind of data is coming */
    *fax_type_of_data = DS_FA_DATA_MSG;

    /* Keeping byte count, just for fun. */
    ui16_dsm_length = (uint16)dsm_length_packet(*fax_downlink_data);
    ctrl_blk_ptr->ui32_msg_byte_count += ui16_dsm_length;

    DATA_MSG1_LOW("Returning %d DATA bytes to T31", ui16_dsm_length);
    T31_byte_cnt += ui16_dsm_length;

    /* We need to check to see if we are still sending training data and
    ** if so, when we reach the max number of nulls to send to the DTE
    ** terminate the training sequence.  If we let it go for the full
    ** 1.5s we will typically cause the remote FA to terminate the call
    ** due to response latencies waiting for training to complete.
    */
    if (ctrl_blk_ptr->f_training_phase) 
    {
      /* Update the current null count */
      ctrl_blk_ptr->ui16_training_nulls_cnt += (uint16)ui16_dsm_length;

      if (FA_fake_training_active == FALSE)
      {
        T31_real_train_cnt += ui16_dsm_length;
      }
    }
  }

  /* If we found a SYNC frame it's time to change states to IDLE */
  if (go_idle)
  {
    ret_val = change_state_to(FA_STATE_TRANS_IDLE, ctrl_blk_ptr);
  }

  return ret_val;
} /* fa_trans_downlink__msg_tra */

/*========================================================================
  Function: fa_reflect_downlink_data

  Description:
    This function performs byte reflection on all downlink data bytes
    contained in the specified DSM item.  The function banks on the
    fact that the received data shouldn't span more than 1 DSM item 
    since the TPS should only be passing up 30 bytes of data.  However,
    the function does support traversing all linked items in the 
    chain.
    
  Dependencies:

  Parameters:
    dsm_downlink_data - Pointer to DSM item containing fax data destined
                        for the local DTE.
  Return Value:
    None
========================================================================*/
INLINE void fa_reflect_downlink_data
(
  dsm_item_type * dsm_downlink_data
)
{
  register uint16    i;
  register uint8 *  tmp_data_ptr;
  dsm_item_type *   tmp_dsm_ptr = dsm_downlink_data;

  
  /* Gotta traverse all items in the chain */
  while (tmp_dsm_ptr != NULL)
  {
    /* Grab the current data buffer */
    tmp_data_ptr = tmp_dsm_ptr->data_ptr;

    /* Traverse the buffer and reflect each byte */
    for (i = tmp_dsm_ptr->used; i > 0; i--, tmp_data_ptr++)
    {
      *tmp_data_ptr = GCSD_REFLECT_BYTE(*tmp_data_ptr);
    }

    /* Get the next item in the chain */
    tmp_dsm_ptr = tmp_dsm_ptr->pkt_ptr;
  } /* while */
} /* fa_reflect_downlink_data */


/*===========================================================================
** The following 6 functions implement the Transparent Fax Adaptation API.
** They are the only functions externalized by this module, and they can
** only be called by T31.
===========================================================================*/

/*========================================================================
  Function: dsgcsdfatrans_call_init

  Description:
    This function is invoked by T31, through dsgcsdfa_call_init(), at the
    beginning of every fax call.  It is responsible for doing Fax Adaptation
    initialization for transparent mode.

  Dependencies:
    None.

  Parameters:
    ctrl_blk_ptr - Transparent Fax Adaptor control block pointer.

  Return Value:
    None.

  Side Effects:
    Initializes transparent Fax Adaptation state.
========================================================================*/
void dsgcsdfatrans_call_init
(
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  DATA_MSG0_HIGH("FA initialization started");

  if (ctrl_blk_ptr->uplink_buf_dsm_ptr != NULL)
  {
    /* Indicates memory leak in previous call. */
    DATA_MSG0_HIGH("uplink_buf_dsm_ptr not NULL on call init");
    dsm_free_packet(&ctrl_blk_ptr->uplink_buf_dsm_ptr);
  }

  if (ctrl_blk_ptr->downlink_buf_dsm_ptr != NULL)
  {
    /* Indicates memory leak in previous call. */
    DATA_MSG0_HIGH("downlink_buf_dsm_ptr not NULL on call init");
    dsm_free_packet(&ctrl_blk_ptr->downlink_buf_dsm_ptr);
  }

  /* Initialize our plethora of state variables. */
  ctrl_blk_ptr->fa_state_trans                = FA_STATE_TRANS_IDLE;

  ctrl_blk_ptr->ui16_frames_per_bcs_byte      = 1;
  ctrl_blk_ptr->i16_bytes_between_sync_frames = UNKNOWN_DATA_PER_SYNC;
  ctrl_blk_ptr->ui16_data_per_frame           = FRAME_SIZE;
  ctrl_blk_ptr->timer_type                    = FA_NO_TIMER_TYPE;
  ctrl_blk_ptr->f_goto_idle                   = FALSE;
  ctrl_blk_ptr->f_start_dcs_timer             = FALSE;
  ctrl_blk_ptr->f_synchronized                = FALSE;
  ctrl_blk_ptr->f_remote_fa_in_msg_tra        = FALSE;
  ctrl_blk_ptr->f_remote_fa_in_bcs_tra        = FALSE;
  ctrl_blk_ptr->f_training_phase              = FALSE;
  ctrl_blk_ptr->f_send_connect                = TRUE;
  ctrl_blk_ptr->dcn_pending                   = FALSE;
  ctrl_blk_ptr->bits_to_shift                 = -1;
  ctrl_blk_ptr->ui16_training_nulls_cnt       = 0;
  ctrl_blk_ptr->sync_hysterisis_cnt           = 0;
  ctrl_blk_ptr->sync_loss_hysterisis_cnt      = 0;

  (void) set_num_repeated_sync_frames(ctrl_blk_ptr);

  DATA_MSG0_HIGH("FA initialization completed. FA waiting to synchronize!");
  
} /* dsgcsdfatrans_call_init */

/*========================================================================
  Function: dsgcsdfatrans_process_uplink

  Description:
    This function performs Fax Adaptation on the uplink.  It is invoked
    periodically by T31, through dsgcsdfa_process_uplink(), to take data
    destined for the remote fax, perform transparent Fax Adaptation
    processing on it, and pass it down the protocol stack to GCSD
    Transport Protocol Stack.

  Dependencies:
    Current state of transparent Fax Adaptation state machine.

  Parameters:
    fax_uplink_data   - Pointer to DSM item containing fax data destined
                        for the remote fax device, if any data available.
    fax_type_of_data  - Type of fax data in previous paramenter.  Values:
      DS_FA_DATA_NONE - No data at this time.
      DS_FA_DATA_BCS  - BCS (300 bps signaling) fax data.
      DS_FA_DATA_MSG  - MSG (higher speed image) fax data.
    ctrl_blk_ptr      - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_UPLINK_COMPLETE    - Function completed successfully, AND there's
                                 no use calling us again with no data to
                                 generate more uplink data.  We are in a
                                 state where we send DATA frames, and we
                                 need data to do that.
      DS_FA_SUCCESS            - Function completed successfully, AND T31 may
                                 call us again to generate more uplink data
                                 to fill the channel, if necessary.
      DS_FA_UPLINK_DATA_QUEUED - Same as DS_FA_SUCCESS, AND we were unable
                                 to send all uplink data to the remote FA.
                                 Remote FA is not accepting DATA frames
                                 yet.  If T31 calls return_to_idle() at this
                                 point, all queued data is freed and is never
                                 transmitted to the remote FA.
      DS_FA_INVALID_DATA_TYPE  - Data type, BCS or MSG, invalid for current
                                 Fax Adaptation state.
      DS_FA_FAILURE            - Fatal error.  Call should be aborted.

  Side Effects:
    Potentially takes uplink data from T31, performs a transparent Fax
    Adaptation massage on it, and sends it to GCSD Transport Protocol Stack.

    Transparent Fax Adaptation state may change.
========================================================================*/
ds_fa_status_enum_type dsgcsdfatrans_process_uplink
(
  dsm_item_type                 *fax_uplink_data,
  ds_fa_data_enum_type           fax_type_of_data,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  fa_state_trans_enum_type  previous_state;
  ds_fa_status_enum_type    ret_val;


  /* This is just paranoia.  If we don't have any new data, T31 probably sets
  ** this NULL anyway.  Make sure because it affects processing later on. */
  if (fax_type_of_data == DS_FA_DATA_NONE)
  {
    fax_uplink_data = NULL;
  }

  /* Stay in this loop as long as handler calls return DS_FA_SUCCESS,
  ** AND the state keeps changing.  A change in state means we probably
  ** only processed some of the uplink data, and we need to process the
  ** rest while in the next state. */
  do
  {
    /* Grab state before calling state machine handler. */
    previous_state = ctrl_blk_ptr->fa_state_trans;

    if (ctrl_blk_ptr->fa_state_trans < FA_TRANS_NUM_STATES)
    {
      /* Invoke proper handler, depending upon the data received and the current
      ** transparent Fax Adaptation state. */
      ret_val = fa_trans_uplink_state_handler
                  [fax_type_of_data][ctrl_blk_ptr->fa_state_trans]
                  (fax_uplink_data, ctrl_blk_ptr);
    }
    else 
    { /* In bad state, return FAILURE*/
      ret_val = DS_FA_FAILURE;
    }
  }
  while ((ret_val == DS_FA_SUCCESS) &&
         (previous_state != ctrl_blk_ptr->fa_state_trans));

  if ((ret_val != DS_FA_UPLINK_COMPLETE) && (ret_val != DS_FA_SUCCESS) &&
      (ret_val != DS_FA_UPLINK_DATA_QUEUED))
  {
    DATA_MSG1_HIGH("process_uplink returns: %d", (word)ret_val);
  }

  return ret_val;
} /* dsgcsdfatrans_process_uplink */


/*========================================================================
  Function: dsgcsdfatrans_process_downlink

  Description:
    This function performs transparent Fax Adaptation on the downlink.
    It is invoked periodically by T31, through dsgcsdfa_process_downlink(), to
    take data received from the remote fax, perform transparent Fax
    Adaptation processing on it, and pass it up the protocol stack to T31.

  Dependencies:
    Current state of transparent Fax Adaptation state machine.

  Parameters:
    fax_downlink_data   - Pointer to DSM item containing fax data received
                          from the remote fax device, if any data available.
    fax_type_of_data    - Type of fax data in previous paramenter.  Values:
      DS_FA_DATA_NONE   - No data at this time.
      DS_FA_DATA_BCS    - BCS (300 bps signaling) fax data.
      DS_FA_DATA_MSG    - MSG (higher speed image) fax data.
    ctrl_blk_ptr        - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SUCCESS           - Call successful.
      DS_FA_SWITCHING_TO_IDLE - Call successful, and transparent Fax Adaptor
                                switching its state to IDLE because of state
                                change in T.30 protocol.  Keep T31 layer
                                informed.
      DS_FA_FAILURE           - Fatal error.  Call should be aborted.

  Side Effects:
    Potentially takes downlink data from GCSD Transport Protocol Statck,
    performs a transparent Fax Adaptation massage on it, and returns it up
    to T31.

    Transparent Fax Adaptation state may change.
========================================================================*/
ds_fa_status_enum_type dsgcsdfatrans_process_downlink
(
  dsm_item_type                **fax_downlink_data,
  ds_fa_data_enum_type          *fax_type_of_data,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  uint16                    plen;
  dsm_item_type            *new_dsm_item_ptr;
  ds_fa_status_enum_type    temp_ret_val;
  ds_fa_status_enum_type    ret_val        = DS_FA_SUCCESS;
  fa_state_trans_enum_type  previous_state = FA_TRANS_NUM_STATES;


  /* Just being paranoid.  We really should never be hanging onto a lot of
  ** data between calls to this function.  If so, we are not processing it
  ** correctly and passing it up to T31 like we should. */
  if ((plen = (uint16)dsm_length_packet(ctrl_blk_ptr->downlink_buf_dsm_ptr)) >
      MAX_DOWNLINK_BYTES_TO_PROCESS)
  {
    DATA_MSG2_HIGH("DL buffer %d > %d bytes", plen,MAX_DOWNLINK_BYTES_TO_PROCESS);
  } 

  /* Start off assuming we got nothing from remote FA to return to T31. */
  *fax_type_of_data  = DS_FA_DATA_NONE;
  *fax_downlink_data = NULL;

  /* This loop grabs all the DSM items from the downlink watermark and
  ** sticks them on the end of our DSM packet chain.  It does have a
  ** self-imposed limit, just so we don't go crazy here. */
  while ((new_dsm_item_ptr =
          DSM_DEQUEUE( ctrl_blk_ptr->downlink_wm_ptr )) != NULL)
  {
#ifndef DSGCSDFA_RACAL_OCTET_ALIGN_TEST
    /* Gotta reverse the bit order on downlink data */
    fa_reflect_downlink_data(new_dsm_item_ptr);
#endif

    if (ctrl_blk_ptr->bits_to_shift > 0)
    {
      fa_perform_octet_alignment(&new_dsm_item_ptr, ctrl_blk_ptr);
    }

    dsm_append(&ctrl_blk_ptr->downlink_buf_dsm_ptr, &new_dsm_item_ptr);

    if (dsm_length_packet(ctrl_blk_ptr->downlink_buf_dsm_ptr) >
        MAX_DOWNLINK_BYTES_TO_PROCESS)
    {
      break;
    }
  }

  /* Stay in this loop as long as handler calls return DS_FA_SUCCESS,
  ** AND we don't have any data yet to return to T31,
  ** AND the state keeps changing,
  ** AND we've got some data to process.  A change in state means we
  ** probably only processed some of the downlink data, and we need to
  ** process the rest while in the next state. */
  while ((ret_val == DS_FA_SUCCESS)                            &&
         (*fax_type_of_data == DS_FA_DATA_NONE)                &&
         (previous_state != ctrl_blk_ptr->fa_state_trans)      &&
         (ctrl_blk_ptr->fa_state_trans < FA_TRANS_NUM_STATES)  &&
         (ctrl_blk_ptr->downlink_buf_dsm_ptr != NULL))
  {
    /* Grab state before calling state machine handler. */
    previous_state = ctrl_blk_ptr->fa_state_trans;

    /* Invoke proper handler, depending upon the current transparent Fax
    ** Adaptation state. */
    ret_val = fa_trans_downlink_state_handler[ctrl_blk_ptr->fa_state_trans]
                (fax_downlink_data, fax_type_of_data, ctrl_blk_ptr);
  }

  if (ctrl_blk_ptr->f_goto_idle)
  {
    /* Our timer expired between calls to dsgcsdfatrans_process_downlink().  We
    ** need to go IDLE and return status to T31 telling him what happened. */
    ctrl_blk_ptr->f_goto_idle = FALSE;
    temp_ret_val = change_state_to(FA_STATE_TRANS_IDLE, ctrl_blk_ptr);

    if (ret_val == DS_FA_SUCCESS)
    {
      /* Only blow away original ret_val is there were no problems above. */
      ret_val = temp_ret_val;
    }
  }

  if ((ret_val != DS_FA_SUCCESS) && (ret_val != DS_FA_SWITCHING_TO_IDLE))
  {
    DATA_MSG1_HIGH("process_downlink returns: %d", (word)ret_val);
  }

  return ret_val;
} /* dsgcsdfatrans_process_downlink */


/*========================================================================
  Function: dsgcsdfatrans_return_to_idle

  Description:
    This function is invoked by T31, through dsgcsdfa_return_to_idle(), when
    it decides that the Fax Adaptation layer should return to IDLE.  This
    happens normally during the T.30 protocol and can also happen for
    error and timeout handling.  Always happens on call termination.

  Dependencies:
    None.

  Parameters:
    f_call_ended - TRUE means call has terminated.
                   FALSE means call is still up.
    ctrl_blk_ptr - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type with values:
      DS_FA_SWITCHING_TO_IDLE - Call successful and switching to IDLE.
      DS_FA_FAILURE           - Fatal error.  Call should be aborted.

  Side Effects:
    Transparent Fax Adaptation state returns to IDLE, if not there already.
========================================================================*/
/*ARGSUSED*/
ds_fa_status_enum_type dsgcsdfatrans_return_to_idle
(
  boolean                        f_call_ended,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  ds_fa_status_enum_type  ret_val = DS_FA_SWITCHING_TO_IDLE;


  DATA_MSG0_HIGH("Requested change to IDLE state");

  /* Only need to do this stuff if we're not already IDLE. */
  if (ctrl_blk_ptr->fa_state_trans != FA_STATE_TRANS_IDLE)
  {
    if (ctrl_blk_ptr->fa_state_trans == FA_STATE_TRANS_MSG_REC)
    {
      /* Follow up DATA frames with 10 SYNC frames, so remote FA sees our
      ** state change to IDLE, as specified in GSM 03.45, 5.2.3.4. */
      DATA_MSG0_HIGH("Page done, send 10 SYNCS");
      ret_val = send_sync_frames(10, ctrl_blk_ptr, TRUE);
    }

    if (ctrl_blk_ptr->uplink_buf_dsm_ptr != NULL)
    {
      /* Buffered uplink data is never gonna make it to the remote FA. */
      DATA_MSG0_HIGH("Discarding buffered uplink data!");
      dsm_free_packet(&ctrl_blk_ptr->uplink_buf_dsm_ptr);
    }

    if (ctrl_blk_ptr->downlink_buf_dsm_ptr != NULL)
    {
      /* Buffered downlink data is never gonna make it to T31. */
      DATA_MSG0_HIGH("Discarding buffered downlink data!");
      dsm_free_packet(&ctrl_blk_ptr->downlink_buf_dsm_ptr);
    }

    /* If we have detected a disconnect (DCN) then we are done and can
    ** enter the IDLE state.
    */
    if (ctrl_blk_ptr->dcn_pending == TRUE)
    {
      DATA_MSG0_HIGH("queue DCN closing sync frames");

      (void) send_sync_frames(10, ctrl_blk_ptr, TRUE);

      if( ctrl_blk_ptr->uplink_wm_ptr )
      {
        FA_MSG_HIGH("TPS-UL:Qcnt=%d Qhi=%d Qlo=%d", 
                    ctrl_blk_ptr->uplink_wm_ptr->current_cnt,
                    ctrl_blk_ptr->uplink_wm_ptr->hi_watermark,
                    ctrl_blk_ptr->uplink_wm_ptr->lo_watermark);
      }
    }

    /* Finally go IDLE, man. */
    ret_val = change_state_to(FA_STATE_TRANS_IDLE, ctrl_blk_ptr);

    if (ret_val == DS_FA_SUCCESS)
    {
      DATA_MSG0_HIGH("FA now in IDLE state");
    }
  }

  return ret_val;
} /* dsgcsdfatrans_return_to_idle */


/*========================================================================
  Function: dsgcsdfatrans_process_bcs_frame

  Description:
    This function is called by T31, through
    dsgcsdfa_process_bcs_frame(), once it packages up an entire BCS
    frame received on the downlink.  T31 also calls this before it calls
    dsgcsdfatrans_process_uplink(), to give us a chance to look at the
    BCS frame before it gets bit-stuffed.

    Its job is to look at the BCS frame and determine if it's a DCS
    (Digital Command Signal).  The DCS frame identifies the fax rate
    negotiated by the two fax devices.  If it is a DCS, the fax rate is
    stored away in the control block for use later.  If the fax rate is
    one of the goofy ones, 12000 or 7200 bps, additional info about the
    number of DATA frames per SYNC frame is also stored in the control block.
    If this is not a DCS, no action is necessary.

    This function does absolutely no error checking on the BCS frame.  If
    it's corrupt, we just return, assuming it will be dealt with elsewhere.

    A T.30 HDLC BCS frame looks as follows:
      ------------------------------------------------------------------
      | Flag | ... | Flag | Address | Control | FCF | FIF | FCS | Flag |
      ------------------------------------------------------------------

    All fields are 1 byte, except FIF, which has arbitrary length.  The FCF
    identifies the frame.  We're looking for the DCS identifier in this
    function.  If it is a DCS, the middle 4 bits of the 2nd byte of the FIF
    identify the fax rate as follows (from Table 2/T.30):

      X X 0 0 0 0 X X -  2400 bps, Rec. V.27 ter
      X X 0 1 0 0 X X -  4800 bps, Rec. V.27 ter
      X X 1 0 0 0 X X -  9600 bps, Rec. V.29
      X X 1 1 0 0 X X -  7200 bps, Rec. V.29
      X X 0 0 0 1 X X - 14400 bps, Rec. V.17
      X X 0 1 0 1 X X - 12000 bps, Rec. V.17
      X X 1 0 0 1 X X -  9600 bps, Rec. V.17
      X X 1 1 0 1 X X -  7200 bps, Rec. V.17

  Dependencies:
    None.

  Parameters:
    fax_bcs_frame  - Pointer to DSM item containing BCS frame.
    f_uplink_frame - TRUE = uplink frame, FALSE = downlink frame.
    ctrl_blk_ptr   - Transparent Fax Adaptor control block pointer.

  Return Value:
    ds_fa_status_enum_type - Always returns DS_FA_SUCCESS.

  Side Effects:
    Frame may or may not be inspected and relevant information recorded.
========================================================================*/
ds_fa_status_enum_type dsgcsdfatrans_process_bcs_frame
(
  const uint8                   *ui8_data_ptr,
  uint16                         ui16_data_count,
  boolean                        f_uplink_frame,
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  uint8          ui8_hdlc_byte;
  uint16         ui16_offset = 0;
  ds_fa_status_enum_type fa_stat = DS_FA_SUCCESS;

  /* Each structure in this array has the following four members:
  **   (1) The fax rate bits of the T.30 DCS byte.
  **   (2) Number of DATA frame bytes between SYNC frames.
  **   (3) Average number of data bytes per frame during MSG phase.
  **   (4) Fax rate description string for debug.
  **
  ** So we do a linear search through this table with the DCS byte we got
  ** from T31 (on fax transmit) or from the remote FA (on fax receive).  Then
  ** we store away the interesting data in our control block for use later. */
  #define FAX_RATE_ARRAY_MAX_OFFSET  8
  static const struct
  {
    uint8   ui8_fax_rate;
    int16   i16_bytes_between_sync_frames;
    uint16  ui16_data_per_frame;
    uint16  ui16_max_training_nulls;
    char   *name_str;
  } fax_rate_array[FAX_RATE_ARRAY_MAX_OFFSET + 1] =
  {
    /* 2,400 bps - DATA frames only */
    {FA_FAX_RATE_2400_V27ter, DATA_FRAMES_ONLY, FRAME_SIZE, 
     450, "DCS: 2400 V.27ter"},

    /* 4,800 bps - DATA frames only */
    {FA_FAX_RATE_4800_V27ter, DATA_FRAMES_ONLY, FRAME_SIZE,
     900, "DCS: 4800 V.27ter"},

    /* 9,600 bps - DATA frames only */
    {FA_FAX_RATE_9600_V29, DATA_FRAMES_ONLY, FRAME_SIZE,
     1800, "DCS: 9600 V.29"},

    /* 7,200 bps - DATA and SYNC frames */
    {FA_FAX_RATE_7200_V29, (DATA_PER_SYNC_AT_7200 * FRAME_SIZE), FRAME_SIZE,
     1350, "DCS: 7200 V.29"},

    /* 14,400 bps - DATA frames only */
    {FA_FAX_RATE_14400_V17, DATA_FRAMES_ONLY, FRAME_SIZE,
     2700, "DCS: 14400 V.17"},

    /* 12,000 bps - DATA and SYNC frames */
    {FA_FAX_RATE_12000_V17, (DATA_PER_SYNC_AT_12000 * FRAME_SIZE), FRAME_SIZE,
     2250, "DCS: 12000 V17"},

    /* 9,600 bps - DATA frames only */
    {FA_FAX_RATE_9600_V17, DATA_FRAMES_ONLY, FRAME_SIZE,
     1800, "DCS: 9600 V.17"},

    /* 7,200 bps - DATA and SYNC frames */
    {FA_FAX_RATE_7200_V29, (DATA_PER_SYNC_AT_7200 * FRAME_SIZE), FRAME_SIZE,
     1350, "DCS: 7200 V.29"},

    /* UNKNOWN - DCS rate bits correspond to nothing in T.30. */
    {FA_FAX_RATE_UNKNOWN, UNKNOWN_DATA_PER_SYNC, FRAME_SIZE,
     1800, "DCS: UNKNOWN"}
  };


  DATA_MSG1_LOW("Parsing BCS frame of %d bytes", ui16_data_count);

  if ((ui8_data_ptr[0] == 0xFF) && (ui8_data_ptr[1] == 0x13))
  {
    /* We found the address and control fields.  Determine frame type. */
    ui16_offset += sizeof(uint16);
    if ((ui16_offset + sizeof(uint8)) <= ui16_data_count)
    {
      DATA_MSG1_HIGH("HDLC FrameID=0x%x",
        (*(ui8_data_ptr + ui16_offset) & HDLC_FCF_MASK));

      if ((*(ui8_data_ptr + ui16_offset) & HDLC_FCF_MASK) == HDLC_DCS_FRAME)
      {
        /* Far out!  It's the DCS frame.  Training always comes next. */
        DATA_MSG0_HIGH("Start TRAINING phase");

        ctrl_blk_ptr->f_training_phase = TRUE;

        /* Grab the fax rate byte from the FIF. */
        ui16_offset += 2;
        if ((ui16_offset + sizeof(ui8_hdlc_byte)) > ui16_data_count)
        {
          /* Out of data.  Incomplete frame.  This is bad.  Leave loop. */
          DATA_MSG0_HIGH("No rate byte in DCS frame!");
          return fa_stat;
        }

        /* Mask off the fax rate bits from T.30/Table 2. */
        ui8_hdlc_byte = *(ui8_data_ptr + ui16_offset) & DIS_DTC_FAX_RATE_BITS;

        /* Search through fax_rate_array[] for the entry that matches the fax
        ** rate bits we just got.  Pop out once we find it. */
        ui16_offset = 0;

        while ((ui8_hdlc_byte != fax_rate_array[ui16_offset].ui8_fax_rate) &&
               (ui16_offset < FAX_RATE_ARRAY_MAX_OFFSET))
        {
          ui16_offset++;
        }

        /* Store away interesting stuff in control block for use later. */
        ctrl_blk_ptr->i16_bytes_between_sync_frames =
                  fax_rate_array[ui16_offset].i16_bytes_between_sync_frames;
        ctrl_blk_ptr->ui16_data_per_frame =
                  fax_rate_array[ui16_offset].ui16_data_per_frame;
        ctrl_blk_ptr->ui16_max_training_nulls =
                  fax_rate_array[ui16_offset].ui16_max_training_nulls;

        DATA_MSG_SPRINTF_1(MSG_LEGACY_HIGH," Fax Rate: %s",
                           fax_rate_array[ui16_offset].name_str);
        DATA_MSG2_HIGH( "t_nulls=%d frame_sz=%d", 
                       ctrl_blk_ptr->ui16_max_training_nulls,
                       ctrl_blk_ptr->ui16_data_per_frame);

        if (!f_uplink_frame)
        {
          /* It's DCS on the downlink from the remote FA.  That means
          ** remote FA will soon send training.  Normally we wait for the
          ** remote FA to enter MSG_REC before we enter MSG_TRA.  However,
          ** the attached DTE needs to see training 75 +/- 20 msec after
          ** DCS.  We may fail that in GSM's digital environment.  So set
          ** a timer, so we don't fail it. */
          DATA_MSG1_HIGH("Waiting to start %dmsec DCS->TCF timer", DCS_TO_TRAIN_DELAY);

          ctrl_blk_ptr->f_start_dcs_timer = TRUE;
        }
      }
      else if ((*(ui8_data_ptr + ui16_offset) & HDLC_FCF_MASK) == HDLC_DCN_FRAME)
      {
        DATA_MSG0_HIGH("DCN - disconnect detected");
              ctrl_blk_ptr->dcn_pending = TRUE;
      }
    }
  }

  return fa_stat;
} /* dsgcsdfatrans_process_bcs_frame */

/*========================================================================
  Function: dsgcsdfatrans_timer_expired

  Description:
    This function is invoked by the main GCSD-PS task loop, through
    dsgcsdfa_timer_expired(), when it detects that the Fax Adaptor timer
    signal is set.  This allows transparent Fax Adaptation software to
    service the timer in the context of the GCSD-PS task.

  Dependencies:
    Current state of transparent Fax Adaptation state machine.

  Parameters:
    ctrl_blk_ptr - Transparent Fax Adaptor control block pointer.

  Return Value:
    None.

  Side Effects:
    Transparent Fax Adaptation state may change.
========================================================================*/
void dsgcsdfatrans_timer_expired
(
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  switch (ctrl_blk_ptr->fa_state_trans)
  {
    case FA_STATE_TRANS_IDLE:
      /* 75 msec timeout, which is the T.30 protocol delay between DCS
      ** and TCF (training).  Change state so we can start generating
      ** some training. */
      if (ctrl_blk_ptr->timer_type == FA_CED_TIMER_TYPE)
      {
        ds_gcsd_report_fax_connect( ctrl_blk_ptr->call_id );
        ctrl_blk_ptr->timer_type = FA_NO_TIMER_TYPE,
        ctrl_blk_ptr->f_send_connect = FALSE;
      }
      else
      {
        DATA_MSG0_HIGH("DCS->TCF delay complete. To MSG_TRA state");
        (void)change_state_to(FA_STATE_TRANS_MSG_TRA, ctrl_blk_ptr);
      }
      break;

    case FA_STATE_TRANS_BCS_TRA:
      /* 2.5 second timeout, which is how long GSM 03.45, 5.2.3.3 says to
      ** endure loss of synchronization before going IDLE. */
      DATA_MSG0_HIGH("Sync loss timeout in BCS_TRA. To IDLE state");

      /* The trick here is we can't go IDLE yet.  We must wait for T31 to
      ** call dsgcsdfatrans_process_downlink(), to tell him about it. */
      ctrl_blk_ptr->f_goto_idle = TRUE;
      break;

    case FA_STATE_TRANS_MSG_TRA:
      /* 1.5 second timeout, which is the T.30, 5.3.6.1.3 protocol duration
      ** of TCF (training).  We are done sending training data back to the
      ** DTE, so return to idle. */
      DATA_MSG0_HIGH("Training complete in MSG_TRA. To IDLE state");

      /* The trick here is we can't go IDLE yet.  We must wait for T31 to
      ** call dsgcsdfatrans_process_downlink(), to tell him about it. */
      ctrl_blk_ptr->f_goto_idle = TRUE;
      break;

    default:
      /* Invalid state for timer expiration. */
      DATA_MSG1_HIGH("Timeout state: invalid state=%d!",
                  ctrl_blk_ptr->fa_state_trans);
      break;
  }

  ctrl_blk_ptr->timer_type = FA_NO_TIMER_TYPE;
} /* dsgcsdfatrans_timer_expired */

/*========================================================================
  Function: dsgcsdfatrans_tx_ready

  Description:
    This function is used to indicate to the upper layers of the FPS
    whether or not it is ok to transmit data to the remote FA.

  Dependencies:
    Current state of transparent Fax Adaptation state machine.

  Parameters:
    ctrl_blk_ptr - Transparent Fax Adaptor control block pointer.

  Return Value:
    TRUE if it is ok to transmit, FALSE if it is not.

  Side Effects:
    None.
========================================================================*/
boolean dsgcsdfatrans_tx_ready
(
  const fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  boolean tx_ready;

  switch (ctrl_blk_ptr->fa_state_trans)
  {
    /* This state should not be transmitting any data from T31 */
    case FA_STATE_TRANS_IDLE:
      tx_ready = FALSE;
      break;

    /* These states are ok to transmit at any time. */
    case FA_STATE_TRANS_BCS_REC:
      tx_ready = ctrl_blk_ptr->f_synchronized;
      break;

    case FA_STATE_TRANS_BCS_TRA:
    case FA_STATE_TRANS_MSG_TRA:
      tx_ready = TRUE;
      break;

    /* This state needs to wait until the remote FA is ready to
    ** receive the data.  The remote side will let us know when
    ** to start sending.
    */
    case FA_STATE_TRANS_MSG_REC:
      tx_ready = (ctrl_blk_ptr->f_remote_fa_in_msg_tra);
      break;

    default:
      tx_ready = FALSE;
  } /* switch */

  return (tx_ready);
} /* dsgcsdfatrans_tx_ready */

/*========================================================================
  Function: dsgcsdfatrans_is_idle

  Description:
    This function is used to indicate to the upper layers of the FPS
    whether or not the FA layer is currently IDLE.

  Dependencies:
    Current state of transparent Fax Adaptation state machine.

  Parameters:
    ctrl_blk_ptr - Transparent Fax Adaptor control block pointer.

  Return Value:
    TRUE if the FA is IDLE, FALSE if it is not.

  Side Effects:
    None.
========================================================================*/
boolean dsgcsdfatrans_is_idle
(
  fa_trans_ctrl_blk_struct_type *ctrl_blk_ptr
)
{
  (void) ctrl_blk_ptr;

  return (ctrl_blk_ptr->fa_state_trans == FA_STATE_TRANS_IDLE);
} /* dsgcsdfatrans_is_idle */

#endif /* FEATURE_DATA_GCSD_FAX */
