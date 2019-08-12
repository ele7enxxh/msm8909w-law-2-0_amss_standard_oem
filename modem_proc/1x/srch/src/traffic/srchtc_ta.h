#ifndef SRCHTC_TA_H
#define SRCHTC_TA_H
/*===========================================================================

                   S R C H T C I   H E A D E R    F I L E

DESCRIPTION
   This file contains definitions used internally by Search Traffic state.
   It should NOT be included by any source file outside the Search task.

  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.


===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/srch/src/traffic/srchtc_ta.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/18/16   eye     Increasing release timer to 38ms to avoid G denials.
02/19/16   pk      Changes to handle unlock required for traffic TA
02/10/16   pk      Fix for Tiberium Strip/Pack Build failure
02/05/16   ab/pk   1x2GTA Feature changes
12/08/15   ab      First Cut
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "comdef.h"
#include "customer.h"
#include "modem_1x_defs.h"

/* Srch */
#include "srch_fing_driver_i.h"
#include "srch_genlog.h"
#include "srch_genlog_i.h"

#include "srch_hw_t.h"
#include "srch_hw_t_i.h"

/* Others*/
#include "trm.h"
#include "timer.h"
/*===========================================================================
              LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*-------------------------------------------------------------------------
      Constants
-------------------------------------------------------------------------*/

#define LONG_TUNEAWAY_RELEASE_TIME_BUFFER_MS               38

#define QUICK_TUNEAWAY_RELEASE_TIME_BUFFER_MS              12

/* Max Duration in SCLK for which idle TRM client could hold chain for QTA */
#define MAX_TUNEAWAY_TIME                                  8132

/* Minimum time required to start next TA, after ending LTA */
#define TRM_CHAIN_FORCE_HOLD_TIME_MS                       100

/* Minimum time required to start next TA, after ending QTA */
#define TRM_CHAIN_FORCE_HOLD_TIME_MS_AFTER_QTA             30

/* Conversion macro for time in "ms" to "sclks" */
#define GET_TIMETICK_MS_FROM_SCLKS(sclk)      ((sclk)*1000/TIMETICK_NOMINAL_FREQ_HZ)

#define ASD_METRIC_RESET_TIME_BUFFER_MS                    300  

/*---------------------------------------------------------------------------
  Traffic State TuneAway  Data structure
---------------------------------------------------------------------------*/

typedef PACK(struct)
{
  uint8 				version;

  uint8 				ta_supported_mask;

  uint64				reserved1;	

  uint64				reserved2;	
  
} tc_ta_nv_type;

typedef struct
{
  boolean                 ta_supported;
  
  boolean                 force_lta_mode;

  boolean                 ta_proceed;

  boolean                 ta_in_progress;
  
  boolean                 rf_granted_in_ta;

  boolean                 unlock_notification_sent;
  
  boolean                 unlock_by_honored;

  boolean                 unlock_immediate_received;

  boolean                 unlock_required_received;
  
  boolean                 allow_1x_ta;

  boolean                 inv_trm_prio;

  boolean                 block_asdiv;

  boolean                 ppm_is_active; 

  boolean                 remember_ext_cmd;
  
  /* QTA specifc Parameters */
  boolean                 qta_is_enabled;

  /* Used to keep track, whether FW sent CDMA_FW_IND_QTA_RXAGC_ACQ_DONE msg */
  boolean                 rxagc_acq_done_ind;

  boolean                 rf_granted_after_qta;

  /* flag to indicate that LC state is latched or not */
  boolean                 lc_latched;

  int32                   ta_time_sclk;

  /* ttrk fing pos before sleep in cx8 */
  uint32                  sleep_ttrk_fing_pos;

  /* ttrk fing pos before sleep in cx8 */
  uint32                  best_fing;

  uint32                  ta_start_time;

  uint32                  ta_end_time;

  srch_ta_rsn_type         tuneaway_type;
  
  tc_ta_nv_type           nv;

  srch_cmd_type           *ext_cmd_payload;

  /* timer used to unlock TRM chain */
  timer_type              unlock_timer;

  /* timer used to crash if chain is not granted back on time.*/
  timer_type              sanity_timer;

  timer_type              release_unblock_timer;

  /* Winning client information as sent by TRM */
  trm_client_info_t       winning_client_info_from_trm;

  struct
  {
    /* Sclk timestamp */
    uint32                unlock_by_sclk;

    /* Target client information */
    trm_client_info_t     winning_client_info;
  } curr_ta_info;

  srch_genlog_traffic_ta_sub_v1  ta_stats;

} srchtc_tuneaway_parms_type;

/*-------------------------------------------------------------------------
      Helper/Utility Function Definitions
-------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION       SRCHTC_GET_UNLOCK_TIME_SCLK

DESCRIPTION    This function returns the unlock_by_sclk

DEPENDENCIES   None

RETURN VALUE   unlock_by_sclk

SIDE EFFECTS   None
===========================================================================*/
extern uint32 srchtc_get_unlock_time_sclk( void );

/*===========================================================================

FUNCTION       SRCHZZ_TL_PROF_GET_DATA

DESCRIPTION    This function gets ZZ Timeline profile log data for generic
               packet logging

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srchtc_ta_get_ta_stats
(
  srch_genlog_packet_id_type  id,           /* ID of packet to commit sub */
  sub_commit_func_type        commit_fp     /* func to commit the subpacket */
);

/*==========================================================================
FUNCTION       SRCHTC_HANDLE_QTA_RXAGC_ACQ_DONE_IND

DESCRIPTION    This function posts cmd on TC SM to handle QTA_RXAGC_ACQ_DONE
               indicator msg from FW.

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
==========================================================================*/
extern void srchtc_handle_qta_rxagc_acq_done_ind( void );


/*===========================================================================
FUNCTION       SRCHTC_IS_TA_SUPPORTED

DESCRIPTION    This function returns the variable TRUE if 1X is
               supporting Tuneaway
DEPENDENCIES

RETURN VALUE   TRUE if tuneaway is supported
               FALSE otherwise
               
SIDE EFFECTS   None

===========================================================================*/
extern boolean  srchtc_is_ta_supported( void );

/*===========================================================================
FUNCTION       SRCHTC_IS_TA_ALLOWED

DESCRIPTION    This function returns if TA is allowed or not

DEPENDENCIES   None

RETURN VALUE   TRUE if tuneaway is allowed
               FALSE otherwise

SIDE EFFECTS   None
===========================================================================*/
extern boolean srchtc_is_ta_allowed( void );

/*===========================================================================
FUNCTION       SRCHTC_TA_IN_PROGRESS

DESCRIPTION    This function returns if TA is in progress

DEPENDENCIES   None

RETURN VALUE   TRUE if TA is in progress
               FALSE otherwise

SIDE EFFECTS   None
===========================================================================*/
extern boolean srchtc_ta_in_progress( void );

/*===========================================================================
FUNCTION       SRCHTC_TA_ASD_RESTORE_ALLOWED

DESCRIPTION    This function returns if ASD metric should be reset

DEPENDENCIES   None

RETURN VALUE   TRUE if ASD metric should be reset
               FALSE otherwise

SIDE EFFECTS   None
===========================================================================*/
extern boolean srchtc_ta_asd_restore_allowed( void );

/*==========================================================================
FUNCTION       SRCHTC_IS_QTA_ENABLED

DESCRIPTION    This function checks if TC SM is in QUICK_TUNEAWAY_STATE.

DEPENDENCIES   NONE

RETURN VALUE   TRUE if 1x->X QTA is enabled, else FALSE.

SIDE EFFECTS   NONE
==========================================================================*/
extern boolean srchtc_is_qta_enabled( void );

/*==========================================================================
FUNCTION       SRCHTC_TA_IS_ASD_OP_ALLOWED

DESCRIPTION    This function checks if ASDIV algorithm is allowed to run

DEPENDENCIES   NONE

RETURN VALUE   TRUE if ASDIV algorithm is allowed to run, else FALSE.

SIDE EFFECTS   NONE
==========================================================================*/
extern boolean srchtc_ta_is_asd_op_allowed( void );

/*===========================================================================
FUNCTION       SRCHTC_RX_UNLOCK_CB

DESCRIPTION    This function is called by TRM when 1x is supposed to give up
               the primary chain. The lock will only be given up if 1x is in
               a data SO, BG_TRAFFIC.

DEPENDENCIES       None

RETURN VALUE       None

SIDE EFFECTS       None
===========================================================================*/
void srchtc_rx_unlock_cb
(
  trm_unlock_event_enum_t  event,         /* Event being sent to the client */
  uint32                   unlock_by_sclk /* Sclk timestamp for UNLOCK_BY*/
);

/*==========================================================================
FUNCTION       SRCHTC_TX_UNLOCK_CB

DESCRIPTION    This callback function handles the unlock by received from TRM 
               for the TX client.

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
==========================================================================*/
extern void srchtc_tx_unlock_cb
(
  trm_unlock_event_enum_t  event,         /* Event being sent to the client */
  uint32                   unlock_by_sclk /* Sclk timestamp for UNLOCK_BY*/
);

/*===========================================================================

FUNCTION       SRCHTC_TA_GET_FING_TTRK_POS

DESCRIPTION    This function returns Time Tracking finger position value.

DEPENDENCIES   None

RETURN VALUE   uint32

SIDE EFFECTS   None

===========================================================================*/
extern uint32 srchtc_ta_get_fing_ttrk_pos( void );

/*===========================================================================
FUNCTION       SRCHTC_TA_STORE_PARAMS

DESCRIPTION    This function stores the tuneaway Parameters

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
extern void srchtc_ta_store_params
(
  /* Sclk timestamp */
  uint32                  unlock_by_sclk,

  /* Winning Client Information */
  trm_client_info_t       winning_client_info,

  /* Type of unlock */
  trm_unlock_event_enum_t trm_unlock_type
);

/*===========================================================================
FUNCTION       SRCHTC_TA_CHANGE_PRIORITY_PPM

DESCRIPTION    This function increases or decreases the TRM 
               priority based on the PPM request

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
extern void srchtc_ta_change_priority_ppm( boolean inv_flag);

/*===========================================================================

FUNCTION       SRCHTC_TA_START_SEARCH

DESCRIPTION    This function initiates the searches.

DEPENDENCIES   None

RETURN VALUE   uint32

SIDE EFFECTS   None

===========================================================================*/
extern void srchtc_ta_start_search( void );

/*===========================================================================
FUNCTION       SRCHTC_TA_READ_NV_ITEM

DESCRIPTION    This function reads the NV values for TA Feature

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
extern void srchtc_ta_read_nv_item( void );

#endif /* SRCHTC_TA_H */
