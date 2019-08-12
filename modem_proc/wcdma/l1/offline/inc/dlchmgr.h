#ifndef DLCHMGR_H
#define DLCHMGR_H
/*===========================================================================
                 D L C H M G R . H

GENERAL DESCRIPTION
  This file contains the declarations and definitions of dlchmgr needed
  across modules.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2007 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/tlm.c_v   1.153   17 Jul 2002 16:36:16   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/dlchmgr.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------- 
06/23/14    sad     Moving DC-HSUPA specific code under FEATURE_WCDMA_DC_HSUPA flag
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
01/16/14    sad     TTO validation check for secondary ASET update
01/07/14    amj     Cleaning up old ASET UPDATE APIs
12/16/13    as      Porting G2W TA from Triton to Dime.
06/10/13    pr      Added support to provide Pccpch add/Drop info to MCVS
05/21/13    sad     Added DC HSUPA code changes under FEATURE_WCDMA_DC_HSUPA 
05/20/13    sad     Support for Enhanced Layer-1 Manager
04/01/13    hk      Mainlined FEATURE_WCDMA_EFDPCH
11/27/12    sad     Fixed klocwork errors
09/12/12    hk      Changes to limit the ref pos update for ASET cells
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/05/12    pv      Offline PICH changes.
06/19/12    vr      HSRACH code update
06/08/12    gv      Feature HS RACH Initial code check in
05/23/12    kcm     L/G to W handover optimizations
04/18/12    srk     Freezed CPC-DRX scheduling when pccpch is active
02/10/12    hk      Nikel feature cleanup
01/26/12    vs      Nikel feature cleanup.Removed the features DEMOD_EQ_TD_UPDATE_CB_FUNC_TYPE_DEF
                    & FEATURE_WCDMA_EQ_CONTROLLER.
01/19/12    zr      Mainlined FEATURE_L1_CELL_RESELECTION
01/12/12    kcm     Added API for TTO check during ASET update.
02/14/11    rgn     Externed function that checks if a cell can be added to ASET or not.
11/02/10    dm      De-register DRX entity when channel setup fails
08/23/10    mc      Added API to query TTO change
05/21/10    mc      Added support for DPCH+MIMO SCPICH combo phase reference
04/06/10    ks      Added Enhanced FDPCH support
10/13/09    rgn     Added macro to ascertain usage of pri cpich usage 
09/24/09    hk      Porting of AICH resetup feature to Offline
04/24/09   mr       Fixed Compiler Warnings
03/16/09    rc      Externed demod_hs_td_update_cb callback.
03/12/09    rc      Added support for HS CQI BLER alignment.
03/06/09    vp      Receive pre/post cell transition indications from L1M and pass
                    the same to pich manager.
02/29/09    rgn     Calculate align type and pass it onto phychan module
02/20/09    vp      Handle pre and post cell transition indication from L1M.
12/10/08    ks      Mainlining FEATURE_WCDMA_DL_ENHANCED
10/03/08    hk      Bringing in 7k Mailine fixes
09/12/08    rgn     Added SHO support
01/25/08    vsr     Checking in Enhanced DL files onto mainline
01/18/08    vsr     Moved in dl_aset_update_cmd from dlphch.h
01/15/08    vp      Added code comments and Merged dynamic and static db in to a 
                    single config db.
12/21/07    vb      moved prototype get aich params API 
12/20/07    vp      Added macros to check whether a channel is part of DRX gen-op.
11/07/07    ks      Modified macro DLCHMGR_IS_STATIC_DB_IDX_VALID_FOR_NSCH for
                    F-DPCH
11/06/07    vp      Moved DRX handle from dlchmgr to channel specific managers
                    and added shared channel support in sccpchmgr.
08/23/07    vsr     Moved in variable definitions and made include file changes 
                    related to removal of dlcmd.* files
08/20/07    vsr     Moved in functions and variables from dldem files
08/20/07    vp      Added dlchmgr_get_ch_spec_dyn_db_idx_from_phch()
                    to get channel specific dynamic db index.
                    Added support for new sccpch0/pich reponse functions.
07/12/07    vp      Added support for suspend/resume DRX entities
05/18/07    vp/vsr  Changes for sending response to L1m after dropping both
                    sccpch and pich
05/08/07    sv/vsr  Changes for HHO
04/28/07    scm     Lint fix.
04/25/07    vp/vsr  Lint Fixes
03/30/07    vb      Fixes in general operations to avoid race condition
03/27/07    vsr     Added support for ASET UPD procedure
03/02/07    vp      Added support for R99 cell reselection
02/21/07    vp      Initial version
===========================================================================*/
#include "wcdma_variation.h"
#include "customer.h"
#include "comdef.h"
#include "l1const.h"
#include "l1rrcif.h"
#include "wl1drxmanager.h"
#include "dldec.h"
#include "dlphch.h"
#include "mcalwcdma_dec.h"
#include "dlshmgr.h"

/*Keep the DlChMgr dynamic db size as 6. The worst possible channel combination is:
1. SCCPCH0
2. SCCPCH1
3. PICH
4. PCCPCH setup by RRC
5. PCCPCH setup by SRCH*/
#define DLCHMGR_CFG_DB_SIZE ((dlchmgr_cfg_db_idx_type)6)

/*The number of channel specific managers:
BCCH, SCCPCH, PICH, Non-scheduled channels (AICH, DPCH)*/
#define DLCHMGR_NUM_ALWAYS_PRESENT_CH_SPEC_MGR (4)
#define DLCHMGR_NUM_CH_SPEC_MGR  (DLCHMGR_NUM_ALWAYS_PRESENT_CH_SPEC_MGR)

/*Macros that hold the invalid db index values*/
#define DLCHMGR_INVALID_CFG_DB_IDX 0xFF
#define DLCHMGR_INVALID_CH_SPEC_IDX (0xFF)

/*Macros that hold the done callbacks for the various channel operations*/
#define DLCHMGR_ADD_DONE_CB (dlchmgr_add_done_cb)
#define DLCHMGR_RECFG_DONE_CB (dlchmgr_recfg_done_cb)
#define DLCHMGR_DROP_DONE_CB (dlchmgr_drop_done_cb)
#define DLCHMGR_RESUME_DONE_CB (dlchmgr_resume_done_cb)
#define DLCHMGR_SUSPEND_DONE_CB (dlchmgr_suspend_done_cb)
#define DLCHMGR_ASET_DROP_RL_DONE_CB (dlchmgr_aset_drop_rl_done_cb)
#define DLCHMGR_RESUME_ASET_OPS_DONE_CB (dlchmgr_resume_aset_ops_done_cb)

  /*The type that respresents the dynamic db index of all channel specific manager*/
typedef uint8 dl_ch_spec_cfg_db_idx_type;

  /*The type that respresents the config db index of DlChMgr*/
typedef uint8 dlchmgr_cfg_db_idx_type;

  /*The type that can hold the cleanup bitmask in DlChMgr*/
typedef uint16 dlchmgr_cleanup_bmask_type;

/*The enum that defines the index for the DlChMgr static db*/
typedef enum {
DLCHMGR_STATIC_TYPE_ACQ_BCH,
DLCHMGR_STATIC_TYPE_SFN_WRC,
DLCHMGR_STATIC_TYPE_BCH_S_SIB_READ,
DLCHMGR_STATIC_TYPE_BCH_N_SIB_READ,
DLCHMGR_STATIC_TYPE_BCH_X2W_HHO,
DLCHMGR_STATIC_TYPE_SCCPCH0,
DLCHMGR_STATIC_TYPE_SCCPCH1,
DLCHMGR_STATIC_TYPE_PICH,
DLCHMGR_STATIC_TYPE_AICH,
DLCHMGR_STATIC_TYPE_HS_RACH_AICH,
DLCHMGR_STATIC_TYPE_DPCH,
DLCHMGR_STATIC_TYPE_HS_RACH_FDPCH,
DLCHMGR_STATIC_TYPE_FDPCH,
DLCHMGR_STATIC_TYPE_SEC_FDPCH,
DLCHMGR_STATIC_TYPE_NUM_TYPES
} dlchmgr_static_db_idx_type;

/*Type definitions for the various function types to be used between DlChMgr 
and channel specific manager*/
typedef boolean DLCHMGR_CH_SPEC_PRE_OP_CB_FUNC_TYPE(
  dl_ch_spec_cfg_db_idx_type ch_spec_idx);

typedef boolean DLCHMGR_CH_SPEC_PRE_ADD_CB_FUNC_TYPE(
  dl_ch_spec_cfg_db_idx_type ch_spec_idx, l1_dl_phychan_db_struct_type *phychan_db);

typedef void DLCHMGR_GEN_OP_DONE_CB_FUNC_TYPE( void );

typedef void DLCHMGR_GEN_OP_PROC_FUNC_TYPE( void );

typedef void DLCHMGR_GEN_OP_CHECK_FUNC_TYPE( dlchmgr_cfg_db_idx_type cfg_db_idx);

typedef boolean DLCHMGR_CH_SPEC_POST_OP_CB_FUNC_TYPE(
  dl_ch_spec_cfg_db_idx_type ch_spec_idx);

typedef void DLCHMGR_RESPONSE_CB_FUNC_TYPE(
  dlchmgr_cfg_db_idx_type cfg_db_idx);

typedef dl_ch_spec_cfg_db_idx_type DL_CH_SPEC_IDX_ALLOC_FUN_TYPE
  (dlchmgr_cfg_db_idx_type chmgr_cfg_db_idx);

typedef void DL_CH_SPEC_IDX_DEALLOC_FUN_TYPE
  (dl_ch_spec_cfg_db_idx_type ch_spec_idx);

typedef void DL_CH_SPEC_MGR_INIT_CB_TYPE( void );

typedef boolean DLCHMGR_CH_SPEC_PRE_SUSP_RES_CB_FUNC_TYPE(
  drx_entity_handle_type entity_handle);

typedef boolean DLCHMGR_CH_SPEC_POST_SUSP_RES_CB_FUNC_TYPE(
  dl_ch_spec_cfg_db_idx_type chspec_dyn_db_idx);

/*dlchmgr_resp_func_cb_struct_type holds the function pointers to be called 
for sending repsonse after channel operations*/
typedef struct {
  /*Function Pointer to send response after an addition operation*/
  DLCHMGR_RESPONSE_CB_FUNC_TYPE *chmgr_add_resp_cb;

  /*Function Pointer to send response after a recfg operation*/
  DLCHMGR_RESPONSE_CB_FUNC_TYPE *chmgr_recfg_resp_cb;

  /*Function Pointer to send response after a drop operation*/
  DLCHMGR_RESPONSE_CB_FUNC_TYPE *chmgr_drop_resp_cb;

  /*Function Pointer to send response during cleanup after an operation failure*/
  DLCHMGR_RESPONSE_CB_FUNC_TYPE *chmgr_cleanup_resp_cb;

} dlchmgr_resp_func_cb_struct_type;

/*dlchmgr_idx_mgmt_func_struct_type holds the function pointers to be called 
for allocating and de-allocating index in channel specific manager*/
typedef struct {
  /*Function Pointer to allocate index in channel specific manager*/
  DL_CH_SPEC_IDX_ALLOC_FUN_TYPE *ch_spec_idx_alloc_cb;

  /*Function Pointer to de-allocate index in channel specific manager*/
  DL_CH_SPEC_IDX_DEALLOC_FUN_TYPE *ch_spec_idx_dealloc_cb;
} dlchmgr_idx_mgmt_func_struct_type;

/*dlchmgr_op_cb_func_struct_type holds the pointers to channel specific 
manager functions to be called pre and post an operation from DlChMgr*/
typedef struct {
  /*Pointer to channel specific manager function that has to be 
  called before add*/
  DLCHMGR_CH_SPEC_PRE_ADD_CB_FUNC_TYPE *ch_spec_pre_add_cb;

  /*Pointer to channel specific manager function that has to be 
  called after add*/
  DLCHMGR_CH_SPEC_POST_OP_CB_FUNC_TYPE *ch_spec_post_add_cb;

  /*Pointer to channel specific manager function that has to be called 
  before re-cfg*/
  DLCHMGR_CH_SPEC_PRE_OP_CB_FUNC_TYPE *ch_spec_pre_recfg_cb;

  /*Pointer to channel specific manager function that has to be called 
  after re-cfg*/
  DLCHMGR_CH_SPEC_POST_OP_CB_FUNC_TYPE *ch_spec_post_recfg_cb;

  /*Pointer to channel specific manager function that has to be called 
  before drop*/
  DLCHMGR_CH_SPEC_PRE_OP_CB_FUNC_TYPE *ch_spec_pre_drop_cb;

  /*Pointer to channel specific manager function that has to be called 
  after drop*/
  DLCHMGR_CH_SPEC_POST_OP_CB_FUNC_TYPE *ch_spec_post_drop_cb;

  /*Pointer to channel specific manager function that has to be called 
  before resume*/
  DLCHMGR_CH_SPEC_PRE_SUSP_RES_CB_FUNC_TYPE *ch_spec_pre_resume_cb;

  /*Pointer to channel specific manager function that has to be called 
  after resume*/
  DLCHMGR_CH_SPEC_POST_SUSP_RES_CB_FUNC_TYPE *ch_spec_post_resume_cb;

  /*Pointer to channel specific manager function that has to be called 
  before suspend*/
  DLCHMGR_CH_SPEC_PRE_SUSP_RES_CB_FUNC_TYPE *ch_spec_pre_suspend_cb;

  /*Pointer to channel specific manager function that has to be called 
  after suspend*/
  DLCHMGR_CH_SPEC_POST_SUSP_RES_CB_FUNC_TYPE *ch_spec_post_suspend_cb;
} dlchmgr_op_cb_func_struct_type;

/*Structure that holds the semi static information for any channel. The 
semi-static information can only be changed at compile time*/
typedef struct {
  /*physical channel type*/
  l1_dl_phychan_enum_type phch_type;

  /*Whether the channel needs a CCTrCh*/
  boolean cctrch_needed;

  /*Whether the channel needs a srch set update*/
  boolean srch_set_update_needed;

  /*Whether the suspend/resume operations on a channel should be
    fast/slow in mcal-dl*/
  boolean fast_ch_op_needed;

  /*Whether the channel has to be added in ACTIVE or 
  SUSPENDED state while addition*/
  dl_phch_state_enum_type add_state;

  /*Whether the ALM(Alignment Manager) needs to be notified during addition*/
  boolean notify_alm_on_add_recfg;

  /*Whether the channel is setup by a L1 module or RRC*/
  boolean int_channel;

  /*Pointer to structure that holds response functions*/
  dlchmgr_resp_func_cb_struct_type *chmgr_resp_cb;

  /*Pointer to structure that holds ch-spec-mgr index 
  management functions*/
  dlchmgr_idx_mgmt_func_struct_type *ch_spec_idx_mgmt;

  /*Pointer to structure that holds ch-spec-mgr pre & post 
  operation management functions*/
  dlchmgr_op_cb_func_struct_type *ch_spec_op_cb;

  boolean post_cfg_done_to_l1m;
} dlchmgr_static_db_struct_type;

/*Structure that holds the dynamic information for any channel. The values 
in this structure are decided at run-time.*/
typedef struct {
  /*psc of the channel*/
  uint16 psc;

  /*frequency of the channel*/
  uint16 freq;

  /*Operation that is in progress in the channel*/
  l1_setup_ops_enum_type operation;

  /*For some channels Add state is decided at run time; Store the add state
    for all channels here*/
  dl_phch_state_enum_type curr_add_state;

  /*Whether this entry is free or in-use*/
  boolean idx_in_use;

  /*Whether this channel is */
  boolean shared_ch;

  /*The static db entry associated with this channel*/
  dlchmgr_static_db_idx_type static_db_idx;

  /*The index for the channel in to the corresponding channel manager's db*/
  dl_ch_spec_cfg_db_idx_type ch_spec_idx;

  /*phch handle for the channel in dl-phch*/
  dl_phch_cfg_handle_type phch_handle;

  /*CCTrCh idx associated with the channel*/
  mcalwcdma_dec_cctrch_id cctrch_idx;

  /*Status (pass/failure) of the operation on the channel*/
  boolean op_status;

  /*Any status that affects the overall status of a channel in addition to the 
  operation statsus. Currently, this field carries the CRC state machine 
  status for all PCCPCH. For all other channels, it is always TRUE.*/
  boolean assoc_status;

  /*In case of channel operation failure, cleanup_bmask indicates the 
  clean up operations that has to be performed.*/
  dlchmgr_cleanup_bmask_type cleanup_bmask;

  /* flag to indicate this channel config is part of common/general 
     drop all operation which is used to process idle req or cell trans req */
  boolean ch_part_of_drop_gen_op;

  /* flag to indicate this channel config is part of common/general 
     drx operation such as suspend all or resume all */
  boolean ch_part_of_drx_gen_op;
} dlchmgr_dynamic_db_struct_type;

typedef struct {
  dlchmgr_static_db_struct_type stat_db;

  dlchmgr_dynamic_db_struct_type dyn_db;
} dlchmgr_ch_cfg_struct_type;

/*dlchmgr_int_setup_enum_type holds the various types of internal 
channel setups*/
typedef enum {
DLCHMGR_SETUP_TYPE_ACQ_BCH,
DLCHMGR_SETUP_TYPE_SFN_WRC,
DLCHMGR_SETUP_TYPE_BCH_X2W_HHO,
DLCHMGR_SETUP_TYPE_TOTAL_NUM
} dlchmgr_int_setup_enum_type;

/*dlchmgr_int_setup_struct_type holds all the information that is needed to 
setup an internal channel.*/
typedef struct {
  /*sttd is used or not in this cell*/
  boolean sttd_used;

  /* WCDMA psc for the cell */
  uint16 psc;

  /* WCDMA frequency assignment for the cell */
  uint16  freq;

  dlchmgr_int_setup_enum_type setup_type;
} dlchmgr_int_setup_struct_type;

/******************************************************************************
                     COMMAND: DL_CPHY_SETUP_LOCAL_CMD_TYPE
******************************************************************************/
typedef struct
{
  /* common command header */
  l1_local_cmd_hdr_type hdr;

  /* TRUE if the setup was successful */
  boolean status;

  /* The CCTrCH reference enum type, what kind of CCTrCH was set modified */
  dl_cctrch_ref_type_enum_type cctrch_type;

  /* The index in the CCTrCH table of the CCTrCH that was modified */
  uint8 cctrch_index;

  /* The type of physical channel that was modified */
  l1_dl_phychan_enum_type phch;

} dl_cphy_setup_local_cmd_type;


/* NOTE: L1M handles this command so there is no prototype for the handler
   listed in dl_cmd.h */

/*--------------------------------------------------------------------------
                ENUM: DL_PHYCHAN_ACTION_REASON_ENUM_TYPE

This enumerates the various reasons that a request for the setup or teardown
of a physical channel was issued.
--------------------------------------------------------------------------*/
typedef enum {
  PHYCHAN_REASON_ACQ_STTD,
  PHYCHAN_REASON_ACQ_STTD_DONE,
  PHYCHAN_REASON_SRCH_MEAS_SFN_SFN,
  PHYCHAN_REASON_SRCH_MEAS_SFN_CFN,
  PHYCHAN_REASON_IMPLICIT,
  PHYCHAN_REASON_CELL_TRANS_DROP_ALL,
  PHYCHAN_REASON_CELL_TRANS_RESTORE,
  PHYCHAN_REASON_L1M_CMD,
  PHYCHAN_REASON_IDLE_REQ,
  PHYCHAN_REASON_SLEEP_DROP_ALL,
  PHYCHAN_REASON_MDSP_CLOCK_CHANGE_DROP_ALL,
  PHYCHAN_REASON_MDSP_CLOCK_CHANGE_DROP_NOT_NEEDED,
  PHYCHAN_REASON_WAKE_UP_ALL,
  PHYCHAN_REASON_L1M_CMD_NO_STATUS_NEEDED,
  PHYCHAN_REASON_ANY_NO_STATUS_NEEDED,
  PHYCHAN_REASON_INVALID
} dl_phychan_action_reason_enum_type;

/*--------------------------------------------------------------------------
             STRUCTURE: DL_CELL_TRANS_DROP_DONE_CMD_TYPE

This structure contains the parameters used with the
DL_CELL_TRANS_DROP_DONE_CMD.
--------------------------------------------------------------------------*/
typedef struct {
  /* command l1m local command header */
  l1_local_cmd_hdr_type hdr;

  /* this command is just used for sequencing.  There are no other
     parameters currently associated with it */
} dl_cell_trans_drop_done_cmd_type;

extern dlchmgr_ch_cfg_struct_type dlchmgr_cfg_db[DLCHMGR_CFG_DB_SIZE];

extern dlchmgr_static_db_struct_type dlchmgr_static_db[DLCHMGR_STATIC_TYPE_NUM_TYPES];

#define DLCHMGR_IS_CFG_DB_IDX_VALID(cfg_db_idx)    \
  (!DLCHMGR_IS_CFG_DB_IDX_INVALID(cfg_db_idx))

#define DLCHMGR_IS_CFG_DB_IDX_INVALID(cfg_db_idx)    \
  (cfg_db_idx >= DLCHMGR_CFG_DB_SIZE)

#define DLCHMGR_IS_CCTRCH_IDX_INVALID(cctrch_idx)  \
  (cctrch_idx == DL_L1_ID_INDEX_UNUSED)

/*This is not an accurate check for the invalidity of channel specific index.
Any value larger than the individual channel manager's dynamic db size
should be considered as invalid.*/
#define DLCHMGR_IS_CH_SPEC_IDX_INVALID(ch_spec_idx)  \
  (ch_spec_idx == DLCHMGR_INVALID_CH_SPEC_IDX)

/*This is not an accurate check for the validity of channel specific index.
Any value larger than the individual channel manager's dynamic db size
should be considered as invalid.*/
#define DLCHMGR_IS_CH_SPEC_IDX_VALID(ch_spec_idx)  \
  (ch_spec_idx != DLCHMGR_INVALID_CH_SPEC_IDX)

#define DLCHMGR_IS_DRX_HDLE_INVALID(drx_hdle)  \
  (drx_hdle == DRX_ENTITY_HANDLE_INVALID)

#define DLCHMGR_IS_DRX_HDLE_VALID(drx_hdle)  \
  (drx_hdle != DRX_ENTITY_HANDLE_INVALID)

#define DLCHMGR_IS_SRCH_SET_UPDATE_NEEDED_FROM_L1_CMD_PTR(l1_setup_cmd_ptr)  \
  (l1_setup_cmd_ptr->add_dl_phychan == L1_DL_PHYCHAN_PCCPCH_N)

#define DLCHMGR_IS_PHCH_HDLE_INVALID(phch_cfg_db_idx)  \
  (phch_cfg_db_idx == DL_PHCH_INVALID_CFG_ID)

#define DLCHMGR_IS_STATIC_DB_IDX_VALID_FOR_BCCH(dlchmgr_static_db_idx)  \
  ((dlchmgr_static_db_idx == DLCHMGR_STATIC_TYPE_ACQ_BCH) ||  \
   (dlchmgr_static_db_idx == DLCHMGR_STATIC_TYPE_SFN_WRC) ||  \
   (dlchmgr_static_db_idx == DLCHMGR_STATIC_TYPE_BCH_S_SIB_READ) ||  \
   (dlchmgr_static_db_idx == DLCHMGR_STATIC_TYPE_BCH_N_SIB_READ) || \
   (dlchmgr_static_db_idx == DLCHMGR_STATIC_TYPE_BCH_X2W_HHO))

#define DLCHMGR_IS_STATIC_DB_IDX_VALID_FOR_SCCPCH(dlchmgr_static_db_idx)  \
  ((dlchmgr_static_db_idx == DLCHMGR_STATIC_TYPE_SCCPCH0) ||  \
   (dlchmgr_static_db_idx == DLCHMGR_STATIC_TYPE_SCCPCH1))

#define DLCHMGR_IS_STATIC_DB_IDX_VALID_FOR_PICH(dlchmgr_static_db_idx)  \
  (dlchmgr_static_db_idx == DLCHMGR_STATIC_TYPE_PICH)

#define DLCHMGR_IS_STATIC_DB_IDX_VALID_FOR_NSCH(dlchmgr_static_db_idx)  \
   ((dlchmgr_static_db_idx == DLCHMGR_STATIC_TYPE_DPCH)  || \
    (dlchmgr_static_db_idx == DLCHMGR_STATIC_TYPE_AICH)  || \
    (dlchmgr_static_db_idx == DLCHMGR_STATIC_TYPE_FDPCH))

#define DLCHMGR_GET_PSC_FOR_EXT_ADD(psc, add_info)    \
  do {    \
    if (add_info->phch == L1_DL_PHYCHAN_PCCPCH_N)    \
    {    \
      psc = SCR_CODE_TO_PRI_SCR_CODE(add_info->phychan_db->pccpch.scr_code);    \
    }    \
    else    \
    {    \
      psc = SCR_CODE_TO_PRI_SCR_CODE(srch_aset_ptr[0]->scr_code);    \
    }    \
  } while(0)

#define DLCHMGR_GET_FREQ_FROM_INT_SETUP_PTR(int_setup_ptr)    \
  (int_setup_ptr->freq)

#define DLCHMGR_GET_PSC_FROM_INT_SETUP_PTR(int_setup_ptr)    \
  (int_setup_ptr->psc)

#define DLCHMGR_GET_PSC_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.psc)

#define DLCHMGR_GET_FREQ_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.freq)

#define DLCHMGR_GET_OP_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.operation)

#define DLCHMGR_GET_CURR_ADD_STATE_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.curr_add_state)  

#define DLCHMGR_GET_CCTRCH_IDX_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.cctrch_idx)

#define DLCHMGR_GET_PHCH_TYPE_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.phch_type)

#define DLCHMGR_GET_PHCH_HDLE_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.phch_handle)

#define DLCHMGR_IS_CFG_DB_IDX_NOT_IN_USE(cfg_db_idx)    \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.idx_in_use == FALSE )

#define DLCHMGR_IS_CFG_DB_IDX_IN_USE(cfg_db_idx)    \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.idx_in_use == TRUE )

#define DLCHMGR_GET_CH_SPEC_DB_IDX_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.ch_spec_idx)

#define DLCHMGR_GET_STAT_DB_IDX_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.static_db_idx)

#define DLCHMGR_GET_PHYCHAN_TYPE_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.phch_type)

#define DLCHMGR_IS_SRCH_SET_UPDATE_NEEDED_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.srch_set_update_needed)

#define DLCHMGR_IS_FAST_CH_OP_NEEDED_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.fast_ch_op_needed)

#define DLCHMGR_IS_NOTIFY_ALM_NEEDED_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.notify_alm_on_add_recfg)

#define DLCHMGR_IS_CH_INTERNAL_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.int_channel)

#define DLCHMGR_IS_CH_EXTERNAL_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (!(dlchmgr_cfg_db[cfg_db_idx].stat_db.int_channel))

#define DLCHMGR_IS_CCTRCH_NEEDED_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.cctrch_needed)

#define DLCHMGR_GET_ADD_STATE_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.add_state)

#define DLCHMGR_GET_OP_STATUS_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.op_status)

#define DLCHMGR_GET_ASSOC_STATUS_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.assoc_status)

#define DLCHMGR_IS_POST_CFG_DONE_TO_L1M_NEEDED_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.post_cfg_done_to_l1m)

#define DLCHMGR_GET_CH_SPEC_PRE_ADD_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_add_cb)

#define DLCHMGR_GET_CH_SPEC_POST_ADD_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_add_cb)

#define DLCHMGR_GET_CH_SPEC_PRE_DROP_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_drop_cb)

#define DLCHMGR_GET_CH_SPEC_POST_DROP_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_drop_cb)

#define DLCHMGR_GET_CH_SPEC_PRE_RECFG_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_recfg_cb)

#define DLCHMGR_GET_CH_SPEC_POST_RECFG_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_recfg_cb)

  #define DLCHMGR_GET_CH_SPEC_PRE_RESUME_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_resume_cb)

#define DLCHMGR_GET_CH_SPEC_POST_RESUME_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_resume_cb)

#define DLCHMGR_GET_CH_SPEC_PRE_SUSP_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_suspend_cb)

#define DLCHMGR_GET_CH_SPEC_POST_SUSP_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_suspend_cb)

#define DLCHMGR_GET_ADD_RESP_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.chmgr_resp_cb->chmgr_add_resp_cb)

#define DLCHMGR_GET_RECFG_RESP_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.chmgr_resp_cb->chmgr_recfg_resp_cb)

#define DLCHMGR_GET_DROP_RESP_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.chmgr_resp_cb->chmgr_drop_resp_cb)

#define DLCHMGR_GET_CLEANUP_RESP_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.chmgr_resp_cb->chmgr_cleanup_resp_cb)

#define DLCHMGR_GET_CH_SPEC_IDX_ALLOC_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_idx_mgmt->ch_spec_idx_alloc_cb)

#define DLCHMGR_GET_CH_SPEC_IDX_DEALLOC_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_idx_mgmt->ch_spec_idx_dealloc_cb)

#define DLCHMGR_IS_CH_SPEC_PRE_ADD_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_add_cb != NULL)

#define DLCHMGR_IS_CH_SPEC_POST_ADD_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_add_cb != NULL)

#define DLCHMGR_IS_CH_SPEC_PRE_DROP_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_drop_cb != NULL)

#define DLCHMGR_IS_CH_SPEC_POST_DROP_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_drop_cb != NULL)

#define DLCHMGR_IS_CH_SPEC_PRE_RECFG_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_recfg_cb != NULL)

#define DLCHMGR_IS_CH_SPEC_POST_RECFG_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_recfg_cb != NULL)

#define DLCHMGR_IS_CH_SPEC_PRE_RESUME_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_resume_cb != NULL)

#define DLCHMGR_IS_CH_SPEC_POST_RESUME_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_resume_cb != NULL)

#define DLCHMGR_IS_CH_SPEC_PRE_SUSP_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_suspend_cb != NULL)

#define DLCHMGR_IS_CH_SPEC_POST_SUSP_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_suspend_cb != NULL)

#define DLCHMGR_IS_CH_SPEC_IDX_ALLOC_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_idx_mgmt->ch_spec_idx_alloc_cb != NULL)

#define DLCHMGR_IS_CH_SPEC_IDX_DEALLOC_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_idx_mgmt->ch_spec_idx_dealloc_cb != NULL)

#define DLCHMGR_IS_ADD_RESP_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.chmgr_resp_cb->chmgr_add_resp_cb != NULL)

#define DLCHMGR_IS_RECFG_RESP_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.chmgr_resp_cb->chmgr_recfg_resp_cb != NULL)

#define DLCHMGR_IS_DROP_RESP_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.chmgr_resp_cb->chmgr_drop_resp_cb != NULL)

#define DLCHMGR_IS_CLEANUP_RESP_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].stat_db.chmgr_resp_cb->chmgr_cleanup_resp_cb != NULL)

#define DLCHMGR_GET_OVERALL_STATUS_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (DLCHMGR_GET_OP_STATUS_FROM_CFG_DB_IDX(cfg_db_idx) &&  \
  DLCHMGR_GET_ASSOC_STATUS_FROM_CFG_DB_IDX(cfg_db_idx))

#define DLCHMGR_IS_OP_STATUS_PASSED_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (DLCHMGR_GET_OP_STATUS_FROM_CFG_DB_IDX(cfg_db_idx) == TRUE)

#define DLCHMGR_IS_OP_STATUS_FAILED_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (DLCHMGR_GET_OP_STATUS_FROM_CFG_DB_IDX(cfg_db_idx) == FALSE)

#define DLCHMGR_IS_CH_PART_OF_DROP_GEN_OP_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.ch_part_of_drop_gen_op)

#define DLCHMGR_IS_CH_PART_OF_DRX_GEN_OP_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.ch_part_of_drx_gen_op == TRUE)

#define DLCHMGR_IS_CH_NOT_PART_OF_DRX_GEN_OP_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.ch_part_of_drx_gen_op == FALSE)

#define DLCHMGR_SET_OP_STATUS_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.op_status = TRUE)

#define DLCHMGR_RESET_OP_STATUS_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.op_status = FALSE)

#define DLCHMGR_SET_ASSOC_STATUS_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.assoc_status = TRUE)

#define DLCHMGR_RESET_ASSOC_STATUS_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.assoc_status = FALSE)

#define DLCHMGR_GET_HWCH_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dl_phch_get_hwch_from_handle(DLCHMGR_GET_PHCH_HDLE_FROM_CFG_DB_IDX(cfg_db_idx)))

/** phch function callback ptr */
typedef void (*DLCHMGR_ASET_POST_RL_OPS_FUNC_CB_PTR)(
  /** status of the cell config command */
  boolean status);

#define DLCHMGR_GET_CTCH_ADD_STATE()  \
  (dlchmgr_static_db[DLCHMGR_STATIC_TYPE_SCCPCH1].add_state)

#define DLCHMGR_GET_SHARED_CH_STATUS(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.shared_ch)

#define DLCHMGR_UPD_SHARED_CH_STATUS(cfg_db_idx, status)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.shared_ch = status)

#define DLCHMGR_SHOULD_PRI_CPICH_BE_USED(cpich_usage)  \
  ((cpich_usage == L1_USE_PRI_CPICH) ? TRUE : FALSE)

/******************************************************************************
                         EQ CONTROLLER
******************************************************************************/

/* Callback function reporting TD status to HS module */
typedef void DEMOD_HS_TD_UPDATE_CB_FUNC_TYPE(uint8 td_status);

/* Callback function reporting TD status to HS module */
extern DEMOD_HS_TD_UPDATE_CB_FUNC_TYPE *demod_hs_td_update_cb;

/*===========================================================================
FUNCTION        dlchmgr_init

DESCRIPTION     This function initializes the dlchmgr and all the channel
                specific managers.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_init(void);

/*===========================================================================
FUNCTION        dlchmgr_ext_add

DESCRIPTION     This function will be used for all RRC initiated channel
                additions. This function first does all the necessary actions
                before the channel setup (drx-registration, srch-set-updation
                etc) and then calls dl-phch to add the channel.

DEPENDENCIES    CCTrCh should be already setup by the caller.

RETURN VALUE
                TRUE:Channel addition was successful
                FALSE: Channel addition was failure

SIDE EFFECTS    None
===========================================================================*/
extern boolean dlchmgr_ext_add (dl_phychan_add_type *add_info);

/*===========================================================================
FUNCTION        dlchmgr_int_add

DESCRIPTION     This function will be used for all L1 initiated channel
                additions. This function first does all the necessary actions
                before the channel setup (drx-registration, srch-set-updation
                etc) and then calls dl-phch to add the channel.

DEPENDENCIES    None

RETURN VALUE
                TRUE:Channel addition was successful
                FALSE: Channel addition was failure

SIDE EFFECTS    None
===========================================================================*/
extern boolean dlchmgr_int_add (dlchmgr_int_setup_struct_type *ch_info);

/*===========================================================================
FUNCTION        dlchmgr_ext_recfg

DESCRIPTION     This function will be used for all RRC initiated
                channel drops.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_ext_drop (l1_dl_phychan_enum_type phch);

/*===========================================================================
FUNCTION        dlchmgr_aset_update

DESCRIPTION     This function will be used for all RRC initiated
                aset update procedure

DEPENDENCIES    None

RETURN VALUE
                TRUE: aset update was successful
                FALSE: aset update was failure

SIDE EFFECTS    None
===========================================================================*/
extern boolean dlchmgr_aset_update (dl_phychan_add_type *aset_upd_info);


/*===========================================================================
FUNCTION        dlchmgr_drop

DESCRIPTION     This function does the actual drop in dlchmgr by triggering
                drop in dl-phch.

DEPENDENCIES    None

RETURN VALUE
                TRUE: Channel Drop was started successfully
                FALSE: Channel Drop could not be started successfully

SIDE EFFECTS    None
===========================================================================*/
extern boolean dlchmgr_drop (dlchmgr_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dlchmgr_ext_recfg

DESCRIPTION     This function will be used for all RRC initiated
                re-configurations.

DEPENDENCIES    None

RETURN VALUE
                TRUE:Channel re-configuration was successful
                FALSE: Channel re-configuration was failure

SIDE EFFECTS    None
===========================================================================*/
extern boolean dlchmgr_ext_recfg(dl_phychan_recfg_type *recfg_info);

/*===========================================================================
FUNCTION        dlchmgr_add_done_cb

DESCRIPTION     This function handles the post add functionality for any 
                channel. It is passed as parameter to dl-phch during add and 
                it gets called at the end of addition.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_add_done_cb (uint8 cctrch_idx, l1_dl_phychan_enum_type phch_type,
  dl_phch_cfg_handle_type phch_handle, boolean status);

/*===========================================================================
FUNCTION        dlchmgr_drop_done_cb

DESCRIPTION     This function handles the post drop functionality for any 
                channel. It is passed as parameter to dl-phch during drop and 
                it gets called at the end of drop.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_drop_done_cb (uint8 cctrch_idx, l1_dl_phychan_enum_type phch_type,
  dl_phch_cfg_handle_type phch_handle, boolean status);

/*===========================================================================
FUNCTION        dlchmgr_get_cfg_db_idx_from_cctrch_idx

DESCRIPTION     This function finds the config db idx of the channel 
                identified by CCTrCh Idx.

DEPENDENCIES    This function should only be called for channels that have an 
                associated CCTrCh Idx.It should not be called for channels 
                without CCTrCh Idx (eg: PICH, AICH)

RETURN VALUE
                cfg-db-idx: The config db idx of the channel

SIDE EFFECTS    None
===========================================================================*/
extern dlchmgr_cfg_db_idx_type dlchmgr_get_cfg_db_idx_from_cctrch_idx
  (uint8 cctrch_idx);

/*===========================================================================
FUNCTION        dlchmgr_recfg_done_cb

DESCRIPTION     This function handles the post re-configuration functionality 
                for any channel. It is passed as parameter to dl-phch during 
                re-cfg and it gets called at the end of re-cfg.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_recfg_done_cb (uint8 cctrch_idx, l1_dl_phychan_enum_type phch_type,
  dl_phch_cfg_handle_type phch_handle, boolean status);

/*===========================================================================
FUNCTION        dlchmgr_resume_done_cb

DESCRIPTION     This function handles the post resume functionality 
                for any channel. It is passed as parameter to dl-phch during 
                resume and it gets called at the end of resume.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_resume_done_cb (uint8 cctrch_idx, l1_dl_phychan_enum_type phch_type,
  dl_phch_cfg_handle_type phch_handle, boolean status);

/*===========================================================================
FUNCTION        dlchmgr_suspend_done_cb

DESCRIPTION     This function handles the post suspend functionality 
                for any channel. It is passed as parameter to dl-phch during 
                suspend and it gets called at the end of suspend.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_suspend_done_cb (uint8 cctrch_idx, l1_dl_phychan_enum_type phch_type,
  dl_phch_cfg_handle_type phch_handle, boolean status);

/*===========================================================================
FUNCTION        dlchmgr_check_for_pccpch_configured_for_sib_read

DESCRIPTION     This function checks if pccpch is configured for SIB read pupose and returns TRUE if it is.

DEPENDENCIES    None

RETURN VALUE
                TRUE: if pccpch is configured for sib read
                FALSE: if pccpch is not configured for sib read

SIDE EFFECTS    None
===========================================================================*/
boolean dlchmgr_check_for_pccpch_configured_for_sib_read(void);

/*===========================================================================
FUNCTION        dlchmgr_get_align_type

DESCRIPTION     This function checks whether time alignment change is needed 
                in TLM for the physical channel identified by the input CCTrCh 
                Idx and phch handle.

DEPENDENCIES    None

RETURN VALUE
                TRUE: Alignment is needed for the channel
                FALSE: Alignment is not needed for the channel

SIDE EFFECTS    None
===========================================================================*/
extern seq_align_type dlchmgr_get_align_type(dlchmgr_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dlchmgr_handle_gen_op

DESCRIPTION     This function will be used for all genral operations:
                1. Sleep all request from DRX
                2. Idle Request from RRC
                3. Cell Transtition reqeust from RRC

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_handle_gen_op(dl_phychan_action_reason_enum_type gen_op_ext_type);

/*===========================================================================
FUNCTION        dlchmgr_get_cctrch_idx_idx_from_phch

DESCRIPTION     This function finds the dynamic db idx of the channel 
                identified by physical channel type.

DEPENDENCIES    This function should only be called for channels that can have 
                a maximum of one instantiation at any point of time(eg: PICH, AICH).
                It should not be called for channels that can have more than 
                one instantiation at a time(eg: SCCPCH)

RETURN VALUE
                dyn-db-idx: The dynamic db idx of the channel

SIDE EFFECTS    None
===========================================================================*/
extern mcalwcdma_dec_cctrch_id dlchmgr_get_cctrch_idx_idx_from_phch(l1_dl_phychan_enum_type phch);

/*===========================================================================
FUNCTION        dlchmgr_get_phch_hdle_idx_from_phch

DESCRIPTION     This function finds the phch-handle of the channel 
                identified by physical channel type.

DEPENDENCIES    This function should only be called for channels that can have 
                a maximum of one instantiation at any point of time(eg: PICH, AICH).
                It should not be called for channels that can have more than 
                one instantiation at a time(eg: SCCPCH)

RETURN VALUE
                dyn-db-idx: The dynamic db idx of the channel

SIDE EFFECTS    None
===========================================================================*/
extern dl_phch_cfg_handle_type dlchmgr_get_phch_hdle_idx_from_phch(l1_dl_phychan_enum_type phch);

/*===========================================================================
FUNCTION        dlchmgr_get_ch_spec_idx_from_phch

DESCRIPTION     This function finds the channel specific idx of 
                the channel identified by the input parameters.

DEPENDENCIES    The input parameter "only_ext" must be used cautiously to filter out 
                the un-wanted channels. For example, if there are two PCCPCH neighbors 
                active at a time, the parameter "only_ext" can be used to filter out the 
                un-wanted channel.

RETURN VALUE
                cfg-db-idx: The channel specific config db idx of the channel

SIDE EFFECTS    None
===========================================================================*/
extern dl_ch_spec_cfg_db_idx_type dlchmgr_get_ch_spec_idx_from_phch(
  l1_dl_phychan_enum_type phch, boolean only_ext);

/*===========================================================================
FUNCTION        dlchmgr_wakeup_cb

DESCRIPTION     This function wakes up any DRX entity identified by the input 
                parameter. Channel specific managers can use this function as 
                a callback function to be registered with DRX.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_wakeup_cb(drx_entity_handle_type entity_handle);

/*===========================================================================
FUNCTION        dlchmgr_sleep_cb

DESCRIPTION     This function puts to sleep any DRX entity identified by the input 
                parameter. Channel specific managers can use this function as 
                a callback function to be registered with DRX.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_sleep_cb(drx_entity_handle_type entity_handle);

/*===========================================================================
FUNCTION        dlchmgr_resume_phch

DESCRIPTION     This function starts resume operation based on the physical
                channel type.

DEPENDENCIES    This function can be called only for physical channels that 
                are not shared among drx-entitites (eg: PICH).

RETURN VALUE
                TRUE: Channel resume was started successfully
                FALSE: Channel resume could not be started successfully

SIDE EFFECTS    None
===========================================================================*/
extern boolean dlchmgr_resume_phch (l1_dl_phychan_enum_type phch);

/*===========================================================================
FUNCTION        dlchmgr_suspend_phch

DESCRIPTION     This function starts suspend operation based on the physical
                channel type.

DEPENDENCIES    This function can be called only for physical channels that 
                are not shared among drx-entitites (eg: PICH).

RETURN VALUE
                TRUE: Channel suspend was started successfully
                FALSE: Channel suspend could not be started successfully

SIDE EFFECTS    None
===========================================================================*/
extern boolean dlchmgr_suspend_phch (l1_dl_phychan_enum_type phch);

/*===========================================================================
FUNCTION        dlchmgr_get_nbr_info_for_cell_trans

DESCRIPTION     This function finds the hwch, cell-information (psc & frequency) 
                for the neighbor cell on which sib read is going on currently.

DEPENDENCIES    None

RETURN VALUE
                TRUE: If the neighboring pccpch channel was identified.
                FALSE: If the neighboring pccpch channel was not identified.

SIDE EFFECTS    None
===========================================================================*/
extern boolean dlchmgr_get_nbr_info_for_cell_trans(
  stmr_counter_source_enum_type *nbcch_cntr, tlm_cell_info_struct_type *ncell_info);

/*===========================================================================
FUNCTION        dlchmgr_upd_assoc_status_for_cfg_idx

DESCRIPTION     This function updates the associated status fo the channel 
                identified by dynamic db index.

DEPENDENCIES    None

RETURN VALUE
                None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_upd_assoc_status_for_cfg_idx(dlchmgr_cfg_db_idx_type cfg_db_idx,
  boolean assoc_status);

/*===========================================================================
FUNCTION        dlchmgr_send_setup_done

DESCRIPTION     This function sends a setup done command to L1M for any channel 
                operation initiated by L1M.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_send_setup_done(dlchmgr_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dlchmgr_sccpch0_pich_add_drop_done

DESCRIPTION     This function sends a setup done to L1M after addition/drop 
                when the channels involved are PCH carrying SCCPCH0 and PICH.
                This function ensures that a repsonse to L1M is sent only after 
                the operation has got completed for both PCH carrying SCCPCH0 
                and PICH.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_sccpch0_pich_add_drop_done(dlchmgr_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dlchmgr_drx_cmd_rcvd

DESCRIPTION     This function handles the reception of drx command from RRC 
                via DRX.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_drx_cmd_rcvd(void);

/*===========================================================================
FUNCTION        dlchmgr_upd_drx_hdle_tbl

DESCRIPTION     This function updates the dynamic db idx associated with a drx 
                handle in dlchmgr_drx_hdle_cfg_idx_map_tbl[].

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_upd_drx_hdle_tbl(dlchmgr_cfg_db_idx_type cfg_db_idx,
  drx_entity_handle_type drx_hdle);

/*===========================================================================
FUNCTION        dlchmgr_register_hs_td_update_callback

DESCRIPTION     This function initializes the callback function
                for HS module.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/

extern void dlchmgr_register_hs_td_update_callback(
    /* The callback function pointer */
  DEMOD_HS_TD_UPDATE_CB_FUNC_TYPE *hs_td_update_cb);

/*===========================================================================
FUNCTION        dlchmgr_deregister_hs_td_update_callback

DESCRIPTION     These functions deassigns the callback function
                for HS module
DEPENDENCIES

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/

extern void dlchmgr_deregister_hs_td_update_callback(void);

/*===========================================================================
FUNCTION        DL_CCTRCH_TABLE_IDX_LOOKUP

DESCRIPTION     This function returns the CCTrCH table index associated with
                a physical channel that uses one.

DEPENDENCIES    The phychan database

RETURN VALUE    The table entry of the CCTrCH table that is associated with
                the passed physical channel of NO_CCTRCH_INDEX if either the
                channel has no CCTrCH table entry associated with it or if
                the channel is not active

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_cctrch_table_idx_lookup
(
  /* the physical channel whose associated CCTrCH table index is desired */
  l1_dl_phychan_enum_type phch
);


/*===========================================================================
FUNCTION        DL_PHYCHAN_TO_CCTRCH

DESCRIPTION     This function returns the transport channel or CCTrCH type
                that is associated with the passed physical channel type.
                This function should only be called for channels setup by RRC.
                It should not be called for internal channels.

DEPENDENCIES    The physical channel database

RETURN VALUE    Either the transport channel/CCTrCH type

SIDE EFFECTS
===========================================================================*/
extern dl_cctrch_ref_type_enum_type dl_phychan_to_cctrch
(
  /* the physical channel type */
  l1_dl_phychan_enum_type phch
);

/*===========================================================================
FUNCTION        DL_ENABLE_DEC_EVENTS

DESCRIPTION     This function enables or disables the decoder status and
                data read events according to the state that L1 is in
                at the time.

DEPENDENCIES    The current L1 state as observed from the variable l1m_state

RETURN VALUE    None

SIDE EFFECTS    The decoder events referred to above are either enabled or
                disabled on the currently running timeline as controlled
                by the input parameter "on".
===========================================================================*/
extern void dl_enable_dec_events(
  /* TRUE to enable events, FALSE to disable */
  boolean on
);

/*===========================================================================
FUNCTION        DL_SEND_SETUP_DONE_TO_L1M

DESCRIPTION     This function sets the status of a given CPHY_SETUP_REQ
                operation based on the input parameters and allocates,
                formats, and sends a setup done command.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The l1_cphy_setup_status.dl_phychan_status field is updated
                and one of PHYCHAN_DROP_DONE_CMD, PHYCHAN_CFG_DONE_CMD or
                PHYCHAN_ADD_DONE_CMD local commands is sent to L1M.
===========================================================================*/
extern void dl_send_setup_done_to_l1m
(
  /* the channel just setup/torndown */
  l1_dl_phychan_enum_type phch,

  /* TRUE if the operation was successful */
  boolean status,

  /* The operation that was performed */
  l1_setup_ops_enum_type op
);

/*===========================================================================
FUNCTION        DL_PHYCHAN_LOOKUP

DESCRIPTION     This function looks up the hardware channel index that a
                given physical channel type is active on.

DEPENDENCIES    The physical channel database

RETURN VALUE    0-3 if the channel is active on a hardware channel, where
                the actual value returned is the index of that hardware
                channel

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_phychan_lookup
(
  /* the reference physical channel to lookup */
  l1_dl_phychan_enum_type phch
);

/*===========================================================================
FUNCTION        DL_PHYCHAN_LOOKUP_FOR_ACTIVE

DESCRIPTION     This function looks up the hardware channel index that a
                given physical channel type is active on.

DEPENDENCIES    The physical channel database

RETURN VALUE    0-3 if the channel is active on a hardware channel, where
                the actual value returned is the index of that hardware
                channel

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_phychan_lookup_for_active
(
  /* the reference physical channel to lookup */
  l1_dl_phychan_enum_type phch
);

/*===========================================================================
FUNCTION        DL_ALIVE_PHYCHAN_LOOKUP

DESCRIPTION     This function looks up the hardware channel index on which a
                given physical channel type is not yet completely deactivated
                (that is, it is either active or ramping up or ramping down or
                sleeping).

DEPENDENCIES    The physical channel database

RETURN VALUE    The hardaware channel number on which the channel is located.
                If no such hardware is found, the return value is that of an
                invalid hardware channel.

SIDE EFFECTS    None
===========================================================================*/
uint8 dl_alive_phychan_lookup
(
  /* the reference physical channel to lookup */
  l1_dl_phychan_enum_type phch
);

/*===========================================================================
FUNCTION        dl_get_aich_parms

DESCRIPTION     This function returns the AICH power offset and transmit
                time offset and is provided as a utility for the UL module.

DEPENDENCIES    The dl_phychan_db

RETURN VALUE
                TRUE: if the AICH is assigned, setup, or active and
                additionally, as an output parameter the current value of the
                AICH power offset and AICH transmit time offset.
                FALSE: if the AICH is not active.

SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_get_aich_parms(
  /* AICH power offset output parameter */
  int8 *pwr_offset,

  /* AICH transmit time offset output parameter */
  l1_dl_aich_tt_enum_type *tt
);

/*===========================================================================
FUNCTION        dlchmgr_resume

DESCRIPTION     This function handles the procedures needed for drx entity resume
                in ch-spec-mgr and dl-phch.

DEPENDENCIES    None

RETURN VALUE
                TRUE: Channel resume was started successfully
                FALSE: Channel resume could not be started successfully

SIDE EFFECTS    None
===========================================================================*/
extern boolean dlchmgr_resume (drx_entity_handle_type entity_hdle,
  boolean fast_resume);

/*===========================================================================
FUNCTION        dlchmgr_suspend

DESCRIPTION     This function handles the procedures needed for drx entity suspend
                in ch-spec-mgr and dl-phch.

DEPENDENCIES    None

RETURN VALUE
                TRUE: Channel suspend was started successfully
                FALSE: Channel suspend could not be started successfully

SIDE EFFECTS    None
===========================================================================*/
extern boolean dlchmgr_suspend (drx_entity_handle_type entity_hdle,
  boolean fast_suspend);

/*===========================================================================
FUNCTION        dlchmgr_send_dl_weak_ind

DESCRIPTION     This function sends a DL WEAK IND to RRC. It decides whether 
                operation initiated by L1M.the channel being dropped is 
                serving or not from the physical channel type.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_send_dl_weak_ind(dlchmgr_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dlchmgr_get_cfg_db_idx_from_phch

DESCRIPTION     This function finds the config db index based on the 
                following conditions: physical channel type and the channel
                type-internal/external

DEPENDENCIES    None

RETURN VALUE
                config-db-idx: Config db index associated with the input params

SIDE EFFECTS    None
===========================================================================*/
extern dlchmgr_cfg_db_idx_type dlchmgr_get_cfg_db_idx_from_phch(
  l1_dl_phychan_enum_type phch, boolean only_ext);

/*===========================================================================
FUNCTION        dl_validate_tto_for_aset_upd

DESCRIPTION     This function does the TTO validation before the ASET update.

DEPENDENCIES    

RETURN VALUE    FALSE if TTO is invalid

SIDE EFFECTS   NONE
===========================================================================*/
extern boolean dl_validate_tto_for_aset_upd
(
  l1_setup_cmd_type *cmd
);

/*===========================================================================
FUNCTION        dl_validate_tto_for_sec_aset_upd

DESCRIPTION     This function does the TTO validation before the SEC ASET update.

DEPENDENCIES    

RETURN VALUE    FALSE if TTO is invalid

SIDE EFFECTS   NONE
===========================================================================*/
boolean dl_validate_tto_for_sec_aset_upd
(
  l1_setup_cmd_type *cmd
);

/*===========================================================================
FUNCTION        DL_PHYCHAN_ADD_VALIDATE

DESCRIPTION     This function validates that the addition of the physical
                channel as specified by the parameters referenced by the
                command can be carried out successfully, i.e. that hardware
                resources are available and the parameters are valid.

DEPENDENCIES    The physical channel database and command parameters

RETURN VALUE    TRUE if the command can be carried out, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_phychan_add_validate(
  /* the physical channel being added */
  l1_dl_phychan_enum_type phch,

  /* pointer to the dl_phychan_db part of ordered config */
  l1_dl_phychan_db_struct_type *phychan_db,

  /*setup req mask*/
  uint16 req_mask,

  /* drop phychan type */
  l1_dl_phychan_enum_type drop_dl_phychan
);




/*===========================================================================
FUNCTION        DL_PHYCHAN_RECFG_VALIDATE

DESCRIPTION     This function validates that the reconfiguration of the
                physical channel as specified by the parameters referenced
                in the command can be carried out successfully, i.e. that
                the channel is currently active and that the parameters are
                valid.

DEPENDENCIES    The physical channel database and command parameters

RETURN VALUE    TRUE if the command can be carried out, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_phychan_recfg_validate(
  /* the physical channel to reconfigure */
  l1_dl_phychan_enum_type phch,

  /* pointer to the RRC ordered config database */
  l1_dl_phychan_db_struct_type *phychan_db
);



/*===========================================================================
FUNCTION        DL_PHYCHAN_DROP_VALIDATE

DESCRIPTION     This function validates that the drop of the physical
                channel as specified by the parameters referenced by the
                command can be carried out successfully, i.e. that the
                channel is currently active.

DEPENDENCIES    The physical channel database and command parameters

RETURN VALUE    TRUE if the command can be carried out, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_phychan_drop_validate(
  /* which physical channel to drop */
  l1_dl_phychan_enum_type phch
);



/*===========================================================================
FUNCTION        DL_VALIDATE_ADD_SCCPCH

DESCRIPTION     This function validates the parameters for the SCCPCH and
                if it's use is specified the PICH.

DEPENDENCIES    None

RETURN VALUE    FALSE if any of a zillion parameters are in error.  TRUE
                otherwise

SIDE EFFECTS    None
===========================================================================*/
extern boolean dlchmgr_validate_sccpch_parms
(
  l1_sf_enum_type sf,

  l1_dl_phychan_enum_type phch,

  l1_dl_phychan_db_struct_type *phychan_db
);




/*===========================================================================
FUNCTION        DL_VALIDATE_AICH_PARMS

DESCRIPTION     This function validates the parameters associated with the
                AICH that is implicitly set up when a PRACH is added.

DEPENDENCIES    The RRC ordered config database

RETURN VALUE    TRUE if all parameters are valid, FALSE otherwise.

SIDE EFFECTS    None
===========================================================================*/
extern boolean dlchmgr_validate_aich_parms
(
  /* pointer to the AICH portion of the RRC ordered config database */
  l1_aich_parms_struct_type *aich_db
);



/*===========================================================================
FUNCTION        DL_VALIDATE_DPCH_PARMS

DESCRIPTION     This function validates the DPCH parameters appearing in
                Ordered Config

DEPENDENCIES    RRC Ordered config as observed by the passed in pointer to
                a l1_dl_phychan_struct_type

RETURN VALUE    TRUE if all parameters are valid, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
extern boolean dlchmgr_validate_dpch_parms
(
  l1_dl_phychan_db_struct_type *phychan_db
);

/*===========================================================================
FUNCTION        dlchmgr_en_dis_svc

DESCRIPTION     This function enables or disables any r99 service on the 
                shared channel.

DEPENDENCIES    None

RETURN VALUE
                TRUE: Enable/Disable R99 Service is successful.
                FALSE: Enable/Disable R99 Service fails

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dlchmgr_en_dis_svc(
/*cctrch index*/
uint8 cctrch_idx,

/*Service to be handled*/
dlshmgr_svr_type_enum_type shmgr_svc_type,

/*Whether the service is getting enabled/disabled*/
boolean enable
);

/*===========================================================================
FUNCTION        dlchmgr_pre_cell_trans_cb

DESCRIPTION     This function should be called by L1M before starting to process 
                CPHY_CELL_TRANSITION_REQ from RRC.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void dlchmgr_pre_cell_trans_cb(void);

/*===========================================================================
FUNCTION        dlchmgr_post_cell_trans_cb

DESCRIPTION     This function should be called by L1M before sending response for 
                CPHY_CELL_TRANSITION_REQ to RRC.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void dlchmgr_post_cell_trans_cb(boolean cell_trans_status);

/*===========================================================================
FUNCTION        dlchmgr_rl_dpch_frame_offset_not_zero

DESCRIPTION     This function finds the new tau_dpch of the channel 
                identified by physical channel type and compares with the 
                old tau_dpch value.

DEPENDENCIES    

RETURN VALUE
                True: If tau-DPCH change greater than 1

SIDE EFFECTS    None
===========================================================================*/
extern boolean dlchmgr_rl_dpch_frame_offset_not_zero(const l1_setup_cmd_type* l1_setup_cmd, uint16 rl_tto, uint16 psc);

/*===========================================================================
FUNCTION        dlchmgr_is_drx_cleanup_needed

DESCRIPTION     This function should be called by individual channel specific
                managers to find whether DRX cleanup is needed during Failure handling

DEPENDENCIES    None

RETURN VALUE
                TRUE: DRX cleanup needed.
                FALSE: DRX cleanup not needed

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dlchmgr_is_drx_cleanup_needed(dlchmgr_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dl_aich_send_add_drop_done

DESCRIPTION     This function sends a setup done to L1M after addition/drop 
                for AICH.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dl_aich_send_add_drop_done(dlchmgr_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dlchmgr_get_cfg_db_idx_from_drx_hdle

DESCRIPTION     This function finds the config db index associated with a drx 
                handle.

DEPENDENCIES    None

RETURN VALUE
                config-db-idx: Config db index associated with the drx handle.

SIDE EFFECTS    None
===========================================================================*/
extern dlchmgr_cfg_db_idx_type dlchmgr_get_cfg_db_idx_from_drx_hdle(
  drx_entity_handle_type drx_hdle);

/*===========================================================================
FUNCTION        DL_VALIDATE_DPCH_RECFG_WITH_TD

DESCRIPTION     This function validates the STTD configuration to be applied during DPCH reconfiguration

DEPENDENCIES    None

RETURN VALUE    TRUE if MIMO with S-CPICH is not active and cell has No-TD
                FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_validate_dpch_recfg_with_td
(
  l1_setup_cmd_type *setup
);

/*===========================================================================
FUNCTION        dlchmgr_is_cell_fit_for_aset_addition

DESCRIPTION     This function checks if a cell can be added to ASET or not

DEPENDENCIES    None

RETURN VALUE    TRUE, if cell can be added
                FALSE, otherwise.

SIDE EFFECTS    None
===========================================================================*/
extern boolean dlchmgr_is_cell_fit_for_aset_addition(
  /** input cell ref pos */
  uint32 inp_cell_max_pos,
  /** input cell scr code */
  uint16 inp_cell_scr_code,
  /** request type */
  boolean ext_req);

/*===========================================================================
FUNCTION        dlchmgr_setup_pich_auto_sccpch

DESCRIPTION     This function prepares the database to enable SCCPCH autonomously by FW

DEPENDENCIES    None

RETURN VALUE    Returns the status of Auto SCCPCH resume

SIDE EFFECTS    None
===========================================================================*/
extern boolean dlchmgr_setup_pich_auto_sccpch(boolean enable);

/*===========================================================================
FUNCTION        dlchmgr_clear_pich_auto_sccpch_bmsk

DESCRIPTION     This function clears the auto SCCPCH bit mask in SCCPCH/PICH cfg databases

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_clear_pich_auto_sccpch_bmsk(void);

/*===========================================================================
FUNCTION        dlchmgr_is_any_pccpch_active

DESCRIPTION     This function returns TRUE if any PCCPCH is active

DEPENDENCIES    None

RETURN VALUE
                TRUE: PCCPCH active
                FALSE: No PCCPCH is active

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dlchmgr_is_any_pccpch_active(void);

/*===========================================================================
FUNCTION        dlchmgr_fill_decoded_hsrach_fpdch_params

DESCRIPTION     

DEPENDENCIES    None

RETURN VALUE
                

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dlchmgr_fill_decoded_hsrach_fpdch_params( 
  uint16 ovsf, 
  uint32 trans_time_offset);

/*===========================================================================
FUNCTION        dlchmgr_is_ext_pccpch_active

DESCRIPTION     This function returns TRUE if S PCCPCH is active

DEPENDENCIES    None

RETURN VALUE
                TRUE: PCCPCH active
                FALSE: No PCCPCH is active

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dlchmgr_is_ext_pccpch_active(void);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION        dlchmgr_send_dl_weak_ind_for_no_lock

DESCRIPTION     This function sends DL weak indication due to no lock.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_send_dl_weak_ind_for_no_lock(l1_dl_phychan_enum_type phch);
#endif

/*===========================================================================
FUNCTION        dlchmgr_aset_drop_rl_done_cb

DESCRIPTION     This function hadnles the drop of radio links during and ASU
                procedure

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_aset_drop_rl_done_cb(uint8 cctrch_idx, l1_dl_phychan_enum_type phch_type,
                                         dl_phch_cfg_handle_type phch_handle, boolean status);

/*===========================================================================
FUNCTION        dlchmgr_validate_sec_fdpch_parms

DESCRIPTION     This function validates the SEC FDPCH parameters appearing in
                Ordered Config

DEPENDENCIES    RRC Ordered config as observed by the passed in pointer to
                a l1_dl_phychan_struct_type

RETURN VALUE    TRUE if all parameters are valid, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/

extern boolean dlchmgr_validate_sec_fdpch_parms
(
  l1_dl_phychan_db_struct_type *phychan_db
);


/*===========================================================================
FUNCTION        dl_phychan_sec_fdpch_tto_validate

DESCRIPTION     Function to validate the sec fdpch Tau DPCH as specified in the 
                CPHY_SETUP_REQ. 

DEPENDENCIES    None

RETURN VALUE    Boolean

SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_phychan_sec_fdpch_tto_validate(l1_setup_cmd_type *setup);

/*------------------EXTERNAL FUNCTIONS DEFINITION END------------------------------------*/

#endif /* DLCHMGR_H */
