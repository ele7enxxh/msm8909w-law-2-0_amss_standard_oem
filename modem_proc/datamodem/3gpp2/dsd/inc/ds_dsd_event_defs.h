#ifndef DS_DSD_EVENT_DEFS_H
#define DS_DSD_EVENT_DEFS_H

/*===========================================================================

         D S D S D   E V E N T  D E F I N I T I O N S
                   
DESCRIPTION

 The Data Services  DSD events  Header File.  Contains the definition
 for DSD events 

 Copyright (c) 2011-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/inc/ds_dsd_event_defs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/11   sk     Initial definition. 
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "event.h"
#include "qw.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================

                             EXPORTED DEFINITIONS

===========================================================================*/

//!!! Do not change the order. This is aligned with DSD Timers IDs
/*-----------------------------------------------------------------------
DSD timer IDs.
------------------------------------------------------------------------*/
#define DS_EVENT_1xAcqHystTimerID_T1          1
#define DS_EVENT_1xOOSHystTimerID_T2          2
#define DS_EVENT_HDRAcqHystTimerID_T3         3
#define DS_EVENT_HDROOSHystTimerID_T4         4
#define DS_EVENT_LTEAcqHystTimerID_T5         5
#define DS_EVENT_LTEOOSHystTimerID_T6         6
#define DS_EVENT_HDRHandDownTimerID_T7        7
#define DS_EVENT_DMaxTimerID  DS_EVENT_HDRHandDownTimerID_T7

/*===========================================================================
  NOTE: changes to these enums MUST be coordinated with the diag/tools team.
        as of the creation date mail had to be sent to asw.diag.request
===========================================================================*/

/*---------------------------------------------------------------------------
  TYPEDEF DS_DSD_EVENT_ENUM_TYPE

  Translated by the tool into strings. 
---------------------------------------------------------------------------*/
//!!! Do not change the enum order. This is aligned with
//!!! radio types defined in DSD
typedef enum
{
  DS_DSD_LTE_RADIO    = 1,
  DS_DSD_HDR_RADIO    = 2,
  DS_DSD_1X_RADIO     = 3,
  DS_DSD_GSM_RADIO    = 4,
  DS_DSD_WCDMA_RADIO  = 5
}ds_dsd_event_radio;



//!!! Do not change the enum order. This is aligned with
//!!! state machine states
typedef enum
{
  DS_DSD_OOS_RADIO          = 1,
  DS_DSD_ACQ_HYST_RADIO     = 2,
  DS_DSD_IN_SVC_RADIO       = 3,
  DS_DSD_OOS_HYST_RADIO     = 4
}ds_dsd_event_radio_state;

/*----------------------------------------------------------------------
  System Events
-----------------------------------------------------------------------*/
typedef struct
{
  qword                         time_stamp; 
  uint8                         dsd_pref_radio;
  uint8                         dsd_lte_radio_state;
  uint8                         dsd_hrpd_radio_state;
  uint8                         dsd_onex_radio_state;
  uint8                         dsd_gsm_radio_state;
  uint8                         dsd_tdscdma_radio_state;
  uint8                         dsd_wcdma_radio_state; 
}ds_dsd_system_event_type;

typedef enum
{
  DS_DSD_3GPP_NETWORK,
  DS_DSD_3GPP2_NETWORK,
  DS_DSD_INVALID_NETWORK
}ds_dsd_preferred_network_type;

typedef struct
{
  uint8 radio_state:4;
  uint8 throttle_state:2;
  uint8 eligible_flag:2;
}ds_dsd_radio_state_info_type;
	
/* size of structure 15 bytes, maximun size could be 17  */
typedef PACKED struct PACKED_POST
{
  uint8                         subscription_id:4;
  uint8                         preferred_network:4;
  uint8                         preferred_radio:4;
  uint8                         Reserved:4;
  uint32                        rat_mask;
  uint32                        so_mask;
  ds_dsd_radio_state_info_type  lte_sm_info;
  ds_dsd_radio_state_info_type  hrpd_sm_info;
  ds_dsd_radio_state_info_type  cdma_sm_info;
  ds_dsd_radio_state_info_type  wcdma_sm_info;
  ds_dsd_radio_state_info_type  tdscdma_sm_info;
  ds_dsd_radio_state_info_type  gsm_sm_info;
  ds_dsd_radio_state_info_type  wlan_sm_info;
}ds_dsd_preferred_radio_event_type;

typedef enum
{
  DS_DSD_TIMER_STARTED_EVENT,
  DS_DSD_TIMER_STOPPED_EVENT,
  DS_DSD_TIMER_EXPIRED_EVENT,
  DS_DSD_INVALID_TIMER_EVENT
}ds_dsd_timer_event_type;

/*----------------------------------------------------------------------
  Timer Events
-----------------------------------------------------------------------*/
/* size is 6 bytes  */
typedef PACKED struct PACKED_POST
{
  uint8              subscription_id:4;
  uint8              timer_event:4;
  uint8              timer_id;
  uint32             timer_value;
}ds_dsd_timer_event_info_type;

/*----------------------------------------------------------------------
  Throttle Events
-----------------------------------------------------------------------*/
typedef struct
{
  uint8   event_radio;
}ds_dsd_throttle_event_type;

#ifdef TEST_FRAMEWORK
#error code not present
#endif

#endif /* DS_DSD_EVENT_DEFS_H */
