#ifndef MCALWCDMA_EDL_H
#define MCALWCDMA_EDL_H

/*============================================================================*/
/** @file  
 * This module has definition and declaration related to MCAL WCDMA EDL (EUL Downlink)
 * module APIs
 */
/*============================================================================*/

/*============================================================================
Copyright (c) 2007 - 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/mcalwcdma_edl.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------   
10/01/14    gv     Code cleanup. Removal of stale and unused code.  
07/01/14    hk     Header file inclusion cleanup
05/13/14    ymu    Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
04/08/14    ar     Cleaned up mcalwcdma_e_tti_enum_type
10/14/13    pr/sr  Channel config optimizations
05/22/13    oh     Adding DC-HSUPA framework support
05/09/12    ar     BOLT Compilation changes
03/19/13    vr     HSRACH fix to retrieve updated cell_db_idx even for HSRACH 
                   suspend-resume operation
03/06/13    vr     Code changes for HS RACH opt handling CU CNF
07/24/12    geg    Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/08/12    gv     Feature HS RACH Initial code check in
02/01/12    raj    Nikel Feature cleanup.
02/01/12    gnk    Nikel Feature cleanup.
01/30/12    vs     Feature cleanup.
01/20/12    zr     Mainlined FEATURE_MCAL_WCDMA. FEATURE_WCDMA_GENESIS_SW, and   
                   removed FEATURE_WCDMA_TAAXIS_SW
04/27/11    gv     EUL DL SW-FW interface changes for NikeL.
02/08/10    oh     Taxis/Genesis Code merge.     
07/22/09    gnk    Mainlining the feature FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT
10/03/08    hk     Bringing in 7k Mailine fixes
09/26/08    sa     Support for EUL DL reconfig/SHO
08/13/08    gv     EUL fixes for stop and other clean up.
07/31/08    oh     EUL changes for Taxis.
01/25/08   vsr     Checking in Enhanced DL files onto mainline
08/08/07   gv/vp   Created initial version
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "customer.h"
#include "comdef.h"
#include "mcalwcdma_common.h"
#include "mcalwcdma_dl.h"
#include "l1def.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define   MCALWCDMA_E_DL_MAX_RL                        4


#define MCALWCDMA_INVALID_INTF_BUF_IDX 0xFF

#ifdef FEATURE_WCDMA_HS_RACH
/* Define for default start and final sub fn for EUL/DL channels for config
** when the channels are added using JUDC*/
#define MCALWCDMA_EDL_START_SUB_FN_CHANNEL_ADD_DFLT_VAL 0
#define MCALWCDMA_EDL_FINAL_SUB_FN_CHANNEL_ADD_DFLT_VAL 0

#define MCALWCDMA_EDL_INVALID_FINAL_SUBFN_VAL 0xFFFF
#define MCALWCDMA_EDL_MAX_CFN_CNT 256
#define MCALWCDMA_EDL_MAX_SUBFN_CNT 1280
#define MCALWCDMA_EDL_NUM_SUBFRAMES_IN_CFN 5
/* MACROS to set the EDL Channel Bitmasks*/
#define MCALWCDMA_EDL_SET_PHCH_BMSK_VAR(val)\
  mcalwcdma_e_dl_phch_cfg_upd_bmsk = val

#define MCALWCDMA_EDL_SHIFT_AND_SET_PHCH_BMSK_VAR(intf_buf_idx)\
  mcalwcdma_e_dl_phch_cfg_upd_bmsk = \
      (mcalwcdma_e_dl_phch_cfg_upd_bmsk | ((uint32)(0x1) << intf_buf_idx))

#define MCALWCDMA_EDL_GET_PHCH_BMSK_VAR() mcalwcdma_e_dl_phch_cfg_upd_bmsk

#endif /*FEATURE_WCDMA_HS_RACH*/

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

typedef WfwDemodEulAgChannelInfoStruct mcalwcdma_eulAgChannelInfo_struct_type;
typedef WfwDemodEulRgHiChannelInfoStruct mcalwcdma_eulHichRgchChannelInfo_struct_type;


typedef WfwDemodPhyChanInfoStruct mcalwcdma_eulDemodPhyChanInfo_struct_type;
typedef enum {
  MCALWCDMA_E_DL_EVENT_CFG_EVT,
  MCALWCDMA_E_DL_EVENT_REG_EVT,
  MCALWCDMA_E_DL_EVENT_FRM_EVT,
  MCALWCDMA_E_DL_EVENT_NONE
} mcalwcdma_e_dl_event_type;

/*EUL channel action types*/
typedef enum 
{
  MCALWCDMA_E_DL_CHAN_DISABLE,
  MCALWCDMA_E_DL_CHAN_ENABLE,
  MCALWCDMA_E_DL_CHAN_RECONFIG_WITHOUT_TC,
  MCALWCDMA_E_DL_CHAN_RECONFIG_WITH_TC,
  MCALWCDMA_E_DL_CHAN_NO_OPS,
  MCALWCDMA_E_DL_CHAN_NO_ACTION
} mcalwcdma_e_dl_chan_action_enum_type;

typedef enum
{
  MCALWCDMA_E_DL_STATE_INACTIVE,
  MCALWCDMA_E_DL_STATE_ACTIVE,
  MCALWCDMA_E_DL_STATE_CONFIG_START,
  MCALWCDMA_E_DL_STATE_CONFIG_RECFG,
  MCALWCDMA_E_DL_STATE_CONFIG_STOP
} mcalwcdma_e_dl_state_enum_type;

#ifdef  FEATURE_WCDMA_DC_HSUPA 
typedef enum
{
  MCALWCDMA_E_DL_SECONDARY_STATE_INIT,
  MCALWCDMA_E_DL_SECONDARY_STATE_CONFIG_START,
  MCALWCDMA_E_DL_SECONDARY_STATE_CFGD,
  MCALWCDMA_E_DL_SECONDARY_STATE_ACTIVE,
  MCALWCDMA_E_DL_SECONDARY_STATE_CONFIG_RECONFIG,
  MCALWCDMA_E_DL_SECONDARY_STATE_CONFIG_STOP
} mcalwcdma_e_dl_secondary_state_enum_type;

#endif /*FEATURE_WCDMA_DC_HSUPA */


typedef struct
{
  /* Delta slots between CH frame boundary and DPCH frame boundary */
  uint16                     ch2dpch_delta_slots;
  /* Flag indicating if CH and DPCH are slot boundary aligned */
  boolean                    ch2dpch_slot_aligned;
  /* Flag indicating if CH subfn and DPCH cfn are odd or even aligned,
     ie if cfn of current DPCH frame is even/odd, then the sub frame 
     number of the CH frame starting in current DPCH frame is also 
     even/odd respectively (and not odd/even). This is used to correctly
     update the channel bitmask (0 or 1) for current frame and next frame.
   */
  boolean                    ch2dpch_cfn_subfn_aligned;
} mcalwcdma_e_dl_channel_cm_info;

typedef struct 
{
  boolean                    new_cfg;
  boolean                    agch_sttd;
  mcalwcdma_e_dl_chan_action_enum_type action;
  uint16                     ovsf_code;
  uint16                     cell_idx;
  uint16                     agch2dpch_offset;
  uint16                     start_sub_fn;
  uint16                     final_sub_fn;
  uint16                     pri_ernti;
  uint16                     sec_ernti;
  mcalwcdma_e_dl_channel_cm_info       agch_cm_info;

  uint16 num_sub_frms_within_dpch_frm_bdry;
  uint8 intf_buf_idx;

  uint32 stmr_action_time;
} mcalwcdma_e_dl_agch_info_page_type;

typedef struct 
{
  boolean new_cfg;
  boolean                    e_sttd;
  mcalwcdma_e_dl_chan_action_enum_type action;
  uint16                     ovsf_code;
  uint16                     cell_idx;
  uint16                     hich2dpch_offset;
  uint16                     start_sub_fn;
  uint16                     final_sub_fn;
  uint16                     sig_seq_idx;
  mcalwcdma_e_dl_channel_cm_info       hich_cm_info;

  uint16 num_sub_frms_within_dpch_frm_bdry;
  uint8 intf_buf_idx;
  uint32 stmr_action_time;
} mcalwcdma_e_dl_hich_info_page_type;

typedef struct
{
  boolean new_cfg;
  mcalwcdma_e_dl_chan_action_enum_type action;
  boolean                    srv_rls_flag;
  uint16                     rg_comb_idx;
  uint16                     rgch2dpch_offset;
  uint16                     start_sub_fn;
  uint16                     final_sub_fn;
  uint16                     sig_seq_idx;
  mcalwcdma_e_dl_channel_cm_info       rgch_cm_info;

  uint16 num_sub_frms_within_dpch_frm_bdry;
  uint8 intf_buf_idx;
  uint32 stmr_action_time;
} mcalwcdma_e_dl_rgch_info_page_type;

typedef struct 
{
  mcalwcdma_e_dl_hich_info_page_type hich_info;

  mcalwcdma_e_dl_rgch_info_page_type rgch_info;
} mcalwcdma_e_dl_rl_info_struct_type;

/*dl config db */
typedef struct 
{
  uint8 dpch_ch_num[WL1_MAX_UL_CARR_IDX];

  uint16 farthest_ch2dpch_offset[WL1_MAX_UL_CARR_IDX];

  mcalwcdma_e_dl_state_enum_type state;

  e_tti_enum_type e_tti;

  mcalwcdma_e_dl_agch_info_page_type agch_info[WL1_MAX_UL_CARR_IDX];

  mcalwcdma_e_dl_rl_info_struct_type e_dl_rl_info[WL1_MAX_UL_CARR_IDX][MCALWCDMA_E_DL_MAX_RL];

  #ifdef FEATURE_WCDMA_HS_RACH
  /* For HS RACH there would be only one RL, so using the below
   ** index to know which one of the e_dl_idx has the HS RACH information*/
  uint16 hs_rach_rl_idx;
  #endif /*FEATURE_WCDMA_HS_RACH*/

  #ifdef  FEATURE_WCDMA_DC_HSUPA 
  mcalwcdma_e_dl_secondary_state_enum_type secondary_state;
  #endif /*FEATURE_WCDMA_DC_HSUPA */

} mcalwcdma_e_dl_channel_db_struct_type;

typedef struct
{
  /** ID of the event used by the MCAL sequencer for channel configuration 
    * purposes */
  uint8 evt_id;

  mcalwcdma_e_dl_event_type evt_type;

  mcalwcdma_e_dl_channel_db_struct_type *channel_db_ptr;
} mcalwcdma_e_dl_config_db_struct_type;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* MCAL structure that stores the config information for EUL DL config*/
extern mcalwcdma_e_dl_channel_db_struct_type e_dl_mcal_info;

#ifdef FEATURE_WCDMA_HS_RACH
extern mcalwcdma_e_dl_config_db_struct_type mcalwcdma_e_dl_cfg_db;
#endif /* FEATURE_WCDMA_HS_RACH */

extern uint32 mcalwcdma_e_dl_phch_cfg_upd_bmsk;

/* =======================================================================
**                        Function Declarations
** ======================================================================= */
 /*===========================================================================
 
 FUNCTION        MCALWCDMA_ERNTI_UPDATE
 
 DESCRIPTION     This function updates the ERNTI information in the MCAL structures
              
 DEPENDENCIES    Unknown
 
 RETURN VALUE    None
 
 SIDE EFFECTS    Unknown
 
 ===========================================================================*/
extern  void mcalwcdma_ernti_update(uint16 pri_ernti);

/*===========================================================================

FUNCTION        MCALWCDMA_E_DL_RESUME_PHCH

DESCRIPTION     This function updates the mcal edl global that holds
                the channel mask for all the EUL DL channels that are being
                configured
             
DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/

extern void mcalwcdma_e_dl_resume_phch(void);

/*===========================================================================

FUNCTION        MCALWCDMA_E_DL_SUSPEND_PHCH

DESCRIPTION     This function updates the mcal edl global that holds
                the channel mask for all the EUL DL channels that are being
                configured
             
DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void mcalwcdma_e_dl_suspend_phch(uint16 agch_final_sfn,
                                           uint16 hich_final_sfn,uint16 rgch_final_sfn);

/*===========================================================================

FUNCTION MCALWCDMA_E_DL_PHYCHAN_CFG

DESCRIPTION     This function is the API to be called by the EDL Host layer to
                 configure EDL physical channels.

DEPENDENCIES     R99 DPCH channel must be already setup and the Layer-1 must
                 be aligned to that already.

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void mcalwcdma_e_dl_phychan_cfg(mcalwcdma_e_dl_channel_db_struct_type *channel_db_ptr);

/*===========================================================================

FUNCTION        MCAL_E_DL_CFG_EVT_PRE_PROC

DESCRIPTION     This function is called when mcalwcdma config
                event handler is called. 

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void mcalwcdma_e_dl_cfg_evt_pre_proc(void);

/*===========================================================================

FUNCTION        MCALWCDMA_E_DL_CFG_EVT_POST_PROC

DESCRIPTION     This function is called after sending the config command
                to the MDSP. This function calls a function to update the SG
                regarding the EDL reconfiguration and also post a local command
                for the config command handler to trigger.

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void mcalwcdma_e_dl_cfg_evt_post_proc(void);

/*===========================================================================

FUNCTION        MCAL_E_DL_FRM_EVT_POST_PROC

DESCRIPTION     This function wraps up the EUL DL configuration
                by posting a local command to trigger an update of the
                EUL DL structures.

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void mcalwcdma_e_dl_frm_evt_post_proc(void);

/*===========================================================================

FUNCTION        MCAL_E_DL_UPDATE_TTI_INFO

DESCRIPTION     This function updates the tti information in the e_dl_config_db
                and also sets the TTI CHANGED FLAG to false.This is critical operation
                for TTI reconfiguration scenarios.

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void mcalwcdma_e_dl_update_tti_info(void);

/*===========================================================================

FUNCTION MCALWCDMA_E_DL_EVT_HANDLER

DESCRIPTION     This is the router function within MCAL_EDL for all the
                  interrupts.

DEPENDENCIES     None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void mcalwcdma_e_dl_evt_handler(uint8 evt_id);

/*===========================================================================

FUNCTION        MCALWCDMA_E_DL_INIT_EVENT

DESCRIPTION     This function does all the initialization necessary for setting
                 up events: Allocation of event, setting the callback etc.

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void mcalwcdma_e_dl_init_event(void);

/*===========================================================================

FUNCTION        MCALWCDMA_E_DL_CLEAN_EVENT

DESCRIPTION     This function does all the cleanup necessary for events.

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void mcalwcdma_e_dl_clean_event(void);

/*===========================================================================

FUNCTION        MCALWCDMA_E_DL_SET_EVENT

DESCRIPTION     This function sets up an event by modifying the offset and
                 then enabling the event.

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void mcalwcdma_e_dl_set_event(uint16 off_in_ticks);

#ifdef FEATURE_WCDMA_HS_RACH
/*===========================================================================
FUNCTION        MCALWCDMA_EDL_DEALLOCATE_INTF_BUF_IDX

DESCRIPTION    This function to deallocate the interface buffer indices
             
DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void mcalwcdma_edl_deallocate_intf_buf_idx(void);

/*===========================================================================

FUNCTION        MCALWCDMA_EDL_UPDATE_CHANNEL_DB

DESCRIPTION     This function is called during HS RACH setup add
            This populates the mcal db for eul dl config
             
DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void mcalwcdma_edl_update_channel_db(
            mcalwcdma_e_dl_channel_db_struct_type *channel_db_ptr);

/*===========================================================================
FUNCTION        MCALWCDMA_E_DL_CELL_IDX_UPDATE

DESCRIPTION     This function is used to update the cell index (mcal db).
                Cell index is typically populated during CPHY_SETUP_REQ.
                When HSRACH suspend-resume operation happens we need to 
                update again, in case it has changed.
             
DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void mcalwcdma_e_dl_cell_idx_update(uint8 hs_rach_eul_dl_cell_idx);

#endif /*FEATURE_WCDMA_HS_RACH*/
#endif

