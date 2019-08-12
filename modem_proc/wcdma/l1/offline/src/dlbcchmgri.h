#ifndef DLBCCHMGR_I_H
#define DLBCCHMGR_I_H

/*===========================================================================
                               D L B C C H M G R I. H

GENERAL DESCRIPTION

  This file contains the declarations and definitions that are internal to
  dlbcchmgr.

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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/dlbcchmgri.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------
05/09/14    sr      Changes to introduce new client RF voting for Sec PCCPCH 
09/14/13    dm      DSDS: InterSIB sleep support for G2W tuneaway.
10/09/12    sr      Changes to initiate schedule update after receiving every BCH block
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/23/12    kcm     L/G to W handover optimizations
12/10/08    ks      Mainlining FEATURE_WCDMA_DL_ENHANCED
10/03/08    hk      Bringing in 7k Mailine fixes
01/25/08    vsr     Checking in Enhanced DL files onto mainline
01/16/08    vp      Added support for PCCPCH weak indication
01/15/08    vp      Added code comments, Merged dynamic and static db in to a 
                    single config db, Added support for "Serving cell change abort procedure"
11/06/07    vp      Moved DRX handle from dlchmgr to channel specific managers
                    and added shared channel support in sccpchmgr.
08/23/07    vsr     Made include file change related to removal of dlcmd.* files
08/19/07    vsr     Changed cm_intraf_rsv_status_ok to l1_cm_intraf_rsv_status_ok
03/02/07    vp      Added support for R99 cell reselection
02/21/07    vp      Initial version
===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "l1const.h"
#include "dlchmgr.h"
#include "dlphch.h"

/*--------------------------------------------------------------------------
                      Included Header Files
--------------------------------------------------------------------------*/


/*------------------GLOBAl VARIABLES DEFINITION BEGIN------------------------------------*/

/*Macro to hold the maximum report count for pccpch whose TTI is un-known*/
#define DL_BCCH_CRC_MAX_RPT_CNT_FOR_DUAL_CFN_SETUP 8

/*Macro to hold the maximum report count for pccpch whose TTI is known*/
#define DL_BCCH_CRC_MAX_RPT_CNT_FOR_MONO_CFN_SETUP 4

/*Macro to hold the minimum pass count for all types of bcch*/
#define DL_BCCH_CRC_MIN_PASS_CNT 1

#define DL_BCCH_MAX_TR_BLOCKS_PER_TTI (1)

/*Macro to indicate bcch manager invalid dynamic db index*/
#define DL_BCCH_INVALID_DYN_DB_IDX (DLCHMGR_INVALID_CH_SPEC_IDX)

/*Macro to indicate the minimum energy a bcch block should have to be 
considered to be passed*/
#define DOB_ENERGY_THRESHOLD 0x100

#define DL_BCCH_DEFAULT_TRCH_IDX (0)

#define DL_BCCH_RESLAM_DONE_CB (dl_bcch_reslam_cb)

#define DL_BCCH_INVALID_TFN_FRAME_QTY (0xFFFF)
#define DL_BCCH_INVALID_TFN_CHIP_QTY (0xFFFFFFFF)

/*The decoder returns the cfn aligned with the decoded cfn;
  The even/odd cfn in the decoder interface is the cfn from the mdsp i/f;
  It is plus 1 of decoder
*/
#define DL_BCCH_GET_PASS_CNT_IDX_MDSP_TYPE(cfn) ((cfn+1)%2)

#define DL_BCCH_GET_PASSED_CFN_MDSP_TYPE(cfn) (((cfn%2) == 0) ? DL_ODD_CFN_PCCPCH : DL_EVEN_CFN_PCCPCH)

#define DL_BCCH_GET_FAILED_CFN_MDSP_TYPE(cfn) (((cfn%2) == 0) ? DL_EVEN_CFN_PCCPCH : DL_ODD_CFN_PCCPCH)

#define DL_BCCH_CM_RSV_NOT_OK(cfg_db_idx)    \
  ((DL_BCCH_GET_STATIC_DB_IDX(cfg_db_idx) == DL_BCCH_SETUP_TYPE_SFN_WRC) &&    \
  (l1_cm_intraf_rsv_status_ok != TRUE))

/*Macro to check whether a bcch static db index is invalid*/
#define DL_BCCH_IS_STATIC_DB_IDX_INVALID(static_db_idx)  \
  (static_db_idx >= DL_BCCH_STAT_DB_SIZE)

/*Macro to do a preliminary check on a bcch crc status command
before running the crc state machine*/
#define DL_BCCH_CRC_CMD_PRELIM_CHECK_FAILED(cmd)    \
  ((cmd->eng < DOB_ENERGY_THRESHOLD) || (cmd->sfn == 0))    \

#define DL_BCCH_IS_CFG_DB_IDX_NOT_IN_USE(cfg_db_idx)    \
  (dl_bcch_cfg_db[cfg_db_idx].dyn_db.in_use == FALSE )

#define DL_BCCH_IS_CFG_DB_IDX_IN_USE(cfg_db_idx)    \
  (dl_bcch_cfg_db[cfg_db_idx].dyn_db.in_use == TRUE )

#define DL_BCCH_GET_BLER_ERR_BLKS_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dl_bcch_cfg_db[cfg_db_idx].dyn_db.bler_state_mc_err_blks)

#define DL_BCCH_GET_BLER_TOTAL_BLKS_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dl_bcch_cfg_db[cfg_db_idx].dyn_db.bler_state_mc_total_blks)

#define DL_BCCH_GET_BLER_WDW_IDX_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (dl_bcch_cfg_db[cfg_db_idx].dyn_db.bler_window_curr_index)

#define DL_BCCH_IS_CRC_STATEMC_NOT_FINISHED(cfg_db_idx)    \
    (DL_BCCH_GET_CRC_STATEMC_FINISHED(cfg_db_idx) == FALSE)

#define DL_BCCH_IS_CRC_STATEMC_FINISHED(cfg_db_idx)    \
    (DL_BCCH_GET_CRC_STATEMC_FINISHED(cfg_db_idx) == TRUE)

#define DL_BCCH_GET_CRC_STATEMC_FINISHED(cfg_db_idx)    \
    (dl_bcch_cfg_db[cfg_db_idx].dyn_db.crc_statemc_finished)

#define DL_BCCH_IS_DUAL_CFN_SETUP_FROM_CFG_DB_IDX(cfg_db_idx)    \
    (dl_bcch_cfg_db[cfg_db_idx].stat_db.dual_cfn_setup)

#define DL_BCCH_GET_MIN_PASS_CNT_FROM_CFG_DB_IDX(cfg_db_idx)    \
    (dl_bcch_cfg_db[cfg_db_idx].stat_db.min_pass_cnt)

#define DL_BCCH_GET_MAX_RPT_CNT_FROM_CFG_DB_IDX(cfg_db_idx)    \
    (dl_bcch_cfg_db[cfg_db_idx].stat_db.max_rpt_cnt)

#define DL_BCCH_GET_STATIC_DB_IDX(cfg_db_idx)    \
    (dl_bcch_cfg_db[cfg_db_idx].dyn_db.static_db_idx)

#define DL_BCCH_OVERALL_BCCH_OP_STATUS(crc_status, drop_status)    \
    (crc_status && drop_status)

#define DL_BCCH_IS_CCTRCH_IDX_INVALID(cctrch_idx)    \
  (DLCHMGR_IS_CCTRCH_IDX_INVALID(cctrch_idx))

#define DL_BCCH_IS_BCCH_DB_IDX_INVALID(cfg_db_idx)    \
  (cfg_db_idx == DL_BCCH_INVALID_DYN_DB_IDX)

#define DL_BCCH_IS_SFN_WRC_NEEDED_FROM_CFG_DB_IDX(cfg_db_idx)    \
    (dl_bcch_cfg_db[cfg_db_idx].stat_db.sfn_wrc_diff_needed)

#define DL_BCCH_IS_BLER_MEAS_NEEDED_FROM_CFG_DB_IDX(cfg_db_idx)    \
    (dl_bcch_cfg_db[cfg_db_idx].stat_db.run_bler_meas)

#define DL_BCCH_IS_SCHEDULE_UPDATE_NEEDED_FROM_CFG_DB_IDX(cfg_db_idx)    \
    (dl_bcch_cfg_db[cfg_db_idx].stat_db.schedule_update_needed)

#define DL_BCCH_DOES_MAC_NEEDS_BCH_FROM_CFG_DB_IDX(cfg_db_idx)    \
    (dl_bcch_cfg_db[cfg_db_idx].stat_db.turn_bch_on_to_mac)

#define DL_BCCH_GET_HWCH_FROM_CFG_DB_IDX(cfg_db_idx)  \
  (DLCHMGR_GET_HWCH_FROM_CFG_DB_IDX(DL_BCCH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)))

#define DL_BCCH_GET_HWCH_FROM_CFG_DB_IDX_TO_BE_RESLAMMED(cfg_db_idx)    \
  (DLCHMGR_GET_HWCH_FROM_CFG_DB_IDX(DL_BCCH_GET_CHMGR_CFG_DB_IDX_TO_BE_RESLAMMED(cfg_db_idx)))

#define     DL_BCCH_PREP_TLM_DATA_FROM_CRC_STATUS_CMD_PTR(crc_status_cmd, tlm_data)    \
  do {    \
    tlm_data.decoded_sfn = crc_status_cmd->sfn;    \
    tlm_data.mdsp_cfn = crc_status_cmd->cfn;    \
    tlm_data.hwch = crc_status_cmd->hwch;    \
  } while (0)

#define DL_BCCH_BUILD_SFN_WRC_DIFF_TYPE(status,sfn_wrc_diff,srch_sfn_wrc)    \
  do {         \
    (srch_sfn_wrc).sfn_wrc_valid = status;         \
    TLM_COPY_TLM_TFN(sfn_wrc_diff, (srch_sfn_wrc).sfn_wrc_diff);         \
  } while(0)

#define DL_BCCH_SHOULD_CHANNEL_BE_RESLAMMED_FROM_CFG_DB_IDX(cfg_db_idx)    \
    (dl_bcch_cfg_db[cfg_db_idx].stat_db.reslam_needed)

#define DL_BCCH_SHOULD_ALM_BE_NOTIFIED_FROM_CFG_DB_IDX(cfg_db_idx)    \
    (dl_bcch_cfg_db[cfg_db_idx].stat_db.notify_alm)

#define DL_BCCH_OK_TO_RESLAM_CHANNEL(channe_to_be_dropped, reslam_info_from_db)    \
    ((!channe_to_be_dropped) && (reslam_info_from_db))

#define DL_BCCH_GET_PHCH_HDLE(cfg_db_idx)    \
    (DLCHMGR_GET_PHCH_HDLE_FROM_CFG_DB_IDX(DL_BCCH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)))

#define DL_BCCH_GET_CHMGR_CFG_DB_IDX_TO_BE_RESLAMMED(cfg_db_idx)    \
    (dl_bcch_cfg_db[cfg_db_idx].dyn_db.chmgr_reslam_cfg_db_idx)

#define DL_BCCH_GET_PHCH_HDLE_FOR_CFG_DB_IDX_TO_BE_RESLAMMED(cfg_db_idx)    \
     (DLCHMGR_GET_PHCH_HDLE_FROM_CFG_DB_IDX(DL_BCCH_GET_CHMGR_CFG_DB_IDX_TO_BE_RESLAMMED(cfg_db_idx)))

#define DL_BCCH_GET_CRC_STATUS(cfg_db_idx)    \
    (dl_bcch_cfg_db[cfg_db_idx].dyn_db.crc_status)

#define DL_BCCH_GET_PSC(cfg_db_idx)    \
    (DLCHMGR_GET_PSC_FROM_CFG_DB_IDX(DL_BCCH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)))

#define DL_BCCH_GET_CCTRCH_IDX_FROM_CFG_DB_IDX(cfg_db_idx)    \
    (DLCHMGR_GET_CCTRCH_IDX_FROM_CFG_DB_IDX(DL_BCCH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)))

#define DL_BCCH_GET_CCTRCH_IDX_FROM_CFG_DB_IDX_TO_BE_RESLAMMED(cfg_db_idx)  \
    (DLCHMGR_GET_CCTRCH_IDX_FROM_CFG_DB_IDX(DL_BCCH_GET_CHMGR_CFG_DB_IDX_TO_BE_RESLAMMED(cfg_db_idx)))

#define DL_BCCH_GET_FREQ(cfg_db_idx)    \
    (DLCHMGR_GET_FREQ_FROM_CFG_DB_IDX(DL_BCCH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)))

#define DL_BCCH_GET_DRX_HDLE_FROM_CFG_DB_IDX(cfg_db_idx)    \
    (dl_bcch_cfg_db[cfg_db_idx].dyn_db.drx_handle)

#define DL_BCCH_IS_DRX_HDLE_INVALID_FROM_CFG_DB_IDX(cfg_db_idx)    \
    (DL_BCCH_GET_DRX_HDLE_FROM_CFG_DB_IDX(cfg_db_idx) == DRX_ENTITY_HANDLE_INVALID)

#define DL_BCCH_GET_DRX_ENTITY_TYPE_FROM_CFG_DB_IDX(cfg_db_idx)    \
    (dl_bcch_cfg_db[cfg_db_idx].stat_db.drx_type)

#define DL_BCCH_GET_DRX_CB_FUNC_FROM_CFG_DB_IDX(cfg_db_idx)    \
    (dl_bcch_cfg_db[cfg_db_idx].stat_db.drx_cb_functions)

#define DL_BCCH_GET_SFN_WRC(cfg_db_idx)    \
    (dl_bcch_cfg_db[cfg_db_idx].dyn_db.sfn_wrc_diff)

#define DL_BCCH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)    \
    (dl_bcch_cfg_db[cfg_db_idx].dyn_db.chmgr_cfg_db_idx)

#define DL_BCCH_IS_CFG_DB_IDX_INVALID(cfg_db_idx)    \
    (cfg_db_idx >= DL_BCCH_CFG_DB_SIZE)

#define DL_BCCH_GET_CURRENT_FREQ() (srchsetutil_get_serving_cell(CARR_0)->freq)

#define DL_BCCH_GET_BLER_STATS_FOR_BLER_IDX(cfg_db_idx, bler_index)  \
  (dl_bcch_cfg_db[cfg_db_idx].dyn_db.bler_running_status & (uint64)(((uint64)1) << bler_index))

#define DL_BCCH_SET_BLER_STATS_FOR_BLER_IDX(cfg_db_idx, bler_index)  \
  (dl_bcch_cfg_db[cfg_db_idx].dyn_db.bler_running_status |= (uint64)(((uint64)1) << bler_index))

#define DL_BCCH_RESET_BLER_STATS_FOR_BLER_IDX(cfg_db_idx, bler_index)  \
  (dl_bcch_cfg_db[cfg_db_idx].dyn_db.bler_running_status &= (uint64)(~(((uint64)1) << bler_index)))

#define DL_BCCH_RESET_TFN_TYPE(tfn_struct)         \
  do {         \
    (tfn_struct).frame_qty = (int16) DL_BCCH_INVALID_TFN_FRAME_QTY;         \
    (tfn_struct).chip_offset_cx8_qty = (int32) DL_BCCH_INVALID_TFN_CHIP_QTY;         \
  }while(0)

#define DL_BCCH_IS_HWCH_DROP_NEEDED(cfg_db_idx) \
  (dl_bcch_cfg_db[cfg_db_idx].stat_db.drop_hwch_needed)

#define DL_BCCH_IS_DRX_HDLE_INVALID(drx_hdle)  \
  (drx_hdle == DRX_ENTITY_HANDLE_INVALID)

/*Enum to hold the results of CRC state machine*/
typedef enum
{
  DL_BCCH_CRC_STATUS_FAILURE,
  DL_BCCH_CRC_STATUS_SUCCESS,
  DL_BCCH_CRC_STATUS_IGNORED
} dl_bcch_report_status_enum_type;

/*Structure that maps a bcch type from chmgr static db type to bcch manager static db type*/
typedef struct {
dlchmgr_static_db_idx_type dlchmgr_static_db_idx;

dl_bcch_static_db_idx_enum_type bcch_static_db_idx;
} dl_bcch_stat_idx_map_struct_type;
/*------------------GLOBAl VARIABLES DEFINITION END------------------------------------*/

/*------------------STATIC VARIABLES DEFINITION BEGIN------------------------------------*/


/*------------------STATIC VARIABLES DEFINITION END------------------------------------*/

/*------------------STATIC FUNCTIONS DECLARATION BEGIN------------------------------------*/

/*===========================================================================
FUNCTION        dl_bcch_prep_db

DESCRIPTION     This function resets an entry in bcch manager dynamic db.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dl_bcch_prep_db(uint8 cfg_db_idx);

/*===========================================================================
FUNCTION        dl_bcch_find_stat_db_idx

DESCRIPTION     This function finds the ch-spec manager dynamic db index 
                associated with the channel identified by the CCTrCh.

DEPENDENCIES    None

RETURN VALUE
                ch-spec-db-idx: The ch-specific db index of the channel identified by the input parameter.

SIDE EFFECTS    None
===========================================================================*/
LOCAL uint8 dl_bcch_get_cfg_db_idx(uint8 cctrch_idx);

/*===========================================================================
FUNCTION        dl_bcch_crc_state_mc_post_process

DESCRIPTION     This function does all the post-processing after the CRC 
                state machine is over.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dl_bcch_crc_state_mc_post_process(uint8 cfg_db_idx, boolean pass, dl_bch_crc_status_type *cmd);

/*===========================================================================
FUNCTION        dl_bcch_run_crc_state_machine

DESCRIPTION     This function runs the crc state machine based on the input 
                params: CCTrCh Idx, crc pass/fail.

DEPENDENCIES    None

RETURN VALUE
                crc-state machine status: success/failure/ignored

SIDE EFFECTS    None
===========================================================================*/
LOCAL dl_bcch_report_status_enum_type dl_bcch_run_crc_state_machine
(
  uint8 cfg_db_idx, dl_bch_crc_status_type *cmd
);

/*===========================================================================
FUNCTION        dl_bcch_find_stat_db_idx

DESCRIPTION     This function finds the static db index associated with the channel identified by 
                the input parameter.

DEPENDENCIES    None

RETURN VALUE
                static-db-idx: The static db index of the channel identified by the input parameter.

SIDE EFFECTS    None
===========================================================================*/
LOCAL dl_bcch_static_db_idx_enum_type dl_bcch_find_stat_db_idx
  (dlchmgr_cfg_db_idx_type chmgr_dyn_db_idx);

/*===========================================================================
FUNCTION        dl_bcch_invoke_schedule_update

DESCRIPTION     This function invokes the schedule update with DRX manager
                such that channel can be put to sleep if possible

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dl_bcch_invoke_schedule_update(uint8 cctrch_idx, uint8 trch_idx, uint8 rece_cfn,
  dsm_item_type *dsm_ptr);

/*===========================================================================
FUNCTION        DL_BCCH_RESET_SIB_MASK

DESCRIPTION
  This function is called when we process BCCH add.  It resets the SIB mask.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dl_bcch_reset_sib_mask(void);

/*===========================================================================
FUNCTION        dl_bcch_vote_on_rf_for_bch_decode

DESCRIPTION     This function votes for RF to be on/off BCH decode

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dl_bcch_vote_on_rf_for_bch_decode(uint8 cfg_db_idx, boolean vote);

/*------------------STATIC FUNCTIONS DECLARATION END------------------------------------*/
#endif

