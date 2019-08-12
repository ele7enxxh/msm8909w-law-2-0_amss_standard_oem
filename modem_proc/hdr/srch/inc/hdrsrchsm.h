#ifndef HDRSRCHSM_H
#define HDRSRCHSM_H
/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  H D R    S R C H    T A S K

                           Common State

GENERAL DESCRIPTION
   This module contains functions pertaining to the Common state of
   the HDR SRCH task's main state machine.

EXTERNALIZED FUNCTIONS

   All state machine transition functions for the CDMA state machine are
   prefaced by com_.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  Copyright (c) 2006-2015 Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/srch/inc/hdrsrchsm.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/16/15   vke     Added new cmd for triggering rpush in afc
06/27/14   wsh     ARD/ASDiv incorrectly switching in RTAP mode 
06/19/14   ljl     Added HDRSRCH_AFC_ENABLE_RGS_REPORT_CMD.
03/05/14   wsh     Bug fixes for DSDS-ASDiv and DSDA-ASDiv support 
01/15/14   wsh     Added DSDS ASDiv support
06/26/13   wsh     Added Selective TxD feature
09/27/12   ljl     Added HDRSRCHAFC_UPDATE_RGS_CMD.
05/25/12   wsh     Moved ARD logic into it's own state machine   
04/02/12   arm     Supported diversity in reacq. 
04/24/12   ljl     Supported XO fine factory cal removal.
03/13/.12  ljl     Added HDRSRCHRFDIV_HARD_TA_REQUEST_DIV_CMD.
12/22/11   rkc     Added HDRSRCHRFDIV_FL_ADPT_DISABLE_DIV_CMD.
11/11/11   arm     HDR idle and access state diversity implemetation.
10/14/11   lyl     Supported FL ARD enhancement.
06/24/10   cnx     Removed STM redeclarations.
06/21/10   cnx     Fixed compiler warnings.
04/27/11   lyl     Supported HDR FL data driven adaptive RxD.
06/10/10   lyl     Added HDRSRCHRFDIV_RUNTIME_PREF_CTRL_CMD.
05/27/10   vlc     Added HDRSRCHAFC_FAST_ACQ_CMD.
05/20/10   smd     Removed HDRSRCH_AFC_ROT_IN_TRK_CMD.
05/19/10   ljl     Supported SVDO diversity.
04/26/10   lyl     Added brute force acq support.
06/19/09   grl     Switched to the hdrsrchrfdiv API.
05/18/09   ljl     Updated for firmware MSGR interface.
08/21/07   rkc     Changed name of union u to payload.
                   Defined HDRSRCHSM_PAYLOAD_NOT_USED.
07/07/07   ljl     Added HDRSRCH_AFC_RPUSH_CMD.
06/20/07   rkc     Added hdrsrchsm_cmd_payload_type for hdr generic payloads
06/23/06   ljl     Initial version.

===========================================================================*/
#include "hdr_variation.h"
#include "stm.h"
#include "customer.h"

#define NOTUSED(i) if(i){}

extern  stm_int_q_type   hdrsrch_int_cmd_q;
   /* Shared internal command queue */

/* Deletes a single internal command corresponding to a given state machine */
#define HDRSRCH_STM_DEL_INTERNAL_CMD( sm, sm_group, cmd )                    \
        {                                                                    \
          stm_int_cmd_id_type _del_cmd_buf = { (sm), (uint32)(cmd) };        \
          stm_delete_internal_cmds( (sm_group), &_del_cmd_buf, 1 );          \
        }

/* Allocates a msg buf based on the type given, fills it with data and
   puts it into the internal command queue. Note that type should not be
   surrounded by parantheses while using this macro. */
#define HDRSRCH_STM_ALLOC_PUT_INTERNAL_CMD( sm, sm_group, cmd, type, data )  \
        {                                                                    \
          type *_payload;                                                    \
          _payload = (type *)                                                \
            stm_alloc_msg_buf( (sm), (sm_group), (uint32)cmd, sizeof(type) );\
          *_payload = (data);                                                \
          stm_put_internal_cmd_payload( (void *)_payload );                  \
        }

#ifdef FEATURE_HDR_SELECTIVE_TXD  
/* Allocates a msg buf based on the type given, fills it with data and
   puts it into the internal command queue. Note that type should not be
   surrounded by parantheses while using this macro. */
#define HDRSRCH_STM_ALLOC_SIZE_PUT_INTERNAL_CMD( sm, sm_group, cmd, size, d_ptr )  \
        {                                                                    \
          void *_payload;                                                    \
          _payload = (void *)                                                \
            stm_alloc_msg_buf( (sm), (sm_group), (uint32)cmd, size );        \
          memcpy(_payload, (d_ptr), size);                                   \
          stm_put_internal_cmd_payload( (void *)_payload );                  \
        }
#endif /* FEATURE_HDR_SELECTIVE_TXD */

/* Puts a msg buf in the internal queue with no payload other than the 
   command itself */
#define HDRSRCH_STM_PUT_INTERNAL_CMD(sm_id, sm_group, cmd)                   \
  stm_put_internal_cmd_payload(                                              \
    stm_alloc_msg_buf(sm_id, sm_group, (uint32)cmd, 0) )

/* Allocate a message buffer for the user to fill with data.  Does not queue
   the command! */
#define HDRSRCH_STM_ALLOC_MSG_BUF( sm, group, cmd, size )                    \
  stm_alloc_msg_buf( sm, group, (uint32)cmd, size )

/* Define a dummy value to use for hdrsrchsm_cmd_payload_type's union's
   not_used field to get around compiler warning. */
#define HDRSRCHSM_PAYLOAD_NOT_USED          1

/*-------------------------------------------------------------------------
      Internal Command IDs
-------------------------------------------------------------------------*/

typedef enum
{
  HDRSRCH_INTERNAL_CMD_BLOCK_START = 200,

  /* Common Internal Commands */

  /**********************************/
  /* HDR SRCH AFC Internal Commands */
  /**********************************/
  HDRSRCHAFC_GRANT_CMD,
    /* Grant command from TXOMgr */

  HDRSRCHAFC_CHANGE_RESTR_CMD,
    /* Change restriction command from TCXOMgr */

  HDRSRCHAFC_RELEASE_CMD,
    /* Release command from TCXOMgr */

  HDRSRCHAFC_DENY_CMD,
    /* Request is denied by TCXOMgr */

  HDRSRCHAFC_OPEN_LOOP_CMD,
    /* Open loop request from HDR SRCH */

  HDRSRCHAFC_CLOSE_LOOP_CMD,
    /* Close loop request from HDR SRCH */

  HDRSRCHAFC_CONTINUE_TRK_CMD,
    /* Conrinue tracking request from HDR SRCH */

  HDRSRCHAFC_START_ACQ_CMD,
    /* Start acqusisition request from HDR SRCH */

  HDRSRCHAFC_STOP_CMD,
    /* Stop tracking request from HDR SRCH */

  HDRSRCHAFC_DEACTIVATE_CMD,
    /* Deactivate AFC request from HDR SRCH */

  HDRSRCHAFC_SET_CHAN_CMD,
    /* Set channel info request from HDR SRCH */

  HDRSRCHAFC_TIMER_CMD,
    /* Time out event */
    
  HDRSRCHAFC_RPUSH_CMD,
    /* Rotator push */

  HDRSRCHAFC_TRIGGER_RPUSH_CMD,
    /* On callback set rpush flag */

  HDRSRCHAFC_ACQ_RSP_CMD,
    /* Acquisition response */

  HDRSRCHAFC_FORCE_R_TRK_CMD,
    /* Force rotator tracking for brute force acq */

  HDRSRCHAFC_FAST_ACQ_CMD,
    /* Fast acq command in inactive state */

  /*************************************************/
  /* HDR SRCH main state machine Internal Commands */
  /*************************************************/
  HDRSRCH_AFC_OPEN_LOOP_CMD,
    /* AFC is in open loop */

  HDRSRCH_AFC_CLOSE_LOOP_CMD,
    /* AFC is in close loop */

  HDRSRCH_AFC_TCXO_RELEASED_CMD,
    /* TCXO is released */

  HDRSRCH_AFC_ACQ_STARTED_CMD,
    /* Acquisition started */

  HDRSRCH_AFC_ACQ_SUCCEEDED_CMD,
    /* Acquisition succeeded */

  HDRSRCH_AFC_ACQ_FAILED_CMD,
    /* Acquisition failed */

  HDRSRCH_AFC_ENABLE_RGS_REPORT_CMD,
    /* HDR enable RGS report */

#ifdef FEATURE_HDR_DYNAMIC_IDLE_DIV
  HDRSRCH_DIV_ENABLED_CMD,
    /* Command to notify that diversity is enabled */

  HDRSRCH_DIV_ENABLE_TIMEOUT_CMD,
    /* Command to notify that diversity couldn't be enabled */
#endif /* FEATURE_HDR_DYNAMIC_IDLE_DIV */ 

  /******************************************************/
  /* HDR SRCH Diversity State Machine Internal Commands */
  /******************************************************/

  HDRSRCHRFDIV_REQUEST_DIV_CMD,
    /* Client requests diversity */

  HDRSRCHRFDIV_DISABLE_DIV_CMD,
    /* Client requests the disabling of diversity */

  HDRSRCHRFDIV_CONSIDER_DIV_SWITCH_CMD,
    /* Consider turning off diversity due to RF Io imbalance */

  HDRSRCHRFDIV_RF_GRANTED_CMD,
    /* TRM granted RF */

  HDRSRCHRFDIV_RF_UNLOCK_CMD,
    /* TRM gave on unlock RF request */

  HDRSRCHRFDIV_WARMUP_DONE_CMD,
    /* Diversity chain is done warming up. */

  HDRSRCHRFDIV_NULL_COVER_CLEARED_CMD,
    /* The null cover has been cleared. */

  HDRSRCHRFDIV_RELEASE_DIV_CMD,
    /* Consider releasing diversity */

  HDRSRCHRFDIV_SET_DIV_PREF_CMD,
    /* Set the diversity preferences that are read from NV. */

  HDRSRCHRFDIV_SET_RAMP_DOWN_STATE_CMD,
    /* Set whether diversity ramp down can be used. */

  HDRSRCHRFDIV_RUNTIME_PREF_CTRL_CMD,
    /* Diversity runtime preference control */

#ifdef FEATURE_HDR_SVDO_DIV_SUPPORT
  HDRSRCHRFDIV_CHANGE_DIV_STATUS_CMD,
    /* Diversity status change in SVDO */
#endif

  HDRSRCHRFDIV_HARD_TA_REQUEST_DIV_CMD,
    /* Request div at Hard Div Tune away state */

#ifdef FEATURE_HDR_FL_DATA_ADAPTIVE_RXD
  HDRSRCHARD_ACTIVATE_CMD,
    /* Activates ARD */

  HDRSRCHARD_DEACTIVATE_CMD,
    /* Deactivates ARD */

  HDRSRCHARD_SET_TAP_CMD,
    /* Set ARD to tap/non-tap */

  HDRSRCHARD_SUSPEND_CMD,
    /* Suspend ARD due to PRI chain tuneaway/ofs */

  HDRSRCHARD_TIMER_EXP_CMD,
    /* FL inactivity timer expired */

  HDRSRCHARD_FL_ACTIVITY_CMD,
    /* FL activity cb */

  HDRSRCHARD_DRC_CHANGED_CMD,
    /* DRC drops/rises across threshold */

  HDRSRCHARD_RESUME_CMD,
    /* Resume ARD due to tuneback */
#endif /* FEATURE_HDR_FL_DATA_ADAPTIVE_RXD */

  HDRSRCHAFC_UPDATE_RGS_CMD,
    /* Update RGS */

#ifdef FEATURE_HDR_SELECTIVE_TXD  
  HDRSRCHRFTXD_INACT_CMD,
  HDRSRCHRFTXD_RX_CMD,
  HDRSRCHRFTXD_END_TX_CMD,
  HDRSRCHRFTXD_ACCESS_CMD,
  HDRSRCHRFTXD_TRAFFIC_CMD,  
  HDRSRCHRFTXD_SUSPEND_CMD,
  HDRSRCHRFTXD_RESUME_CMD,
  HDRSRCHRFTXD_GRANT_MODE_CMD,
  HDRSRCHRFTXD_SET_CONFIG_CMD,
  HDRSRCHRFTXD_ANT_SWITCHED_CMD,
    /* commands leading to state change */

  HDRSRCHRFTXD_DIV_GRANTED_CMD,
  HDRSRCHRFTXD_DIV_RELEASED_CMD,
  HDRSRCHRFTXD_NEXT_ACC_SEQ_CMD,
  HDRSRCHRFTXD_TIMER_EXPIRED_CMD,
  HDRSRCHRFTXD_MDSP_INFO_READY_CMD,
    /* commands related to ant switch algorithm */

  HDRSRCHRFTXD_SLEEP_CMD,
  HDRSRCHRFTXD_DEACTIVATE_CMD,  
  HDRSRCHRFTXD_GRANT_MODE_CONFIG_CMD,
    /* commands added for DSDA/DSDS */

#endif /* FEATURE_HDR_SELECTIVE_TXD */

  HDRSRCH_LAST_INT_CMD

} hdrsrch_int_cmd_code_type;


/* Payload type for open loop cmd */
typedef struct
{
    int32         xo_cal_mode;
      /* XO mode from tcxomgr */

    uint8         ppm;
      /* Freq uncertainty in ppm */
} hdrsrch_open_loop_cmd_payload_type;

/*----------------------------------------------------------------------*
                      hdrsrchsm_cmd_payload_type
 *----------------------------------------------------------------------*/
/* The following hdrsrch payload structure is required to go from .smf's
   state-function to non-smf approach used elsewhere in hdrsrch.  This is
   necessary since the stm payload must be used to carry the internal
   command code, so to have a real payload, this structure is defined.
   Note: Payload should not have a pointer to non-static or local
   variables. */
typedef struct
{
  uint32        cmd;
    /* Internal command code */

  union union_payload
  {
    hdrsrch_open_loop_cmd_payload_type open_loop;
      /* Open loop cmd payload */
    uint32                             not_used;
      /* To avoid compiler warning */
  } payload;
    /* Union of payloads since this type is used by multiple HDR commands */

} hdrsrchsm_cmd_payload_type;


/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHSM_INIT

DESCRIPTION  : This function initializes hdrsrch_int_cmd_q structure. 

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchsm_init( void );


/*===========================================================================

FUNCTION HDRSRCH_STM_DEBUG_HOOK

DESCRIPTION    This function is called every time an input is processed.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/

void hdrsrch_stm_debug_hook
(
  user_debug_info_type       *debug_info
    /* Debug info */
);

#endif /* HDRSRCHSM_H */

