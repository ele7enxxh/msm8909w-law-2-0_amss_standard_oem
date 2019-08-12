#ifndef DLPHCHI_H
#define DLPHCHI_H

/*===========================================================================
                 WCDMA L1 DOWNLINK PHYSICAL CHANNEL CONFIGURATION

GENERAL DESCRIPTION
  This file contains the code for handling validation, configuration and
  management of downlink physical channels 
  (PCCPCH, SCCPCH,  DPCH, AICH, PICH and MICH).

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dlcmd.c_v   1.109   02 Jul 2002 18:11:42   cemani  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/dlphchi.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------- 
06/23/14   sad     Moving DC-HSUPA specific code under FEATURE_WCDMA_DC_HSUPA flag
03/18/14   mit     F3 cleanup, enabling via NV's
05/13/14   ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
02/27/14   pr      Increase the dl cell holdoff count to 40 frames.
05/21/13   sad     Added DC HSUPA code changes under FEATURE_WCDMA_DC_HSUPA 
11/6/12    kcm     Changes to reduce f3 prints in idle mode.
08/07/12   dp      W2L PSHO time improvements
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/11/12   vs      MBMS feature cleanup.
06/08/12   gv      Feature HS RACH Initial code check in
02/10/12   hk      Nikel feature cleanup
01/28/12   vs      Nikel feature cleanup.
08/29/11   kcm     Added structure which contains params for handling the DCH drop/reconfig 
                   after CM  suspend/freeze 
05/25/09   hk      Fixed compiler warnings
04/17/09   rgn     Fixed compiler warnings
12/22/08   ks      Mainlinign FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT
12/10/08   ks      Mainlining FEATURE_WCDMA_DL_ENHANCED
11/19/08   vsr     Changes to support Compressed Mode
10/06/08   vsr     Fixes for Active Set Update procedure
10/03/08   hk      Bringing in 7k Mailine fixes
09/12/08   rgn     Added SHO support
08/20/07   vsr     Moved in dl_get_data_bits_per_frame()
07/05/07   vsr     Modified notify_actions mask to actions mask in order to handle 
                   pre MCAL operations.
                   Added PSC and freq fields in dl_phch_rl_db to allow flexibility 
                   to deal with loss of cell db idx during BPLMN procs
                   Moved old_phch_cfg_db_idx field from combiner db to phch cfg db to 
                   facilitate recfg procedure in suspend state
                   Added code to handle pending recfg
                   Removed dl_phch_find_mcal_dl_operation                 
06/20/07   rgn     Added mich support
06/18/07   rgn     Added support for HSDPA and added apis to retrieve various phch params
06/08/07   vsr     MCAL_DL code cleanup changes
05/10/07   rgn     Added macro to reset delete old cctrch actions
05/02/07   rgn     Added support for compressed mode
04/20/07   vsr     Lint changes
03/27/07   vsr     Modularised functions to copy dpch rl info
02/21/07   scm     Added support for the new drx mgr
02/02/07   scm     cros to undo links between combiner db and phch config db.
01/26/06   vb      Added code to support pich tto additional i/f parameter
10/17/06   vp/vb   Created this module

===========================================================================*/


/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "dlphychanlog.h"

#include "l1rrcif.h"
#include "l1const.h"
#include "mcalwcdma_demod.h"
#include "mcalwcdma_dl.h"
#include "dlphch.h"

#include "tlm.h"

/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define DL_PHCH_NO_DATA_BITS 0

#define DL_PHCH_ACTIONS_ALLOCATE_CFG_DB_BMSK                        0x00000001
#define DL_PHCH_ACTIONS_DISABLE_CCTRCH_BMSK                         0x00000002
#define DL_PHCH_ACTIONS_RESET_CM_ACTIVE_BMSK                        0x00000004
#define DL_PHCH_ACTIONS_SUSPEND_TRIAGE_BMSK                         0x00000008
#define DL_PHCH_ACTIONS_GET_FREE_COMB_ID_BMSK                       0x00000010
#define DL_PHCH_ACTIONS_RSV_COMB_ID_BMSK                            0x00000020
#define DL_PHCH_ACTIONS_SYNC_DB_BMSK                                0x00000040
#define DL_PHCH_ACTIONS_ISSUE_MCAL_ADD_BMSK                         0x00000080
#define DL_PHCH_ACTIONS_ISSUE_MCAL_DROP_BMSK                        0x00000100
#define DL_PHCH_ACTIONS_ISSUE_MCAL_RECFG_BMSK                       0x00000200
#define DL_PHCH_ACTIONS_ISSUE_MCAL_DROP_OLD_CFG_BMSK                0x00000400
#define DL_PHCH_ACTIONS_RESUME_TRIAGE_BMSK                          0x00000800
#define DL_PHCH_ACTIONS_ENABLE_CCTRCH_BMSK                          0x00001000
#define DL_PHCH_ACTIONS_FREE_CCTRCH_BMSK                            0x00002000
#define DL_PHCH_ACTIONS_UNRSV_COMB_ID_BMSK                          0x00004000
#define DL_PHCH_ACTIONS_DEREF_COMB_ID_BMSK                          0x00008000
#define DL_PHCH_ACTIONS_DEALLOCATE_CFG_DB_BMSK                      0x00010000
#define DL_PHCH_ACTIONS_FREE_OLD_CCTRCH_N_DEALLOCATE_OLDCFG_DB_BMSK 0x00020000
#define DL_PHCH_ACTIONS_CHECK_CELL_CONFIG                           0x00040000
#define DL_PHCH_ACTIONS_CM_CLEANUP_BMSK                             0x00080000

#define DL_PHCH_ACTIONS_MCAL_OP (DL_PHCH_ACTIONS_ISSUE_MCAL_ADD_BMSK| \
                                                              DL_PHCH_ACTIONS_ISSUE_MCAL_DROP_BMSK| \
                                                              DL_PHCH_ACTIONS_ISSUE_MCAL_RECFG_BMSK| \
                                                              DL_PHCH_ACTIONS_ISSUE_MCAL_DROP_OLD_CFG_BMSK)

#define DL_PHCH_INVALID_CELL_CONFIG_EVENT 0xFF

#define DL_PHCH_IS_ACTION_NEEDED(bmsk, phch_cfg_db_idx) \
  (((dl_phch_cfg_db[phch_cfg_db_idx].actions_bmsk&bmsk) != 0)?TRUE:FALSE)
 
/* Not done yet */
#define DL_PHCH_IS_MCALOP_NEEDED(phch_cfg_db_idx) \
  (((dl_phch_cfg_db[phch_cfg_db_idx].actions_bmsk&DL_PHCH_ACTIONS_MCAL_OP) != 0)?TRUE:FALSE)

#define DL_PHCH_ACTION_SET(bmsk, actions_bmsk) \
  (actions_bmsk|= bmsk)

#define DL_PHCH_ACTION_SET_DB(bmsk, phch_cfg_db_idx) \
  (dl_phch_cfg_db[phch_cfg_db_idx].actions_bmsk|= bmsk)

#define DL_PHCH_ACTION_RESET(bmsk, phch_cfg_db_idx) \
  (dl_phch_cfg_db[phch_cfg_db_idx].actions_bmsk &= ~bmsk)

                                                                                             
#define DL_PHCH_ACTION_RESET_ALL(phch_cfg_db_idx) \
  (dl_phch_cfg_db[phch_cfg_db_idx].actions_bmsk = 0x00)
                                                                                             
#define DL_PHCH_ACTION_RESET_ALL_PRE_MCAL(phch_cfg_db_idx) \
  (dl_phch_cfg_db[phch_cfg_db_idx].actions_bmsk &= 0xFFFFFF80)
                                                                                             
#define DL_PHCH_ACTION_RESET_MCALOPS(phch_cfg_db_idx) \
  (dl_phch_cfg_db[phch_cfg_db_idx].actions_bmsk &= ~DL_PHCH_ACTIONS_MCAL_OP)

#define DL_PHCH_GET_FR_OFF_FROM_PHCH_CFG_IDX(phch_cfg_idx)    \
  (dl_phch_cfg_db[phch_cfg_idx].cfg_params.frame_offset)

#define DL_PHCH_GET_TTO_FROM_PHCH_CFG_IDX(phch_cfg_idx)    \
  (dl_phch_cfg_db[phch_cfg_idx].cfg_params.trans_time_offset)

#define DL_PHCH_GET_FREQ_FROM_PHCH_CFG_IDX(phch_cfg_idx)  \
    (dl_phch_cfg_db[phch_cfg_idx].cfg_params.freq)

#define DL_PHCH_GET_PSC_FROM_PHCH_CFG_IDX(phch_cfg_idx)  \
    (dl_phch_cfg_db[phch_cfg_idx].cfg_params.psc)

#define DL_PHCH_CONN_MSG(str, a, b, c) \
if (dl_phch_dbg_msgs_enable) \
{ \
 MSG_HIGH(str, a, b, c); \
}\
else \
{ \
  MSG_LOW(str, a, b, c); \
}

#define DL_PHCH_MSG(str, a, b, c) \
if (dl_phch_dbg_msgs_nv_enable) \
{ \
 MSG_HIGH(str, a, b, c); \
}\
else \
{ \
  MSG_LOW(str, a, b, c); \
}

#define DL_PHCH_MSG_4(str, a, b, c, d) \
if (dl_phch_dbg_msgs_nv_enable) \
{ \
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH, str, a, b, c, d); \
}\
else \
{ \
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_LOW, str, a, b, c, d); \
}

#define DL_PHCH_MSG_5(str, a, b, c, d, e) \
if (dl_phch_dbg_msgs_nv_enable) \
{ \
  MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH, str, a, b, c, d, e); \
}\
else \
{ \
  MSG_5(MSG_SSID_DFLT, MSG_LEGACY_LOW, str, a, b, c, d, e); \
}

#define DL_PHCH_MAX_HOLDOFF_FRAMES 40

/*--------------------------------------------------------------------------
                 STRUCTURE: DL_PHCH_rl_DB_STRUCT_TYPE

This structure contains all of the various parameters for RL physical channel
configuration process
--------------------------------------------------------------------------*/
                                          
typedef struct {
  /* cell db index that is used for this configuration */
  uint8 cell_db_idx;

  /* psc of the cell for which the rl got added */
  uint16 psc;

  /* frequency on which the rl got added */
  uint16 freq;

  /* RL physical channel config parameters to MCAL DEMOD Module which 
       typically holds OVSF, physical channel tx div mode, sec scr coe, 
       alt sec scr code mode, trans time offset, fdpch info */
  
  mcalwcdma_dl_rl_phychan_parms_struct_type cfg_params;  
}dl_phch_rl_db_struct_type;


/*--------------------------------------------------------------------------
                 STRUCTURE: DL_PHCH_COMBCHAN_DB_STRUCT_TYPE

This structure contains all of the various parameters for combiner
configuration process
--------------------------------------------------------------------------*/

typedef enum
{
  DL_PHCH_PEND_CFG_NONE,
  DL_PHCH_PEND_CFG_RECFG_TO_ACTIVATE,
  DL_PHCH_PEND_CFG_RECFG_TO_SUSPEND,  
  DL_PHCH_PEND_CFG_SUSPEND,
  DL_PHCH_PEND_CFG_RESUME,
  DL_PHCH_PEND_CFG_RESLAM,
  DL_PHCH_PEND_CFG_DROP,
  DL_PHCH_PEND_CFG_ASET_UPDATE,
  DL_PHCH_PEND_CFG_PRE_ASET_RL_DROP
} dl_phch_pend_cfg_enum_type;

typedef enum
{
  DL_PHCH_NOTIFY_NONE,
  DL_PHCH_NOTIFY_ADD,
  DL_PHCH_NOTIFY_RECFG,
  DL_PHCH_NOTIFY_DROP,
  DL_PHCH_NOTIFY_SUSPEND,
  DL_PHCH_NOTIFY_RESUME,
  DL_PHCH_NOTIFY_RESLAM,
  DL_PHCH_NOTIFY_ASET_UPDATE,
  DL_PHCH_NOTIFY_PRE_ASET_RL_DROP
}dl_phch_notify_enum_type;

typedef struct {
  /* index points to dl_phch_cfg_db array */
  dl_phch_cfg_handle_type phch_cfg_idx;

  /* will be set to TRUE if any configuration procedure 
      (add/drop/recfg/suspend/resume) is on progress*/
  boolean cfg_on_progress;

  /* TRUE indicates active cm config */
  boolean cm_cfg_active;

  /* handles notification type */
  dl_phch_notify_enum_type notify_type;

  /* holds pending config type */
  dl_phch_pend_cfg_enum_type pend_cfg_type;
  
  /* call back function pointer to indicate the clients once after 
      config operation is done */
  DL_PHCH_CFG_DONE_CB_FUNC_TYPE *cfg_done_cb;
  
  /* call back function pointer to indicate the clients once after 
       pending config operation is done */
  DL_PHCH_CFG_DONE_CB_FUNC_TYPE *pend_cfg_done_cb;

  /* index points to dl_phch_cfg_db array of pending recfg */
  dl_phch_cfg_handle_type pend_phch_cfg_idx;

  /* place holder for MCAL cfg info, this is purely for debug purpose */
  mcalwcdma_dl_phch_cfg_parms_struct_type phch_cfg_info_for_mcal;
  
}dl_phch_comb_ch_db_struct_type;

/*--------------------------------------------------------------------------
                 STRUCTURE: DL_PHCH_HOLD_CFG_DB_STRUCT_TYPE

This structure contains all of the  parameters to handle holding
configuration process
--------------------------------------------------------------------------*/

typedef struct {
  /* holds evt id for scheduled event to check locked finger */
  uint8 frm_dly_evt_id;

  /* points to combiner channel db*/
  dl_phch_comb_ch_db_handle_type comb_ch_db_idx;
  
}dl_phch_hold_cfg_db_struct_type;

/*--------------------------------------------------------------------------
                 ENUM: DL_PHCH_HOLD_CFG_DB_STRUCT_TYPE

This structure contains all of the  parameters to handle holding
configuration process
--------------------------------------------------------------------------*/
typedef enum
{
  DL_PHCH_MCAL_NOOP,
  DL_PHCH_MCAL_ADD,
  DL_PHCH_MCAL_RECFG,
  DL_PHCH_MCAL_DROP
}dl_phch_mcal_dl_op_enum_type;

/*--------------------------------------------------------------------------
                 STRUCTURE: DL_PHCH_HOLD_CFG_DB_STRUCT_TYPE

This structure contains all of the  parameters to handle holding
configuration process
--------------------------------------------------------------------------*/
typedef struct {
  boolean single_rl_per_phch;

} dl_phch_ch_properties_struct_type;

/*--------------------------------------------------------------------------
                 STRUCTURE: DL_PHCH_STMR_CTR_INFO_STRUCT_TYPE

This structure contains all of the  parameters to handle holding
configuration process
--------------------------------------------------------------------------*/

typedef struct {
boolean ctr_in_use;
boolean time_tracking_enable;
uint16 ref_finger;
tlm_cfn_struct_type trans_time_offset;
uint16 ref_cell_idx;
}dl_phch_stmr_ctr_info_struct_type;

/*--------------------------------------------------------------------------
                 STRUCTURE: dl_phch_resume_channel_ops_struct_type

This structure contains all of the  parameters to handle  DPCH drop/reconfig 
after CM suspend/freeze is done 
--------------------------------------------------------------------------*/
typedef struct
{
  /* phch_cfg_db_idx*/
  dl_phch_cfg_handle_type phch_cfg_db_idx;
  /* operation type*/
  dl_phch_notify_enum_type operation;
  /* phch next state */
  dl_phch_state_enum_type phch_next_state;
  /* cfg done ptr*/
  DL_PHCH_CFG_DONE_CB_FUNC_TYPE *phch_done_cb_ptr;
}dl_phch_resume_channel_ops_struct_type;

/*--------------------------------------------------------------------------
                 DL PHCH Notification Handler Functions
                 1. dl_phch_add_notify
                 2. dl_phch_recfg_notify
                 3. dl_phch_drop_notify
                 4. dl_phch_suspend_notify
                 5. dl_phch_resume_notify
--------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION dl_phch_add_notify

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

extern void dl_phch_add_notify(
  /* combiner channel for which add notification process needed */
  dl_phch_comb_ch_db_handle_type comb_ch_idx,

  /* status if the add operation is successful or not */
  boolean status);

/*===========================================================================
FUNCTION dl_phch_recfg_notify

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

extern void dl_phch_recfg_notify(
  /* combiner channel for which recfg notification process needed */
  dl_phch_comb_ch_db_handle_type comb_ch_idx, boolean status);

/*===========================================================================
FUNCTION dl_phch_reslam_notify

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

extern void dl_phch_reslam_notify(
  /* combiner channel for which recfg notification process needed */
  dl_phch_comb_ch_db_handle_type comb_ch_idx);

/*===========================================================================
FUNCTION dl_phch_drop_notify

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dl_phch_drop_notify(
  /* combiner channel for which drop notification process needed */
  dl_phch_comb_ch_db_handle_type comb_ch_idx, boolean status);

/*===========================================================================
FUNCTION dl_phch_suspend_notify

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dl_phch_suspend_notify(
  /* combiner channel for which suspend notification process needed */
  dl_phch_comb_ch_db_handle_type comb_ch_idx, boolean status);

/*===========================================================================
FUNCTION dl_phch_resume_notify

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dl_phch_resume_notify(
  /* combiner channel for which add notification process needed */
  dl_phch_comb_ch_db_handle_type comb_ch_idx, boolean status);

/*===========================================================================
FUNCTION dl_phch_aset_update_notify

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dl_phch_aset_update_notify(
  /* combiner channel for which add notification process needed */
  dl_phch_comb_ch_db_handle_type comb_ch_idx, boolean status);

/*===========================================================================
FUNCTION dl_phch_pre_aset_rl_drop_notify

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dl_phch_pre_aset_rl_drop_notify(
  /* combiner channel for which rl drop notification process needed */
  dl_phch_comb_ch_db_handle_type comb_ch_idx, boolean status);

/*===========================================================================
FUNCTION dl_phch_notify

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dl_phch_notify(
  /* access to physical channel database */
  dl_phch_cfg_handle_type phch_cfg_db_idx, 
  /* status for notification */
  boolean status,
  /* notification function ptr */
  DL_PHCH_CFG_DONE_CB_FUNC_TYPE *cfg_done_cb);

/*--------------------------------------------------------------------------
                 DL PHCH Pending Config Handler Functions
                 1. dl_phch_update_pend_cfg
--------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION dl_phch_update_pend_cfg

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
boolean dl_phch_update_pend_cfg(
  /* combiner channel for which pending config update needed */
  dl_phch_comb_ch_db_handle_type comb_ch_idx,
  /* combiner channel for which pending config update needed */
  dl_phch_pend_cfg_enum_type pend_cfg_type,
  /* combiner channel for which pending config update needed */
  DL_PHCH_CFG_DONE_CB_FUNC_TYPE *pend_cfg_done_cb_ptr,
  /* is this called during fast cfg */
  boolean fast_cfg);

/*--------------------------------------------------------------------------
                 DL PHCH Holding Command Handler Functions
                 1. dl_phch_update_hold_cmd
--------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION dl_phch_update_hold_cmd

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dl_phch_update_hold_cmd(
  /* combiner channel for which holding command update needed */
  dl_phch_comb_ch_db_handle_type comb_ch_idx);


/*--------------------------------------------------------------------------
                 DL PHCH Database Update Functions
                 1. dl_phch_copy_pccpch_info
                 2. dl_phch_copy_sccpch_info
                 3. dl_phch_copy_dpch_info
                 4. dl_phch_copy_aich_info
                 5. dl_phch_copy_pich_info
                 6. dl_phch_copy_mich_info
--------------------------------------------------------------------------*/
/*===========================================================================
FUNCTION dl_phch_copy_pccpch_info

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
boolean dl_phch_copy_pccpch_info(dl_phch_cfg_handle_type phch_cfg_db_idx, uint8 cctrch_idx);

/*===========================================================================
FUNCTION dl_phch_copy_sccpch_info

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
boolean dl_phch_copy_sccpch_info(
  /* to which cfg db location the information should be copied */
  dl_phch_cfg_handle_type phch_cfg_db_idx,

  /* the physical channel */
  l1_dl_phychan_enum_type phch,
  
  /* the CCTrCH table index chosen by dl_dec for this channel */
  uint8 cctrch_idx,

  /* freq on which channel being added */
  uint16 freq,

  /* pointer to the dl phychan ordered config database */
  l1_dl_phychan_db_struct_type *phychan_db);

/*===========================================================================
FUNCTION dl_phch_copy_dpch_info

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
boolean dl_phch_copy_dpch_info(
  /* to which cfg db location the information should be copied */
  dl_phch_cfg_handle_type phch_cfg_db_idx,

  /* the CCTrCH table index chosen by dl_dec for this channel */
  uint8 cctrch_idx,

  /* pointer to the ordered config database */
  l1_dl_phychan_db_struct_type *phychan_db);

/*===========================================================================
FUNCTION dl_phch_copy_hsrach_fdpch_info

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
boolean dl_phch_copy_hsrach_fdpch_info(
dl_phch_cfg_handle_type phch_cfg_db_idx,
l1_dl_phychan_db_struct_type *phychan_db);

#ifdef FEATURE_WCDMA_DC_HSUPA
/*===========================================================================
FUNCTION dl_phch_copy_sec_fdpch_info

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
boolean dl_phch_copy_sec_fdpch_info(
dl_phch_cfg_handle_type phch_cfg_db_idx,
l1_dl_phychan_db_struct_type *phychan_db);
#endif /* FEATURE_WCDMA_DC_HSUPA */

/*===========================================================================
FUNCTION dl_phch_copy_aich_info

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
boolean dl_phch_copy_aich_info(
  /* to which cfg db location the information should be copied */
  dl_phch_cfg_handle_type cfg_db_idx,
  
  /* pointer to the ordered config database */
  l1_dl_phychan_db_struct_type *phychan_db);

/*===========================================================================
FUNCTION dl_phch_copy_pich_info

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
boolean dl_phch_copy_pich_info(
  /* to which cfg db location the information should be copied */
  dl_phch_cfg_handle_type cfg_db_idx,
  
  /* pointer to the ordered config database */
  l1_dl_phychan_db_struct_type *phychan_db);


/*===========================================================================
FUNCTION dl_phch_pend_cfg_handler

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dl_phch_pend_cfg_handler(
  /* combiner channel for which pending config handling needed */
  dl_phch_comb_ch_db_handle_type comb_ch_idx);

/*--------------------------------------------------------------------------
                 DL PHCH Database Maintenance Functions
                 1. dl_phch_allocate_cfg_db
                 2. dl_phch_allocate_rl_db
                 3. dl_phch_deallocate_cfg_db
                 4. dl_phch_deallocate_rl_db
--------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION dl_phch_allocate_cfg_db

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

dl_phch_cfg_handle_type dl_phch_allocate_cfg_db(void);

/*===========================================================================
FUNCTION dl_phch_allocate_rl_db

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

dl_phch_rl_handle_type dl_phch_allocate_rl_db(void);
/*===========================================================================
FUNCTION dl_phch_allocated_and_add_cfg_db

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

dl_phch_cfg_handle_type dl_phch_allocated_and_add_cfg_db(uint8 num_rl);
/*===========================================================================
FUNCTION dl_phch_deallocate_cfg_db

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void dl_phch_deallocate_cfg_db(dl_phch_cfg_handle_type phch_cfg_db_idx);

/*===========================================================================
FUNCTION dl_phch_free_cfg_db

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void dl_phch_free_cfg_db(dl_phch_cfg_handle_type phch_cfg_db_idx);

/*===========================================================================
FUNCTION dl_phch_deallocate_rl_db

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void dl_phch_deallocate_rl_db(dl_phch_rl_handle_type rl_db_idx);

/*===========================================================================
FUNCTION dl_phch_copy_dpch_rl_info

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_phch_copy_dpch_rl_info(dl_phch_cfg_handle_type phch_cfg_db_idx, l1_dl_phychan_db_struct_type *phychan_db);


/*===========================================================================
FUNCTION        DL_PHCH_COPY_DPCH_INDIVIDUAL_RL_INFO

DESCRIPTION     

DEPENDENCIES   

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
boolean dl_phch_copy_dpch_individual_rl_info(uint8 phch_rl_db_idx, 
                                                l1_dl_phychan_db_struct_type*  phychan_db,
                                                uint8 ordered_cfg_rl_idx,
                                                dl_phch_cfg_handle_type phch_cfg_db_idx);


/*===========================================================================
FUNCTION dl_phch_configure

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_phch_configure(dl_phch_cfg_handle_type phch_cfg_db_idx,
                                      dl_phch_notify_enum_type operation,
                                      dl_phch_state_enum_type phch_next_state,
                                      DL_PHCH_CFG_DONE_CB_FUNC_TYPE *phch_done_cb_ptr);


/*===========================================================================
FUNCTION dl_phch_configure_undo

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dl_phch_configure_undo(uint32 bmsk, dl_phch_cfg_handle_type phch_cfg_db_idx);

/*===========================================================================
FUNCTION dl_phch_configure_ops

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_phch_configure_ops(
  dl_phch_cfg_handle_type phch_cfg_db_idx,
  dl_phch_notify_enum_type operation,
  DL_PHCH_CFG_DONE_CB_FUNC_TYPE *phch_done_cb_ptr);

/*===========================================================================
FUNCTION dl_phch_update_actions_mask

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint32 dl_phch_update_actions_mask( dl_phch_notify_enum_type operation,
                                            dl_phch_state_enum_type phch_cur_state,
                                            dl_phch_state_enum_type phch_next_state);

/*===========================================================================
FUNCTION        dl_phch_hho_save_phch_params_info

DESCRIPTION     This function saves necessary phch params which may be used for
                resuming back to old config upon HHO failure.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dl_phch_hho_save_phch_params_info(void);


/*===========================================================================
FUNCTION        DL_PHCH_COMMON_PHYCHAN_LOG

FILE NAME       dlphch.c

DESCRIPTION     This function logs a common physical channel either coming up
                or going down.

DEPENDENCIES    The phychan database

RETURN VALUE    None

SIDE EFFECTS    a common phychan log packet may be sent
===========================================================================*/
void dl_phch_common_phychan_log
(
  boolean active,
  l1_dl_phychan_enum_type phch,
  uint8 hwch
#ifdef FEATURE_L1_LOG_ON_DEMAND
  ,
  WCDMA_COMMON_DL_PHYCHAN_LOG_PACKET_type *log_pkt
#endif
);

/*===========================================================================
FUNCTION        DL_PHCH_DEDICATED_PHYCHAN_LOG

FILE NAME       dlphch.c

DESCRIPTION     This funtion logs a dedicated physical channel

DEPENDENCIES    The phychan database

RETURN VALUE    None

SIDE EFFECTS    A dedicated phychan log packet may be sent
===========================================================================*/
void dl_phch_dedicated_phychan_log
(
  boolean active,
  uint8 hwch
#ifdef FEATURE_L1_LOG_ON_DEMAND
  ,
  WCDMA_DED_DL_PHYCHAN_LOG_PACKET_type *log_pkt
#endif
);

/*===========================================================================
FUNCTION        DL_PHYCHAN_LOG

FILE NAME       dldem.c

DESCRIPTION     This function logs the facts that a downlink physical channel
                has come up or gone down.

DEPENDENCIES

RETURN VALUE    None

SIDE EFFECTS    A common or dedicated phychan log packet may be sent
===========================================================================*/
extern void dl_phch_log
(
  boolean active,

  l1_dl_phychan_enum_type phch,

  uint8 hwch
);


/*===========================================================================
FUNCTION dl_phch_suspend_triage_for_phychan_config

DESCRIPTION
  suspend triage for all relavent RLs

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_phch_suspend_triage_for_phychan_config(dl_phch_cfg_handle_type phch_cfg_db_idx);

/*===========================================================================
FUNCTION        DL_PHCH_HANDLE_CELL_STATUS_UPDATE

DESCRIPTION     

DEPENDENCIES   

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void dl_phch_handle_cell_status_update(dl_phch_cfg_handle_type phch_cfg_db_idx, boolean status);

/*===========================================================================
FUNCTION        DL_PHCH_UPDATE_ASET_RADIO_LINKS_IN_DB

DESCRIPTION     

DEPENDENCIES   

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern boolean dl_phch_update_aset_radio_links_in_db(
  /* to which cfg db location the information should be copied */
  dl_phch_cfg_handle_type phch_cfg_db_idx,
  
  /* add type phychan confguration */ 
  dl_phychan_add_type *aset_upd_info
);

/*===========================================================================
FUNCTION dl_phch_cfg_db_init

DESCRIPTION
  This function initializes all the information of the physical channle config database of the given index

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dl_phch_cfg_db_init(uint8 index);

/*===========================================================================
FUNCTION dl_phch_rl_db_init

DESCRIPTION
  This function initializes all the information of the RL physical channl config database of the given index

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dl_phch_rl_db_init(uint8 index);

/*===========================================================================
FUNCTION dl_phch_cell_db_init

DESCRIPTION
  This function initializes all the information of the cell database of the given index

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dl_phch_cell_db_init(uint8 cell_db_index);

/*===========================================================================
FUNCTION dl_phch_comb_ch_db_init

DESCRIPTION
  This function initializes all the information of the combiner channel config database of the given index

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dl_phch_comb_ch_db_init(uint8 comb_id);

/*===========================================================================
FUNCTION        DL_GET_PILOT_BITS_FROM_CMD

DESCRIPTION     This function returns the presence of pilot bits for a
                physical channel referred to in the passed command.

DEPENDENCIES    RRC phychan ordered config database

RETURN VALUE    The number of pilot bits

SIDE EFFECTS    None
===========================================================================*/
uint16 dl_get_pilot_bits(
  l1_dl_phychan_enum_type phch,

  l1_dl_phychan_db_struct_type *phychan_db
);

/*===========================================================================
FUNCTION        DL_GET_TFCI_USE_FROM_CMD

DESCRIPTION     This function returns the TFCI usage from a CPHY_SETUP_REQ
                command packet.

DEPENDENCIES    None

RETURN VALUE    TRUE if TFCI usage is specified for the physical channel
                being added or configured via the CPHY_SETUP_REQ.  FALSE
                in all other cases.

SIDE EFFECTS    None
===========================================================================*/
boolean dl_get_tfci_use_from_ord_cfg
(
  l1_dl_phychan_enum_type phch,

  l1_dl_phychan_db_struct_type *phychan_db
);

#endif /* DLPHCHI_H */

