#ifndef TDSHSCFG_H 
#define TDSHSCFG_H

/*==========================================================================
             TD-SCDMA L1 HSDPA configuration header file

DESCRIPTION
  This file contains definition, declaration required for HSDPA configuration
  and maintainance code.

  Copyright (c) 2010 - 2011 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdshscfg.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $

when       who    what, where, why
-------   ---    ---------------------------------------------------------- 
01/12/11   ow     switched to new RRC/L1 IF 
10/28/10   ow     cleaned up all CM related functions and vaiables from UMTS   
                  code 
===========================================================================*/

#include "tdscdma_variation.h"
#include "tdsuecomdef.h"
#include "customer.h"


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "tdsl1const.h"
#include "tdsl1def.h"
#include "tdsl1rrcif.h"
#include "mcaltds_dl.h"
#include "mcaltds_hscfg.h"
#include "tfw_sw_intf_msg.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/* for cdp 9x15 bring-up with FW team */
#define FEATURE_TDSCDMA_HSDPA_DEBUG 

#define mcaltds_hs_get_cur_ldb_info_idx tdshscfg_get_cur_ldb_info_idx 
#define mcaltds_hs_get_cfg_dsch_incl tdshscfg_get_cur_dsch_cfg_incl

/* HSDPA information buffers for ordered and current config */
#define TDSHSCFG_INFO_COUNT 2

/* Maximum value of HS-SCCH channelization code */
#define TDSHSCFG_SCCH_MAX_CH_CODE_VAL 16

/* various macros related to mDSP interface */
/* ---------------------------------------- */

/* mDSP has 2 HSDPA information table */
#define TDSHSCFG_INFO_TABLE_COUNT 2
/* invalid HS table index value */
#define TDSHSCFG_INFO_TABLE_INVALID_VAL 0xFF

/* this clears and set parameter value at appropriate location in
   buffer */

#define TDSHSCFG_SET_BUF_VAL(buffer, offset, shift, mask, val) \
  buffer[offset] &= (~((uint32)(mask))); \
  buffer[offset] |= (((val) << (shift)) & (mask))

/* This macro fetches the value at the appropriate location in the 
   buffer */
#define TDSHSCFG_GET_BUF_VAL(buffer, offset, shift, mask) \
  ((buffer[offset] & (mask)) >> (shift))
     
/* HSDPA information table fields */
/* ------------------------------ */

#define TDSHSCFG_INFO_TABLE_UPDATE_REQD(hs_info) \
  (TDSL1_CPHY_HS_CFG_REQ_IS_CFG_INCL((hs_info)->hs_req_mask, DSCH) || \
   TDSL1_CPHY_HS_CFG_REQ_IS_CFG_INCL((hs_info)->hs_req_mask, SCCH_SICH))

/* Number of WORD32 entries in each HSDPA information table */
#define TDSHSCFG_INFO_TABLE_LEN_W32 (sizeof(tfw_hs_dataChan_config_t)/4)

/* HS DEMOD/MOD control table related macros */
/* ----------------------------------------- */

#define TDSHSCFG_NUM_CTRL_TABLE_ENTRIES 16

/* HS channel timing offset related macros */
/* --------------------------------------- */

/* Global sub frame number maximum value */
#define TDSHSCFG_MAX_GLOBAL_SUB_FRAME_COUNT 4096
#define TDSHSCFG_NORMALIZE_GLOBAL_SUB_FN(x) \
  tds_normalize((int32) (x), TDSHSCFG_MAX_GLOBAL_SUB_FRAME_COUNT);

/* Number of HS sub frame per radio length 2 */
#define TDSHSCFG_NUM_SUB_FR_PER_RADIO_FR  2

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Call back function type declaration */
/* ----------------------------------- */

/* HS event call back function type */
typedef void TDSHSCFG_CHAN_EVENT_FUNC_TYPE(void);

/* HS configuration done post call back function type */
typedef void TDSHSCFG_CHANNEL_CFG_DONE_POST_CB_FUNC_TYPE(boolean status);

/* Enum type declaration */
/* --------------------- */

/* HSDPA channel configuration action types used by mDSP */
typedef mcaltds_dl_hs_channel_cfg_mdsp_action_enum_type tdshscfg_channel_cfg_mdsp_action_enum_type;

typedef mac_hs_e_type tdshscfg_mac_ver_enum_type;
/* Structure type declaration */
/* -------------------------- */

typedef mcaltds_dec_hs_mac_d_pdu_sz_info_struct_type  tdshscfg_dl_mac_d_pdu_sz_info_struct_type;

/* Information of MAC-d PDU size Q */
typedef mcaltds_dec_hs_mac_d_pdu_sz_q_info_struct_type  tdshscfg_dl_mac_d_pdu_sz_q_info_struct_type;

/* HS DL local information db structure */
typedef struct
{
  /* indicate if this configuration is valid or not */
  boolean valid;

  /* HS channel info variable */
  /* ------------------------ */

  /* CCTrCh Id for this configuration */
  uint8 cctrch_id;

  /* indicate info for MAC-d PDI size Qs
     Q Id goes with index of this array */
  tdshscfg_dl_mac_d_pdu_sz_q_info_struct_type
    mac_d_pdu_sz_q_info[TDSL1_DL_CCTRCHCFG_MAC_HS_Q_MAX_COUNT];

 #ifdef FEATURE_TDSCDMA_64QAM
  boolean hs_64_qam_configured;
 #endif

  /* HS configuration related variable */
  /* --------------------------------- */

  /* configuration CFN */
  uint8 config_cfn;

  /* HSDPA channel configuration action used by mDSP */
  tdshscfg_channel_cfg_mdsp_action_enum_type action;

  /* Reset IR buffer or not */
  boolean softBufferFlush;

  /* HRNTI, add this because FW/SW interface change */
  uint16 h_rnti;

  /* hs-pdsch midamble information */  
  tdsl1_midamble_ss_burst_struct_type  pdsch_midamble;

  /* hs-scch midamble */
  mcaltds_dl_update_fw_midamble_info_struct_type midamble_para;

  /* TB table alignment type */
  boolean tfri_table_alignment_type;
 
} tdshscfg_dl_channel_cfg_db_struct_type;

/* HS call type enum */
typedef struct
{
 #ifdef FEATURE_TDSCDMA_64QAM
  /* 64QAM status on primary carrier */
  boolean hs_64_qam_configured;
 #endif

  /* Number of HARQs configured */
  uint8 num_harq_configured;

  /* TB table alignment type */
  uint8 tfri_table_alignment_type;

}tdshscfg_dec_call_info_struct_type;

/* Local command structure for HS channel config command done */
typedef struct
{
  /* local command header type */
  tdsl1def_l1_local_cmd_hdr_type           hdr;

  /* status of HS channel config */
  boolean status;
} tdshscfg_chan_cfg_done_cmd_type;

/* Local command structure for HS channel config command done */
typedef struct
{
  /* local command header type */
  tdsl1def_l1_local_cmd_hdr_type           hdr;
} tdshscfg_log_submit_cmd_type;

/* Local command structure for DL channel cleanup after HS teardown is done */
typedef struct
{
  /* local command header type */
  tdsl1def_l1_local_cmd_hdr_type           hdr;
} tdshscfg_module_cleanup_after_hs_teardown_cmd_type;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* Mutex for all HS configuration */
#define TDSHSCFG_CFG_INTLOCK()  REX_ISR_LOCK(&tdshscfg_cfg_crit_sec)
#define TDSHSCFG_CFG_INTFREE()  REX_ISR_UNLOCK(&tdshscfg_cfg_crit_sec)

#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED
#define TDSHSCFG_LOG_INTLOCK()  REX_ISR_LOCK(&tdsmcal_hs_logging_crit_sec)
#define TDSHSCFG_LOG_INTFREE()  REX_ISR_UNLOCK(&tdsmcal_hs_logging_crit_sec)
#endif

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION tdshscfg_valid_scch_sich_ch_code

DESCRIPTION
  This function check the HS-SCCH or HS-SICH channelisation code.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if HSDPA channel is active.
  FALSE if HSDPA channel is inactive.

SIDE EFFECTS
  None
===========================================================================*/

/* check HS-SCCH and HS-CICH channelisation code */
boolean tdshscfg_valid_scch_sich_ch_code(tdsl1_dl_ch_code_enum_type ch_code);


/*===========================================================================
FUNCTION tdshscfg_save_hsdpa_rrc_operation

DESCRIPTION
  This function check the HS-SCCH or HS-SICH channelisation code.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if HSDPA channel is active.
  FALSE if HSDPA channel is inactive.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdshscfg_save_hsdpa_rrc_operation(tdsl1_hsdpa_setup_ops_enum_type channel_op);

/*===========================================================================
FUNCTION tdshscfg_get_hsdpa_rrc_operation()

DESCRIPTION
  This function returns the last saved RRC HS op

DEPENDENCIES
  None

RETURN VALUE
  Last saved RRC HS op

SIDE EFFECTS
  None
===========================================================================*/
extern tdsl1_hsdpa_setup_ops_enum_type tdshscfg_get_hsdpa_rrc_operation();

/*===========================================================================
FUNCTION tdshscfg_get_cur_state

DESCRIPTION
  This function returns the current state of HSDPA channel in L1.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if HSDPA channel is active.
  FALSE if HSDPA channel is inactive.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdsl1_get_hs_cur_state(void);

/*===========================================================================
FUNCTION tdshscfg_get_cell_db_idx

DESCRIPTION
  This function returns the HS cell DB index if HS is active else returns
  NO_CELL

DEPENDENCIES
  None

RETURN VALUE
  HS cell DB index if HS active
  NO_CELL otherwise

SIDE EFFECTS
  None
===========================================================================*/

extern  uint8 tdshscfg_get_cell_db_idx(void);


#ifdef FEATURE_TDSCDMA_64QAM
/*===========================================================================
FUNCTION tdshscfg_get_64qam_state

DESCRIPTION
  This function returns the current state of mimo configuration.

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdshscfg_get_64qam_state(void);
#endif

/*===========================================================================
FUNCTION tdshscfg_get_num_harq_configured

DESCRIPTION
  This function returns the valid harq ids.

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint8 tdshscfg_get_num_harq_configured(void);

/*===========================================================================
FUNCTION tdshscfg_get_ordered_psc

DESCRIPTION
  This function returns the HS PSC after querying from ordered and current
  configuration. It returns HS current PSC if HS is getting stopped.


DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdshscfg_get_ordered_psc(
  /* TDSL1_CPHY_SETUP_REQ command pointer */
  tdsl1_setup_cmd_type *setup);

/*===========================================================================
FUNCTION tdshscfg_get_call_info

DESCRIPTION
  This function maps info table index to config db index and then queries HS
  call type information. This will be used to do dec status logging.

DEPENDENCIES
  Assumed to be called after HS has indicated to start logging to DECHS

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdshscfg_get_call_info(
  /* Pointer to HS call type */
  tdshscfg_dec_call_info_struct_type *call_info, 
  /* HS info index */
  uint8 info_table_idx);


/*===========================================================================
FUNCTION tdshscfg_get_channel_active_state

DESCRIPTION
  global vaiable access funtion

DEPENDENCIES
  None

RETURN VALUE
  HS cell DB index if HS active
  NO_CELL otherwise

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 tdshscfg_get_channel_active_state(void);

/*===========================================================================
FUNCTION tdshscfg_get_a2_pc_completed

DESCRIPTION
  get a2 power collapse status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 tdshscfg_get_a2_pc_completed(void);

/*===========================================================================
FUNCTION tdshscfg_get_cur_ldb_info_idx

DESCRIPTION
  global vaiable access funtion

DEPENDENCIES
  None

RETURN VALUE
  current Local config DB index

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 tdshscfg_get_cur_ldb_info_idx(void);

/*===========================================================================
FUNCTION tdshscfg_get_ord_ldb_info_idx

DESCRIPTION
  global vaiable access funtion

DEPENDENCIES
  None

RETURN VALUE
  ord Local config DB index

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 tdshscfg_get_ord_ldb_info_idx(void);

/*===========================================================================
FUNCTION tdshscfg_get_cur_dsch_cfg_incl

DESCRIPTION
  global vaiable access funtion

DEPENDENCIES
  None

RETURN VALUE
  in current HSDPA config, is HS-DSCH reconfigured or not

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 tdshscfg_get_cur_dsch_cfg_incl(void);

#ifdef TEST_FRAMEWORK
#error code not present
#endif
/*===========================================================================
FUNCTION tdshscfg_set_mac_cfg_indication

DESCRIPTION
  global vaiable access funtion

DEPENDENCIES
  None

RETURN VALUE
 
SIDE EFFECTS
  None
===========================================================================*/

extern void tdshscfg_set_mac_cfg_indication(boolean indct);

/*===========================================================================
FUNCTION tdshscfg_get_mac_cfg_indication

DESCRIPTION
  global vaiable access funtion

DEPENDENCIES
  None

RETURN VALUE
  MAC configuation indication

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdshscfg_get_mac_cfg_indication(void);

/*===========================================================================

FUNCTION tdshscfg_idle_init

DESCRIPTION
  This function initialize HS configuration and maintainance related
  variables when L1 goes to IDLE state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdshscfg_idle_init(void);

/*===========================================================================
FUNCTION tdshscfg_init

DESCRIPTION
  This function initialize HS configuration and maintainance related
  variables. Following are the initializations done in this function.
  - Init HS active state to FALSE
  - Set HSDPA information table availability to TRUE
  - Clear HSDPA information shadow

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdshscfg_init(void);

/*===========================================================================
FUNCTION tdshscfg_validate_config

DESCRIPTION
  This function validate HS channel configuration.

DEPENDENCIES
  This function is called only when HS operation type is not NOOP

RETURN VALUE
  TRUE or FALSE based on validation result

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdshscfg_validate_config(
  /* TDSL1_CPHY_SETUP_REQ command pointer */
  tdsl1_setup_cmd_type *setup);


/*===========================================================================
FUNCTION tdshscfg_channel_config_cmd

DESCRIPTION
  This function configures HS channel

  HSDPA information table update is required for following cases:
  - HSDPA action type is START
  - HSDPA action type is start and
   * There is any change in HSDPA related parameter
     HSDPA information table gets its information from following
     - HS-DSCH information
     - HS-SCCH information
     - HS-DPCCH feedback information
   * There is change in HSDPA serving cell. Repointing requires
     HS combiner config and HS combiner config DB index is same
     as HSDPA info index.
     Note that HS information table update function keeps its shadow
     copy and is capable to updating only required information field
     thyat have changed for this configuration.

  For HSDPA ops type stop there is no HSDPA information table update
  required. HS combiner config always use DB table other than current
  table without touching HSDPA information table index

  HS combiner config update is required for following cases:
  - HSDPA action type START and STOP always need HSDPA combiner config
  - HSDPA information table need update at reconfig
  - Change in HSDPA serving cell at reconfig
  - Change in serving cell timing ??????????

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdshscfg_channel_config_cmd(
  /* HSDPA configuration operation type */
  tdsl1_hsdpa_setup_ops_enum_type hs_ops,
  /* HSDPA channel configuration information structure pointer */
  tdsl1_hs_info_struct_type *hs_info,
  /* Function to be called when HS configuration has been completed */
  TDSHSCFG_CHANNEL_CFG_DONE_POST_CB_FUNC_TYPE *cfg_done_cb_func);

/*===========================================================================
FUNCTION tdshscfg_generate_serv_cell_change_evt

DESCRIPTION
  This function gets the UARCFN (freq), PSC, TPC index and H-RNTI for the ordered
  HS cell and then compares it with the current values. If the PSC or the 
  UARCFN of the HS serving cell has changed, then it generates an event and 
  posts it to the DM.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If HS serving cell changed, then it generates an EVENT_SERVING_CELL_CHANGE
  event, else it updates the current UARCFN, TPC index and H-RNTI variables.
===========================================================================*/

extern void tdshscfg_generate_serv_cell_change_evt(
  /* HSDPA configuration operation type */
  tdsl1_hsdpa_setup_ops_enum_type hs_ops);

 
/*===========================================================================
FUNCTION tdshscfg_dl_post_config_handler

DESCRIPTION
  This function is HS DL combiner config done call back function. It posts
  frame boundary event using DL combiner event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdshscfg_dl_post_config_handler( void );

/*===========================================================================
FUNCTION tdshscfg_dl_comb_chan_event_handler

DESCRIPTION
  This function is HS DL combiner event handler used by DL timline module.
  It disables event and calls HS DL combiner event that is configured.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

//extern void tdshscfg_dl_comb_chan_event_handler(void);

/*===========================================================================
FUNCTION tdshscfg_dl_switch_config

DESCRIPTION
  This function is HS combiner channel frame boundary event call back function.
  It is called when HS combiner chanel frame boundary event fires DPCH frame
  boundary. It switches ordered config to current config. Old config is marked
  as invalid and ordered config index is set to invalid too.

  It there is no pending UL ordered config then traige is released if it was
  delayed by HS config module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/


extern void tdshscfg_dl_switch_config(uint16 arg1, uint32 arg2);

/*===========================================================================
FUNCTION tdshscfg_chan_cfg_done_send_cmd

DESCRIPTION
  This function sends local command to L1 manager for HS channel config
  command done.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdshscfg_chan_cfg_done_send_cmd(boolean status);

/*===========================================================================
FUNCTION tdshscfg_deactivate_hsdpa_chan

DESCRIPTION
  This function deactivates HSDPA channels. It is intended to be called at
  transition out of DCH state or when HS serving RL is dropped without
  repointing it before. It registers deactivate compete post call back function
  with HS channel configuration function and it it is not NULL then it will
  be called to mark end of deactivation process.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdshscfg_deactivate_hsdpa_chan(
  /* Function to be called when HS channel deactvivation has been completed */
  TDSHSCFG_CHANNEL_CFG_DONE_POST_CB_FUNC_TYPE *cfg_done_cb_func);

#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED

/*===========================================================================
FUNCTION tdshscfg_handle_hs_cfg_log_pkt

DESCRIPTION
  This function handles HS configuration log packet submission in task
  context.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdshscfg_handle_hs_cfg_log_pkt(void);

#endif /* FEATURE_TDSCDMA_L1_LOGGING_ENABLED */

/*===========================================================================
FUNCTION tdshscfg_get_cur_dl_cfg_ptr

DESCRIPTION
  This function returns pointer to current HS DL/UL configuration information
  structure pointer.
  If current information index is not valid, it returns NULL. Possible only
  if HSDPA has not yet been activated since dedicated mode has started

DEPENDENCIES
  None

RETURN VALUE
  Pointer to structure type tdshscfg_dl_channel_cfg_db_struct_type or 
  tdshscfg_ul_channel_cfg_db_struct_type or NULL

SIDE EFFECTS
  None
===========================================================================*/

extern tdshscfg_dl_channel_cfg_db_struct_type* tdshscfg_get_cur_dl_cfg_ptr(void);

/*===========================================================================
FUNCTION tdshscfg_send_submit_log_cmd

DESCRIPTION
  This function send HS submit log packet local command to L1M.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdshscfg_send_submit_log_cmd(void);

/*===========================================================================
FUNCTION tdshscfg_do_log_submit

DESCRIPTION
  This function is called as result of submit HS log local command. It calls
  all possible candidates in HS that do accumulated logging.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdshscfg_do_log_submit(void);


/*===========================================================================
FUNCTION tdshscfg_get_cctrch_id_for_cfg_db_idx

DESCRIPTION
  This function returns the CCTrCh id in the DL cfg DB.

DEPENDENCIES
  None

RETURN VALUE
  CCTrCh ID.

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 tdshscfg_get_cctrch_id_for_cfg_db_idx(uint8 hs_cfg_db_idx);

/*===========================================================================
FUNCTION tdshscfg_chan_deactivate_done

DESCRIPTION
  This function handles the post HS chan deactivation and posts the local 
  command to proceed with the DL DCH cleanup
  initiate drop of the second carrier.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdshscfg_chan_deactivate_done(boolean status);

/*===========================================================================
FUNCTION tds_hsdpa_module_drop()

DESCRIPTION
  This function drops all channels for HSDPA.

DEPENDENCIES
  None

RETURN VALUE
  clean-up done or not

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tds_hsdpa_module_drop(void);

/*===========================================================================
FUNCTION tdshscfg_a2_pc_operation()

DESCRIPTION
  Delay mac hs reset by 5 ms

DEPENDENCIES
  None

RETURN VALUE
  clean-up done or not

SIDE EFFECTS
  None
===========================================================================*/
void tdshscfg_a2_pc_operation(boolean channel_active);

#ifdef FEATURE_TDSCDMA_MCPM_POWER_REPORTING
/*===========================================================================
FUNCTION tdshscfg_get_num_of_active_hs_channel

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 tdshscfg_get_num_of_active_hs_channel(void);
#endif


/*===========================================================================
FUNCTION tdshscfg_is_suspend_during_lta()

DESCRIPTION
  This function returns if HSDPA is suspended due to LTA.

DEPENDENCIES
  None

RETURN VALUE
  clean-up done or not

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdshscfg_is_suspend_during_lta(void);

/*===========================================================================
FUNCTION tdshscfg_clear_suspend_during_lta()

DESCRIPTION
  This function clears the flag that indicates HSDPA is suspended due to LTA.

DEPENDENCIES
  None

RETURN VALUE
  clean-up done or not

SIDE EFFECTS
  None
===========================================================================*/
extern void tdshscfg_clear_suspend_during_lta(void);

#endif /* TDSHSCFG_H */
