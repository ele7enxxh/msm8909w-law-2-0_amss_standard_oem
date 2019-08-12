#ifndef HDRSRCHRFTXD_H
#define HDRSRCHRFTXD_H
/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            H D R   S E A R C H   A R D   S T A T E M A C H I N E

GENERAL DESCRIPTION

  This module provides routines for diversity RF control

EXTERNALIZED FUNCTIONS (Global)
  
EXTERNALIZED FUNCTIONS (Regional)

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2009 - 2015
                by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/srch/inc/hdrsrchrftxd.h#1 $
$DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when         who     what, where, why
--------     ---     --------------------------------------------------------- 
06/17/15     wsh     Fixed some issues found in bring up
04/21/15     wsh     Added per-band enable/disable to all states
02/04/15     wsh     Added support 2->2a transition, removed PA on requirement
01/12/15     wsh     Added L1/FW interface to confirm ASDiv prio ended
11/19/14     wsh     Added DR-DSDS support for ASDiv
10/22/14     wsh     Fixed compiler warning
10/17/14     wsh     Fixed reporting wrong mode to TRM in 4a->3a transistion
07/09/14     cnx     Support NV refresh without reset.
05/08/14     wsh     Fixed concurrent RF/ASDiv call that led to RF issue
03/15/14     wsh     Change to support T/T and new RF concurrency concern 
03/05/14     wsh     Bug fixes for DSDS-ASDiv and DSDA-ASDiv support  
03/05/14     wsh     Bug fixes for DSDA and DSDA support
01/15/14     wsh     Added DSDS ASDiv support
11/07/13     wsh     Disable ASDiv during TAP call
05/25/12     wsh     Initial implementation

==========================================================================*/

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_HDR_SELECTIVE_TXD
#include "rfm.h"
#include "trm.h"
#include "hdrsrchmac.h"
#include "hdrsrchstate.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#define HDRSRCH_EFS_NV_STXD_CONFIG           "/nv/item_files/modem/hdr/srch/hdr_stxd_config2"
#define HDRSRCH_EFS_NV_STXD_INTERNAL_CONFIG  "/nv/item_files/modem/hdr/srch/hdr_stxd_internal_config"

#define HDRSRCHRFTXD_TRACE_ENABLED

#define HDRSRCHRFTXD_INVALID_BAND  0xFF

typedef enum
{ 
  HDRSRCHRFTXD_EVENT_DISABLING_RX,
  HDRSRCHRFTXD_EVENT_RX_ENABLED,
    /* For non-TX states, trigger from TRM release/grant. This is
       to simplify numerous code paths that release chain without state
       change (ie, in ACQ/IDLE) */  
  HDRSRCHRFTXD_EVENT_DEACTIVATING,
  HDRSRCHRFTXD_EVENT_EXITING_ACCESS,
  HDRSRCHRFTXD_EVENT_ACCESS_ENTERED,
  HDRSRCHRFTXD_EVENT_EXITING_TRAFFIC, /* 5 */
  HDRSRCHRFTXD_EVENT_TRAFFIC_ENTERED,
    /* For TX states, cannot trigger from TRM release/grant or
       rfm_disable_tx. Exiting TX state may trigger antenna
       switching and hence must give TxD advanced notice to start
       the shutdown process early. */
  HDRSRCHRFTXD_EVENT_SUSPENDING,
  HDRSRCHRFTXD_EVENT_RESUMED,
    /* Suspend and resume for TX states, again sent early to
       start shutdown process. */
  HDRSRCHRFTXD_EVENT_MODE_GRANTED,
    /* TRM granted the mode change request */
    /* Above commands may lead to a state change */

  HDRSRCHRFTXD_EVENT_SET_CONFIG, /* 10 */
    /* TRM set config */

  HDRSRCHRFTXD_EVENT_ANT_SWITCHED,
    /* Antenna switch completed */

  HDRSRCHRFTXD_EVENT_DIV_GRANTED,
  HDRSRCHRFTXD_EVENT_DIV_RELEASED,
  HDRSRCHRFTXD_EVENT_NEXT_ACC_SEQ,
  HDRSRCHRFTXD_EVENT_TIMER_EXPIRED, /* 15 */
  HDRSRCHRFTXD_EVENT_MDSP_INFO_READY,
    /* commands related to ant switch algorithm and wil.
       not lead to a state change */

  HDRSRCHRFTXD_EVENT_MODE_CONFIG_GRANTED,
    /* For set_mode_and_config API */

  HDRSRCHRFTXD_EVENT_SLEEPING,
    /* Transition into sleep state */
  HDRSRCHRFTXD_EVENT_ENUM_SIZE
} hdrsrchrftxd_event_enum_type;

typedef struct 
{
  int rx0;
  int rx1;
    /* AGC on PRI/DIV chain */

  uint32 ratchet;
    /* rachet count within last frame */

  uint8 slots;
    /* Number of samples read */

  boolean log_miss;
    /* Possible log miss since last read */
} hdrsrchrftxd_mdsp_info_rdy_cmd_payload_type;

typedef enum
{
  HDRSRCHRFTXD_WAIT_FOR_STATE_TIMEOUT,
  HDRSRCHRFTXD_WAIT_FOR_STATE_CANCELED,
  HDRSRCHRFTXD_WAIT_FOR_STATE_SUCCESS
} hdrsrchrftxd_wait_status;

typedef enum
{
  HDRSRCHRFTXD_SS,
  HDRSRCHRFTXD_DSDS,
  HDRSRCHRFTXD_DSDA,
  HDRSRCHRFTXD_DRDSDS
} hdrsrchrftxd_dualsim_mode_type;

typedef void (*hdrsrchrftxd_wait_cb_type)
(
  hdrsrchrftxd_wait_status status,
  stm_state_type prev_state,
  stm_state_type curr_state,
  void* userdata
);

typedef struct
{
  trm_ant_switch_div_config_type position;
  boolean gap_ended;
} hdrsrchrftxd_ant_sw_payload_type;

typedef struct
{
#ifdef HDRSRCHRFTXD_TRACE_ENABLED
  uint8 idx;
#endif /* HDRSRCHRFTXD_TRACE_ENABLED */

  union
  {
    uint32 timer_id;
    hdrsrchrftxd_ant_sw_payload_type ant_sw;
    trm_ant_switch_set_mode_type set_mode;
    trm_asd_set_mode_and_config_type set_mode_cfg;
    trm_ant_switch_cb_info_type set_cfg;
    hdrsrchrftxd_mdsp_info_rdy_cmd_payload_type mdsp_info;
    uint8 tca_band;
    hdrsrch_state_enum_type next_state;
  };
} hdrsrchrftxd_event_payload_type;

/*===========================================================================

                  COMMAND  FUNCTION  DECLARATIONS

===========================================================================*/

/*=========================================================================

FUNCTION HDRSRCHRFTXD_START_STM

DESCRIPTION 
  Start TxD stm.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrsrchrftxd_start_stm( void );

/*==========================================================================

FUNCTION  HDRSRCHRFTXD_EVENT 
          HDRSRCHRFTXD_EVENT_WITH_PAYLOAD 

DESCRIPTION
  This function reports an TxD event to the state machine. As a
  result, TxD state will be updated.
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
#define hdrsrchrftxd_event( event ) \
        hdrsrchrftxd_event_with_payload( (event), NULL)

void hdrsrchrftxd_event_with_payload
( 
  hdrsrchrftxd_event_enum_type event,
  hdrsrchrftxd_event_payload_type* payload
);

/*=========================================================================

FUNCTION HDRSRCHRFTXD_INIT_ANT_SWITCH

DESCRIPTION 
  This function initialize the antenna switch to IDLE DEFAULT for given
  device only if it is currently in HIGH-Z state. Otherwise, it
  does nothing.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrsrchrftxd_init_ant_switch
( 
  rfm_device_enum_type rf_chain
);

/*=========================================================================

FUNCTION HDRSRCHRFTXD_IS_IN_TX_MODE

DESCRIPTION 
  This checks if TxD is in one of the TX mode. Per requirement, antenna
  switch must be reset to proper position when exiting TX. Caller need
  to ensure TRM lock is not released before the switching is complete.
 
DEPENDENCIES
  None

RETURN VALUE
  True - if TxD is in the process of transitioning out of RXTX mode
  False - Otherwise

SIDE EFFECTS
  None

=========================================================================*/
boolean hdrsrchrftxd_is_in_tx_mode( void );

/*=========================================================================

FUNCTION HDRSRCHRFTXD_WAIT_FOR_TX_SHUTDOWN

DESCRIPTION 
  Wait for TRM to grant mode out of TX
 
DEPENDENCIES
  Must be called from HDRSRCH task

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrsrchrftxd_reg_tx_shutdown_cb
(
   hdrsrchmac_txd_tx_shutdown_cb_type tx_shutdown_cb
);

#if 0 /* to be removed after some testing */
/*=========================================================================

FUNCTION HDRSRCHRFTXD_WAIT_FOR_TX_SHUTDOWN

DESCRIPTION 
  Wait for TRM to grant mode out of TX
 
DEPENDENCIES
  Must be called from HDRSRCH task and cannot be called within
  SM processing.

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrsrchrftxd_wait_for_tx_shutdown( void );
#endif /* to be removed after some testing */

/*=========================================================================

FUNCTION HDRSRCHRFTXD_SET_BAND

DESCRIPTION 
  Set the future band to be used.
 
DEPENDENCIES
  Must be called from HDRSRCH task. Usually called in TCA processing

RETURN VALUE
  True  - if band has changed (first call always returns TRUE)
  False - otherwise

SIDE EFFECTS
  None

=========================================================================*/
void hdrsrchrftxd_set_band
(
  sys_band_class_e_type band
);

/*=========================================================================

FUNCTION HDRSRCHRFTXD_SET_TAP_CALL

DESCRIPTION 
  Set tap call status
 
DEPENDENCIES
  Must be called from HDRSRCH task. 

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrsrchrftxd_set_tap_call
(
  boolean tap_call
);

/*==========================================================================

FUNCTION  HDRSRCHRFTXD_GET_DUAL_SIM_MODE
          

DESCRIPTION
  This function returns whether AT is DSDS or DSDA mode. This
  function is basically a wrapper to hdrmultirat_get_mode_pref.
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
hdrsrchrftxd_dualsim_mode_type hdrsrchrftxd_get_dualsim_mode( void );

/*=========================================================================

FUNCTION HDRSRCHRFTXD_SUSPEND_ASDIV

DESCRIPTION 
  Sets if ASDiv switching should be tempororily suspended
 
DEPENDENCIES
  Must be called from HDRSRCH task. 

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
boolean hdrsrchrftxd_suspend_asdiv
( 
  boolean suspend,
  trm_asdiv_switch_state_update_reason_t reason
);

/*=========================================================================

FUNCTION HDRSRCHRFTXD_RF_CALL_STARTED

DESCRIPTION 
  Informs ASD that a conflicting RF call has started. This function
  requests (and may block waiting) for ASDiv critical section
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None


=========================================================================*/
void hdrsrchrftxd_rf_call_started( void );

/*=========================================================================

FUNCTION HDRSRCHRFTXD_RF_CALL_FINISHED

DESCRIPTION 
  Informs ASD that a conflicting RF call has finished. This function
  releases ASDiv critical section
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None


=========================================================================*/
void hdrsrchrftxd_rf_call_finished( void );


/*==========================================================================

FUNCTION  HDRSRCHRFTXD_READ_NV_CONFIG

DESCRIPTION
  This function reads NV config parameters
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  Time since the start of this sampling period

SIDE EFFECTS
  None

==========================================================================*/
void hdrsrchrftxd_read_nv_config( void );

/*=========================================================================

FUNCTION HDRSRCHRFTXD_SET_REACQ_STATUS

DESCRIPTION 
  Sets whether reacq is successful
 
DEPENDENCIES
  Must be called from HDRSRCH task. 

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrsrchrftxd_set_reacq_status
(
  boolean status
);

/*==========================================================================

FUNCTION        HDRSRCHRFTXD_ASDIV_PRI_ENDED

DESCRIPTION
  This function should be called to signify FW has ended ASDiv prio boost
  switch
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE 
 None
 

SIDE EFFECTS
  none

==========================================================================*/
void hdrsrchrftxd_asdiv_pri_ended( void );

/*=========================================================================

FUNCTION HDRSRCHRFTXD_ASDIV_IS_ENABLED

DESCRIPTION 
  Return whether ASDiv feature is enabled (from NV). It doesn't look at
  per band setting or current state.
 
DEPENDENCIES
  None

RETURN VALUE
  True   -  If feature is enabled from NV
  False  -  If feature is not enabled.

SIDE EFFECTS
  None

=========================================================================*/
boolean hdrsrchrftxd_asdiv_is_enabled( void );
#endif /* FEATURE_HDR_SELECTIVE_TXD */

#endif /* HDRSRCHRFTXD_H */

