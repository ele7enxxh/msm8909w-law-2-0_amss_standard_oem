#ifndef WL1_CPCDRX_EDL_ENTITY_H
#define WL1_CPCDRX_EDL_ENTITY_H

/*===========================================================================
                 WCDMA L1 DOWNLINK CPC DRX EDL MODULE

GENERAL DESCRIPTION
  This file contains the code for carrying out CPC DRX EDL operations

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2007 - 2010 by Qualcomm Technologies, Inc.  All Rights Reserved

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/wl1cpcdrxedlentity.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $  

when       who      what, where, why
--------   ---      --------------------------------------------------------  
06/23/15   cjz      Use last-logged SFN instead of first subFN to decide EDCH counter logging in 0x423A
05/21/14   raj      Initial code checkin to allow CPC with QTA
05/13/14   ymu      Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
04/08/14   ar       Cleaned up mcalwcdma_e_tti_enum_type 
01/08/14   yw       DCHSUPA: CPC DRX support
08/10/12   ash      Fixing false AGCH decode during CPC wake up occasion
02/27/12   oh       Fixed compilation error.
02/22/12   oh       Ignore the EDL DRX entity results after entities are demod_inactive,
                    if the complete results for the decode are already available.
02/22/12   oh       Register EDL entities again after every CPC DRX Drop and Add.
07/27/11   amj      Put some F3s within FEATURE_WL1_DL_DRX_CTRL_DEBUG_MSG_LEVEL1.
04/28/11    sj      upmerge from offline/main
04/22/11   oh       Log EUL 10ms Tx info from HS.  
04/19/11   oh       Fixed cpc drx logging issues related to EDCH inact. threshold and 
                    EDL entities sleep/wakeup.
09/08/10   dm       Added changes for 0x423A log packet
09/07/10   rgn      Added macro to check if entity id is valid   
08/23/10   oh       Rearranged inclusion of header files.   
08/20/10   rmsd     Fixed Lint and Compiler Warnings
04/05/10   oh       First cut
===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_WCDMA_CPC_DRX
#include "wl1dldrxctrl.h"
#include "edlcfg.h"
#include "tlm.h"
#ifdef FEATURE_WCDMA_DC_HSUPA 
#include "euledpch.h"
#endif

#define WL1_EDLDRX_NUM_CHANNELS 4
#define WL1_EDLDRX_INVALID_SUBFN 0xFFFF
#define WL1_EDLDRX_INVALID_FN 0xFFFF
#define WL1_EDLDRX_INVALID_CH_OFFSET 0xFF
#define WL1_EDLDRX_NUM_HICH_SUBFRAMES_OFFSET 6
#define WL1_EDLDRX_INVALID_EDL_TO_DPCH_OFFSET WL1_EDL_INVALID_EDL_TO_DPCH_OFFSET
#define WL1_EDLDRX_INVALID_SUBFR_OFFSET WL1_EDL_INVALID_SUBFR_OFFSET
#define WL1_EDLDRX_INVALID_ENTITY_ID WL1_EDL_INVALID_ENTITY_ID
#define WL1_EDLDRX_MAX_FN_COUNT(tti) (((tti) == E_TTI_2MS)?(EUL_MAX_SUB_FN_COUNT):(MAX_CFN_COUNT))
#define WL1_EDLDRX_GET_HICH_RESULT_OFFSET() (wl1_edldrx_entity_ctrl_info.wl1_edldrx_hich_ctrl_info.hich_result_offset)
#define WL1_EDLDRX_SET_HICH_RESULT_OFFSET(x) (wl1_edldrx_entity_ctrl_info.wl1_edldrx_hich_ctrl_info.hich_result_offset = (x))  
#define WL1_EDLDRX_NUM_EDL_FNS 8
#define WL1_EDLDRX_SET_ENTITY_AWAKE_FN_FLAG(entity_ptr, subfn, val) \
        ((entity_ptr)->is_entity_awake_fn[(subfn)%WL1_EDLDRX_NUM_EDL_FNS]|= (val))
#define WL1_EDLDRX_CLEAR_ENTITY_AWAKE_FN_FLAG(entity_ptr, subfn, val) \
        ((entity_ptr)->is_entity_awake_fn[(subfn)%WL1_EDLDRX_NUM_EDL_FNS] = (val))
#define WL1_EDLDRX_IS_ENTITY_AWAKE_FN(entity_ptr, subfn) \
        ((entity_ptr)->is_entity_awake_fn[(subfn)%WL1_EDLDRX_NUM_EDL_FNS])
#define WL1_EDLDRX_SET_SCHED_TX_HICH_RSLT_FN_FLAG(subfn, val) \
  (wl1_edldrx_entity_ctrl_info.wl1_edldrx_hich_ctrl_info.is_sched_tx_hich_rslt_fn[(subfn)%WL1_EDLDRX_NUM_EDL_FNS]= (val))
#define WL1_EDLDRX_IS_SCHED_TX_HICH_RSLT_FN(subfn) \
  (wl1_edldrx_entity_ctrl_info.wl1_edldrx_hich_ctrl_info.is_sched_tx_hich_rslt_fn[(subfn)%WL1_EDLDRX_NUM_EDL_FNS])

#define WL1_EDLDRX_IS_ENTITY_ID_VALID(entity_id) \
  (entity_id < WL1_EDLDRX_NUM_CHANNELS)

/*EDL DRX state enum type*/
typedef enum
{
  /*If the EDL channel is not confg for DRX*/
  WL1_EDLDRX_ENTITIY_INIT           = 0,
  /*If DRX is confg*/
  WL1_EDLDRX_ENTITIY_CFGD           = 1,
  /*EDL channel is monitored during DRX*/
  WL1_EDLDRX_ENTITY_DEMOD_ACTIVE    = 2,
  /*EDL channel is not monitored during DRX*/
  WL1_EDLDRX_ENTITY_DEMOD_INACTIVE  = 3,
  WL1_EDLDRX_EDL_NUM_ENTITY_STATE   = 4

}wl1_edldrx_entity_state_enum_type;

typedef struct
{
  /*flag to indicate that entity is ready to sleep*/
  boolean entity_ready_to_sleep;
  tlm_cfn_info_in_cfn_tick sleep_timing_info;
  
}wl1_edldrx_entity_sleep_prog_struct_type;

/*Data config DB for EDL DRX*/
typedef struct 
{
  /* entity id is in use */
  boolean in_use;
  /*Entity ID locally maintained by EDL DRX module.*/
  uint8 entity_id;
  /** id provided by drx controller */
  uint8 drx_ctrl_id;
  /*EDL channel to DPCH offset*/
  uint16 channel2dpchoffset;
  /* Array to indicate whether entity needs to be active for coming subframes/frames*/
  boolean is_entity_awake_fn[WL1_EDLDRX_NUM_EDL_FNS];
  /* Flag to indicate that entity is awake already programmed*/
  boolean entity_awake_already_programmed;
  /*State of EDL Channel wrt DRX*/
  wl1_edldrx_entity_state_enum_type entity_state;
  /*Type of EDL Channel*/
  e_dl_chan_enum_type entity_type;
  /** next occasion time */
  wl1_dl_drxctrl_time_var_struct_type next_prog_occ_time;
  /*Needs to be removed later when the sleep handler bug in DRX ctrlr is fixed????????*/
  uint16 updated_next_next_tti_fn;
  /* Below boolean is used to find out if the entity just woke up and whether first read results 
  ** needs to be considered or ignored because of possible partial decoding*/
  boolean entity_first_read_after_wakeup_2ms;
  /* Below structure contains the sleep info. for the entity, to be used when prog. sleep indef.to 
  ** DRX ctrlr */
  wl1_edldrx_entity_sleep_prog_struct_type sleep_prog_info;

}wl1_edldrx_entity_struct_type;

typedef struct
{
  /*Subfn offset after which HICH results are rcvd. corresp. to EDCH tx*/
  uint16 hich_result_offset;

  /* Array to indicate whether a subfn has a HICH result corresponding to 
  ** scheduled EDCH transmission*/
  boolean is_sched_tx_hich_rslt_fn[WL1_EDLDRX_NUM_EDL_FNS];

  uint8 num_e_rl;

}wl1_edldrx_hich_ctrl_struct_type;

/*Ctrl config DB for EDL DRX*/
typedef struct
{
  /* If enabled only then EDL channels will be 
  ** monitored during HS SCCH recep.*/
  boolean ue_drx_grant_monitor_enabled;
  /* Signalled by network*/
  l1_inact_thrshd_for_ue_grant_monitoring_info_enum_type inact_thresh_cfg_grant_monitor;
  /* Will be reset after EDCH tx to the signalled value. 
  ** EDL channels will be monitiored till the below value becomes 0*/
  uint16 curr_inact_thresh_grant_monitor;
  /*Status of CPC DRX, whether it is active or not*/
  boolean cpc_drx_status_active;
  /*Current TTI*/
  e_tti_enum_type curr_tti;
  /*event id used to setup sleep time for AGCH/S_RGCH channels*/
  uint8 event_id_s_rg_agch;
   /*event id used to setup sleep time for HICH channel*/
  uint8 event_id_hich;
  /*event id used to setup sleep time for NS-RGCH channel*/
  uint8 event_id_ns_rg;
  /*need to wakeup NS RGCH used only for 2ms TTI*/
  boolean need_to_wakeup_ns_rgch_2ms;
  /*maximum NS RGCH to DPCH offset*/
  uint8 max_ns_rgch_dpch_offset;
  /*maximum S RGCH to DPCH offset*/
  uint8 max_s_rgch_dpch_offset;
  /*maximum HICH to DPCH offset*/
  uint8 max_hich_dpch_offset;
  /* Additonal information related to HICH entity*/
  wl1_edldrx_hich_ctrl_struct_type wl1_edldrx_hich_ctrl_info;

}wl1_edldrx_ctrl_struct_type;

/*Structure holding the active entity bitmask and EDL DRX Data Config DB*/
typedef struct
{
  /*Bitmask to tell us which EDLDRX entities are active*/
  uint8 active_entity_btmsk;
  /*Array of structures representing the data config DB for EDL DRX*/
  wl1_edldrx_entity_struct_type edldrx_entity_arr[WL1_EDLDRX_NUM_CHANNELS];

}wl1_edldrx_entity_db_struct_type;

/*Structure holding the timing info. for HS SCCH recep.*/

typedef struct
{
  /*Wakeup cfn for HS SCCH recep.*/
  uint8 wakeup_cfn;
  /*Wakeup time in ticks within the cfn*/
  uint8 wakeup_ticks;
  /*Wake up subfn*/
  uint16 wakeup_subfn;
}wl1_edldrx_hs_timing_info_struct_type;

/*Structure holding EUL tx info*/
typedef struct
{
  /*Is there tx in this TTI*/
  boolean tx_this_tti;
  /*Is the TTI tx sched. data*/
  boolean is_tti_sched_tx;
  /*Is TEBS non zero*/
  boolean is_tebs_non_zero;
}wl1_edldrx_eul_tx_info_struct_type;


/* -----------------------------------------------------------------------
** Extern Declarations
** ----------------------------------------------------------------------- */

extern wl1_edldrx_ctrl_struct_type wl1_edldrx_entity_ctrl_info;
extern wl1_edldrx_entity_db_struct_type wl1_edldrx_entity_db;
extern wl1_edldrx_hs_timing_info_struct_type wl1_edldrx_hs_timing_info;
extern uint16 wl1_edldrx_last_logged_sfn;

/* -----------------------------------------------------------------------
** Macro/ Define Declarations
** ----------------------------------------------------------------------- */

#define WL1_EDLDRX_ENTITY_ID_VALID(x) (( x < WL1_EDLDRX_NUM_CHANNELS) ? TRUE : FALSE)
#define WL1_EDLDRX_ENTITY_STATE_VALID(x) (( x < WL1_EDLDRX_EDL_NUM_ENTITY_STATE) ? TRUE : FALSE)
#define WL1_EDLDRX_E_DL_CHAN_VALID(x) (( x < E_DL_NUM_CHAN) ? TRUE : FALSE)
#define WL1_EDLDRX_CH_2_DPCH_OFFSET_10MS_VALID(x) ((x < 150) ? TRUE : FALSE)
#define WL1_EDLDRX_CH_2_DPCH_OFFSET_2MS_VALID(x) ((x < 30) ? TRUE : FALSE)
#define WL1_EDLDRX_HICH_RESULT_OFFSET_VALID(x)((x < WL1_EDLDRX_NUM_HICH_SUBFRAMES_OFFSET)?TRUE : FALSE)

#define WL1_EDLDRX_GET_CURR_INACT_THRESH() \
        (wl1_edldrx_entity_ctrl_info.curr_inact_thresh_grant_monitor)

#if defined(FEATURE_QTA) && defined(FEATURE_WCDMA_CPC_DRX)
#define WL1_EDLDRX_SET_CURR_INACT_THRESH(x) \
        (wl1_edldrx_entity_ctrl_info.curr_inact_thresh_grant_monitor = (uint16)x)

#define WL1_EDLDRX_GET_CURR_INACT_THRES_SIG_VALUE() \
        ((uint16)wl1_edldrx_entity_ctrl_info.inact_thresh_cfg_grant_monitor)
#endif

        
#ifdef FEATURE_WL1_DL_DRX_CTRL_DEBUG_MSG_LEVEL1
#define WL1_EDLDRX_RESET_CURR_INACT_THRES_SIG_VALUE() \
        MSG_HIGH("WL1CPCEDLDRX: resetting inact threshold from %d to %d", wl1_edldrx_entity_ctrl_info.curr_inact_thresh_grant_monitor, wl1_edldrx_entity_ctrl_info.inact_thresh_cfg_grant_monitor, 0); \
        (wl1_edldrx_entity_ctrl_info.curr_inact_thresh_grant_monitor = (uint16)wl1_edldrx_entity_ctrl_info.inact_thresh_cfg_grant_monitor)
#define WL1_EDLDRX_DECREMENT_CURR_INACT_THRES() \
        ((wl1_edldrx_entity_ctrl_info.curr_inact_thresh_grant_monitor > 0) ? wl1_edldrx_entity_ctrl_info.curr_inact_thresh_grant_monitor-- : 0);\
        MSG_HIGH("WL1CPCEDLDRX: decremented inact threshold to %d", (wl1_edldrx_entity_ctrl_info.curr_inact_thresh_grant_monitor), 0, 0)
#else
#define WL1_EDLDRX_RESET_CURR_INACT_THRES_SIG_VALUE() \
        (wl1_edldrx_entity_ctrl_info.curr_inact_thresh_grant_monitor = (uint16)wl1_edldrx_entity_ctrl_info.inact_thresh_cfg_grant_monitor)
#define WL1_EDLDRX_DECREMENT_CURR_INACT_THRES() \
        ((wl1_edldrx_entity_ctrl_info.curr_inact_thresh_grant_monitor > 0) ? wl1_edldrx_entity_ctrl_info.curr_inact_thresh_grant_monitor-- : 0)
#endif

#define WL1_EDLDRX_IS_CURR_INACT_THRES_ZERO() \
        ((wl1_edldrx_entity_ctrl_info.curr_inact_thresh_grant_monitor == 0) ? TRUE : FALSE)

#define WL1_EDLDRX_WAKEUP_TICKS_VALID(x) WL1_EDLDRX_CH_2_DPCH_OFFSET_10MS_VALID(x)
 
#define WL1_EDLDRX_DRX_CYCLE_LEN_VALID(x) ((x <= 20) ? TRUE : FALSE)


/*extern functions for edldrx*/
/*===========================================================================

FUNCTION        wl1_edldrx_entity_init

DESCRIPTION     This function will initialize the edldrx entities.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void   wl1_edldrx_entity_init(void);

/*===========================================================================

FUNCTION        wl1_edldrx_validate_params

DESCRIPTION     This function is called from l1msetup when new config is rcvd. 
                with drx params to validate edl drx params. 

DEPENDENCIES    None

RETURN VALUE    BOOLEAN

SIDE EFFECTS    None

===========================================================================*/
//extern boolean wl1_edldrx_validate_params(wl1_setup_drx_ctrl *);

/*===========================================================================

FUNCTION        wl1_edldrx_cfg_drx_ctrl_params

DESCRIPTION     This function will be called to setup EDL DRX Ctrl DB. Called 
                from EDL Config Module after Frame boundary event happens.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_cfg_drx_ctrl_params(l1_drx_info_struct_type *);

/*===========================================================================

FUNCTION        wl1_edldrx_cfg_entity_entry

DESCRIPTION     This function will register the edldrx entities with EDL module. 
                This function will be called from EDL CONFIG MODULE
                (after Frame boundary event happens). 

DEPENDENCIES    None

RETURN VALUE    UINT8

SIDE EFFECTS    None

===========================================================================*/
extern uint8 wl1_edldrx_register_entity_entry(e_dl_chan_enum_type);

/*===========================================================================

FUNCTION        wl1_edldrx_release_entity_entry

DESCRIPTION     This function will release entity entry as indicated by entity id.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_release_entity_entry(uint8);

/*===========================================================================

FUNCTION        wl1_edldrx_release_all_entities

DESCRIPTION     This function will set the state of EDL DRX entities to inactive.
                Function will be called when DRX goes inactive.
DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_release_all_entities(void);

/*===========================================================================

FUNCTION        wl1_edldrx_check_entity_state

DESCRIPTION     This function will return the state corresp. to the entity ID passed
                as argument by DRX Ctrlr.
DEPENDENCIES    None

RETURN VALUE    wl1_edldrx_entity_state_enum_type

SIDE EFFECTS    None

===========================================================================*/
extern wl1_edldrx_entity_state_enum_type wl1_edldrx_check_entity_state(uint8);

/*===========================================================================

FUNCTION        wl1_edldrx_update_cpc_drx_status

DESCRIPTION     This function will be called to set ctrl. info. regarding 
                CPC DRX STATUS. Will be called BY DRX Ctrlr when DRX
                goes inactive/active.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_update_cpc_drx_status(boolean);

/*===========================================================================

FUNCTION        wl1_edldrx_cpc_drx_active

DESCRIPTION     This function is called to find out if DRX is active or not 
                from FIQ handler. 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
boolean wl1_edldrx_cpc_drx_active(void);

/*===========================================================================

FUNCTION        wl1_edldrx_wakeup_handler

DESCRIPTION     This function will set state to demod_active for the entities 
                indicated by DRX CTRLR ID. Fucntion is registered with DRX Ctrlr.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_wakeup_handler(uint16);

/*===========================================================================

FUNCTION        wl1_edldrx_sleep_handler

DESCRIPTION     This function will set state to demod_inactive for the entities 
                indicated by DRX CTRLR ID. Fucntion is registered with DRX Ctrlr.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_sleep_handler(uint16);

/*===========================================================================

FUNCTION        wl1_edldrx_update_smallest_grant_ch_dpch_offset

DESCRIPTION     This function will update smallest grant channel to dpch offset 
                ctrl info. for EDL Channels. Function Will be called from EDL
                Config module.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_update_smallest_grant_ch_dpch_offset(uint8);

/*===========================================================================

FUNCTION        wl1_edldrx_next_sched_edch_tx

DESCRIPTION     This function will update upcoming tx info, required by EDL DRX 
                module. Function will be called from EUL FIQ handler.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_next_sched_edch_tx(uint16);

/*===========================================================================

FUNCTION        wl1_edldrx_update_smallest_agch_to_dpch_offset

DESCRIPTION     This function will update smallest agch or S-RGCH to DPCH offset. 
                Note that offset is between 0..29 for 2ms TTI and 0..149 for
                10ms TTI. EDL CFG calls this function and provides offset values
                during config time.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_update_smallest_agch_to_dpch_offset(uint8, e_tti_enum_type, uint8);

/*===========================================================================

FUNCTION        wl1_edldrx_update_smallest_s_rgch_to_dpch_offset

DESCRIPTION     This function will update smallest  S-RGCH to DPCH offset. 
                Note that offset is between 0..29 for 2ms TTI and 0..149 for
                10ms TTI. EDL CFG calls this function and provides offset values
                during config time.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_update_smallest_s_rgch_to_dpch_offset(uint8 , e_tti_enum_type, uint8);

/*===========================================================================

FUNCTION        wl1_edldrx_update_smallest_ns_rgch_to_dpch_offset

DESCRIPTION     This function will update smallest NS-to-DPCH offset. 
                Note that offset is always 0..149 . EDL CFG calls this
                function and provides offset values during config time.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_update_smallest_ns_rgch_to_dpch_offset(uint8, uint8);

/*===========================================================================

FUNCTION        wl1_edldrx_update_smallest_hich_to_dpch_offset

DESCRIPTION     This function will update smallest HICH-to-DPCH offset. 
                Note that offset is between 0..29 for 2ms TTI and 0..149 for
                10ms TTI. Also this function updates the offset in terms of DPCH subfn
                where after EDCH tx, HICH results will be obtained. EDL CFG calls this
                function and provides offset values during config time.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_update_smallest_hich_to_dpch_offset(uint8, uint8, e_tti_enum_type, uint8, uint8);

/*===========================================================================

FUNCTION        wl1_edldrx_update_eul_tti

DESCRIPTION     This function will update EUL TTI signalled by the network . 
                EDL CFG calls this function and provides offset values during
                config time.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_update_eul_tti(e_tti_enum_type);

/*===========================================================================

FUNCTION        wl1_edldrx_ready_to_sleep

DESCRIPTION     This function is called when inactivity timer for grant monitor is 
                zero and hence need to DRX ctlr can go to sleep for EDL DRX perspective. 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_ready_to_sleep(e_dl_chan_enum_type);

/*===========================================================================

FUNCTION        wl1_edldrx_get_agch_entity_state

DESCRIPTION     This function is called to find out edl drx entity state for 
                AGCH. 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern wl1_edldrx_entity_state_enum_type wl1_edldrx_get_agch_entity_state(void);
/*===========================================================================

FUNCTION        wl1_edldrx_get_edl_entity_state

DESCRIPTION     This function is called to find out edl drx entity state.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/

extern wl1_edldrx_entity_state_enum_type wl1_edldrx_get_edl_entity_state(e_dl_chan_enum_type edl_chan);
/*===========================================================================

FUNCTION        wl1_edldrx_validate_edldrx_params

DESCRIPTION     This function validates the edldrx ctrl params signalled by 
                network.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern boolean wl1_edldrx_validate_edldrx_params(uint8 drx_req_mask, l1_drx_info_struct_type *drx_ctrl_info);

/*===========================================================================

FUNCTION        wl1_edldrx_dlhs_sleep_cb

DESCRIPTION     This function will be called to save the next HS SCCH reception 
                timing info. 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_dlhs_sleep_cb(uint8, uint8);

/*===========================================================================

FUNCTION        wl1_edldrx_program_sleep_wakeup_times_for_edl_entities

DESCRIPTION     


DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    None

===========================================================================*/

extern void wl1_edldrx_program_sleep_wakeup_times_for_edl_entities(
              uint16, e_tti_enum_type, boolean, boolean, boolean);


/*===========================================================================

FUNCTION        wl1_edldrx_max_ag_s_rg_dpch_offset

DESCRIPTION     This function is called to find out max. ch-to-dpch offset 
                between agch and s-rgch. 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern uint8 wl1_edldrx_max_ag_s_rg_dpch_offset(void);

/*===========================================================================

FUNCTION        wl1_edldrx_hs_wakeup_handler

DESCRIPTION     This function is called when after HS SCCH recep. it is time to turn 
                off the rcvr. following EDL channel decode. 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/

extern void wl1_edldrx_hs_wakeup_handler(uint16 subfn_sleep, uint8 offset);
/*===========================================================================

FUNCTION        wl1_edldrx_get_entity_id

DESCRIPTION     Get entity ID for the given EDL channel

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern uint8 wl1_edldrx_get_entity_id(e_dl_chan_enum_type);

/*===========================================================================

FUNCTION        wl1_edl_chan_results_to_be_ignored

DESCRIPTION     Find out if EDL channels results need to be ignored as expected
                when EDL DRX entity is demod_inactive. 
                  
DEPENDENCIES    

RETURN VALUE    boolean

SIDE EFFECTS    None

===========================================================================*/
extern boolean wl1_edl_chan_results_to_be_ignored(e_dl_chan_enum_type e_dl_chan);

/*===========================================================================

FUNCTION        wl1_edldrx_update_max_ns_rgch_dpch_offset

DESCRIPTION     
                  
DEPENDENCIES    

RETURN VALUE    void

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_update_max_ns_rgch_dpch_offset(uint8);
/*===========================================================================

FUNCTION        wl1_edldrx_update_max_s_rgch_dpch_offset

DESCRIPTION     
                  
DEPENDENCIES    

RETURN VALUE    void

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_update_max_s_rgch_dpch_offset(uint8);
/*===========================================================================

FUNCTION        wl1_edldrx_update_max_hich_dpch_offset

DESCRIPTION     
                  
DEPENDENCIES    

RETURN VALUE    void

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_update_max_hich_dpch_offset(uint8);

/*===========================================================================

FUNCTION        wl1_edldrx_reset_inact_thresh

DESCRIPTION     This function resets the inact threshold. 
                Called from HS entity after HS Sync is established.  
DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/

extern void wl1_edldrx_reset_inact_thresh(void);

/*===========================================================================

FUNCTION        wl1_edldrx_log_params_10ms_tti

DESCRIPTION     This function is called from HS entity every 2ms TTI but 
                logging will happen only in the first subfn.  
DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_log_params_10ms_tti(void);

/*===========================================================================
FUNCTION         E_DL_UPDATE_EDL_DRX_ENTITIES

DESCRIPTION    Registers/releases entites based on the current config. Also recomputes the entity
                      channel to DPCH offsets.

                 
DEPENDENCIES  Takes input from EUL DL config DB

RETURN VALUE  The values in e_dl_config_db.e_dl_drx_params get updated   

SIDE EFFECTS     

===========================================================================*/
extern void e_dl_update_edl_drx_entities(wl1_ul_carr_id_enum_type carr_idx);


#endif
#endif/*WL1_CPCDRX_EDL_ENTITY_H*/
