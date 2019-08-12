#ifndef WL1_FET_CTRL_H
#define WL1_FET_CTRL_H
/*===========================================================================
                 WCDMA L1 FET Controller

GENERAL DESCRIPTION
  This file contains the code for controlling the operation of Frame Early Termination feature

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/wl1fetctrl.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who    what, where, why
--------   ---    ---------------------------------------------------------
05/19/16   svh     Added new client for HSUPA. 
02/24/15   ap       Initial check-in for WL1 QSH framework 
06/02/14   ac      Add client for multi-carrier state
12/27/13   pr      Added api to check if DAPE is enabled or not
12/04/13   qj      Fix fet not enabled during non-btfd call
11/25/13   ac      Remove unused FET_TPC_DECODE_EN_DYN and
                   ENABLE_UL_DCCH_GATING NV flags. Add DAPE support. 
10/14/13   pr/sr   Channel config optimizations
08/03/13   sm      FET DCH Cleanup routine returns a boolean on success
05/02/13   sm      Disable FET for invalid p03 signaled in HHO  
10/04/12   vsr     Updated FET & DED Energy threshold for slot format 2
09/17/12   vsr     Added DCH init function
08/14/12   vsr     Added API that checks if FET is enabled
07/16/12   sr      Fixed compiler warnings
02/10/12   vsr     Initial version of file

===========================================================================*/

/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "customer.h"

#include "l1const.h"
#include "l1def.h"

#include "l1cmmgr.h"
#if defined (FEATURE_QSH_DUMP) || defined(FEATURE_QSH_EVENT_METRIC)
#include "wl1_qsh_ext.h"
#endif /* defined (FEATURE_QSH_DUMP) || defined(FEATURE_QSH_EVENT_METRIC) */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* List of various modules/call types that register their status with FET */
typedef enum
{
  WL1_FET_CLIENT_NONE = 0,
  WL1_FET_CLIENT_2TF_BTFD = 1,
  WL1_FET_CLIENT_NON_2TF_BTFD = 2,
  WL1_FET_CLIENT_SINGLE_RATE_BTFD_AMR = 3,
  WL1_FET_CLIENT_MULTI_RATE_BTFD_AMR = 4,
  WL1_FET_CLIENT_SYNC = 5,
  WL1_FET_CLIENT_HS = 6,
  WL1_FET_CLIENT_RXD = 7,
  WL1_FET_CLIENT_CM = 8,
  WL1_FET_CLIENT_SF = 9,
  WL1_FET_CLIENT_PO3 = 10,
  WL1_FET_CLIENT_GSM_TX = 11,
  WL1_FET_CLIENT_VOICE_CALL = 12,
  WL1_FET_CLIENT_TUNED_TO_MULTI_CARR = 13,
  WL1_FET_CLIENT_EUL = 14,
}wl1_fet_client_enum_type;

/* Describes the status of each algorithm 
   as set through NV or QXDM cmd */
typedef enum
{
  ALGO_OFF = 0,
  ALGO_ON = 1,
  ALGO_DYNAMIC
}wl1_fet_algo_state_enum_type;

/* Describes the status of DCCH Aware ILPC algorithm 
   as set through NV or QXDM cmd */
typedef enum
{
  DAI_ALGO_OFF = 0,
  DAI_ALGO_ON = 1,
  DAI_FR_ALGO_ON = 2,
  DAI_BOTH_ALGO_ON
}wl1_fet_dcch_aware_ilpc_algo_state_enum_type;

/* Describes the rate of AMR call */
typedef enum
{
  FET_INVALID_RATE_BTFD_AMR,
  FET_SINGLE_RATE_BTFD_AMR,
  FET_MULTI_RATE_BTFD_AMR
}wl1_fet_btfd_amr_call_enum_type;

/* Describes the type of DCCH in a BTFD call
   (Two Transport Format DCCH, or not) */
typedef enum
{
  FET_NON_BTFD,
  FET_TWO_TF_BTFD_DCCH,
  FET_NON_TWO_TF_BTFD_DCCH
}wl1_fet_btfd_dcch_enum_type;

/* Main Database Structure */
typedef struct
{
  /* Value read from NV */
  uint16 nv_value;
  
  /* Variables that hold the status of various algorithms
     As decided by NV or QXDM commands */
  wl1_fet_algo_state_enum_type dcch_erasure_det_v2_oride_state;
  wl1_fet_dcch_aware_ilpc_algo_state_enum_type dcch_aware_ilpc_oride_state;
  wl1_fet_algo_state_enum_type dcch_fr_eng_comp_oride_state;
  
  wl1_fet_algo_state_enum_type fet_oride_state;
  wl1_fet_algo_state_enum_type msrch_oride_state;
  wl1_fet_algo_state_enum_type dape_oride_state;

  /* Variables that hold the status of various clients/call types
     as registered by those entities */
  wl1_fet_btfd_amr_call_enum_type btfd_amr_call_rate;
  wl1_fet_btfd_dcch_enum_type btfd_dcch_type;
  boolean sync_status;
  boolean hs_status;
  boolean eul_status;
  boolean rxd_status;
  boolean cm_status;
  boolean spread_factor_ok;
  boolean po3_ok;
  boolean gsm_tx_status;
  boolean voice_call_status;
  boolean tuned_to_multi_carr;

  /* Variables that hold latest SW decision on enable/disable of the 
     various algorithms. Decision depends on NV/QXDM cmd and status of
     various entities listed above */
  boolean dcch_erasure_det_v2_enable;
  uint8 dcch_aware_ilpc_mode;
  boolean dcch_fr_eng_comp_enable;
  boolean fet_enable;
  boolean msrch_enable;
  boolean dape_enable;
}wl1_fet_db_type;

/* Demback Extended Log Packet */
/* Maximum number of samples per log packet. */
#define WL1_DEMBACK_EXT_NUM_MAX_SAMPLES  (50)

/* Number of log packet buffers. */
#define WL1_DEMBACK_EXT_NUM_LOG_BUFFERS      (2)

/* Demback extended log packet. */
LOG_RECORD_DEFINE(WL1_DEMBACK_EXT_LOG_PKT)
  uint8                      version_number;
  uint8                      num_samples;
  uint16                     reserved;
  WfwDembackDpchFrameStruct samples[WL1_DEMBACK_EXT_NUM_MAX_SAMPLES];
LOG_RECORD_END


/* -----------------------------------------------------------------------
** Macros
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Function Definitions
** ----------------------------------------------------------------------- */


/*===========================================================================
FUNCTION wl1_fet_init

DESCRIPTION
This function initializes all the one time and control parameters.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_fet_init(void);

/*===========================================================================
FUNCTION wl1_fet_dch_init

DESCRIPTION
This function initializes FET params needed at DCH init

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_fet_dch_init(void);

/*===========================================================================
FUNCTION wl1_fet_reset_database

DESCRIPTION
This function initializes the database

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_fet_reset_database(void);

/*===========================================================================

FUNCTION wl1_fet_save_ctrl_from_nv

DESCRIPTION
  This function reads the NV item NV_WCDMA_FET_CTRL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void wl1_fet_save_ctrl_from_nv( void );

/*===========================================================================
FUNCTION wl1_fet_record_client_state

DESCRIPTION
This function is called by clients of FET to record their current state when it changes

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_fet_record_client_state(wl1_fet_client_enum_type client, boolean status);

/*===========================================================================
FUNCTION wl1_fet_record_active_cctrch

DESCRIPTION
This function keeps track of the CCTRCH index that is active

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_fet_record_active_cctrch(uint8 cctrch_table_index);

/*===========================================================================
FUNCTION wl1_fet_get_active_cctrch

DESCRIPTION
This function returns the CCTRCH index that is active

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
uint8 wl1_fet_get_active_cctrch(void);

/*===========================================================================
FUNCTION wl1_fet_update_status

DESCRIPTION
This function updates FET state based on current UE status

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_fet_update_status(void);

/*===========================================================================
FUNCTION wl1_fet_dch_cleanup

DESCRIPTION
This function cleans up FET state

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_fet_dch_cleanup(boolean conn_state);

/*===========================================================================
FUNCTION wl1_fet_is_dcch_erasure_det_v2_enabled

DESCRIPTION
This function returns status of DCCH erasure detection V2 feature

DEPENDENCIES
None

RETURN VALUE
Boolean: TRUE if DCCH erasure detection V2 is enabled; Else FALSE;

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_fet_is_dcch_erasure_det_v2_enabled(void);

/*===========================================================================
FUNCTION wl1_fet_is_dcch_erasure_det_enabled_in_fw

DESCRIPTION
This function status of DED feature enable in FW

DEPENDENCIES
None

RETURN VALUE
Boolean: TRUE if DED is enabled; Else FALSE;

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_fet_is_dcch_erasure_det_enabled_in_fw(void);

/*===========================================================================
FUNCTION wl1_fet_is_fet_enabled_in_fw

DESCRIPTION
This function status of FET feature enable in FW

DEPENDENCIES
None

RETURN VALUE
Boolean: TRUE if FET is enabled; Else FALSE;

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_fet_is_fet_enabled_in_fw(void);

/*===========================================================================
FUNCTION wl1_fet_is_dape_enabled_in_fw

DESCRIPTION
This function status of DAPE feature enable in FW

DEPENDENCIES
None

RETURN VALUE
Boolean: TRUE if DAPE is enabled; Else FALSE;

SIDE EFFECTS
None
===========================================================================*/
extern boolean wl1_fet_is_dape_enabled_in_fw(void);

/*===========================================================================
FUNCTION wl1_fet_oride_cmd

DESCRIPTION
  API used to update FET params through QXDM command 

DEPENDENCIES
  None

RETURN VALUE
  Return value of API called

SIDE EFFECTS
  None
===========================================================================*/
boolean wl1_fet_oride_cmd(uint8 api_identifier, uint32 input_arg_1, uint32 input_arg_2);

/*===========================================================================
FUNCTION wl1_fet_set_power_offsets

DESCRIPTION
This function populates the power offsets to FW-SW interface

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_fet_set_power_offsets(uint32 po3, uint8 slot_format);

/*===========================================================================
FUNCTION wl1_fet_cm_state_cb

DESCRIPTION
This function is registered with CM module

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_fet_cm_state_cb(l1_cm_active_deactive_enum_type cm_status);

/*===========================================================================
FUNCTION wl1_demback_ext_submit_log

DESCRIPTION
  Called when the local command is processed, submits the Demback (Extended) log packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wl1_demback_ext_submit_log(void);

/*===========================================================================
FUNCTION wl1_demback_ext_update_logging

DESCRIPTION
  Updates the current Demback (Extended) log packet by getting data from the FW/SW interface.
  Prepares to submit the log packet if it fills up 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wl1_demback_ext_update_logging(void);

/*===========================================================================
FUNCTION wl1_fet_record_spread_factor

DESCRIPTION
This function is called by DL to record the SF

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_fet_record_spread_factor(l1_sf_enum_type spread_factor);

/*===========================================================================
FUNCTION wl1_fet_resume_cphy_upn_fet_disable

DESCRIPTION
  This API de-configures and de-registers the allocated Gen TL event. This API
  would post a local command to resume processing the CPHY SETUP REQ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_fet_resume_cphy_upn_fet_disable(uint8 evt);

#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION wl1_fet_debug_dump

DESCRIPTION
  Dumps info into wl1_debug_fet_db_info_struct_type
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wl1_fet_debug_dump(wl1_debug_fet_db_info_struct_type* dump_ptr);
#endif /* FEATURE_QSH_DUMP */

#endif /* WL1_QICE_CTRL_H */


