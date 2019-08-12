#ifndef DLCHMGR_I_H
#define DLCHMGR_I_H

/*===========================================================================
                               D L C H M G R I. H

GENERAL DESCRIPTION

  This file contains the declarations and definitions that are internal to
  dlchmgr.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright(c)2007 - 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/tlm.c_v   1.153   17 Jul 2002 16:36:16   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/dlchmgri.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------
05/20/13    sad     Support for Enhanced Layer-1 Manager
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/08/12    gv      Feature HS RACH Initial code check in
02/08/12    vs      Feature cleanup.
08/17/10    hk      Revert to the old CCTrCh upon phch recofig failure
06/29/10    ms      Corrected argument to dlchmgr_hs_drop_done
06/29/10    ms      Added boolean argument to dlchmgr_hs_drop_done
09/30/09    hk      Replaced INTLOCK with Mutex
09/24/09    hk      Porting of AICH resetup feature to Offline
05/25/09    hk      Fixed compiler warnings
02/20/09    vp      Exclude explicit Pich drop during cell transition
12/10/08    ks      Mainlining FEATURE_WCDMA_DL_ENHANCED
10/03/08    hk      Bringing in 7k Mailine fixes
01/25/08    vsr     Checking in Enhanced DL files onto mainline
01/22/08    hk      Added CLEANUP_CCTRCH bit to the CHSPEC_DB_FAILURE bitmask,
                    Moved the dlchmgr_get_dyn_db_idx_from_phch() declaration 
                    to dlchmgr.h
01/15/08    vp      Added code comments and Merged dynamic and static db in to a 
                    single config db
11/06/07    vp      Moved DRX handle from dlchmgr to channel specific managers
                    and added shared channel support in sccpchmgr.
08/20/07    vp      Added dlchmgr_get_ch_spec_dyn_db_idx_from_phch()
                    to get channel specific dynamic db index.
                    Added support for new sccpch0/pich reponse functions.
07/12/07    vp      Added support for suspend/resume DRX entities
05/08/07    sv/vsr  Lint changes
03/02/07    vp      Added support for R99 cell reselection
02/21/07    vp      Initial version
===========================================================================*/
/*dlchmgr_gen_op_stat_db_idx_enum_type holds the type of general operations.*/
typedef enum {
DLCHMGR_GEN_OP_STAT_TYPE_SLEEP,
DLCHMGR_GEN_OP_STAT_TYPE_CELL_TRANS,
DLCHMGR_GEN_OP_STAT_TYPE_IDLE_REQ,
DLCHMGR_GEN_OP_STAT_TYPE_TOTAL_NUM
} dlchmgr_gen_op_stat_db_idx_enum_type;

/*if the type or maximum possible value of dlchmgr_cfg_db_idx_type changes from
  the current values(uint8, 6), then the type of dlchmgr_dynamic_db_idx_bitmask_type 
  has to be changed accordingly*/
typedef uint32 dlchmgr_cfg_db_idx_genop_bmask_type;

/*dlchmgr_gen_op_dyn_db_struct_type holds all the information pertaining to 
  a general operation.*/
typedef struct {
  /*Status (pass/failed) of the general operation*/
  boolean gen_op_status;

  /*Index in to the gen-op static db corresponding to the general operation 
  in progress*/
  dlchmgr_gen_op_stat_db_idx_enum_type gen_op_stat_db_idx;

  /*gen_op_dyn_db_idx_bmask holds the channel indexes that are involved 
  in the general operation*/
  dlchmgr_cfg_db_idx_genop_bmask_type gen_op_cfg_db_idx_bmask;
} dlchmgr_gen_op_dyn_db_struct_type;

  /*dlchmgr_gen_op_map_struct_type holds the mapping of general operation 
  type from dl_phychan_action_reason_enum_type to 
  dlchmgr_gen_op_stat_db_idx_enum_type*/
typedef struct {
  dl_phychan_action_reason_enum_type gen_op_ext_type;

  dlchmgr_gen_op_stat_db_idx_enum_type gen_op_stat_db_idx;
} dlchmgr_gen_op_map_struct_type;

  /*Structure that holds all semi-static information corresponding to a general 
  operation. The information is decided to compile time.*/
typedef struct {
  boolean drx_rel_needed;

  DLCHMGR_GEN_OP_PROC_FUNC_TYPE *gen_op_proc_func;

  DLCHMGR_GEN_OP_CHECK_FUNC_TYPE *gen_op_check_func;

  DLCHMGR_GEN_OP_DONE_CB_FUNC_TYPE *gen_op_done_cb;
} dlchmgr_gen_op_stat_db_struct_type;

  /*Structure that holds all channel independent information for DlChMgr*/
typedef struct {
  dlchmgr_gen_op_dyn_db_struct_type gen_op_dyn_db;
} dlchmgr_db_struct_type;

#define DLCHMGR_DYN_DB_IDX_GEN_OP_BMASK_NONE (0x0)

#define DLCHMGR_GEN_OP_INVALID_STAT_TYPE (DLCHMGR_GEN_OP_STAT_TYPE_TOTAL_NUM)

/*Bit masks that indicate the clean-up operation to be performed.*/
#define DLCHMGR_CLEANUP_BIT_NONE (0x0)
#define DLCHMGR_CLEANUP_BIT_CHMGR_DYN_DB (0x1)
#define DLCHMGR_CLEANUP_BIT_RESP_CB (0x2)
#define DLCHMGR_CLEANUP_BIT_CHSPEC_DB (0x4)
#define DLCHMGR_CLEANUP_BIT_DRX (0x8)
#define DLCHMGR_CLEANUP_BIT_CCTRCH (0x10)
#define DLCHMGR_CLEANUP_BIT_SRCH (0x20)
#define DLCHMGR_CLEANUP_BIT_PHCH (0x40)
#define DLCHMGR_CLEANUP_BIT_REVERT_CCTRCH (0x80)

/*Macros that indicate the clean-ups to be performed based on the type 
of failure.*/
#define DLCHMGR_CLEANUP_FOR_CHMGR_DYN_DB_FAILURE (DLCHMGR_CLEANUP_BIT_RESP_CB)
#define DLCHMGR_CLEANUP_FOR_CHSPEC_DB_FAILURE (DLCHMGR_CLEANUP_BIT_RESP_CB |  \
  DLCHMGR_CLEANUP_BIT_CHMGR_DYN_DB |  \
  DLCHMGR_CLEANUP_BIT_CCTRCH)

#define DLCHMGR_CLEANUP_FOR_CCTRCH_FAILURE (DLCHMGR_CLEANUP_BIT_RESP_CB |  \
  DLCHMGR_CLEANUP_BIT_CHMGR_DYN_DB |  \
  DLCHMGR_CLEANUP_BIT_CHSPEC_DB)
#define DLCHMGR_CLEANUP_FOR_SRCH_FAILURE (DLCHMGR_CLEANUP_BIT_RESP_CB |  \
  DLCHMGR_CLEANUP_BIT_CHMGR_DYN_DB |  \
  DLCHMGR_CLEANUP_BIT_CHSPEC_DB |  \
  DLCHMGR_CLEANUP_BIT_CCTRCH)
#define DLCHMGR_CLEANUP_FOR_PRE_ADD_DRX_FAILURE (DLCHMGR_CLEANUP_BIT_RESP_CB |  \
  DLCHMGR_CLEANUP_BIT_CHMGR_DYN_DB |  \
  DLCHMGR_CLEANUP_BIT_CHSPEC_DB |  \
  DLCHMGR_CLEANUP_BIT_CCTRCH |  \
  DLCHMGR_CLEANUP_BIT_SRCH)
#define DLCHMGR_CLEANUP_FOR_PHCH_FAILURE (DLCHMGR_CLEANUP_BIT_RESP_CB |  \
  DLCHMGR_CLEANUP_BIT_CHMGR_DYN_DB |  \
  DLCHMGR_CLEANUP_BIT_CHSPEC_DB |  \
  DLCHMGR_CLEANUP_BIT_CCTRCH |  \
  DLCHMGR_CLEANUP_BIT_SRCH |  \
  DLCHMGR_CLEANUP_BIT_DRX)

#define DLCHMGR_CLEANUP_FOR_RECFG_PHCH_FAILURE (DLCHMGR_CLEANUP_BIT_RESP_CB |  \
  DLCHMGR_CLEANUP_BIT_CCTRCH |  \
  DLCHMGR_CLEANUP_BIT_REVERT_CCTRCH)

#define DLCHMGR_CLEANUP_FOR_RECFG_DRX_FAILURE (DLCHMGR_CLEANUP_BIT_RESP_CB |  \
  DLCHMGR_CLEANUP_BIT_CCTRCH |  \
  DLCHMGR_CLEANUP_BIT_REVERT_CCTRCH)

#define DLCHMGR_CLEANUP_BMASK_HAS_RESP_CB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  ((dlchmgr_cfg_db[cfg_db_idx].dyn_db.cleanup_bmask & DLCHMGR_CLEANUP_BIT_RESP_CB) != DLCHMGR_CLEANUP_BIT_NONE)

#define DLCHMGR_CLEANUP_BMASK_HAS_CHMGR_DB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  ((dlchmgr_cfg_db[cfg_db_idx].dyn_db.cleanup_bmask & DLCHMGR_CLEANUP_BIT_CHMGR_DYN_DB) != DLCHMGR_CLEANUP_BIT_NONE)

#define DLCHMGR_CLEANUP_BMASK_HAS_CHSPEC_DB_FROM_CFG_DB_IDX(cfg_db_idx)  \
  ((dlchmgr_cfg_db[cfg_db_idx].dyn_db.cleanup_bmask & DLCHMGR_CLEANUP_BIT_CHSPEC_DB) != DLCHMGR_CLEANUP_BIT_NONE)

#define DLCHMGR_CLEANUP_BMASK_HAS_DRX_FROM_CFG_DB_IDX(cfg_db_idx)  \
  ((dlchmgr_cfg_db[cfg_db_idx].dyn_db.cleanup_bmask & DLCHMGR_CLEANUP_BIT_DRX) != DLCHMGR_CLEANUP_BIT_NONE)

#define DLCHMGR_CLEANUP_BMASK_HAS_CCTRCH_FROM_CFG_DB_IDX(cfg_db_idx)  \
  ((dlchmgr_cfg_db[cfg_db_idx].dyn_db.cleanup_bmask & DLCHMGR_CLEANUP_BIT_CCTRCH) != DLCHMGR_CLEANUP_BIT_NONE)

#define DLCHMGR_CLEANUP_BMASK_HAS_REVERT_CCTRCH_FROM_CFG_DB_IDX(cfg_db_idx)  \
  ((dlchmgr_cfg_db[cfg_db_idx].dyn_db.cleanup_bmask & DLCHMGR_CLEANUP_BIT_REVERT_CCTRCH) != DLCHMGR_CLEANUP_BIT_NONE)

#define DLCHMGR_CLEANUP_BMASK_HAS_SRCH_FROM_CFG_DB_IDX(cfg_db_idx)  \
  ((dlchmgr_cfg_db[cfg_db_idx].dyn_db.cleanup_bmask & DLCHMGR_CLEANUP_BIT_SRCH) != DLCHMGR_CLEANUP_BIT_NONE)

#define DLCHMGR_CLEANUP_BMASK_HAS_PHCH_FROM_CFG_DB_IDX(cfg_db_idx)  \
  ((dlchmgr_cfg_db[cfg_db_idx].dyn_db.cleanup_bmask & DLCHMGR_CLEANUP_BIT_PHCH) != DLCHMGR_CLEANUP_BIT_NONE)

#define DLCHMGR_SET_CLEANUP_FOR_CHMGR_DYN_DB_FAILURE(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.cleanup_bmask = DLCHMGR_CLEANUP_FOR_CHMGR_DYN_DB_FAILURE)

#define DLCHMGR_SET_CLEANUP_FOR_CHSPEC_DB_FAILURE(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.cleanup_bmask = DLCHMGR_CLEANUP_FOR_CHSPEC_DB_FAILURE)

#define DLCHMGR_SET_CLEANUP_FOR_PRE_ADD_DRX_FAILURE(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.cleanup_bmask = DLCHMGR_CLEANUP_FOR_PRE_ADD_DRX_FAILURE)

#define DLCHMGR_SET_CLEANUP_FOR_CCTRCH_FAILURE(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.cleanup_bmask = DLCHMGR_CLEANUP_FOR_CCTRCH_FAILURE)

#define DLCHMGR_SET_CLEANUP_FOR_SRCH_FAILURE(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.cleanup_bmask = DLCHMGR_CLEANUP_FOR_SRCH_FAILURE)

#define DLCHMGR_SET_CLEANUP_FOR_PHCH_FAILURE(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.cleanup_bmask = DLCHMGR_CLEANUP_FOR_PHCH_FAILURE)

#define DLCHMGR_SET_CLEANUP_FOR_RECFG_DRX_FAILURE(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.cleanup_bmask = DLCHMGR_CLEANUP_FOR_RECFG_DRX_FAILURE)

#define DLCHMGR_SET_CLEANUP_FOR_RECFG_PHCH_FAILURE(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.cleanup_bmask = DLCHMGR_CLEANUP_FOR_RECFG_PHCH_FAILURE)

#define DLCHMGR_SET_CLEANUP_FOR_CHSPEC_DB_FAILURE(cfg_db_idx)  \
  (dlchmgr_cfg_db[cfg_db_idx].dyn_db.cleanup_bmask = DLCHMGR_CLEANUP_FOR_CHSPEC_DB_FAILURE)

#define DLCHMGR_GET_GEN_OP_STAT_DB_IDX_FROM_DYN_IDX()  \
  (dlchmgr_db.gen_op_dyn_db.gen_op_stat_db_idx)

#define DLCHMGR_GET_CFG_DB_IDX_GEN_OP_BMASK_FROM_DYN_IDX()  \
  (dlchmgr_db.gen_op_dyn_db.gen_op_cfg_db_idx_bmask)

#define DLCHMGR_GET_GEN_OP_STATUS_FROM_DYN_IDX()  \
  (dlchmgr_db.gen_op_dyn_db.gen_op_status)

#define DLCHMGR_IS_DRX_REL_NEEDED_FOR_GEN_OP_FROM_STAT_IDX(stat_db_idx)  \
  (dlchmgr_gen_op_stat_db[stat_db_idx].drx_rel_needed)

#define DLCHMGR_IS_DRX_REL_NEEDED_FOR_GEN_OP_FROM_DYN_IDX(gen_op_stat_db_idx)  \
  (DLCHMGR_IS_DRX_REL_NEEDED_FOR_GEN_OP_FROM_STAT_IDX(gen_op_stat_db_idx))

#define DLCHMGR_IS_GEN_OP_STATIC_DB_IDX_INVALID(static_db_idx)  \
  (static_db_idx >= DLCHMGR_GEN_OP_STAT_TYPE_TOTAL_NUM)

#define DLCHMGR_GET_GEN_OP_PROC_FUNC_FROM_STAT_IDX(stat_db_idx)  \
  (dlchmgr_gen_op_stat_db[stat_db_idx].gen_op_proc_func)

#define DLCHMGR_GET_GEN_OP_CHECK_FUNC_FROM_STAT_IDX(stat_db_idx)  \
  (dlchmgr_gen_op_stat_db[stat_db_idx].gen_op_check_func)

#define DLCHMGR_GET_GEN_OP_DONE_CB_FROM_STAT_IDX(stat_db_idx)  \
  (dlchmgr_gen_op_stat_db[stat_db_idx].gen_op_done_cb)

#define DLCHMGR_GET_GEN_OP_PROC_FUNC_FROM_DYN_IDX(gen_op_stat_db_idx)  \
  (DLCHMGR_GET_GEN_OP_PROC_FUNC_FROM_STAT_IDX(gen_op_stat_db_idx))

#define DLCHMGR_IS_GEN_OP_PROC_FUNC_VALID_FROM_DYN_IDX(gen_op_stat_db_idx)  \
  ((DLCHMGR_GET_GEN_OP_PROC_FUNC_FROM_STAT_IDX(gen_op_stat_db_idx)) != NULL)

#define DLCHMGR_IS_GEN_OP_PROC_FUNC_INVALID_FROM_DYN_IDX(gen_op_stat_db_idx)  \
  ((DLCHMGR_GET_GEN_OP_PROC_FUNC_FROM_STAT_IDX(gen_op_stat_db_idx)) == NULL)

#define DLCHMGR_GET_GEN_OP_CHECK_FUNC_FROM_DYN_IDX(gen_op_stat_db_idx)  \
  (DLCHMGR_GET_GEN_OP_CHECK_FUNC_FROM_STAT_IDX(gen_op_stat_db_idx))

#define DLCHMGR_IS_GEN_OP_CHECK_FUNC_VALID_FROM_DYN_IDX(gen_op_stat_db_idx)  \
  ((DLCHMGR_GET_GEN_OP_CHECK_FUNC_FROM_STAT_IDX(gen_op_stat_db_idx)) != NULL)

#define DLCHMGR_IS_GEN_OP_CHECK_FUNC_INVALID_FROM_DYN_IDX(gen_op_stat_db_idx)  \
  ((DLCHMGR_GET_GEN_OP_CHECK_FUNC_FROM_STAT_IDX(gen_op_stat_db_idx)) == NULL)

#define DLCHMGR_GET_GEN_OP_DONE_CB_FROM_DYN_IDX(gen_op_stat_db_idx)  \
  (DLCHMGR_GET_GEN_OP_DONE_CB_FROM_STAT_IDX(gen_op_stat_db_idx))

#define DLCHMGR_IS_GEN_OP_DONE_CB_VALID_FROM_DYN_IDX(gen_op_stat_db_idx)  \
  ((DLCHMGR_GET_GEN_OP_DONE_CB_FROM_STAT_IDX(gen_op_stat_db_idx)) != NULL)

#define DLCHMGR_IS_GEN_OP_DONE_CB_INVALID_FROM_DYN_IDX(gen_op_stat_db_idx)  \
  ((DLCHMGR_GET_GEN_OP_DONE_CB_FROM_STAT_IDX(gen_op_stat_db_idx)) == NULL)

#define DLCHMGR_IS_GEN_OP_INVALID_FROM_DYN_IDX()  \
  (DLCHMGR_GET_GEN_OP_STAT_DB_IDX_FROM_DYN_IDX() >= DLCHMGR_GEN_OP_INVALID_STAT_TYPE)

  #define DLCHMGR_IS_GEN_OP_RESEL_FROM_DYN_IDX()  \
  (DLCHMGR_GET_GEN_OP_STAT_DB_IDX_FROM_DYN_IDX() == DLCHMGR_GEN_OP_STAT_TYPE_CELL_TRANS)

  #define DLCHMGR_IS_GEN_OP_SLEEP_DROP_ALL_FROM_DYN_IDX()  \
  (DLCHMGR_GET_GEN_OP_STAT_DB_IDX_FROM_DYN_IDX() == DLCHMGR_GEN_OP_STAT_TYPE_SLEEP)

#define DLCHMGR_GET_STAT_DB_IDX_FOR_GEN_OP(gen_op_ext_type)  \
  (dlchmgr_get_stat_db_idx_for_gen_op(gen_op_ext_type))

#define DLCHMGR_DYN_DB_IDX_GEN_OP_BMASK_SET(bmask, cfg_db_idx)  \
  (bmask |= (uint32)((uint32)1 << cfg_db_idx))

#define DLCHMGR_DYN_DB_IDX_GEN_OP_BMASK_RESET(bmask, cfg_db_idx)  \
  (bmask &= (uint32)(~((uint32)1 << cfg_db_idx)))

#define DLCHMGR_CLEAR_ALL_DYN_DB_IDX_GEN_OP_BITS_FROM_DYN_IDX()  \
  (DLCHMGR_GET_CFG_DB_IDX_GEN_OP_BMASK_FROM_DYN_IDX() = DLCHMGR_DYN_DB_IDX_GEN_OP_BMASK_NONE)

#define DLCHMGR_IS_GEN_OP_ATLEAST_ONE_CH_OP_FROM_DYN_IDX()  \
  (DLCHMGR_GET_CFG_DB_IDX_GEN_OP_BMASK_FROM_DYN_IDX() != DLCHMGR_DYN_DB_IDX_GEN_OP_BMASK_NONE)

#define DLCHMGR_IS_GEN_OP_NO_CH_OP_FROM_DYN_IDX()  \
  (DLCHMGR_GET_CFG_DB_IDX_GEN_OP_BMASK_FROM_DYN_IDX() == DLCHMGR_DYN_DB_IDX_GEN_OP_BMASK_NONE)

#define DLCHMGR_IS_GEN_OP_COMPLETED_FROM_DYN_IDX()  \
  (DLCHMGR_IS_GEN_OP_NO_CH_OP_FROM_DYN_IDX())

#define DLCHMGR_ADD_CH_OP_STATUS_TO_GEN_OP_STATUS(cfg_db_idx)  \
  (DLCHMGR_GET_GEN_OP_STATUS_FROM_DYN_IDX() &= DLCHMGR_GET_OP_STATUS_FROM_CFG_DB_IDX(cfg_db_idx))

#define DLCHMGR_IS_PHCH_NEEDED_FOR_RESEL_FROM_CFG_DB_IDX(cfg_db_idx)  \
  ((DLCHMGR_GET_STAT_DB_IDX_FROM_CFG_DB_IDX(cfg_db_idx) == DLCHMGR_STATIC_TYPE_BCH_S_SIB_READ))

#define DLCHMGR_IS_PHCH_NEEDED_FOR_SLEEP_DROP_ALL_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (DLCHMGR_GET_STAT_DB_IDX_FROM_CFG_DB_IDX(cfg_db_idx) == DLCHMGR_STATIC_TYPE_PICH)

#define DLCHMGR_IS_PHCH_EXCLUDED_FROM_DROP_ALL(cfg_db_idx)  \
  (DLCHMGR_IS_GEN_OP_RESEL_FROM_DYN_IDX() &&  \
    DLCHMGR_IS_PHCH_NEEDED_FOR_RESEL_FROM_CFG_DB_IDX(cfg_db_idx))

#define DLCHMGR_IS_PHCH_NOT_EXCLUDED_FROM_DROP_ALL(cfg_db_idx)  \
  (!DLCHMGR_IS_PHCH_EXCLUDED_FROM_DROP_ALL(cfg_db_idx))

#define DLCHMGR_IS_PHCH_EXCLUDED_FROM_SUSPEND_ALL(cfg_db_idx)  \
  (DLCHMGR_IS_GEN_OP_SLEEP_DROP_ALL_FROM_DYN_IDX() &&  \
    DLCHMGR_IS_PHCH_NEEDED_FOR_SLEEP_DROP_ALL_FROM_CFG_DB_IDX(cfg_db_idx))

#define DLCHMGR_IS_PHCH_NOT_EXCLUDED_FROM_SUSPEND_ALL(cfg_db_idx)  \
  (!DLCHMGR_IS_PHCH_EXCLUDED_FROM_DROP_ALL(cfg_db_idx))

/*currently, CTCH-FACH cannot be enabled by means of en_dis_trch mechanism*/
#define DLCHMGR_IS_SHMGR_SVC_TYPE_INVALID(shmgr_svc_type)  \
  (!((shmgr_svc_type == DLSHMGR_SVC_TYPE_R99_FACH) || \
    (shmgr_svc_type == DLSHMGR_SVC_TYPE_R99_PCH)))

/*===========================================================================
FUNCTION        dlchmgr_alloc_cfg_db_idx

DESCRIPTION     This function allocates a free index in the ChMgr dynamic db.

DEPENDENCIES    None

RETURN VALUE
                dyn-db-idx: The allotted index in dynamic db.

SIDE EFFECTS    None
===========================================================================*/
LOCAL dlchmgr_cfg_db_idx_type dlchmgr_alloc_cfg_db_idx( void );

/*===========================================================================
FUNCTION        dlchmgr_alloc_dyn_db_idx

DESCRIPTION     This function frees the index in the ChMgr dynamic db passed 
                as input parameter.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dlchmgr_dealloc_cfg_db_idx
  (dlchmgr_cfg_db_idx_type dlchmgr_dyn_db_idx);

/*===========================================================================
FUNCTION        dlchmgr_get_cfg_db_idx_from_phch_hdle

DESCRIPTION     This function finds the config db index associated with phch 
                handle provided as input parameter.

DEPENDENCIES    None

RETURN VALUE
                config-db-idx: Config db index associated with the phch handle.

SIDE EFFECTS    None
===========================================================================*/
LOCAL dlchmgr_cfg_db_idx_type dlchmgr_get_cfg_db_idx_from_phch_hdle
  (dl_phch_cfg_handle_type phch_handle);

/*===========================================================================
FUNCTION        dlchmgr_get_cfg_db_idx_from_dl_param

DESCRIPTION     This function finds the config db index associated with the CCTrCh
                or the phch handle in that order.

DEPENDENCIES    None

RETURN VALUE
                config-db-idx: Config db index associated with the input params

SIDE EFFECTS    None
===========================================================================*/
LOCAL dlchmgr_cfg_db_idx_type dlchmgr_get_cfg_db_idx_from_dl_param
  (uint8 cctrch_idx, dl_phch_cfg_handle_type phch_handle);

/*===========================================================================
FUNCTION        dlchmgr_alloc_ch_spec_idx

DESCRIPTION     This function allocates a free index in the channel specific 
                manager's config db.

DEPENDENCIES    None

RETURN VALUE
                ch_spec_idx: The allotted index in channel specific manager dynamic db.

SIDE EFFECTS    None
===========================================================================*/
LOCAL dl_ch_spec_cfg_db_idx_type dlchmgr_alloc_ch_spec_idx
  (dlchmgr_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dlchmgr_handle_cfg_idx_failure_ext_op

DESCRIPTION     This function handles all the operations after a external 
                operation (RRC initiated) has failed because of lack of free 
                entry in dynamic db.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dlchmgr_handle_cfg_idx_failure_ext_op(
  l1_dl_phychan_enum_type phch, l1_setup_ops_enum_type op, uint8 cctrch_index);

/*===========================================================================
FUNCTION        dlchmgr_handle_cfg_idx_failure_int_op

DESCRIPTION     This function handles all the operations after a internal 
                operation (L1 initiated) has failed because of lack of free 
                entry in dynamic db.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dlchmgr_handle_cfg_idx_failure_int_op(
  dlchmgr_int_setup_struct_type *ch_info, l1_setup_ops_enum_type op);

/*===========================================================================
FUNCTION        dlchmgr_get_static_db_idx

DESCRIPTION     This function finds the static db index associated with the channel identified by 
                the input parameter.

DEPENDENCIES    None

RETURN VALUE
                static-db-idx: The static dn index of the channel identified by the input parameter.

SIDE EFFECTS    None
===========================================================================*/
LOCAL dlchmgr_static_db_idx_type dlchmgr_get_static_db_idx(dl_phychan_add_type *add_info);

/*===========================================================================
FUNCTION        dlchmgr_handle_cleanup_bmask

DESCRIPTION     This function handles the clean-up after any failure during 
                channel addition or re-cfg.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dlchmgr_handle_cleanup_bmask(dlchmgr_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dlchmgr_init_db

DESCRIPTION     This function initializes a entry identified by the input 
                parameter in dynamic db.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dlchmgr_init_db(dlchmgr_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dlchmgr_get_stat_db_idx_for_gen_op

DESCRIPTION     This function finds the general operation static db index 
                associated with the general operation identified by the input parameter.

DEPENDENCIES    None

RETURN VALUE
                static-db-idx: The general operation static db index for the operation.

SIDE EFFECTS    None
===========================================================================*/
LOCAL dlchmgr_gen_op_stat_db_idx_enum_type dlchmgr_get_stat_db_idx_for_gen_op(
  dl_phychan_action_reason_enum_type gen_op_ext_type);

/*===========================================================================
FUNCTION        dlchmgr_check_for_gen_op_susp_all

DESCRIPTION     This function checks whether a suspend-all general operation 
                has been finished completely in dl-phch. If yes, the done-function 
                corresponding to the general operation shall be called.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dlchmgr_check_for_gen_op_susp_all( dlchmgr_cfg_db_idx_type cfg_db_idx );

/*===========================================================================
FUNCTION        dlchmgr_check_for_gen_op_susp_all

DESCRIPTION     This function checks whether a drop-all general operation 
                has been finished completely in dl-phch. If yes, the done-function 
                corresponding to the general operation shall be called.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dlchmgr_check_for_gen_op_drop_all( dlchmgr_cfg_db_idx_type cfg_db_idx );

/*===========================================================================
FUNCTION        dlchmgr_gen_op_sleep_processed

DESCRIPTION     This function sends response to DRX after a sleep all general 
                operation is completed.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dlchmgr_gen_op_sleep_processed( void );

/*===========================================================================
FUNCTION        dlchmgr_gen_op_cell_trans_processed

DESCRIPTION     This function sends response to L1M after a cell transition general
                operation is completed.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dlchmgr_gen_op_cell_trans_processed( void );

/*===========================================================================
FUNCTION        dlchmgr_gen_op_idle_req_processed

DESCRIPTION     This function sends response to L1M after a idle request general 
                operation is completed.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dlchmgr_gen_op_idle_req_processed( void );

/*===========================================================================
FUNCTION        dlchmgr_hdle_gen_op_post_ch_op

DESCRIPTION     This function should be called after any channel operation if 
                the channel is part of general operation. This function calls the 
                check function corresponding to the general operation.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dlchmgr_hdle_gen_op_post_ch_op(dlchmgr_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dlchmgr_hdle_gen_op_done

DESCRIPTION     This function should be called after a general operation has 
                been completed across all channels. This function calls the 
                Done function corresponding to the general operation.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dlchmgr_hdle_gen_op_done( void );

/*===========================================================================
FUNCTION        dlchmgr_drop_all

DESCRIPTION     This function initiates a drop in dl-phch for the channels 
                that are filtered by the general operation in progress.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dlchmgr_drop_all( void );

/*===========================================================================
FUNCTION        dlchmgr_suspend_all

DESCRIPTION     This function initiates a suspend in dl-phch for the channels 
                that are filtered by the general operation in progress.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dlchmgr_suspend_all( void );

/*===========================================================================
FUNCTION        dlchmgr_init_gen_op_db

DESCRIPTION     This function initializes dlchmgr general operation db.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dlchmgr_init_gen_op_db( void );

/*===========================================================================
FUNCTION        dlchmgr_get_drx_hdle_for_non_sh_ch

DESCRIPTION     This function finds the drx handle associated with a physical 
                channel identified by the dynamic db index.

DEPENDENCIES    This function should only be used for the channels that are 
                non-shared (eg: PICH/AICH).

RETURN VALUE
                drx handle: The drx handle associated with the input dynamic 
                db index

SIDE EFFECTS    None
===========================================================================*/
LOCAL drx_entity_handle_type dlchmgr_get_drx_hdle_for_non_sh_ch(
  dlchmgr_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dlchmgr_fill_stat_info

DESCRIPTION     This function copies the static information pertaining to a channel 
                from static db to config db.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dlchmgr_fill_stat_info(dlchmgr_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dlchmgr_is_ok_for_ext_pccpch_setup

DESCRIPTION     This function finds if it is ok to setup an external pccpch(setup by RRC).
                1. If an external pccpch is already setup and if it is being
                dropped, then it is OK.
                2. Else if an external pccpch is already setup and this
                cphy_setup_req is not dropping it, then it is not OK.
                3. Else(if no external pccpch is already setup), then it is OK.

DEPENDENCIES    None

RETURN VALUE
                TRUE: if it is OK to setup external pccpch.
                FALSE: if it is not OK to setup external pccpch.

SIDE EFFECTS    None
===========================================================================*/
LOCAL boolean dlchmgr_is_ok_for_ext_pccpch_setup(
  /*setup req mask*/
  uint16 req_mask,

  /* drop phychan type */
  l1_dl_phychan_enum_type drop_dl_phychan
);

/*===========================================================================
FUNCTION        dlchmgr_en_svc

DESCRIPTION     This function enables the R99 service on the MBMS Shared Channel

DEPENDENCIES    None

RETURN VALUE
                TRUE: Enabling Service is successful.
                FALSE: Enabling Service fails.

SIDE EFFECTS    None.
===========================================================================*/
LOCAL boolean dlchmgr_en_svc(
/*cctrch index*/
uint8 cctrch_idx,

/*Service to be handled*/
dlshmgr_svr_type_enum_type shmgr_svc_type
);

/*===========================================================================
FUNCTION        dlchmgr_dis_svc

DESCRIPTION     This function disables the R99 service on the MBMS Shared Channel

DEPENDENCIES    None

RETURN VALUE
                TRUE: Disabling Service is successful.
                FALSE: Disabling Service fails.

SIDE EFFECTS    None.
===========================================================================*/
LOCAL boolean dlchmgr_dis_svc(
/*cctrch index*/
uint8 cctrch_idx,

/*Service to be handled*/
dlshmgr_svr_type_enum_type shmgr_svc_type
);

/*===========================================================================
FUNCTION        dlchmgr_drop_sh_ch_for_gen_op

DESCRIPTION     This function drops the shared channel for general opearion

DEPENDENCIES    None

RETURN VALUE
                TRUE: Channel drop is successful.
                FALSE: Drop operation failes.

SIDE EFFECTS    None.
===========================================================================*/
LOCAL boolean dlchmgr_drop_sh_ch_for_gen_op(dlchmgr_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dlchmgr_get_add_state

DESCRIPTION     This function finds the add state for a channel: ACTIVE or SUSPEND

DEPENDENCIES    None

RETURN VALUE
                ACTIVE: If the channel has to be added in ACTIVE state (combiner is setup)
                SUSPEND: If the channel has to be added in SUSPEND state (combiner is not setup)

SIDE EFFECTS    None.
===========================================================================*/
LOCAL dl_phch_state_enum_type dlchmgr_get_add_state(dlchmgr_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dlchmgr_is_fach_to_dch_transition_with_hsrach

DESCRIPTION     This function returns TRUE if this is FACH to DCH transition 
                with HS RACH channels enabled.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
boolean dlchmgr_is_fach_to_dch_transition_with_hsrach(boolean chan_is_fdpch);

#endif /* DLCHMGR_I_H */

