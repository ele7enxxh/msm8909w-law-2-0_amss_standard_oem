#ifndef MCALWCDMA_DL_H
#define MCALWCDMA_DL_H

/*============================================================================*/
/** @file
 * This module has definition and declaration related to MCAL UMTS downlink
 * channel configurations (add, reconfigure, drop) module APIs
 */
/*============================================================================*/

/*============================================================================
Copyright (c) 2007 - 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/mcalwcdma_dl.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
02/24/15    ap      Initial check-in for WL1 QSH framework 
>>>> ORIGINAL //components/rel/wcdma.mpss/4.2.0/l1/offline/inc/mcalwcdma_dl.h#9
==== THEIRS //components/rel/wcdma.mpss/4.2.0/l1/offline/inc/mcalwcdma_dl.h#10
10/13/14    pr      Deprecate RX_GEN_DEC_HDR_READ_EVENT and add a new event for comb chan cfg.
==== YOURS //skkota.wcdma.mpss.4.2.0.CR675524_JO_RLP/modem_proc/wcdma/l1/offline/inc/mcalwcdma_dl.h
06/20/14    skk     FR19218: ReacqLessPICH(RLP) porting for JO.
<<<<
09/02/14    sr      Changes to retrieve correct intf idx for a channel
07/03/14    sks     Changes for FCDC
06/23/14    sad     Moving DC-HSUPA specific code under FEATURE_WCDMA_DC_HSUPA flag
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
09/14/13    pr      VSTMR Changes for BOLT
11/05/13    hk      Added the enum for CPICH type
10/14/13    pr/sr   Channel config optimizations
05/21/13    sad     Added DC HSUPA code changes under FEATURE_WCDMA_DC_HSUPA 
05/23/13    geg     WSW decoupling (except for component of QCHAT)
04/11/13    pr      Change the mechanism to allocate and deallocate the interface
                    buffer index.
04/01/13    hk      Mainlined FEATURE_WCDMA_EFDPCH
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/22/12    pr      STMR changes for Dime
07/05/12    pv      Offline PICH changes.
06/08/12    gv      Feature HS RACH Initial code check in
06/07/12    vs      MBMS feature cleanup.
02/15/12    hk      Timeline alignment changes for HHO & W2G Failure recovery
02/12/12    hk      Nikel feature cleanup
02/03/12    zr      Fixing feature trimming issue
01/26/12    vs      Nikel feature cleanup.
01/20/12    zr      Mainlined FEATURE_MCAL_WCDMA, FEATURE_8220_WCDMA_SW, and 
                    FEATURE_WCDMA_GENESIS_SW
05/31/11    geg     Changes for RRC and L2 api files clean-up wcdma dependencies
01/11/11    stk     included rex.h to compile for nikel
10/08/10    vb      Added support to make fast pich configuration for pich 
                    suspend and resume operations
04/06/10    ks      Added Enhanced FDPCH support
09/22/09    sup     Removed log code assignments and placed them in 
                    log_codes_wcdma_int_v.h
04/20/09    yh      Fixed compilation warning.
12/29/08    hk      For certain kind of channel ops, compute the CFN
                    using combiner timing
10/27/08    hk      Lint Fixes
10/13/08    vsr     Added interface function to query combiner and phychan 
                    pending bitmask
10/03/08    hk      Bringing in 7k Mailine fixes
06/17/08    vsr     Removed references to HS channels
06/16/08    vsr     Offline modem MCAL DL code cleanup
06/10/08    ms      Added PACKED_POST attribute for structures to make it
                    compatible for GNU compiler.
05/28/08    vsr     Fixing TLM related merge issues
01/25/08    vsr     Checking in MCAL files onto mainline
01/21/08    vsr     Added structure member and macro needed for API for wcb 
                    size calculation
11/07/07    ks      FDPCH related Flag added in structure
                    mcalwcdma_dl_phch_cfg_parms_struct_type
07/11/07    vsr     Updated mDSP sync command related structures and logic 
                    for Offline modem interface.
07/05/07    vsr     Merged data structures and modified combiner evt mapping 
                    functions to support MCAL-EDL
06/08/07    vsr     MCAL_DL code cleanup changes
05/23/07    vsr     Added support for combiner log packet

===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "wcdma_variation.h"
#include "comdef.h"
#include "log.h"
#include "log_codes.h"
#include "mcalwcdma_demod.h"
#include "tlm.h"
#include "log_codes_wcdma_int.h"
#include "rex.h"
#include "l1rrcif.h"

#if defined FEATURE_QSH_DUMP || defined FEATURE_QSH_EVENT_METRIC
#include "wl1_qsh_ext.h"
#endif /* defined FEATURE_QSH_DUMP || defined FEATURE_QSH_EVENT_METRIC*/
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/** Maximum size of RL set */
#define MCALWCDMA_DEMOD_MAX_RL_SET                   6

/** Total number of R99/MBMS channels that can be setup */
/** Also includes PICH, MICH and AICH which do not need FW combiners */
#define MCALWCDMA_DL_NUM_MAX_SW_COMBINERS          (WFW_DEMOD_NUM_MAX_COMBINERS + 2)

#define MCALWCDMA_DL_NUM_MAX_PHCHS WFW_DEMOD_NUM_MAX_PHCHS

/* Number of combiners from FW point of view - Only includes DPCH, SCCPCH and PCCPCH */
/* Combiners are only a concept - FW time multiplexes the use of the same HW blocks */
#define MCALWCDMA_DL_NUM_MAX_FW_COMBINERS WFW_DEMOD_NUM_MAX_COMBINERS

/** indicate invalid DL channel Id */
#define MCALWCDMA_DL_INVALID_CHAN_ID 0xFF

/* Maximum number fo combiners that can use MCAL evt services */
#define MCALWCDMA_DL_MAX_NUM_COMB_CHAN_EVENT 5

/* definition of Invalid EVT id */
#define MCALWCDMA_DL_INVALID_COMB_CHAN_EVT_ID NUM_RX_GEN_TL_EVENTS


#define MCALWCDMA_DL_PHYCHAN_CFG_CMD_IN_CM_BMSK 0x80

#define MCALWCDMA_DL_WCB_SIZE_PER_BIT 2

#define MCALWCDMA_DL_AICH_BITS_PER_IND 16

/*Adding macro for buffer index to be exclusively used for DC HSDPA */
#define MCALWCDMA_DL_HS_DC_INTERFACE_BUF_IDX 26

/*--------------------------------------------------------------------------
Macro for checking if the RL phch interface buffer ID is valid
--------------------------------------------------------------------------*/
#define MCALWCDMA_DL_IS_RL_PHCH_INTERFACE_BUF_IDX_INVALID(idx) \
  (idx >= MCALWCDMA_DL_NUM_MAX_PHCHS)

/*--------------------------------------------------------------------------
Macro for checking if the Cell db idx is valid
--------------------------------------------------------------------------*/
#define MCALWCDMA_DL_IS_CELL_DB_IDX_INVALID(cell_db_idx)    \
    (cell_db_idx >= MCALWCDMA_MAX_NUM_CELLS)

/*--------------------------------------------------------------------------
  Macro for picking the STMR interrupt line on which the config events will be setup
  --------------------------------------------------------------------------*/
#define MCALWCDMA_DL_CFG_EVENT_INTERRUPT_LINE MCALWCDMA_EVT_RX_TL_INTR
#define MCALWCDMA_DL_FIRST_CFG_EVENT RX_GEN_PHYCHAN_HWCH0_EVENT

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
typedef WfwDemodPhyChanInfoStruct mcalwcdma_dl_comb_cfg_cmd_struct_type;
typedef WfwDemodPhyChanInfoStruct mcalwcdma_dl_phch_cfg_cmd_struct_type;
typedef WfwDemodR99DataControlChannelInfoStruct mcalwcdma_dl_R99_data_ctrl_ch_info_struct_type;
typedef WfwDemodIndChannelInfoStruct mcalwcdma_dl_ind_ch_info_struct_type;

/* Call back function type declaration */
/* ----------------------------------- */

/** event handler callback functionm type declaration */
typedef void (*MCALWCDMA_DL_EVENT_CB_TYPE)(
  /** Event Id of this callback */
  uint8);


/* Enum type declaration */
/* --------------------- */
typedef enum {
  MCALWCDMA_DL_USE_PRI_CPICH = WFW_DEMOD_PHCH_PHASE_REF_PRIMARY,
  MCALWCDMA_DL_DO_NOT_USE_PRI_CPICH,
  MCALWCDMA_DL_INVALID_PHASE_REF = WFW_DEMOD_PHCH_MAX_NUM_PHASE_REF
} mcalwcdma_dl_ch_est_cpich_usage_enum_type;

/** This enumerates the various downlink physical channel reference id's */
typedef enum
{
  /** Primary Common Control Physical Channel */
  MCALWCDMA_DL_PHYCHAN_PCCPCH,
  /** Secondary Common Control Physical Channel */
  MCALWCDMA_DL_PHYCHAN_SCCPCH,
  /** Dedicated Physical Channel */
  MCALWCDMA_DL_PHYCHAN_DPCH,
  MCALWCDMA_DL_PHYCHAN_HS_RACH_FDPCH,
  /** Page Indicator Channel */
  MCALWCDMA_DL_PHYCHAN_PICH,
  /** Acquisition Indicator Channel */
  MCALWCDMA_DL_PHYCHAN_AICH,
/** Acquisition Indicator Channel for HS RACH */
  MCALWCDMA_DL_PHYCHAN_HS_RACH_AICH,
  /** Fractional DPCH Channel */
  MCALWCDMA_DL_PHYCHAN_FDPCH,
  /** Number of physical channel types */
#ifdef FEATURE_WCDMA_DC_HSUPA 
  /**Fractional DPCH channel for secondary**/
  MCALWCDMA_DL_PHYCHAN_SEC_FDPCH,
#endif
  MCALWCDMA_NUM_DL_PHYCHAN
} mcalwcdma_dl_phychan_enum_type;

/** This enumerates the various action types that L1 can send to mDSP in a cfg cmd */
typedef enum
{
  MCALWCDMA_DL_SYNC_CMD_DISABLE = WFW_DEMOD_PHCH_CFG_DISABLE,
  MCALWCDMA_DL_SYNC_CMD_ENABLE = WFW_DEMOD_PHCH_CFG_ENABLE,
  MCALWCDMA_DL_SYNC_CMD_RECFG_NO_TIMING_SLAM = WFW_DEMOD_PHCH_CFG_RECFG_NO_TIMING_CHANGE,
  MCALWCDMA_DL_SYNC_CMD_RECFG_TIMING_SLAM = WFW_DEMOD_PHCH_CFG_RECFG_TIMING_SLAM,
  MCALWCDMA_DL_SYNC_CMD_INVALID_ACTION = WFW_DEMOD_PHCH_NUM_CFG
} mcalwcdma_dl_sync_cmd_action_type;

/* CFN calculation types
  a.Calculate CFN from cell timing for all channel additions (MCALWCDMA_DL_CALC_CFN_FROM_CELL)
  b.Calculate CFN from combiner timing for channel drop and recfg and 
    during resuming WCDMA (MCALWCDMA_DL_CALC_CFN_FROM_COMB)
  c.Calculate CFN from cfn_wrc_diff for TM HHO and 
    Reverse TI HHO (MCALWCDMA_DL_CALC_CFN_FROM_WRC_CFN_DIFF)*/
typedef enum {
  MCALWCDMA_DL_CALC_CFN_UNKNOWN_TYPE,
  MCALWCDMA_DL_CALC_CFN_FROM_CELL,
  MCALWCDMA_DL_CALC_CFN_FROM_COMB,
  MCALWCDMA_DL_CALC_CFN_FROM_WRC_CFN_DIFF
}mcalwcdma_dl_calc_cfn_enum_type;

/* Structure type declaration */
/* -------------------------- */

/** Structure for RL phychan parameters (These were per finger before Offline modem)*/
typedef struct {
  /** Secondary Scrambling Code. If not enabled set to 0. Valid Range is [1,15] */
  uint8 sec_scr_code;
  /** OVSF code.  Valid range [0,511] */
  uint16 ovsf;
  /** Transmit Diversity Mode */
  mcalwcdma_demod_tx_div_mode_enum_type tx_div_mode;
  /** Alternate Scrambling Code select (left, right, or no change) */
  mcalwcdma_demod_scr_code_change_type_enum_type alt_scr_code_mode;
  /** Fractional DPCH enable */
  boolean fdpch;
  /* fdpch slot format */
  uint8 efdpch_slot_format;

  uint8 hw_tpc_idx;

  /** Action required by mDSP */
  mcalwcdma_dl_sync_cmd_action_type action;
    
  /** Power Control index for DPCH */ 
  uint8 tpc_index;

  /** Cell Index */
  uint8 cell_index;

  /** Specifies whether primary or secondary CPICH is to be used for phase reference */
  mcalwcdma_dl_ch_est_cpich_usage_enum_type phase_ref;

  /** PICH bits per indication */
  uint8 bits_per_ind;

  /** mDSP Interface Buffer Index */
  uint8 interface_buf_id;

  /** Transmit time offset from PCCPCH in units of 256 chips.
   *  Range [0-149] */
  uint16 trans_time_offset;

  /** Indication to FW to setup the channel in dormant mode
   *  or active mode */
  boolean waitOnScchOrder;
#ifdef FEATURE_WCDMA_REACQLESS_PICH
  /*To enable ReacqLessPICH timeline in FW*/
  boolean rlp_enabled;
#endif
} mcalwcdma_dl_rl_phychan_parms_struct_type;

/** This structure is used to contain the parameters for the downlink
  * physical channel config command to the mDSP.  Also included are
  * masks for the valid bits for each field */
typedef struct
{
  /** The type of downlink physical channel */
  mcalwcdma_dl_phychan_enum_type phychan_type;

  /* Flag to indicate if channel is FDPCH*/
  boolean fdpch_enabled;

  /** The slot format, for this physical channel */
  uint8 slot_format;

  /** The table index for the CCTrCH that this physical channel will carry.
    * Set to 0 if no CCTrCH applies, i.e. for PICH, AICH */
  uint8 cctrch_id;

  /* The WCB requirement for this physical channel*/
  uint32 wcb_size;

  /** Action required by mDSP */
  mcalwcdma_dl_sync_cmd_action_type action;

  /* Referece Cell Index */
  uint16 ref_cell_index;

  /* Active cell bitmask */
  uint16 active_cell_bmsk;

  /** The frame offset of this channel wrt to the PCCPCH frame timing, in frame
    * numbers (Range: 0-4095)*/
  uint8 frame_offset;

  /** The transmit time offset relative to PCCPCH, in ticks resolution
    * (Range: 0-149)*/
  uint32 trans_time_offset;

  /** Bit mask for APT, reslam, ASAP bits
    *         7    6     5     4     3     2     1     0
    *     +-----+-----+-----+-----+--------------+-----+-----+-----+
    *     | RESERVED                |AUTO SCCPCH | APT |RSLM |ASAP
    *     |
    *     +-----+-----+-----+-----+--------------+-----+-----+-----+
    */
  uint8 arae_bmsk;

  /* PSC on which the channel is going to be configured */
  uint16 psc;

  /* frequency on which the channel is going to be configured */
  uint16 freq;

  /* reserved combiner id, should be set to MCALWCDMA_DL_INVALID_CHAN_ID 
    * if no combiner is reserved for the channel setup, in such cases MCAL DL module
    * will choose the available ones */
  uint8 rsv_comb_id;

  /*Indicate to MCAL-DL whether CFN should be calculated from cell's timing &
    Frame Offset or from any combiner that tracks the channel's timing or
    from cfn_wrc_diff stored during DPCH drop of FW TI/TM HHO*/
  mcalwcdma_dl_calc_cfn_enum_type cfn_calc_type;

  /*Structure to hold the CFN_WRC difference calculated during DPCH drop of FW TI/TM HHO*/
  tlm_cfn_struct_type cfn_wrc_diff;

  boolean reslam_needed;
} mcalwcdma_dl_combiner_cfg_parms_struct_type;

/** This structure is used to contain the parameters for the downlink
  * combiner and physical channel config command to the mDSP */
typedef struct
{
  /** Pointer to combiner channel configuration parameters */
  mcalwcdma_dl_combiner_cfg_parms_struct_type* comb_chan_cfg_ptr;

  /** Number of Radio links that are linked to the combiner */
  uint8 phychan_set_cnt;

  /** Pointer to array of physical channel configuration parameters */
  mcalwcdma_dl_rl_phychan_parms_struct_type* phychan_set_ptr[MCALWCDMA_DEMOD_MAX_RL_SET];
} mcalwcdma_dl_phch_cfg_parms_struct_type;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
extern rex_crit_sect_type mcal_dl_rsv_comb_crit_sect;

#define MCAL_DL_RSV_COMB_LOCK()  REX_ISR_LOCK(&mcal_dl_rsv_comb_crit_sect)
#define MCAL_DL_RSV_COMB_UNLOCK()  REX_ISR_UNLOCK(&mcal_dl_rsv_comb_crit_sect)

/*--------------------------------------------------------------------------
             LOG_PACKET: WCDMA_COMBINER_TIMING_LOG_PACKET

This struct is used for storing and logging combiner timing information samples.
There is room for 20 individual samples.
--------------------------------------------------------------------------*/

#ifdef WCDMA_COMBINER_TIMING_LOG_PACKET
#define WCDMA_COMBINER_TIMING_LOG_PACKET_NUM_SAM 20
#define WCDMA_COMBINER_TIMING_LOG_PACKET_LOGGING_FREQ 4  /* Once in so many frames */
#endif

typedef PACKED struct PACKED_POST
{
  /* Reference adjusted count */
  uint32 ref_cnt_adj;

  /* TCXO adjusted accumulation */
  int16 tcxo_adj_accum;
  
  /* Frequency Error */
  int16 freq_common;

  /* Tx System Counter */
  uint32 tx_sys_cnt;
  
  /* Frame Reference Counter */
  uint32 frame_ref_cnt;

  /* Combiner count for all hardware channels */
  /* 0-7 For Combiners0-7, and 8 for HS*/
  uint32 comb_cnt[9];

}wcdma_combiner_timing_log_struct_type;

LOG_RECORD_DEFINE(WCDMA_COMBINER_TIMING_LOG_PACKET)
  uint8 num_sample;
  uint8 start_cfn;
  uint16 comb_logging_freq;
  wcdma_combiner_timing_log_struct_type wcdma_combiner_timing[WCDMA_COMBINER_TIMING_LOG_PACKET_NUM_SAM];
LOG_RECORD_END


/* =======================================================================
**                        Function Declarations
** ======================================================================= */
/*============================================================================*/
/**
  * This initializes all the downlink physical channel stuff
  */
/*============================================================================*/
extern void mcalwcdma_dl_init(void);

/*============================================================================*/
/**
 * Function for setting up a physical layer channel based on passed arguments.
 * Phycical channel is not setup immediately. Separate indication is sent in
 * once physical channel is setup and can be assumed functions towards its
 * frame boundary. In case this function is not successful no such indication
 * is sent.
 *
 * @return DL channel Id that can be used to reconfigure or tear down channel.
 * In case of failure invalid H/W channel Id is return and no channel cfg done
 * indication is sent.
 *
 * @see MCALWCDMA_DL_INVALID_HWCH_ID
 * @see mcalwcdma_dl_phychan_cfg_done_ind
 */
/*============================================================================*/

extern uint8 mcalwcdma_dl_phychan_add(
  /** Parameters for setting up the downlink physical channel */
  mcalwcdma_dl_phch_cfg_parms_struct_type *phch_cfg_parms_ptr);

/*============================================================================*/
/**
 * Function for reconfiguring a physical layer channel based on updated
 * arguments. This function is to be used for channel that is already
 * configured using mcalwcdma_dl_phychan_add function. ust like add function
 * recfg os also not immediately. Separate indication is sent in, once physical
 * channel is setup and can be assumed functions towards its frame boundary.
 *
 * @return Boolean indicating that channel can be configured properly or not.
 * In case of failure no channel cfg done indication is sent
 *
 * @see mcalwcdma_dl_phychan_cfg_done_ind
 */
/*============================================================================*/

extern boolean mcalwcdma_dl_phychan_reconfigure(
  /** Combiner Id for the channel to be reconfigured */
  uint8 comb_id,
  /** Parameters for reconfigure the downlink physical channel */
  mcalwcdma_dl_phch_cfg_parms_struct_type *phch_cfg_parms_ptr);

/*============================================================================*/
/**
 * Function for dropping an existing physical layer channel. It sends indication
 * of channel cfg done once tear down is assumed to be complete.
 *
 * @return Boolean indicating that channel can be dropped properly or not.
 * In case of failure no channel cfg done indication is sent
 *
 * @see mcalwcdma_dl_phychan_cfg_done_ind
 */
/*============================================================================*/

extern boolean mcalwcdma_dl_phychan_drop(
  /** Combiner Id for the channel to be dropped */
  uint8 comb_id,
  /** Parameters for droping down the downlink physical channel */ 
  mcalwcdma_dl_phch_cfg_parms_struct_type* phch_cfg_parms_ptr);

/*============================================================================*/
/**
  * Indicator function when all channel configuration is over. It is at frame
  * boundry of channel getting added, reconfigured or dropped
  */
/*============================================================================*/

void mcalwcdma_dl_phychan_cfg_done_ind(
  /** Combiner Id for the channel to be configured */
  uint8 comb_id,
  
  /** cfn at which channel configuration is going to be active */
  uint8 ccfn);

void mcalwcdma_dl_phychan_slam_new_stmr_counter(
  /** Combiner Id for the channel to be configured */
  uint8 comb_id,
  
  /** current cfn */
  tlm_tfn_struct_type *wrc_cfn_diff);

extern uint16 mcalwcdma_dl_cctrch_curr_time(uint8 cctrch_idx);

/*============================================================================*/
/**
 * This function finds free combiner channel and reserves it.
 *
 * @return DL channel Id that is reserved for the channel. If no combiner channel
 * is available it will return invalid ch id
 *
 * @see MCALWCDMA_DL_INVALID_HWCH_ID
 */
/*============================================================================*/
uint8 mcalwcdma_dl_get_free_and_rsv_comb_ch( 
  /** for which physical channel type the comb chan reservation is needed */  
  mcalwcdma_dl_phychan_enum_type phch,

  /** slot format index, which helps in making decision for reservation */
  uint8 slot_format_index);

/*============================================================================*/
/**
 * This function sets rsv status the requested comb id.
 *
 * @return TRUE if success else FALSE
 *
 */
/*============================================================================*/
boolean mcalwcdma_dl_set_comb_id_rsv_status( 
  /** for which combiner id user wanted to change status */  
  uint8 comb_id,
  /** for which physical channel type the comb chan reservation is needed */  
  mcalwcdma_dl_phychan_enum_type phch,
  /** reserve status */  
  boolean rsv_status
);

/*============================================================================*/
/**
 * This function allocates new evt id for the given comb chan from the comb chan evt pool.
 * @return event id
 */
/*============================================================================*/
uint8 mcalwcdma_dl_allocate_evt_id_for_comb_chan_config(
  /** for which combiner id event to be allocated */  
  uint8 comb_id
);

/*============================================================================*/
/**
 * This function de allocates the given evt id
 * @return None
 */
/*============================================================================*/
void mcalwcdma_dl_deallocate_evt_id_for_comb_chan_config(
  /** which event to be deallocated */  
  uint8 evt_id
);

/*============================================================================*/
/**
 * This function modifies the event call back handler
 * @return  TRUE if successful modification
 */
/*============================================================================*/
boolean mcalwcdma_dl_modify_evt_call_back_handler(
  /** evt id on which the call back to be updated */
  uint8 evt_id, 
  /** evt handler */
  MCALWCDMA_DL_EVENT_CB_TYPE evt_cb_func
);

/*============================================================================*/
/**
 * This function handles the combiner channel event
 * @return  None
 */
/*============================================================================*/
void mcalwcdma_dl_phychan_comb_chan_event_handler(uint8 evt);

/*===========================================================================
FUNCTION        mcalwcdma_dl_get_wrc_cfn_diff_for_combiner

DESCRIPTION     This function fills the wrc_cfn_diff for given hwch.

DEPENDENCIES    None

RETURN VALUE    status

SIDE EFFECTS    None
===========================================================================*/
void mcalwcdma_dl_get_wrc_cfn_diff_for_combiner(uint8 hwch, tlm_tfn_struct_type* wrc_cfn_diff, tlm_cfn_struct_type* ch_config_cfn);

/*===========================================================================*/
/**
* This function is a post callback from a command sent to mDSP.
*/
/*===========================================================================*/
extern void mdsp_sync_send_comb_and_phychan_cfg_cmd_post_cb(uint16 status);

/*===========================================================================*/
/**
* This function allocates the RL phychan interface buffer index and returns it
* @return phch config cmd idx
*/
/*===========================================================================*/
extern uint8 mcalwcdma_dl_allocate_interface_buf(void);

/*===========================================================================*/
/**
* This function deallocates the RL phychan interface buffer index
* @return TRUE if deallocation was successful.
*/
/*===========================================================================*/
extern boolean mcalwcdma_dl_deallocate_interface_buf(uint8 idx);

/*===========================================================================
FUNCTION        MCALWCDMA_DL_GET_INTERFACE_BUFFER_ID

FILE            mcalwcdma_dl.c

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern uint8 mcalwcdma_dl_get_interface_buffer_id(uint8 channel_type, uint8 cell_idx, uint8 cctrch_idx);

/*===========================================================================
FUNCTION        MCALWCDMA_DL_GET_INTERFACE_BUFFER_PTR

FILE            mcalwcdma_dl.c

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern mcalwcdma_dl_phch_cfg_cmd_struct_type* mcalwcdma_dl_get_interface_buffer_ptr(uint8 buffer_id);

/*===========================================================================
FUNCTION        MCALWCDMA_DL_QUERY_PHCH_COMB_PENDING_STATUS

FILE            mcalwcdma_dl.c

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void mcalwcdma_dl_query_phch_comb_pending_status(uint16 *pending_comb_info, uint32 *pending_phychan_info);

/*===========================================================================
FUNCTION         mcalwcdma_dl_log_submit

DESCRIPTION      
  Callback function that mcal manager calls when it is signaled that there is
  a packet to be logged.

DEPENDENCIES
  None
  
RETURN VALUE 
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void mcalwcdma_dl_log_submit(void);

/*============================================================================*/
/** This function starts the process for PICH physical channel configuration  */
/*============================================================================*/
extern boolean mcalwcdma_dl_rl_pich_phychan_configure(uint8 db_idx,
  mcalwcdma_dl_rl_phychan_parms_struct_type *rl_parms_ptr,
  mcalwcdma_dl_phch_cfg_parms_struct_type *phch_cfg_parms_ptr);

/*===========================================================================*/
/**
* This function is a post callback from a channel drop command sent to mDSP.
*/
/*===========================================================================*/
extern void mcalwcdma_dl_mdsp_post_cb(uint16 status);

#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION mcalwcdma_dl_qsh_populate_chan_info

DESCRIPTION
  This function will populate comb info for QSH

DEPENDENCIES


RETURN VALUE


SIDE EFFECTS

===========================================================================*/
extern void mcalwcdma_dl_qsh_populate_chan_info(wl1_dl_mini_struct_type *demod_dump_ptr);
#endif /* FEATURE_QSH_DUMP */ 


#endif /* MCALWCDMA_DL_H */
