#ifndef TDSDLCHMGR_H
#define TDSDLCHMGR_H
/*===========================================================================
                 D L C H M G R . H

GENERAL DESCRIPTION
  This file contains the declarations and definitions of dlchmgr needed
  across modules.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2007 - 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/tlm.c_v   1.153   17 Jul 2002 16:36:16   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsdlchmgr.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------

===========================================================================*/
#include "tdscdma_variation.h"
#include "customer.h"
#include "comdef.h"
#include "tdsl1const.h"
#include "tdsl1rrcif.h"
#include "tdsdrxmgr.h"
//#include "tdsdldec.h"
#include "tdsdlcctrch.h"
#include "tdsdldecdata.h"
#include "tdsdlphch.h"
#include "mcaltds_dec.h"

//#define FEATURE_TDSCDMA_PLT_SKIP_PCCPCH
/*Keep the DlChMgr dynamic db size as 6. The worst possible channel combination is:
1. SCCPCH0
2. SCCPCH1
3. PICH
4. PCCPCH setup by RRC
5. PCCPCH setup by SRCH*/
#define TDSDLCHMGR_CFG_DB_SIZE ((tdsdlchmgr_cfg_db_idx_type)6)

/*The number of channel specific managers:
BCCH, SCCPCH, PICH, Non-scheduled channels (AICH, DPCH)*/
#define TDSDLCHMGR_NUM_ALWAYS_PRESENT_CH_SPEC_MGR (4)
#define TDSDLCHMGR_NUM_CH_SPEC_MGR  (TDSDLCHMGR_NUM_ALWAYS_PRESENT_CH_SPEC_MGR)

/*Macros that hold the invalid db index values*/
#define TDSDLCHMGR_INVALID_CFG_DB_IDX 0xFF
#define TDSDLCHMGR_INVALID_CH_SPEC_IDX (0xFF)

/*Macros that hold the done callbacks for the various channel operations*/
#define TDSDLCHMGR_ADD_DONE_CB (tdsdlchmgr_add_done_cb)
#define TDSDLCHMGR_RECFG_DONE_CB (tdsdlchmgr_recfg_done_cb)
#define TDSDLCHMGR_DROP_DONE_CB (tdsdlchmgr_drop_done_cb)
#define TDSDLCHMGR_RESUME_DONE_CB (tdsdlchmgr_resume_done_cb)
#define TDSDLCHMGR_SUSPEND_DONE_CB (tdsdlchmgr_suspend_done_cb)
//#define TDSDLCHMGR_ASET_DROP_RL_DONE_CB (tdsdlchmgr_aset_drop_rl_done_cb)//TDS no use
//#define TDSDLCHMGR_RESUME_ASET_OPS_DONE_CB (tdsdlchmgr_resume_aset_ops_done_cb)

  /*The type that respresents the dynamic db index of all channel specific manager*/
typedef uint8 tdsdlchmgr_dlch_spec_cfg_db_idx_type;

  /*The type that respresents the config db index of DlChMgr*/
typedef uint8 tdsdlchmgr_cfg_db_idx_type;

  /*The type that can hold the cleanup bitmask in DlChMgr*/
typedef uint16 tdsdlchmgr_cleanup_bmask_type;

/*The enum that defines the index for the DlChMgr static db*/
typedef enum {
TDSDLCHMGR_STATIC_TYPE_ACQ_BCH,
TDSDLCHMGR_STATIC_TYPE_BCH_S_SIB_READ,
TDSDLCHMGR_STATIC_TYPE_BCH_N_SIB_READ,
TDSDLCHMGR_STATIC_TYPE_SCCPCH0,
TDSDLCHMGR_STATIC_TYPE_PICH,
TDSDLCHMGR_STATIC_TYPE_DPCH,

TDSDLCHMGR_STATIC_TYPE_NUM_TYPES
} tdsdlchmgr_static_db_idx_type;

/*The enum that defines the index for the DlChMgr static db*/
typedef struct
{
  /*0 - One time, 1- Continuous.*/
  /*Yong comment out TDM mode for new interface of SW and FW*/
  /*tdsl1_dl_phycfg_dl_tdm_mode_enum_type tdm_mode;*/
  /*Yong comment out TDM mode for new interface of SW and FW*/
  /*Channel Present repetition period. Valid regardless of TDM mode. 
    Valid values: 2,4,8,16,32,64 frames */
  uint8  chanOnRepPeriod;
  
  /*Channel Present on-off bit mask to be applied at action time.  
  Bit[0..(chanOnRepPeriod-1)] with each bit indicates if Physical channel is present in
  the corresponding frame. 1: phy channel is present. 0: not present. Bit[chanOnRepPeriod..63] 
  shall be set to 0 when chanOnRepPeriod is less than 64.*/
  uint64  chanPresentMask;

  /*index FW to insret dob when zero tb, PCH will set to 1*/
  uint8 insertDobForZeroTB;

#ifdef FEATURE_TDSCDMA_PCH_FET
  uint8 pch_fet_enable;
#endif

  /*! \brief SFN number within TTI at the action time when config is applied  Valid :0..255.*/
  //uint8  ttiSfn;
}tdsdlchmgr_spec_tdm_bitmask_type;

typedef struct
{
   uint16 tdsdlchmgr_wakeup_sfn_mode_DRXLength;
   uint16 tdsdlchmgr_DRX_length;
   uint8  tdsdlchmgr_wakeup_length;
}tdsdlchmgr_spec_drx_para_type;

#ifdef FEATURE_TDSCDMA_CELL_FACH_MEAS_OCCASION

#define TDSDLCHMGR_CRNTI_QUERY_TIMER_MAX     80

/* FACH FMO measurement parameters */
typedef struct
{
  boolean  fach_meas_incl; /* Whether FACH meas info valid ? */
  uint32   k_value;
  boolean  k_valid;
  uint32   m_rep;    /* M_REP = 2^k only if k_valid is TRUE */
  boolean  c_rnti_valid;
  uint16   c_rnti;   /* C_RNTI value only if c_rnti_valid is TRUE */

  uint8    n_tti;  /* SCCPCH max TTI, N_TTI */
  uint16   first_meas_oksn_sfn; /* first FACH meas oksn SFN */
  uint32   t_meas_rep; /* Ntti * M_REP frame, 0 is invalid */

} tdsdlchmgr_fmo_parms_struct_type;

#endif

//jintao add for dl fach race condition
typedef struct
{
  tdsl1def_l1_local_cmd_hdr_type hdr;

  /*cell parameter ID of the channel, range:0~127*/
  uint16 cell_id;

  /*frequency of the channel*/
  uint16 freq;

  //when drop n-pccpch in fach status,need resump fach and rach
  boolean resume_sccpch_prach;

}tdsdlchmgr_cell_change_struct_type;

/*Type definitions for the various function types to be used between DlChMgr 
and channel specific manager*/
typedef boolean TDSDLCHMGR_CH_SPEC_PRE_OP_CB_FUNC_TYPE(
  tdsdlchmgr_dlch_spec_cfg_db_idx_type ch_spec_idx);

typedef boolean TDSDLCHMGR_CH_SPEC_PRE_ADD_CB_FUNC_TYPE(
  tdsdlchmgr_dlch_spec_cfg_db_idx_type ch_spec_idx, tdsl1dl_phychan_db_struct_type *phychan_db);

typedef void TDSDLCHMGR_GEN_OP_DONE_CB_FUNC_TYPE( void );

typedef void TDSDLCHMGR_GEN_OP_PROC_FUNC_TYPE( void );

typedef void TDSDLCHMGR_GEN_OP_CHECK_FUNC_TYPE( tdsdlchmgr_cfg_db_idx_type cfg_db_idx);

typedef boolean TDSDLCHMGR_CH_SPEC_POST_OP_CB_FUNC_TYPE(
  tdsdlchmgr_dlch_spec_cfg_db_idx_type ch_spec_idx);

typedef void TDSDLCHMGR_RESPONSE_CB_FUNC_TYPE(
  tdsdlchmgr_cfg_db_idx_type cfg_db_idx);

typedef tdsdlchmgr_dlch_spec_cfg_db_idx_type TDSDLCHMGR_DLCH_SPEC_IDX_ALLOC_FUN_TYPE
  (tdsdlchmgr_cfg_db_idx_type chmgr_cfg_db_idx);

typedef void TDSDLCHMGR_DLCH_SPEC_IDX_DEALLOC_FUN_TYPE
  (tdsdlchmgr_dlch_spec_cfg_db_idx_type ch_spec_idx);

typedef void TDSDLCHMGR_DLCH_SPEC_MGR_INIT_CB_TYPE( void );

typedef boolean TDSDLCHMGR_CH_SPEC_PRE_SUSP_RES_CB_FUNC_TYPE(
  tdsdrxmgr_entity_handle_type entity_handle);

typedef boolean TDSDLCHMGR_CH_SPEC_POST_SUSP_RES_CB_FUNC_TYPE(
  tdsdlchmgr_dlch_spec_cfg_db_idx_type chspec_dyn_db_idx);

typedef tdsdlchmgr_spec_drx_para_type   TDSDLCHMGR_CH_SPEC_GET_DRXPARA_CB_FUNC_TYPE( void );

typedef tdsdlchmgr_spec_tdm_bitmask_type TDSDLCHMGR_CH_SPEC_GET_BITMASK_CB_FUNC_TYPE(uint16 activation_time);

/*tdsdlchmgr_resp_func_cb_struct_type holds the function pointers to be called 
for sending repsonse after channel operations*/
typedef struct {
  /*Function Pointer to send response after an addition operation*/
  TDSDLCHMGR_RESPONSE_CB_FUNC_TYPE *chmgr_add_resp_cb;

  /*Function Pointer to send response after a recfg operation*/
  TDSDLCHMGR_RESPONSE_CB_FUNC_TYPE *chmgr_recfg_resp_cb;

  /*Function Pointer to send response after a drop operation*/
  TDSDLCHMGR_RESPONSE_CB_FUNC_TYPE *chmgr_drop_resp_cb;

  /*Function Pointer to send response during cleanup after an operation failure*/
  TDSDLCHMGR_RESPONSE_CB_FUNC_TYPE *chmgr_cleanup_resp_cb;

} tdsdlchmgr_resp_func_cb_struct_type;

/*tdsdlchmgr_idx_mgmt_func_struct_type holds the function pointers to be called 
for allocating and de-allocating index in channel specific manager*/
typedef struct {
  /*Function Pointer to allocate index in channel specific manager*/
  TDSDLCHMGR_DLCH_SPEC_IDX_ALLOC_FUN_TYPE *ch_spec_idx_alloc_cb;

  /*Function Pointer to de-allocate index in channel specific manager*/
  TDSDLCHMGR_DLCH_SPEC_IDX_DEALLOC_FUN_TYPE *ch_spec_idx_dealloc_cb;
} tdsdlchmgr_idx_mgmt_func_struct_type;

/*tdsdlchmgr_op_cb_func_struct_type holds the pointers to channel specific 
manager functions to be called pre and post an operation from DlChMgr*/
typedef struct {
  /*Pointer to channel specific manager function that has to be 
  called before add*/
  TDSDLCHMGR_CH_SPEC_PRE_ADD_CB_FUNC_TYPE *ch_spec_pre_add_cb;

  /*Pointer to channel specific manager function that has to be 
  called after add*/
  TDSDLCHMGR_CH_SPEC_POST_OP_CB_FUNC_TYPE *ch_spec_post_add_cb;

  /*Pointer to channel specific manager function that has to be called 
  before re-cfg*/
  TDSDLCHMGR_CH_SPEC_PRE_ADD_CB_FUNC_TYPE *ch_spec_pre_recfg_cb;

  /*Pointer to channel specific manager function that has to be called 
  after re-cfg*/
  TDSDLCHMGR_CH_SPEC_POST_OP_CB_FUNC_TYPE *ch_spec_post_recfg_cb;

  /*Pointer to channel specific manager function that has to be called 
  before drop*/
  TDSDLCHMGR_CH_SPEC_PRE_OP_CB_FUNC_TYPE *ch_spec_pre_drop_cb;

  /*Pointer to channel specific manager function that has to be called 
  after drop*/
  TDSDLCHMGR_CH_SPEC_POST_OP_CB_FUNC_TYPE *ch_spec_post_drop_cb;

  /*Pointer to channel specific manager function that has to be called 
  before resume*/
  TDSDLCHMGR_CH_SPEC_PRE_SUSP_RES_CB_FUNC_TYPE *ch_spec_pre_resume_cb;

  /*Pointer to channel specific manager function that has to be called 
  after resume*/
  TDSDLCHMGR_CH_SPEC_POST_SUSP_RES_CB_FUNC_TYPE *ch_spec_post_resume_cb;

  /*Pointer to channel specific manager function that has to be called 
  before suspend*/
  TDSDLCHMGR_CH_SPEC_PRE_SUSP_RES_CB_FUNC_TYPE *ch_spec_pre_suspend_cb;

  /*Pointer to channel specific manager function that has to be called 
  after suspend*/
  TDSDLCHMGR_CH_SPEC_POST_SUSP_RES_CB_FUNC_TYPE *ch_spec_post_suspend_cb;
} tdsdlchmgr_op_cb_func_struct_type;
/*tdsdlchmgr_schedule_para_cb_func_struct_type holds the pointers to channel specific 
manager functions to be called get&update channel TDM bitmask an operation from DlChMgr*/
typedef struct {
  TDSDLCHMGR_CH_SPEC_GET_DRXPARA_CB_FUNC_TYPE    *ch_spec_get_drxpara_cb;
  TDSDLCHMGR_CH_SPEC_GET_BITMASK_CB_FUNC_TYPE    *ch_spec_get_bitmask_cb;
} tdsdlchmgr_schedule_para_cb_func_struct_type;

/*Structure that holds the semi static information for any channel. The 
semi-static information can only be changed at compile time*/
typedef struct {
  /*physical channel type*/
  tdsl1_dl_phychan_enum_type phch_type;

  /*Whether the channel needs a CCTrCh*/
  boolean cctrch_needed;

  /* Whether the suspend/resume operations on a channel should be
     * fast/slow in mcal-dl, Fast: immedietly response after configure FW; 
     * slow: response at frame boundary after configure FW 
     */
  boolean fast_ch_op_needed;

  /*Whether the channel has to be added in ACTIVE or 
  SUSPENDED state while addition; */
  tdsdlphch_state_enum_type add_state;

  /*Whether the channel is setup by a L1 module or RRC*/
  boolean int_channel;

  /*Pointer to structure that holds response functions*/
  tdsdlchmgr_resp_func_cb_struct_type *chmgr_resp_cb;

  /*Pointer to structure that holds ch-spec-mgr index 
  management functions*/
  tdsdlchmgr_idx_mgmt_func_struct_type *ch_spec_idx_mgmt;

  /*Pointer to structure that holds ch-spec-mgr pre & post 
  operation management functions*/
  tdsdlchmgr_op_cb_func_struct_type *ch_spec_op_cb;
  /*Pointer to structure that holds ch-spec-mgr get&update channel TDM bitmask 
  operation management functions*/
  tdsdlchmgr_schedule_para_cb_func_struct_type *ch_spec_schedule_para_cb;
} tdsdlchmgr_static_db_struct_type;
/*cell search type for internal PCCPCH*/

/*channel suspend/resume status*/
typedef enum {
TDSDLCHMGR_RESUMED_STATUS,
TDSDLCHMGR_RESUME_ONGOING_STATUS,
TDSDLCHMGR_SUSPENDED_STATUS,
TDSDLCHMGR_SUSPEND_ONGOING_STATUS,
TDSDLCHMGR_SUSPEND_RESUME_TYPE_NUM_TYPES
} tdsdlchmgr_suspend_resume_status_type;


/*Structure that holds the dynamic information for any channel. The values 
in this structure are decided at run-time.*/
typedef struct {
  
  /*cell parameter ID of the channel, range:0~127*/
  uint16 cell_id;

  /*frequency of the channel*/
  uint16 freq;

  /*Operation that is in progress in the channel*/
  tdsl1_setup_ops_enum_type operation;
  /*cell search type for internal PCCPCH*/
  tdsl1_dl_phycfg_pccpch_tti_detection_enum_type tti_detection_algorithm;
  
  /*0 - One time, 1- Continuous.*/
  /*Yong comment out TDM mode for new interface of SW and FW*/
  /*tdsl1_dl_phycfg_dl_tdm_mode_enum_type tdm_mode;*/
  /*Yong comment out TDM mode for new interface of SW and FW*/
  
  /*For some channels Add state is decided at run time; Store the add state
    for all channels here; */
  tdsdlphch_state_enum_type curr_add_state;
  
  /*channel suspend/resume status*/
  tdsdlchmgr_suspend_resume_status_type  ch_suspend_resume_status;
  /*channel delay drop is doing*/
  boolean ch_delay_drop_ongoing_flag;
  /*dl internal operation or not
    TRUE: DL initiate resume/suspend, no respons to DRX;
          DL initiate recfg, no respons to L1M;
    FALSE: DRX initiate resume/suspend, respons to DRX
	      L1M initiate recfg, respons to L1M;
    */
  boolean dl_internal_operation;
  	
  /*Whether this entry is free or in-use*/
  boolean idx_in_use;

  /*The static db entry associated with this channel*/
  tdsdlchmgr_static_db_idx_type static_db_idx;

  /*The index for the channel in to the corresponding channel manager's db*/
  tdsdlchmgr_dlch_spec_cfg_db_idx_type ch_spec_idx;

  /*phch handle for the channel in dl-phch*/
  tdsdlphch_cfg_handle_type phch_handle;

  /*CCTrCh idx associated with the channel*/
  mcaltds_dec_cctrch_id cctrch_idx;

  /*Status (pass/failure) of the operation on the channel*/
  boolean op_status;

  /*In case of channel operation failure, cleanup_bmask indicates the 
  clean up operations that has to be performed.*/
  tdsdlchmgr_cleanup_bmask_type cleanup_bmask;

  /* flag to indicate this channel config is part of common/general 
     drop all operation which is used to process idle req or cell trans req */
  boolean ch_part_of_drop_gen_op;

  /* flag to indicate this channel config is part of common/general 
     drx operation such as suspend all or resume all */
  boolean ch_part_of_drx_gen_op;
#ifdef FEATURE_TDSCDMA_LVA_DECODE
  /*for LVA using*/
  uint8 call_type_mask;
#endif
} tdsdlchmgr_dynamic_db_struct_type;

typedef struct {
  tdsdlchmgr_static_db_struct_type stat_db;

  tdsdlchmgr_dynamic_db_struct_type dyn_db;
} tdsdlchmgr_ch_cfg_struct_type;

/*tdsdlchmgr_int_setup_enum_type holds the various types of internal 
channel setups*/
typedef enum {
TDSDLCHMGR_SETUP_TYPE_ACQ_BCH,
//TDSDLCHMGR_SETUP_TYPE_SFN_WRC,
TDSDLCHMGR_SETUP_TYPE_TOTAL_NUM
} tdsdlchmgr_int_setup_enum_type;

/*tdsdlchmgr_int_setup_struct_type holds all the information that is needed to 
setup an internal channel.*/
/*+Yongh Remove freq and cellid in internal PCCPCH setup+*/
typedef struct {
  /*sttd is used or not in this cell*//*Xuqiang delete*/
  boolean sttd_used;

  /* cell para id for the cell */
  tdsl1_dl_phycfg_pccpch_tti_detection_enum_type tti_detection_algorithm;

  tdsdlchmgr_int_setup_enum_type setup_type;/*Xuqiang delete*/
} tdsdlchmgr_int_setup_struct_type;
/*-Yongh Remove freq and cellid in internal PCCPCH setup-*/


/******************************************************************************
                     COMMAND: TDSDLCHMGR_DL_CPHY_SETUP_LOCAL_CMD_TYPE
******************************************************************************/

// cfitoz
typedef struct
{
  /* common command header */
  tdsl1def_l1_local_cmd_hdr_type hdr;

  /* TRUE if the setup was successful */
  boolean status;

  /* The CCTrCH reference enum type, what kind of CCTrCH was set modified */
  tdsdldec_cctrch_ref_type_enum_type cctrch_type;

  /* The index in the CCTrCH table of the CCTrCH that was modified */
  uint8 cctrch_index;

  /* The type of physical channel that was modified */
  tdsl1_dl_phychan_enum_type phch;

} tdsdlchmgr_dl_cphy_setup_local_cmd_type;

/* NOTE: L1M handles this command so there is no prototype for the handler
   listed in dl_cmd.h */

/*--------------------------------------------------------------------------
                ENUM: TDSDLCHMGR_DL_PHYCHAN_ACTION_REASON_ENUM_TYPE

This enumerates the various reasons that a request for the setup or teardown
of a physical channel was issued.
--------------------------------------------------------------------------*/

// cfitoz
typedef enum {
  TDSDLCHMGR_PHYCHAN_REASON_CELL_TRANS_DROP_ALL,
  TDSDLCHMGR_PHYCHAN_REASON_IDLE_REQ,
  TDSDLCHMGR_PHYCHAN_REASON_SLEEP_DROP_ALL,
  TDSDLCHMGR_PHYCHAN_REASON_INVALID
} tdsdlchmgr_dl_phychan_action_reason_enum_type;

typedef struct {
  /* command l1m local command header */
  tdsl1def_l1_local_cmd_hdr_type hdr;

  /* this command is just used for sequencing.  There are no other
     parameters currently associated with it */
} tdsdlchmgr_dl_cell_trans_drop_done_cmd_type;

extern tdsdlchmgr_ch_cfg_struct_type tdsdlchmgr_cfg_db[TDSDLCHMGR_CFG_DB_SIZE];
//jintao add for dl fach race condition
extern boolean tdsdlchmgr_fach_state_npccpch_ext_add_flag;
//extern boolean tdsdlchmgr_fach_state_npccpch_ext_drop_flag;
#ifdef FEATURE_TDSCDMA_CELL_FACH_MEAS_OCCASION
extern boolean tdsdlchmgr_fmo_enable;
extern uint16  tdsdlchmgr_update_fach_bitmask_sfn;
extern tdsdlchmgr_fmo_parms_struct_type tdsdlchmgr_fmo_parms;
extern boolean tdsdlchmgr_fmo_stop_flag;
extern boolean tdsdlchmgr_fmo_first_sched;
#endif

extern tdsdlchmgr_static_db_struct_type tdsdlchmgr_static_db[TDSDLCHMGR_STATIC_TYPE_NUM_TYPES];

extern boolean tdsdlchmgr_PICH_higher_priority_than_NSIB;
extern uint8  tdsdlchmgr_piVal;

#define TDSDLCHMGR_IS_CFG_DB_IDX_INVALID(cfg_db_idx)    \
	  (cfg_db_idx >= TDSDLCHMGR_CFG_DB_SIZE)
	  
#define TDSDLCHMGR_IS_CFG_DB_IDX_VALID(cfg_db_idx)    \
  (!TDSDLCHMGR_IS_CFG_DB_IDX_INVALID(cfg_db_idx))

#define TDSDLCHMGR_IS_CCTRCH_IDX_INVALID(cctrch_idx)  \
  (cctrch_idx == TDSDLDEC_L1_ID_INDEX_UNUSED)

#define TDSDLCHMGR_IS_CCTRCH_IDX_VALID(cctrch_idx)  \
  (cctrch_idx != TDSDLDEC_L1_ID_INDEX_UNUSED)

/*This is not an accurate check for the invalidity of channel specific index.
Any value larger than the individual channel manager's dynamic db size
should be considered as invalid.*/
#define TDSDLCHMGR_IS_CH_SPEC_IDX_INVALID(ch_spec_idx)  \
  (ch_spec_idx == TDSDLCHMGR_INVALID_CH_SPEC_IDX)

/*This is not an accurate check for the validity of channel specific index.
Any value larger than the individual channel manager's dynamic db size
should be considered as invalid.*/
#define TDSDLCHMGR_IS_CH_SPEC_IDX_VALID(ch_spec_idx)  \
  (ch_spec_idx != TDSDLCHMGR_INVALID_CH_SPEC_IDX)

#define TDSDLCHMGR_IS_DRX_HDLE_INVALID(drx_hdle)  \
  (drx_hdle == TDSDRXMGR_ENTITY_HANDLE_INVALID)

#define TDSDLCHMGR_IS_DRX_HDLE_VALID(drx_hdle)  \
  (drx_hdle != TDSDRXMGR_ENTITY_HANDLE_INVALID)

#define TDSDLCHMGR_IS_SRCH_SET_UPDATE_NEEDED_FROM_L1_CMD_PTR(l1_setup_cmd_ptr)  \
  (l1_setup_cmd_ptr->add_dl_phychan == L1_DL_PHYCHAN_PCCPCH_N)

#define TDSDLCHMGR_IS_PHCH_HDLE_INVALID(phch_cfg_db_idx)  \
  (phch_cfg_db_idx == TDSDLPHCH_INVALID_CFG_HANDLE)

#define TDSDLCHMGR_IS_STATIC_DB_IDX_VALID_FOR_BCCH(dlchmgr_static_db_idx)  \
  ((dlchmgr_static_db_idx == TDSDLCHMGR_STATIC_TYPE_ACQ_BCH) ||  \
   (dlchmgr_static_db_idx == TDSDLCHMGR_STATIC_TYPE_BCH_S_SIB_READ) ||  \
   (dlchmgr_static_db_idx == TDSDLCHMGR_STATIC_TYPE_BCH_N_SIB_READ))

#define TDSDLCHMGR_IS_STATIC_DB_IDX_VALID_FOR_SCCPCH(dlchmgr_static_db_idx)  \
  ((dlchmgr_static_db_idx == TDSDLCHMGR_STATIC_TYPE_SCCPCH0) ||  \
   (dlchmgr_static_db_idx == TDSDLCHMGR_STATIC_TYPE_SCCPCH1))

#define TDSDLCHMGR_IS_STATIC_DB_IDX_VALID_FOR_PICH(dlchmgr_static_db_idx)  \
  (dlchmgr_static_db_idx == TDSDLCHMGR_STATIC_TYPE_PICH)

#define TDSDLCHMGR_IS_STATIC_DB_IDX_VALID_FOR_NSCH(dlchmgr_static_db_idx)  \
   ((dlchmgr_static_db_idx == TDSDLCHMGR_STATIC_TYPE_DPCH) || \
    (dlchmgr_static_db_idx == TDSDLCHMGR_STATIC_TYPE_AICH))

#define TDSDLCHMGR_GET_FREQ_FROM_INT_SETUP_PTR(int_setup_ptr)    \
  (int_setup_ptr->freq)

#define TDSDLCHMGR_GET_CELLID_FROM_INT_SETUP_PTR(int_setup_ptr)    \
  (int_setup_ptr->cell_id)
  
#define TDSDLCHMGR_GET_TTIDET_ALGOR_FROM_INT_SETUP_PTR(int_setup_ptr)    \
	  (int_setup_ptr->tti_detection_algorithm)
	  
#define TDSDLCHMGR_GET_TTIDET_ALGOR_FROM_CFG_DB_IDX(cfg_db_idx)  \
	(tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.tti_detection_algorithm)
	
#define TDSDLCHMGR_GET_FREQ_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.freq)

#define TDSDLCHMGR_GET_CPI_FROM_CFG_DB_IDX(cfg_db_idx) \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.cell_id)

#define TDSDLCHMGR_GET_OP_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.operation)

#define TDSDLCHMGR_GET_CURR_ADD_STATE_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.curr_add_state)  

#define TDSDLCHMGR_GET_SUSPEND_RESUME_STATE_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.ch_suspend_resume_status) 
  
#define TDSDLCHMGR_GET_DELAY_DROP_STATE_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.ch_delay_drop_ongoing_flag)

#define TDSDLCHMGR_GET_CCTRCH_IDX_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.cctrch_idx)

#define TDSDLCHMGR_GET_PHCH_TYPE_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.phch_type)

#define TDSDLCHMGR_GET_PHCH_HDLE_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.phch_handle)

#define TDSDLCHMGR_IS_CFG_DB_IDX_NOT_IN_USE(cfg_db_idx)    \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.idx_in_use == FALSE )

#define TDSDLCHMGR_IS_CFG_DB_IDX_IN_USE(cfg_db_idx)    \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.idx_in_use == TRUE )

#define TDSDLCHMGR_GET_CH_SPEC_DB_IDX_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.ch_spec_idx)

#define TDSDLCHMGR_GET_STAT_DB_IDX_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.static_db_idx)

#define TDSDLCHMGR_GET_PHYCHAN_TYPE_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.phch_type)

#define TDSDLCHMGR_IS_SRCH_SET_UPDATE_NEEDED_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.srch_set_update_needed)

#define TDSDLCHMGR_IS_FAST_CH_OP_NEEDED_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.fast_ch_op_needed)

#define TDSDLCHMGR_IS_NOTIFY_ALM_NEEDED_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.notify_alm_on_add_recfg)

#define TDSDLCHMGR_IS_CH_INTERNAL_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.int_channel)

#define TDSDLCHMGR_IS_CH_EXTERNAL_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (!(tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.int_channel))

#define TDSDLCHMGR_IS_CCTRCH_NEEDED_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.cctrch_needed)

#define TDSDLCHMGR_GET_ADD_STATE_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.add_state)

#define TDSDLCHMGR_GET_OP_STATUS_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.op_status)

#define TDSDLCHMGR_GET_CH_SPEC_PRE_ADD_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_add_cb)

#define TDSDLCHMGR_GET_CH_SPEC_POST_ADD_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_add_cb)

#define TDSDLCHMGR_GET_CH_SPEC_PRE_DROP_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_drop_cb)

#define TDSDLCHMGR_GET_CH_SPEC_POST_DROP_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_drop_cb)

#define TDSDLCHMGR_GET_CH_SPEC_PRE_RECFG_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_recfg_cb)

#define TDSDLCHMGR_GET_CH_SPEC_POST_RECFG_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_recfg_cb)

  #define TDSDLCHMGR_GET_CH_SPEC_PRE_RESUME_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_resume_cb)

#define TDSDLCHMGR_GET_CH_SPEC_POST_RESUME_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_resume_cb)

#define TDSDLCHMGR_GET_CH_SPEC_PRE_SUSP_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_suspend_cb)

#define TDSDLCHMGR_GET_CH_SPEC_POST_SUSP_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_suspend_cb)

#define TDSDLCHMGR_GET_ADD_RESP_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.chmgr_resp_cb->chmgr_add_resp_cb)

#define TDSDLCHMGR_GET_RECFG_RESP_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.chmgr_resp_cb->chmgr_recfg_resp_cb)

#define TDSDLCHMGR_GET_DROP_RESP_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.chmgr_resp_cb->chmgr_drop_resp_cb)

#define TDSDLCHMGR_GET_CLEANUP_RESP_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.chmgr_resp_cb->chmgr_cleanup_resp_cb)

#define TDSDLCHMGR_GET_CH_SPEC_IDX_ALLOC_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_idx_mgmt->ch_spec_idx_alloc_cb)
  
#define TDSDLCHMGR_GET_CH_SPEC_GET_DRXPARA_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
	  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_schedule_para_cb->ch_spec_get_drxpara_cb)

#define TDSDLCHMGR_GET_CH_SPEC_GET_BITMASK_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
	  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_schedule_para_cb->ch_spec_get_bitmask_cb)

#define TDSDLCHMGR_GET_CH_SPEC_IDX_DEALLOC_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_idx_mgmt->ch_spec_idx_dealloc_cb)

#define TDSDLCHMGR_IS_CH_SPEC_PRE_ADD_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_add_cb != NULL)

#define TDSDLCHMGR_IS_CH_SPEC_POST_ADD_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_add_cb != NULL)

#define TDSDLCHMGR_IS_CH_SPEC_PRE_DROP_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_drop_cb != NULL)

#define TDSDLCHMGR_IS_CH_SPEC_POST_DROP_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_drop_cb != NULL)

#define TDSDLCHMGR_IS_CH_SPEC_PRE_RECFG_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_recfg_cb != NULL)

#define TDSDLCHMGR_IS_CH_SPEC_POST_RECFG_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_recfg_cb != NULL)

#define TDSDLCHMGR_IS_CH_SPEC_PRE_RESUME_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_resume_cb != NULL)

#define TDSDLCHMGR_IS_CH_SPEC_POST_RESUME_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_resume_cb != NULL)

#define TDSDLCHMGR_IS_CH_SPEC_PRE_SUSP_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_pre_suspend_cb != NULL)

#define TDSDLCHMGR_IS_CH_SPEC_POST_SUSP_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_op_cb->ch_spec_post_suspend_cb != NULL)

#define TDSDLCHMGR_IS_CH_SPEC_IDX_ALLOC_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_idx_mgmt->ch_spec_idx_alloc_cb != NULL)

#define TDSDLCHMGR_IS_CH_SPEC_IDX_DEALLOC_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_idx_mgmt->ch_spec_idx_dealloc_cb != NULL)

#define TDSDLCHMGR_IS_ADD_RESP_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.chmgr_resp_cb->chmgr_add_resp_cb != NULL)

#define TDSDLCHMGR_IS_RECFG_RESP_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.chmgr_resp_cb->chmgr_recfg_resp_cb != NULL)

#define TDSDLCHMGR_IS_DROP_RESP_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.chmgr_resp_cb->chmgr_drop_resp_cb != NULL)

#define TDSDLCHMGR_IS_CLEANUP_RESP_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.chmgr_resp_cb->chmgr_cleanup_resp_cb != NULL)

#define TDSDLCHMGR_IS_CH_SPEC_SPEC_SCHEDULE_PARA(cfg_db_idx)  \
	  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_schedule_para_cb != NULL)

#define TDSDLCHMGR_IS_CH_SPEC_GET_DRXPARA_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
	  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_schedule_para_cb->ch_spec_get_drxpara_cb != NULL)

#define TDSDLCHMGR_IS_CH_SPEC_GET_BITMASK_CB_VALID_FROM_CFG_DB_IDX(cfg_db_idx)  \
	  (tdsdlchmgr_cfg_db[cfg_db_idx].stat_db.ch_spec_schedule_para_cb->ch_spec_get_bitmask_cb != NULL)

#define TDSDLCHMGR_GET_OVERALL_STATUS_FROM_CFG_DB_IDX(cfg_db_idx)  \
   (TDSDLCHMGR_GET_OP_STATUS_FROM_CFG_DB_IDX(cfg_db_idx)) 

#define TDSDLCHMGR_IS_OP_STATUS_PASSED_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (TDSDLCHMGR_GET_OP_STATUS_FROM_CFG_DB_IDX(cfg_db_idx) == TRUE)

#define TDSDLCHMGR_IS_OP_STATUS_FAILED_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (TDSDLCHMGR_GET_OP_STATUS_FROM_CFG_DB_IDX(cfg_db_idx) == FALSE)

#define TDSDLCHMGR_IS_CH_PART_OF_DROP_GEN_OP_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.ch_part_of_drop_gen_op)

#define TDSDLCHMGR_IS_CH_PART_OF_DRX_GEN_OP_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.ch_part_of_drx_gen_op == TRUE)

#define TDSDLCHMGR_IS_CH_NOT_PART_OF_DRX_GEN_OP_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.ch_part_of_drx_gen_op == FALSE)

#define TDSDLCHMGR_SET_OP_STATUS_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.op_status = TRUE)

#define TDSDLCHMGR_RESET_OP_STATUS_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.op_status = FALSE)

/** phch function callback ptr */
typedef void (*TDSDLCHMGR_ASET_POST_RL_OPS_FUNC_CB_PTR)(
  /** status of the cell config command */
  boolean status);

#define TDSDLCHMGR_GET_CTCH_ADD_STATE()  \
  (tdsdlchmgr_static_db[TDSDLCHMGR_STATIC_TYPE_SCCPCH1].add_state)

#define TDSDLCHMGR_GET_SHARED_CH_STATUS(cfg_db_idx)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.shared_ch)

#define TDSDLCHMGR_UPD_SHARED_CH_STATUS(cfg_db_idx, status)  \
  (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.shared_ch = status)

#define TDSDLCHMGR_GET_DL_INTERNAL_OPERATION_FROM_CFG_DB_IDX(cfg_db_idx) \
        (tdsdlchmgr_cfg_db[cfg_db_idx].dyn_db.dl_internal_operation)

/******************************************************************************
                         EQ CONTROLLER
******************************************************************************/

/* Callback function reporting TD status to HS module */
typedef void TDS_DLCHMGR_DEMOD_HS_TD_UPDATE_CB_FUNC_TYPE
(uint8 td_status);

/* Callback function reporting TD status to HS module */
extern TDS_DLCHMGR_DEMOD_HS_TD_UPDATE_CB_FUNC_TYPE *tdsdlchmgr_demod_hs_td_update_cb;
extern TDSDLCHMGR_DLCH_SPEC_MGR_INIT_CB_TYPE *tdsdlchmgr_ch_spec_mgr_init_cb[TDSDLCHMGR_NUM_CH_SPEC_MGR];

extern uint8 tdsdlchmgr_NBCH_bitmask[512];
extern boolean tdsdlchmgr_pich_done_flag;
extern boolean tdsdlchmgr_PI_flag;
//extern boolean tdsdlchmgr_update_pccpch_bitmask_needed;
//extern uint16  tdsdlchmgr_update_pccpch_bitmask_sfn;
//extern tdsdlchmgr_cfg_db_idx_type tdsdlchmgr_pccpch_cfg_db_idx;

extern boolean tdsdlchmgr_dl_drop_all_channel;
//CR 398004
extern boolean tdsdlchmgr_fach_state_need_resume_rach;

extern boolean tdsdlchmgr_fake_pich_isr_to_force_pch_decode;

/*===========================================================================
FUNCTION        tdsdlchmgr_init

DESCRIPTION     This function initializes the dlchmgr and all the channel
                specific managers.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdlchmgr_init(void);

/*===========================================================================
FUNCTION        tdsdlchmgr_ext_add

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
extern boolean tdsdlchmgr_ext_add (tdsdlphch_add_type *add_info);

/*===========================================================================
FUNCTION        tdsdlchmgr_int_add

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
extern boolean tdsdlchmgr_int_add (tdsdlchmgr_int_setup_struct_type *ch_info);

/*===========================================================================
FUNCTION        tdsdlchmgr_ext_recfg

DESCRIPTION     This function will be used for all RRC initiated
                channel drops.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdlchmgr_ext_drop (tdsl1_dl_phychan_enum_type phch);

/*===========================================================================
FUNCTION        tdsdlchmgr_drop

DESCRIPTION     This function does the actual drop in dlchmgr by triggering
                drop in dl-phch.

DEPENDENCIES    None

RETURN VALUE
                TRUE: Channel Drop was started successfully
                FALSE: Channel Drop could not be started successfully

SIDE EFFECTS    None
===========================================================================*/
extern boolean tdsdlchmgr_drop (tdsdlchmgr_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        tdsdlchmgr_ext_recfg

DESCRIPTION     This function will be used for all RRC initiated
                re-configurations.

DEPENDENCIES    None

RETURN VALUE
                TRUE:Channel re-configuration was successful
                FALSE: Channel re-configuration was failure

SIDE EFFECTS    None
===========================================================================*/
extern boolean tdsdlchmgr_ext_recfg(tdsdlphch_recfg_type *recfg_info);

/*===========================================================================
FUNCTION        tdsdlchmgr_add_done_cb

DESCRIPTION     This function handles the post add functionality for any 
                channel. It is passed as parameter to dl-phch during add and 
                it gets called at the end of addition.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdlchmgr_add_done_cb (uint8 cctrch_idx, tdsl1_dl_phychan_enum_type phch_type,
  tdsdlphch_cfg_handle_type phch_handle, boolean status);

/*===========================================================================
FUNCTION        tdsdlchmgr_drop_done_cb

DESCRIPTION     This function handles the post drop functionality for any 
                channel. It is passed as parameter to dl-phch during drop and 
                it gets called at the end of drop.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdlchmgr_drop_done_cb (uint8 cctrch_idx, tdsl1_dl_phychan_enum_type phch_type,
  tdsdlphch_cfg_handle_type phch_handle, boolean status);

/*===========================================================================
FUNCTION        tdsdlchmgr_get_cfg_db_idx_from_cctrch_idx

DESCRIPTION     This function finds the config db idx of the channel 
                identified by CCTrCh Idx.

DEPENDENCIES    This function should only be called for channels that have an 
                associated CCTrCh Idx.It should not be called for channels 
                without CCTrCh Idx (eg: PICH, AICH)

RETURN VALUE
                cfg-db-idx: The config db idx of the channel

SIDE EFFECTS    None
===========================================================================*/
extern tdsdlchmgr_cfg_db_idx_type tdsdlchmgr_get_cfg_db_idx_from_cctrch_idx
  (uint8 cctrch_idx);

/*===========================================================================
FUNCTION        tdsdlchmgr_recfg_done_cb

DESCRIPTION     This function handles the post re-configuration functionality 
                for any channel. It is passed as parameter to dl-phch during 
                re-cfg and it gets called at the end of re-cfg.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdlchmgr_recfg_done_cb (uint8 cctrch_idx, tdsl1_dl_phychan_enum_type phch_type,
  tdsdlphch_cfg_handle_type phch_handle, boolean status);

/*===========================================================================
FUNCTION        tdsdlchmgr_resume_done_cb

DESCRIPTION     This function handles the post resume functionality 
                for any channel. It is passed as parameter to dl-phch during 
                resume and it gets called at the end of resume.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdlchmgr_resume_done_cb (uint8 cctrch_idx, tdsl1_dl_phychan_enum_type phch_type,
  tdsdlphch_cfg_handle_type phch_handle, boolean status);

/*===========================================================================
FUNCTION        tdsdlchmgr_suspend_done_cb

DESCRIPTION     This function handles the post suspend functionality 
                for any channel. It is passed as parameter to dl-phch during 
                suspend and it gets called at the end of suspend.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdlchmgr_suspend_done_cb (
              uint8 cctrch_idx, 
              tdsl1_dl_phychan_enum_type phch_type,
              tdsdlphch_cfg_handle_type phch_handle, 
              boolean status);

/*===========================================================================
FUNCTION        tdsdlchmgr_handle_gen_op

DESCRIPTION     This function will be used for all genral operations:
                1. Sleep all request from DRX
                2. Idle Request from RRC
                3. Cell Transtition reqeust from RRC

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdlchmgr_handle_gen_op(tdsdlchmgr_dl_phychan_action_reason_enum_type gen_op_ext_type);

/*===========================================================================
FUNCTION        tdsdlchmgr_get_cctrch_idx_idx_from_phch

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
extern mcaltds_dec_cctrch_id tdsdlchmgr_get_cctrch_idx_idx_from_phch(tdsl1_dl_phychan_enum_type phch);

/*===========================================================================
FUNCTION        tdsdlchmgr_get_phch_hdle_idx_from_phch

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
extern tdsdlphch_cfg_handle_type tdsdlchmgr_get_phch_hdle_idx_from_phch(tdsl1_dl_phychan_enum_type phch);

/*===========================================================================
FUNCTION        tdsdlchmgr_get_ch_spec_idx_from_phch

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
extern tdsdlchmgr_dlch_spec_cfg_db_idx_type tdsdlchmgr_get_ch_spec_idx_from_phch(
  tdsl1_dl_phychan_enum_type phch, boolean only_ext);

/*===========================================================================
FUNCTION        tdsdlchmgr_wakeup_cb

DESCRIPTION     This function wakes up any DRX entity identified by the input 
                parameter. Channel specific managers can use this function as 
                a callback function to be registered with DRX.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdlchmgr_wakeup_cb(tdsdrxmgr_entity_handle_type entity_handle);

/*===========================================================================
FUNCTION        tdsdlchmgr_sleep_cb

DESCRIPTION     This function puts to sleep any DRX entity identified by the input 
                parameter. Channel specific managers can use this function as 
                a callback function to be registered with DRX.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdlchmgr_sleep_cb(tdsdrxmgr_entity_handle_type entity_handle);

/*===========================================================================
FUNCTION        tdsdlchmgr_send_setup_done

DESCRIPTION     This function sends a setup done command to L1M for any channel 
                operation initiated by L1M.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdlchmgr_send_setup_done(tdsdlchmgr_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        tdsdlchmgr_sccpch0_pich_add_drop_done

DESCRIPTION     This function sends a setup done to L1M after addition/drop 
                when the channels involved are PCH carrying SCCPCH0 and PICH.
                This function ensures that a repsonse to L1M is sent only after 
                the operation has got completed for both PCH carrying SCCPCH0 
                and PICH.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdlchmgr_sccpch0_pich_add_drop_done(tdsdlchmgr_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        tdsdlchmgr_upd_drx_hdle_tbl

DESCRIPTION     This function updates the dynamic db idx associated with a drx 
                handle in tdsdlchmgr_drx_hdle_cfg_idx_map_tbl[].

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdlchmgr_upd_drx_hdle_tbl(tdsdlchmgr_cfg_db_idx_type cfg_db_idx,
  tdsdrxmgr_entity_handle_type drx_hdle);

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
extern uint8 tdsdlchmgr_dl_cctrch_table_idx_lookup
(
  /* the physical channel whose associated CCTrCH table index is desired */
  tdsl1_dl_phychan_enum_type phch
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
extern tdsdldec_cctrch_ref_type_enum_type tdsdlchmgr_dl_phychan_to_cctrch
(
  /* the physical channel type */
  tdsl1_dl_phychan_enum_type phch
);

/*===========================================================================
FUNCTION        DL_ENABLE_DEC_EVENTS

DESCRIPTION     This function enables or disables the decoder status and
                data read events according to the state that L1 is in
                at the time.

DEPENDENCIES    The current L1 state as observed from the variable tdsl1m_global.l1_state

RETURN VALUE    None

SIDE EFFECTS    The decoder events referred to above are either enabled or
                disabled on the currently running timeline as controlled
                by the input parameter "on".
===========================================================================*/
extern void tdsdlchmgr_dl_enable_dec_events(
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

SIDE EFFECTS    The tdsl1m_global.cphy_setup_status.dl_phychan_status field is updated
                and one of PHYCHAN_DROP_DONE_CMD, PHYCHAN_CFG_DONE_CMD or
                PHYCHAN_ADD_DONE_CMD local commands is sent to L1M.
===========================================================================*/
extern void tdsdlchmgr_dl_send_setup_done_to_l1m
(
  /* the channel just setup/torndown */
  tdsl1_dl_phychan_enum_type phch,

  /* TRUE if the operation was successful */
  boolean status,

  /* The operation that was performed */
  tdsl1_setup_ops_enum_type op
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
extern uint8 tdsdlchmgr_dl_phychan_lookup
(
  /* the reference physical channel to lookup */
  tdsl1_dl_phychan_enum_type phch
);

/*===========================================================================
FUNCTION        tdsdlchmgr_sccpch_pich_add_drop_done //TDS

DESCRIPTION     This function sends a setup done to L1M after addition/drop 
                when the channels involved are PCH carrying SCCPCH0 and PICH.
                This function ensures that a repsonse to L1M is sent only after 
                the operation has got completed for both PCH carrying SCCPCH0 
                and PICH.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdlchmgr_sccpch_pich_add_drop_done(tdsdlchmgr_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        tdsdlchmgr_resume

DESCRIPTION     This function handles the procedures needed for drx entity resume
                in ch-spec-mgr and dl-phch.

DEPENDENCIES    None

RETURN VALUE
                TRUE: Channel resume was started successfully
                FALSE: Channel resume could not be started successfully

SIDE EFFECTS    None
===========================================================================*/
extern boolean tdsdlchmgr_resume (tdsdrxmgr_entity_handle_type entity_hdle,
  boolean fast_resume);

/*===========================================================================
FUNCTION        tdsdlchmgr_suspend

DESCRIPTION     This function handles the procedures needed for drx entity suspend
                in ch-spec-mgr and dl-phch.

DEPENDENCIES    None

RETURN VALUE
                TRUE: Channel suspend was started successfully
                FALSE: Channel suspend could not be started successfully

SIDE EFFECTS    None
===========================================================================*/
extern boolean tdsdlchmgr_suspend (tdsdrxmgr_entity_handle_type entity_hdle,
  boolean fast_suspend);

/*===========================================================================
FUNCTION        tdsdlchmgr_get_cfg_db_idx_from_phch

DESCRIPTION     This function finds the config db index based on the 
                following conditions: physical channel type and the channel
                type-internal/external

DEPENDENCIES    None

RETURN VALUE
                config-db-idx: Config db index associated with the input params

SIDE EFFECTS    None
===========================================================================*/
extern tdsdlchmgr_cfg_db_idx_type tdsdlchmgr_get_cfg_db_idx_from_phch(
  tdsl1_dl_phychan_enum_type phch, boolean only_ext);


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
extern boolean tdsdlchmgr_dl_phychan_add_validate(
  /* the physical channel being added */
  tdsl1_dl_phychan_enum_type phch,

  /* pointer to the dl_phychan_db part of ordered config */
  tdsl1_dl_phycfg_phychan_db_struct_type *phychan_db,

  /*setup req mask*/
  uint16 req_mask,

  /* drop phychan type */
  tdsl1_dl_phychan_enum_type drop_dl_phychan
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
extern boolean tdsdlchmgr_dl_phychan_recfg_validate(
  /* the physical channel to reconfigure */
  tdsl1_dl_phychan_enum_type phch,

  /* pointer to the RRC ordered config database */
  tdsl1dl_phychan_db_struct_type *phychan_db
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
extern boolean tdsdlchmgr_dl_phychan_drop_validate(
  /* which physical channel to drop */
  tdsl1_dl_phychan_enum_type phch
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
extern boolean tdsdlchmgr_validate_sccpch_parms
(
//  tdsl1_sf_enum_type sf,

  tdsl1_dl_phychan_enum_type phch,

  tdsl1_dl_phycfg_phychan_db_struct_type *phychan_db
);

/*===========================================================================
FUNCTION        tdsdlchmgr_suspend_fach_measurement_in_advance

FILE NAME       tdsdlchmgr.c

DESCRIPTION    for sccpch cfg some time will delay to next subframe,
      the measurement will schedule inter freq, so cfg measurement in advance,
      tell measurement the sccpch always take TS0. in real cfg will over write the value

DEPENDENCIES    

RETURN VALUE    void
  

SIDE EFFECTS    
===========================================================================*/
void tdsdlchmgr_suspend_fach_measurement_in_advance( void );

/*===========================================================================
FUNCTION        tdsdlchmgr_suspend_pccpch_measurement_in_advance

FILE NAME       tdsdlchmgr.c

DESCRIPTION    for pccpch cfg some time will delay to next subframe,
      the measurement will schedule inter freq, so cfg measurement in advance,
      tell measurement the sccpch always take TS0. in real cfg will over write the value

DEPENDENCIES    

RETURN VALUE    void
  

SIDE EFFECTS    
===========================================================================*/
void tdsdlchmgr_suspend_pccpch_measurement_in_advance( void );

/*===========================================================================
FUNCTION        DL_VALIDATE_DPCH_PARMS

DESCRIPTION     This function validates the DPCH parameters appearing in
                Ordered Config

DEPENDENCIES    RRC Ordered config as observed by the passed in pointer to
                a l1_dl_phychan_struct_type

RETURN VALUE    TRUE if all parameters are valid, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
extern boolean tdsdlchmgr_validate_dpch_parms
(
  tdsl1_dl_phycfg_phychan_db_struct_type *phychan_db
);

/*===========================================================================
FUNCTION        tdsdlchmgr_pre_cell_trans_cb

DESCRIPTION     This function should be called by L1M before starting to process 
                CPHY_CELL_TRANSITION_REQ from RRC.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void tdsdlchmgr_pre_cell_trans_cb(void);

/*===========================================================================
FUNCTION        tdsdlchmgr_post_cell_trans_cb

DESCRIPTION     This function should be called by L1M before sending response for 
                CPHY_CELL_TRANSITION_REQ to RRC.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void tdsdlchmgr_post_cell_trans_cb(boolean cell_trans_status);

#if 0
/*===========================================================================
FUNCTION        tdsdlchmgr_dl_aich_send_add_drop_done

DESCRIPTION     This function sends a setup done to L1M after addition/drop 
                for AICH.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdlchmgr_dl_aich_send_add_drop_done(tdsdlchmgr_cfg_db_idx_type cfg_db_idx);
#endif
/*===========================================================================
FUNCTION        tdsdlchmgr_get_cfg_db_idx_from_drx_hdle

DESCRIPTION     This function finds the config db index associated with a drx 
                handle.

DEPENDENCIES    None

RETURN VALUE
                config-db-idx: Config db index associated with the drx handle.

SIDE EFFECTS    None
===========================================================================*/
extern tdsdlchmgr_cfg_db_idx_type tdsdlchmgr_get_cfg_db_idx_from_drx_hdle(
  tdsdrxmgr_entity_handle_type drx_hdle);

/*===========================================================================
FUNCTION        tdsdlchmgr_update_NBCH_bitmask

DESCRIPTION     This function update N-PCCPCH bitmask. 
                (1)called by DRX after DRX processed DRX req
                (2)called by DL after PICH decode done

DEPENDENCIES    None

RETURN VALUE

SIDE EFFECTS    None.
===========================================================================*/
extern void tdsdlchmgr_update_NBCH_bitmask(void);

/*===========================================================================
FUNCTION        tdsdlchmgr_get_nbch_exist_status

DESCRIPTION     This function search for NBCH and return the NBCH exist status.

DEPENDENCIES    

RETURN VALUE
                TRUE:  NBCH exist
                FALSE: NBCH not exist

SIDE EFFECTS    None
===========================================================================*/
extern boolean tdsdlchmgr_get_nbch_exist_status(
          tdsdlchmgr_cfg_db_idx_type *nbch_cfg_db_idx);

#if 0
/*===========================================================================
FUNCTION        tdsdlchmgr_recfig_pccpch_bitmask

DESCRIPTION     This function recofig PCCPCH bitmask. 
DEPENDENCIES    None

RETURN VALUE

SIDE EFFECTS    None.
===========================================================================*/
extern void tdsdlchmgr_recfg_pccpch_bitmask();
#endif
#ifdef FEATURE_TDSCDMA_CELL_FACH_MEAS_OCCASION
/*===========================================================================
FUNCTION        tdsdlchmgr_get_fmo_infor

FILE NAME       

DESCRIPTION     update non-intra-F measurement parameters for FACH

DEPENDENCIES    

RETURN VALUE    void

SIDE EFFECTS
    
===========================================================================*/
extern void tdsdlchmgr_get_fmo_infor( tdsl1_fach_meas_info_struct_type *parms_ptr );

/*===========================================================================

FUNCTION        SRCHFACH_SCCPCH_EVT_HANDLER

DESCRIPTION     ISR for CELL_FACH SCCPCH frame boundary event for inter-RAT 
                and inter-F measurments at measurement occasions 

DEPENDENCIES    Only to be enabled in CELL_FACH state

RETURN VALUE    None
  
SIDE EFFECTS    None

===========================================================================*/
extern void tdsdlchmgr_fmo_handler( void );
#if 0
/*===========================================================================
FUNCTION        tdsdlchmgr_recfg_fach_bitmask

DESCRIPTION     This function recofig fach bitmask. 
DEPENDENCIES    None

RETURN VALUE

SIDE EFFECTS    None.
===========================================================================*/
extern void tdsdlchmgr_recfg_sccpch_fach_bitmask();
#endif
/*===========================================================================

FUNCTION        tdsdlchmgr_clear_fmo_infor

DESCRIPTION     This function cleans up FACH meas occasion info

DEPENDENCIES    None

RETURN VALUE    None
  
SIDE EFFECTS    As said
                
===========================================================================*/
extern void tdsdlchmgr_clear_fmo_infor(void);

/*===========================================================================

FUNCTION        tdsdlchmgr_fmo_cleanup_crnti

DESCRIPTION     This function cleans up FACH meas occasion c_rnti_valid

DEPENDENCIES    None

RETURN VALUE    None
  
SIDE EFFECTS    As said
                
===========================================================================*/
extern void tdsdlchmgr_fmo_cleanup_crnti(void);

#endif
/*===========================================================================
FUNCTION        tdsdlchmgr_is_ue_paged

FILE NAME       

DESCRIPTION     Return whether UE is paged or not

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
 
extern boolean tdsdlchmgr_is_ue_paged(void);

#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED
/*===========================================================================
FUNCTION        tdsdlchmgr_pich_decode_log_submit(void);
FILE NAME       

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
extern void tdsdlchmgr_pich_decode_log_submit(void);
#endif
//CR 398004
extern boolean tdsdlchmgr_check_fach_state_rach_need_wait_resume(void);
/*------------------EXTERNAL FUNCTIONS DEFINITION END------------------------------------*/

#ifdef FEATURE_TDSCDMA_MCPM_POWER_REPORTING
/*===========================================================================
FUNCTION tdsdlchmgr_get_num_of_active_channel

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 tdsdlchmgr_get_num_of_active_channel(void);
#endif
extern void tdsdl_fake_pich_done_isr_to_force_pch_decode( void );
extern boolean tdsdlchmgr_get_pich_decoded_status(void);
extern void    tdsdlchmgr_set_pich_decoded_status(boolean decoded);
#endif /* TDSDLCHMGR_H */
