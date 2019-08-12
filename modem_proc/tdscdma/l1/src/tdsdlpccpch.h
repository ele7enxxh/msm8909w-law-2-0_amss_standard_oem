#ifndef TDSDLPCCPCH_H
#define TDSDLPCCPCH_H

/*===========================================================================
                 T D S D L P C C P C H . H

GENERAL DESCRIPTION
  This file contains the declarations and definitions of dlbcchmgr needed
  across modules.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2007 - 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsdlpccpch.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "tdsl1const.h"
#include "tdsdlchmgr.h"
#include "tdsl1def.h"
#include "tdsl1mtask.h"
#include "tdsdldecdata.h"

/*Enum to index in to the bcch manager static db*/
typedef enum {
  TDSDLPCCPCH_SETUP_TYPE_ACQ_BCH=0,
  TDSDLPCCPCH_SETUP_TYPE_BCH_SIB_READ,
  TDSDLPCCPCH_SETUP_TYPE_TOTAL_NUM
}tdsdlpccpch_static_db_idx_enum_type;


typedef struct {
  /*Whether the bcch dynamic entry is in use*/
  boolean in_use;
  
  /*Index for the BCCH channel in to the DlChMgr cfg db*/
  tdsdlchmgr_cfg_db_idx_type chmgr_cfg_db_idx;
  
  /*Index for the BCCH channel in to the bcch manager static db*/
  tdsdlpccpch_static_db_idx_enum_type static_db_idx;
  
  /*the tdseulenc_result of the bcch crc state machine*/
  boolean crc_status;
  
  /*drx handle associated with the bcch*/
  tdsdrxmgr_entity_handle_type drx_handle;
  
} tdsdlpccpch_dyn_db_struct_type;
typedef struct {
   
  /*Whether BCH has to be turned ON to MAC*/
  boolean turn_bch_on_to_mac;
 
  /*Whether a channel reslam is needed to correct the CFN in STMR*/
  boolean reslam_needed;
  
  /*Type of entity to be used in registration with DRX*/
  tdsdrxmgr_entity_type drx_type;
  
  /*Call back functions to be registered with DRX*/
  tdsdrxmgr_channel_cb_struct_type *drx_cb_functions;
  
  /*Maximum time waiting for pccpch decode result in shallow mode*/
  uint16 max_detection_time_shallow;
    
  /*Maximum time waiting for pccpch decode result in deep mode*/
  uint16 max_detection_time_deep;  
} tdsdlpccpch_static_db_struct_type;
  
typedef struct {
  tdsdlpccpch_static_db_struct_type stat_db;
  
  tdsdlpccpch_dyn_db_struct_type dyn_db;
} tdsdlpccpch_cfg_struct_type;

extern tdsdlpccpch_cfg_struct_type tdsdlpccpch_cfg_db;

/*Macro to identify the maximum number of PCCPCH allowed at a time*/
#define TDSDLPCCPCH_MAX_ALLOWED_PCCPCH 1

/*Macro to identify the bcch manager cfg db size*/
#define TDSDLPCCPCH_CFG_DB_SIZE (TDSDLPCCPCH_MAX_ALLOWED_PCCPCH)

/*Macro to identify the bcch manager static db size*/
#define TDSDLPCCPCH_STAT_DB_SIZE (3)

/* This is the value we use to return an invalid sfn */
#define TDSDLPCCPCH_SIB_INVALID_SFN  0xFFFF

#define TDSDLPCCPCH_GET_CHMGR_CFG_DB_IDX()    \
    (tdsdlpccpch_cfg_db.dyn_db.chmgr_cfg_db_idx)

#define TDSDLPCCPCH_GET_FREQ()    \
    (TDSDLCHMGR_GET_FREQ_FROM_CFG_DB_IDX(TDSDLPCCPCH_GET_CHMGR_CFG_DB_IDX()))

#define TDSDLPCCPCH_GET_CPI()    \
    (TDSDLCHMGR_GET_CPI_FROM_CFG_DB_IDX(TDSDLPCCPCH_GET_CHMGR_CFG_DB_IDX()))

/*--------------------------------------------------------------------------
                  STRUCTURE: DL_ACQ_STTD_DONE_CMD_TYPE

This struture contains the parameter for the DL_ACQ_STTD_DONE_CMD.
--------------------------------------------------------------------------*/
typedef struct {
  tdsl1def_l1_local_cmd_hdr_type hdr;

  /* the status of the STTD detection procedure, TRUE if successful */
  boolean status;

} tdsdlpccpch_acq_bch_decode_done_cmd_type;


/******************************************************************************
                      COMMAND: TDS_DL_BCH_CRC_STATUS_CMD
******************************************************************************/
/*--------------------------------------------------------------------------
                  STRUCTURE: DL_TRBLK_CRC_STATUS_TYPE

This structure contains parameters for the TDS_DL_TRBLK_CRC_STATUS_CMD.
--------------------------------------------------------------------------*/

typedef struct {
  tdsl1def_l1_local_cmd_hdr_type hdr;

  /* set to TRUE if this begin slam. */
  boolean pass;

} tdsdlpccpch_bch_crc_status_type;


/******************************************************************************
                      COMMAND: DL_BCH_BLER_STATUS_CMD
******************************************************************************/
/*--------------------------------------------------------------------------
                  STRUCTURE: DL_TRBLK_BLER_STATUS_TYPE

This structure contains parameters for the DL_TRBLK_BLER_STATUS_CMD.
--------------------------------------------------------------------------*/
typedef struct {
  tdsl1def_l1_local_cmd_hdr_type hdr;

  /* which CCTrCH table index is being reported on */
  uint8 cctrch_idx;
} dl_bcch_weak_ind_cmd_type;

/* This structure holds the reponse functions for the ACQ BCH*/
extern tdsdlchmgr_resp_func_cb_struct_type tdsdlpccpch_acq_bch_resp_func_cb;

/* This structure holds the reponse functions for the BCH setup by RRC*/
extern tdsdlchmgr_resp_func_cb_struct_type tdsdlpccpch_sib_resp_func_cb;

extern tdsdlchmgr_idx_mgmt_func_struct_type tdsdlpccpch_idx_mgmt_func;

extern tdsdlchmgr_op_cb_func_struct_type tdsdlpccpch_op_cb_func;

extern tdsdlchmgr_schedule_para_cb_func_struct_type tdsdlpccpch_schedule_para_cb_func;

extern rex_timer_type tdsdlpccpch_internal_decode_timer;

extern boolean tdsdlpccpch_blind_decode;

extern boolean tdsdlpccpch_sib_update_received;
/*===========================================================================
FUNCTION        tdsdlpccpch_process_bch_crc_status_cmd

DESCRIPTION     This is the local command handler for the CRC status command.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern boolean tdsdlpccpch_process_bch_crc_status_cmd(tsdl1dec_bch_status_info_struct_type *bch_status_info);

/*===========================================================================
FUNCTION        tdsdlpccpch_status_rpt

DESCRIPTION     This function posts a CRC status command to L1M.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void tdsdlpccpch_status_rpt (boolean status);

/*===========================================================================
FUNCTION        tdsdlpccpch_init

DESCRIPTION     This function initializes the bcch manager. 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void tdsdlpccpch_init( void );

/*===========================================================================
FUNCTION        L1M_CPHY_SIB_SCHED_CHANGE_IND

DESCRIPTION
  We got the TDSL1_CPHY_SIB_SCHED_CHANGE_IND primitive from RRC.  
  This means WL1 is supposed to change schedules for receiving data on the 
  N_PCCPCH. This schedule consists of a bitmask indicating which SIB sfns
  are required.

PARAMETERS
  tdsl1_ext_cmd_type *cmd_ptr - Pointer to TDSL1_CPHY_SIB_SCHED_CHANGE_IND command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tds_l1m_cphy_sib_sched_change_ind(const tdsl1_ext_cmd_type *cmd_ptr);

/*===========================================================================
FUNCTION        tdsdlpccpch_SIB_SAVE_CHAN

DESCRIPTION
  This function is called when we start processing a TDSL1_CPHY_SETUP_REQ for
  NBCH carrying PCCPCH_N from RRC.  It saves away the inter-SIB sleep
  schedule passed down from RRC.

PARAMETERS
  setup - Current TDSL1_CPHY_SETUP_REQ command from RRC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdsdlpccpch_sib_save_chan(const tdsl1_setup_cmd_type* setup);

/*===========================================================================
FUNCTION        tdsdlpccpch_get_cell_info_for_n_sib_read

DESCRIPTION     This function finds the BCCH setup by RRC for neighbor SIB 
                read and return the corresponding cell pointer 

DEPENDENCIES    None

RETURN VALUE
                cell pointer: cell pointer corresponding to the BCCH setup
                by RRC for neighbor SIB read

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdlpccpch_get_cell_info_for_n_sib_read(tdstlm_cell_info_struct_type *cell_info);

/*===========================================================================
FUNCTION        tdsdlpccpch_send_acq_bch_decode_done_cmd

DESCRIPTION     This function posts ACQ BCH decode done command.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void tdsdlpccpch_send_acq_bch_decode_done_cmd(tdsdlchmgr_cfg_db_idx_type chmgr_cfg_db_idx);

/*===========================================================================
FUNCTION        tdsdlpccpch_internal_decode_failed

DESCRIPTION     This function called by l1m when internal pccpch decode time expired.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void tdsdlpccpch_internal_decode_failed(void);

/*===========================================================================
FUNCTION        tdsdlpccpch_slam_done_cb

DESCRIPTION     This function handles reslam done operations.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void tdsdlpccpch_slam_done_cb(void);

/*===========================================================================
FUNCTION        tdsdlpccpch_gen_original_bitmask

DESCRIPTION     This function called by dlchmgr befor schedule pccpch bitmask.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void tdsdlpccpch_gen_original_bitmask(void);

//n-pccpch high priority than pich
/*===========================================================================
FUNCTION        tdsdlpccpch_get_chmgr_cfg_db_idx

DESCRIPTION     This function get tdsdlchmgr cfg db idx for pccpch.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern tdsdlchmgr_cfg_db_idx_type tdsdlpccpch_get_chmgr_cfg_db_idx(void);

/*===========================================================================
FUNCTION      tdsdlpccpch_sib_next_state_sfn

DESCRIPTION
  This function returns the next sfn which is wanted.

PARAMETERS
  uint16 sfn - current sfn.

DEPENDENCIES
  None.

RETURN VALUE
  uint16 - returns the next sfn number RRC wants.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern uint16 tdsdlpccpch_sib_next_state_sfn
(
  /*current sfn*/
  uint16 activation_time,
  /*to find next bitmask is "1"*/
  boolean find_one
);
/*===========================================================================
FUNCTION        tdsdlpccpch_sib_get_mask_bit

DESCRIPTION     This function return add status.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern boolean tdsdlpccpch_sib_get_mask_bit(uint16 actviation_time);
#ifdef FEATURE_TDSCDMA_DSDS
/*===========================================================================
FUNCTION        tdsdlpccpch_no_lock_handle_for_sib_reading

DESCRIPTION     when wake up to recevie pch, if current not get lock, will set pch sleep forvery waiting for next drx cycle

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern boolean tdsdlpccpch_no_lock_handle_for_sib_reading(
  tdsdrxmgr_entity_handle_type         entity_handle,
  tdsdrxmgr_sleep_struct_type *sleep_info_ptr,
  uint16 activation_subfn
);
#endif

/*===========================================================================
FUNCTION        tdsdlpccpch_send_weak_ind_cmd

DESCRIPTION     This function send weak_ind to rrc.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
void tdsdlpccpch_send_weak_ind_cmd(boolean nolock_ind);

#endif

