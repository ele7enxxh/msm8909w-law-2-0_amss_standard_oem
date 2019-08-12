#ifndef DLSCCPCHMGR_I_H
#define DLSCCPCHMGR_I_H

/*===========================================================================
                               D L S C C P C H M G R I. H

GENERAL DESCRIPTION

  This file contains the declarations and definitions that are internal to
  dlsccpchmgr.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright(c)2007 - 2008 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/tlm.c_v   1.153   17 Jul 2002 16:36:16   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/dlsccpchmgri.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------
12/10/08    ks      Mainlining FEATURE_WCDMA_DL_ENHANCED
10/03/08    hk      Bringing in 7k Mailine fixes
01/25/08    vsr     Checking in Enhanced DL files onto mainline
01/23/08    vsr     Lint fixes
01/15/08    vp      Added code comments and Removed static db
11/06/07    vp      Moved DRX handle from dlchmgr to channel specific managers
                    and added shared channel support in sccpchmgr.
08/20/07    vp      Added support for new sccpch0/pich reponse functions.
02/21/07    vp      Initial version
                    ===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "l1const.h"
#include "l1dlphychancfg.h"
#include "l1msetup.h"
#include "dlchmgr.h"

/*Bit mask to check whether PCH service is present on a sccpch*/
#define DL_SCCPCH_PCH_SVC_CHECK_MASK (dlshmgr_svc_mask_type)(DLSHMGR_SVC_BMSK(DLSHMGR_SVC_TYPE_R99_PCH))

/*Bit mask to check whether FACH service is present on a sccpch*/
#define DL_SCCPCH_R99_FACH_SVC_CHECK_MASK (dlshmgr_svc_mask_type)(DLSHMGR_SVC_BMSK(DLSHMGR_SVC_TYPE_R99_FACH))

#define DL_SCCPCH_INVALID_DYN_DB_IDX (DLCHMGR_INVALID_CH_SPEC_IDX)

#define DL_SCCPCH_IS_CFG_DB_IDX_NOT_IN_USE(cfg_db_idx)    \
  (dl_sccpch_cfg_db[cfg_db_idx].in_use == FALSE )

#define DL_SCCPCH_IS_CFG_DB_IDX_IN_USE(cfg_db_idx)    \
  (dl_sccpch_cfg_db[cfg_db_idx].in_use == TRUE )

#define DL_SCCPCH_IS_CFG_DB_IDX_INVALID(cfg_db_idx)    \
  (cfg_db_idx >= DL_SCCPCH_CFG_DB_SIZE)

#define DL_SCCPCH_IS_DRX_DB_IDX_INVALID(drx_db_idx)    \
  (drx_db_idx >= DL_SCCPCH_R99_LOGICAL_ENTITY_MAX)

#define DL_SCCPCH_IS_DRX_DB_IDX_VALID(drx_db_idx)    \
  (drx_db_idx < DL_SCCPCH_R99_LOGICAL_ENTITY_MAX)

#define DL_SCCPCH_IS_CCTRCH_IDX_INVALID(cctrch_idx)    \
  (DLCHMGR_IS_CCTRCH_IDX_INVALID(cctrch_idx))

#define DL_SCCPCH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)    \
  (dl_sccpch_cfg_db[cfg_db_idx].chmgr_cfg_db_idx)

#define DL_SCCPCH_GET_CCTRCH_IDX_FROM_CFG_DB_IDX(cfg_db_idx)    \
  (DLCHMGR_GET_CCTRCH_IDX_FROM_CFG_DB_IDX(DL_SCCPCH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)))

#define DL_SCCPCH_GET_PHCH_HDLE_FROM_CFG_DB_IDX(cfg_db_idx)    \
  (DLCHMGR_GET_PHCH_HDLE_FROM_CFG_DB_IDX(DL_SCCPCH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)))

#define DL_SCCPCH_GET_CCTRCH_TO_SVC_MAP_MASK_FROM_CFG_DB_IDX(cfg_db_idx)    \
  (dl_sccpch_cfg_db[cfg_db_idx].cctrch_to_svc_map_mask)

#define DL_SCCPCH_IS_ASSOC_PICH_ALLOWED_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (DLCHMGR_GET_PHCH_TYPE_FROM_CFG_DB_IDX(  \
    DL_SCCPCH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)) == L1_DL_PHYCHAN_SCCPCH0)

#define DL_SCCPCH_GET_PHCH_TYPE_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (DLCHMGR_GET_PHCH_TYPE_FROM_CFG_DB_IDX(  \
    DL_SCCPCH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)))

#define DL_SCCPCH_IS_ASSOC_PICH_INVOLVED_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dl_sccpch_cfg_db[cfg_db_idx].assoc_pich_involved == TRUE)

#define DL_SCCPCH_IS_NO_SVC_ON_FROM_CFG_DB_IDX(cfg_db_idx)    \
  (DL_SCCPCH_GET_CCTRCH_TO_SVC_MAP_MASK_FROM_CFG_DB_IDX(cfg_db_idx) == DLSHMGR_SVC_MASK_NONE)

#define DL_SCCPCH_IS_SVC_ON_FROM_CFG_DB_IDX(cfg_db_idx, svc_check_mask)    \
  ((DL_SCCPCH_GET_CCTRCH_TO_SVC_MAP_MASK_FROM_CFG_DB_IDX(cfg_db_idx) &    \
  ((dlshmgr_svc_mask_type) svc_check_mask)) != 0)

#define DL_SCCPCH_IS_R99_FACH_SVC_ON_FROM_CFG_DB_IDX(cfg_db_idx)    \
  DL_SCCPCH_IS_SVC_ON_FROM_CFG_DB_IDX(cfg_db_idx, DL_SCCPCH_R99_FACH_SVC_CHECK_MASK)

#define DL_SCCPCH_IS_PCH_SVC_ON_FROM_CFG_DB_IDX(cfg_db_idx)    \
  DL_SCCPCH_IS_SVC_ON_FROM_CFG_DB_IDX(cfg_db_idx, DL_SCCPCH_PCH_SVC_CHECK_MASK)

#define DL_SCCPCH_GET_DRX_ENTITY_TYPE_FROM_LOGICAL_ENTITY(logical_entity)    \
  (dl_sccpch_r99_logical_entity_db[logical_entity].entity_type)

#define DL_SCCPCH_GET_DRX_CB_FROM_LOGICAL_ENTITY(logical_entity)    \
  (dl_sccpch_r99_logical_entity_db[logical_entity].drx_cb_functions)

#define DL_SCCPCH_GET_SVC_TYPE_FROM_LOGICAL_ENTITY(logical_entity)    \
  (dl_sccpch_r99_logical_entity_db[logical_entity].trch_service_type)

#define DL_SCCPCH_GET_FREQ_FROM_CFG_DB_IDX(cfg_db_idx)    \
  (DLCHMGR_GET_FREQ_FROM_CFG_DB_IDX(DL_SCCPCH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)))

#define DL_SCCPCH_GET_PSC_FROM_CFG_DB_IDX(cfg_db_idx)    \
  (DLCHMGR_GET_PSC_FROM_CFG_DB_IDX(DL_SCCPCH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)))

#define DL_SCCPCH_GET_CFG_TRCH_MASK_FROM_LOGICAL_ENTITY(logical_entity)    \
  (dl_sccpch_r99_logical_entity_db[logical_entity].cfg_trch_mask)

#define DL_SCCPCH_GET_DRX_HDLE_FROM_LOGICAL_ENTITY(logical_entity)    \
  (dl_sccpch_r99_logical_entity_db[logical_entity].drx_entity_handle)

#define DL_SCCPCH_IS_NO_TRCH_FROM_LOGICAL_ENTITY(logical_entity)    \
  (DL_SCCPCH_GET_CFG_TRCH_MASK_FROM_LOGICAL_ENTITY(logical_entity) == DLSHMGR_SVC_TO_TRCH_MAP_NONE)

#define DL_SCCPCH_IS_DRX_HDLE_INVALID_FROM_LOGICAL_ENTITY(logical_entity)    \
  (DL_SCCPCH_GET_DRX_HDLE_FROM_LOGICAL_ENTITY(logical_entity) == DRX_ENTITY_HANDLE_INVALID)

#define DL_SCCPCH_IS_LOGICAL_ENTITY_ON(logical_entity)    \
  (dl_sccpch_r99_logical_entity_db[logical_entity].logical_entity_on == TRUE)

#define DL_SCCPCH_IS_LOGICAL_ENTITY_OFF(logical_entity)    \
  (dl_sccpch_r99_logical_entity_db[logical_entity].logical_entity_on == FALSE)

#define DL_SCCPCH_GET_CH_SPEC_IDX_FROM_LOGICAL_ENTITY(logical_entity)    \
  (dl_sccpch_r99_logical_entity_db[logical_entity].ch_spec_idx)

#define DL_SCCPCH_GET_STATE_FROM_LOGICAL_ENTITY(logical_entity)    \
  (dl_sccpch_r99_logical_entity_db[logical_entity].curr_state)

#define DL_SCCPCH_IS_LOGICAL_ENTITY_VALID(logical_entity)    \
  (logical_entity < DL_SCCPCH_R99_LOGICAL_ENTITY_MAX)

#define DL_SCCPCH_IS_LOGICAL_ENTITY_INVALID(logical_entity)    \
  (!DL_SCCPCH_IS_LOGICAL_ENTITY_VALID(logical_entity))

#define DL_SCCPCH_IS_LOGICAL_ENTITY_STATE_VALID(logical_entity_state)  \
  (logical_entity_state < DL_SCCPCH_LOGICAL_ENT_STATE_MAX)

#define DL_SCCPCH_IS_LOGICAL_ENTITY_STATE_INVALID(logical_entity_state)  \
  (!DL_SCCPCH_IS_LOGICAL_ENTITY_STATE_VALID(logical_entity_state))

#define DL_SCCPCH_UPD_LOGICAL_ENTITY_STATE(logical_entity, new_state)  \
  (dl_sccpch_r99_logical_entity_db[logical_entity].curr_state = new_state)


/*===========================================================================
FUNCTION        dl_sccpch_prep_db

DESCRIPTION     This function resets an entry in sccpch manager dynamic db.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dl_sccpch_prep_db(dl_ch_spec_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dl_sccpch0_post_op_cb

DESCRIPTION     This function does any processing common to add or drop 
                of a channel.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
LOCAL void dl_sccpch0_post_op_cb(dl_ch_spec_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dl_sccpch_do_drx_reg

DESCRIPTION     This function does drx registration for all the entities 
                active on the channel identified by the input parameter.

DEPENDENCIES    None

RETURN VALUE
                TRUE: All drx registrations were successful.
                FALSE: At least one drx registration failed.

SIDE EFFECTS    None.
===========================================================================*/
LOCAL boolean dl_sccpch_do_drx_reg(dl_ch_spec_cfg_db_idx_type cfg_db_idx,
  l1_dl_phychan_db_struct_type *phychan_db);

/*===========================================================================
FUNCTION        dl_sccpch_find_logical_entity_from_drx_hdle

DESCRIPTION     This function finds the logical entity associated with the 
                DRX handle passed as input parameter.

DEPENDENCIES    None

RETURN VALUE
                logical entity type: logical entity associated with the DRX handle

SIDE EFFECTS    None
===========================================================================*/
LOCAL dl_sccpch_r99_logical_entity_enum_type dl_sccpch_find_logical_entity_from_drx_hdle(
  drx_entity_handle_type entity_handle);

/*===========================================================================
FUNCTION        dl_sccpch_find_logical_entity_from_drx_hdle

DESCRIPTION     This function updates the state of a logical entity in logical entity db.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dl_sccpch_upd_entities_state_for_ch(
  dl_ch_spec_cfg_db_idx_type cfg_db_idx, dl_sccpch_r99_logical_state_enum_type new_state);

/*===========================================================================
FUNCTION        dl_sccpch_clean_logical_entity

DESCRIPTION     This function resets an entry in logical entity db.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dl_sccpch_clean_logical_entity(
  dl_sccpch_r99_logical_entity_enum_type logical_entity);
#endif

