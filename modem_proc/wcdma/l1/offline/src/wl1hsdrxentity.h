#ifndef WL1HSDRXENTITY_H
#define WL1HSDRXENTITY_H
/*===========================================================================
                 WCDMA L1 DOWNLINK HS DRX ENTITY module

GENERAL DESCRIPTION
  This file is the header file for the code to carry out hs channel wakeup
  and teardown according to CPC DRX schedule 

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2007 - 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/wl1hsdrxentity.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $       


when       who      what, where, why
--------   ---      -------------------------------------------------------- 
05/09/14   sr       Fixed 0x423A logging issue
01/27/12   hk       Changes to distinguish the type of SCCH order
07/07/11   hk       Up-merge from offline/main/latest
04/28/11   sj       upmerge from offline/main
04/25/11   geg      Replaced *.h with *_v.h
04/08/11   rgn      Made some logging changes
03/31/11   amj      Fixed merging error.
03/31/11   amj      Added code for setting/getting cfn-cfndrx offset.
03/23/11   rgn      REorganized logging changes 
09/08/10   dm       Added changes for 0x423A log packet
08/16/10   dm       Added support for logging CPC DRX parameters.
03/05/10   rgn      First cut
===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_WCDMA_CPC_DRX
#include "wl1dldrxctrl.h"

#ifdef WL1_DL_DRX_CTRL_UNIT_TEST
#include "test_main.h"
#else
#include "seq_v.h"
#endif

#define WL1_HSDRX_MAX_NUM_FW_PARAM_BUFS 2
#define WL1_HSDRX_INVALID_FW_BUF_IDX 0xFF
#define WL1_HSDRX_NO_DRX_CYCLE_LEN 1
#define WL1_HSDRX_NO_DRX_INACT_THRESH_LEN 0
#define WL1_HSDRX_NO_DRX_DTX_DRX_OFFSET_LEN 0
#define WL1_HSDRX_SCCH_ORDER_READ_DONE_FLAG 0xFFFF

#define WL1_HSDRX_SCCH_ORDER_DRX_BIT_MASK 0x1
#define WL1_HSDRX_SCCH_ORDER_DRX_BIT_SHFT 0

#define WL1_HSDRX_SCCH_ORDER_DC_BIT_MASK 0x2
#define WL1_HSDRX_SCCH_ORDER_DC_BIT_SHFT 1

#define WL1_HSDRX_GET_ENTITY_STATE() \
        wl1_hsdrx_entity_db.entity_state

/** enum state describing the entity's awake state */
typedef enum
{
  /** entity is inactive */
  WL1_HSDRX_ENTITY_INACTIVE,
  /** entity is configured but scheduling hasnt started */
  WL1_HSDRX_ENTITY_CONFIGURED,
  /** entity is actively demodulating data */
  WL1_HSDRX_ENTITY_DEMOD_ACTIVE,
  /** entity is demodulating but in inactive monitoring */
  WL1_HSDRX_ENTITY_DEMOD_ACTIVE_INACTIVE_MONITORING,
  /** entity is not demodulating and is currently asleep */
  WL1_HSDRX_ENTITY_DEMOD_INACTIVE,
  /** max num entity states */
  WL1_HSDRX_ENTITY_MAX_NUM_STATES
}wl1_hsdrx_entity_state_enum_type;

typedef enum
{
  /** higher mips request */
  WL1_HSDRX_REQ_HIGHER_MIPS,
  /** higher mips relinquish */
  WL1_HSDRX_REL_HIGHER_MIPS,
  /** invalid mips val */
  WL1_HSDRX_INV_HIGHER_MIPS
}wl1_hsdrx_mips_req_enum_type;

/** structure holding hs drx entity's drx params */
typedef struct
{
  /** hs drx cycle lenght in subframes   */
  uint8 hs_drx_cycle_len;
  /** dtx-drx offset */
  uint16 dtx_drx_offset;
  /** inactivity threshold in subframes   */
  uint16 hs_drx_inactivity_threshold_len;
}wl1_hsdrx_ctrl_params_struct_type;

/** structure containing all hs drx information */
typedef struct
{
  /** is scheduling needed */
  uint8 scheduling_needed;
  /** entity id provided by drx ctrlr   */
  uint8 drx_ctrlr_id;
  /** entity state */
  wl1_hsdrx_entity_state_enum_type entity_state;
  /** m0 val */
  uint8 hs_cfnx5_subfr_offset_val;
  /** cfn-cfndrx offset */
  uint8 hs_cfn_cfn_drx_offset;
  /** params configured by higher layers */
  wl1_hsdrx_ctrl_params_struct_type cfged_drx_params;
  /** current drx state params */
  wl1_hsdrx_ctrl_params_struct_type curr_drx_ctrl_params;
  /** timing occasion info   */
  wl1_dl_drxctrl_time_var_struct_type proged_timing_occ;
  /** channel timing wrt rxtl */
  tlm_ch_cfn_info_struct_type ch_timing;
}wl1_hsdrx_entity_struct_type;

/** structure containing mdsp fw buf params */
typedef struct
{
  /** drx offset */
  uint16 drx_offset;
  /** drx cycle length */
  uint16 drx_cycle_length;
  /** drx inact threshold length */
  uint16 drx_inact_threshold_length;
}wl1_hsdrx_fw_param_struct_type;

typedef struct
{
  /** drx offset */
  uint32 *drx_offset;
  /** drx cycle length */
  uint8 *drx_cycle_length;
  /** drx inact threshold length */
  uint16 *drx_inact_threshold_length;
}wl1_hsdrx_fw_param_ptr_struct_type;

typedef struct
{
  /** current idx in use */
  uint16 curr_fw_buf_idx;
  /** fw buf ptrs */
  wl1_hsdrx_fw_param_ptr_struct_type fw_buf[WL1_HSDRX_MAX_NUM_FW_PARAM_BUFS];
}wl1_hsdrx_fw_cfg_buf_struct_type;

typedef struct
{
  /** access type */
  boolean ops_type;
  /** correspoding cfn num */
  uint8 cfn_num;
  /** mod_val */
  uint8 mod_val;
  /** offset from curr time */
  uint8 offset_subframes;
  /** CPC-DRX Logging CFN count */
  uint8 cfn_count;
  /** subframe input/output */
  uint16 subfr_num;
}wl1_hsdrx_subfr_access_struct_type;

extern wl1_hsdrx_entity_struct_type wl1_hsdrx_entity_db;

/*===========================================================================

FUNCTION        wl1_hsdrx_sleep_handler

DESCRIPTION     This function sleep handler

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_hsdrx_sleep_handler(void);

/*===========================================================================

FUNCTION        wl1_hsdrx_wakeup_handler

DESCRIPTION     This function sleep handler

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_hsdrx_wakeup_handler(void);

/*===========================================================================

FUNCTION        wl1_hsdrx_drx_scheduling_ctrl

DESCRIPTION     This function starts/stops scheduling procedures

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_hsdrx_drx_scheduling_ctrl(
  /** flag to indicate whether scheduling should be started */
  boolean start_scheduling);

/*===========================================================================

FUNCTION        wl1_hsdrx_scch_decode_done_isr

DESCRIPTION     This function handles the scch decode done interrupt from mdsp

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_hsdrx_scch_decode_done_isr(void);

/*===========================================================================

FUNCTION        wl1_hsdrx_entity_init

DESCRIPTION     This function initialises the hs drx entity database

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_hsdrx_entity_init(void);

/*===========================================================================

FUNCTION        wl1_hsdrx_ctrl_is_hs_entity_active

DESCRIPTION     This function returns TRUE if hs drx mode of operation is active

DEPENDENCIES    None

RETURN VALUE    True/False

SIDE EFFECTS    None

===========================================================================*/  
extern boolean wl1_hsdrx_ctrl_is_hs_entity_active(void);

/*===========================================================================

FUNCTION        wl1_hsdrx_validate_hsdrx_params

DESCRIPTION     This function validates hsdrx params received from RRC

DEPENDENCIES    None

RETURN VALUE    True/False based upon validation success/failure

SIDE EFFECTS    None

===========================================================================*/
extern boolean wl1_hsdrx_validate_hsdrx_params(
  /** drx request mask */
  uint8 drx_req_mask, 
  /** input drx info ptr */
  l1_drx_info_struct_type *inp_drx_info);

/*===========================================================================

FUNCTION        wl1_hsdrx_ctrl_initiate_sleep_with_drxctrl

DESCRIPTION     This function initiates sleep time with drx ctrlr. This is actually 
                a isr handler for the hs comb chan event 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/   
extern void wl1_hsdrx_ctrl_initiate_sleep_with_drxctrl(void);

/*===========================================================================

FUNCTION        wl1_hsdrx_configure_drx_params

DESCRIPTION     This function hs drx parameters

DEPENDENCIES    None

RETURN VALUE    True/False based upon success/failure

SIDE EFFECTS    None

===========================================================================*/
extern boolean wl1_hsdrx_configure_drx_params(
  /** drx req mask */
  uint8 drx_req_mask,
  /** drx info from rrc */
  l1_dtx_drx_info_struct_type *drx_dtx_info);

/*===========================================================================

FUNCTION        wl1_hsdrx_ret_curr_fw_buf_idx

DESCRIPTION     This function returns current fw buf idx

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/  
extern uint8 wl1_hsdrx_ret_curr_fw_buf_idx(void);

/*===========================================================================

FUNCTION        wl1_hsdrx_stop_drx_mode

DESCRIPTION     This function stops drx mode of operation

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_hsdrx_stop_drx_mode(void);

/*===========================================================================

FUNCTION        wl1_hsdrx_get_drx_cycle_len

DESCRIPTION     This function returns current drx cycle lenght to caller

DEPENDENCIES    DRX entity should be active

RETURN VALUE    DRX cycle lenght, caller should check validity of the return val

SIDE EFFECTS    None

===========================================================================*/    
extern uint8 wl1_hsdrx_get_drx_cycle_len(void);

/*===========================================================================

FUNCTION        wl1_hsdrx_get_cfn_cfndrx_offset

DESCRIPTION     This function returns the offset between cfn and cfn drx.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/    
extern void wl1_hsdrx_set_cfn_cfndrx_offset(void);

/*===========================================================================

FUNCTION        wl1_hsdrx_ctrl_access_last_decoded_subfr

DESCRIPTION     This function last decoded subframe

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/    
extern void wl1_hsdrx_ctrl_access_last_decoded_subfr_data(
  wl1_hsdrx_subfr_access_struct_type *access_type_ptr);

/*===========================================================================

FUNCTION        wl1_hsdrx_get_cfn_cfndrx_offset

DESCRIPTION     This function returns the offset between cfn and cfn drx.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/    
extern uint8 wl1_hsdrx_get_cfn_cfndrx_offset(void);

/*===========================================================================

FUNCTION        wl1_hsdrx_get_dtx_drx_offset

DESCRIPTION     This function returns current dtx drx offset  to caller

DEPENDENCIES    DRX entity should be active

RETURN VALUE    DTX DRX offset, caller should check validity of the return val

SIDE EFFECTS    None

===========================================================================*/ 
extern uint16 wl1_hsdrx_get_dtx_drx_offset(void);

/*===========================================================================

FUNCTION        wl1_edldrx_log_drxactivity

DESCRIPTION     This function logs drx activity
                  
DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_edldrx_log_drxactivity(uint32 log_field_mask, uint16 log_field_val);

/*===========================================================================

FUNCTION        wl1_hsdrx_populate_fw_drx_intf_params

DESCRIPTION     This function populates the DRX parameters, DRX cycle length
                and Inactivity threshold to the FW-SW interface buffer
                  
DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void wl1_hsdrx_populate_fw_drx_intf_params(uint8 , uint16 );

#endif /* FEATURE_WCDMA_CPC_DRX */
#endif /*WL1HSDRXENTITY_H*/
