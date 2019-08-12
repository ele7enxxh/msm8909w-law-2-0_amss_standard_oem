#ifndef ULCMD_H
#define ULCMD_H

/*===========================================================================
                              U L C M D . H

GENERAL DESCRIPTION
  
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright (c) 2000 - 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/ulcmd.h_v   1.15   19 Jul 2002 12:31:52   shawnm  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/ulcmd.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
10/13/14    vr      Changes to skip RFM-INIT and RFM-DECFG when not needed.
10/10/14    ar      Moved CXM frequency ID requests and release to RFM Init and
                    RFM Deconfig separately.
08/11/14    yw      Fix for incorrect blocking PDU building during Sec RLF
08/09/14    gp      Seperation of R99 and EUL variables init for QTA
08/08/14    sks     MCPM driver code cleanup
06/23/14    yw      Ensure no 2 UL DPCCH config command is sent to FW in the same CFN
06/19/14    ash     Backed out code for 0x410E Log Pkt
06/03/14    yw      Mods to support secondary carrier Tx freq/pwr reporting 
05/29/14    raj     Call mcalwcdma_evt_enc_get_data() from task context only if any 
                    UL DPCCH cfg cmd is not send in the current frame.
05/23/14    ash     Added support for graceful handling of RFM INIT TX Failure
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
04/27/14    jkb     Add check for Feature dual sim with feature qta
02/05/13    kcm     Changes to trigger TxLM disable when AICH is suspended in EDRX config
01/22/14    gp      Fix for race condition during QTA cleanup
09/17/13    jkb     Porting Dual Sim from Triton to Dime
08/22/13    gp      Moved UL Maint parameters into new API enc_set_ul_maint_event_parameters()
                    and ensured that UL DPCCH Cfg Cmd is sent within FW Config Window
08/15/13    at      Post UL DPCH cfg disable done command to L1M after UL DPCH disable done 
                    interrupt from FW is received.
08/08/13    yw      Call SetModTiming cmd before send UL DPCCH cfg cmd to FW from WPLT 
07/02/13    yw      Send Sec UL DPCCH disable cfg command from enc_get_data ISR.
07/01/13    pr      Optimizations done to change the UL DPCH Sequence
06/25/13    yw      Support for skipping PDU building during 6-slot interruption peroid
06/19/13    yw      Ensure that DPCCH cfg command for Sec UL and Primary UL is not sent in same frame.
06/04/13    sad     CCTRCH validation moved to ul_cctrch_cfg_validate
05/28/13    ash     UL unused Mutex cleanup
05/22/13    oh      Adding DC-HSUPA framework support
05/09/13    gp      Code cleanup for FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND and FEATURE_WCDMA_CONCURRENT_R99_EUL_TX_ENCODE
04/01/13    gp      Handling race condition of simultaneously sending Tx event disable command 
                    and polling of Tx event due to data availability. 
03/26/13    sad     Changes to handle new RRC-L1 interface.
12/12/12    ash     MCPM changes for Dime to turn off Tx block only when Tx phy chan is disabled.
12/10/12    vr      Fix for compiler warnings when HSRACH feature is enabled
10/26/12    ash     Dime Feature clean up. Mainlined FEATURE_WCDMA_GENESIS_SW_FW flag
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/19/12    vr      HSRACH code update
06/08/12    gv      Feature HS RACH Initial code check in
03/27/12    hk      Added support for FEATURE_WCDMA_HS_FACH_DRX
02/01/12    raj     Nikel Feature cleanup.
01/31/12    raj     Nickel feature cleanup. Mainlined the features FEATURE_WCDMA_UL_TFC_PL_REV_2
01/27/12    vs      Nikel feature cleanup.
01/20/12    bs      free floating changes.
11/29/11    gnk     Removed some unused features
10/21/11    sp      Nikel UL CM workaround under FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND
04/04/11    oh      Changes to support UL DCH on NikeL
03/30/11    gnk     Mainlined several features
01/17/11    sp      Added support for UL_RACH_DEBUG_LOG_PKT (0x4025)
01/17/11    sp      Featurized UL RACH FW Log Pkt (0x4026) under FEATURE_WCDMA_GENESIS_SW_FW
01/07/11    sp      Added support for new UL RACH FW Log Pkt (0x4026)
                    Removed support for 0x4180, 0x4181, 0x4182
11/19/10    sp      Optimized R99 UL Data Dump Log Pkt implementation
07/30/09    gnk     Mainlining the feature FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT
07/24/09    ka      Fixed flow control between ENC_GET_DATA ISR and L1 task for 
                    x410E log packet. 
07/17/09    ka      Removed ERR_FATAL from log buff read code for 0x410E
06/29/09    oh      Mutex protection between UL DCH ISR and UL reconfig in WL1 task.
05/25/09    hk      Fixed compiler warnings
04/22/09    gnk     Fixed compiler warnings
02/13/09    ka      Fixed concurrency issue in processing UL_TRCH_CODING_LOG_PKT
10/03/08    hk      Bringing in 7k Mailine fixes
02/08/08    gnk     Added QXDM support for R99 interface
10/15/07    gnk     Added Prach log packets & F3 msgs temporarily
08/13/07    vp      Moved event handlers from dltimeline to individual modules.
09/27/06    asm     Added Feature UL TFC PL rev 2
08/15/06    asm     Added interface to query TPC Step size
08/14/06    yiz     Mod to mainline HSUPA code
08/23/05    asm     Add support for FEATURE_WCDMA_LOWER_POWER_FOR_AMR
07/26/05    yiz     Add ul_dpcch_pre_reconfig() for UL DPCCH slot format and 
                    scrambling code reconfig at ACT.
07/13/05    gs      Removed the #else parts concerning the flag definition
                    FEATURE_WCDMA_HFN_UPDATE. The #else part contained 
                    the older form of MAC-CFN update mechanism
05/12/05    gs      Externalized UL frame boundry event enabler and event
                    handler function
03/31/05    yiz     Add decl of ul_hfn_update_evt_enabled
03/17/05    yiz     Added interface to trigger dummy BFI for resume.
11/19/04    yiz     Support DYNAMIC_POWER_CLASS
11/09/04    yiz     Add WCDMA_PRACH_SELECTION support.
10/22/04    yiz     Add ul_maintain_dpch_tx_pwr flag for Sync Proc A.
                    Delete FEATURE_DROP_ADD_FOR_PHYCHL_RECONFIG.
10/05/04    yiz     Add UL_DPCH_PWR_INFO_LOG_PKT.
09/30/04    yiz     Add decl's for UL_TFC_PWR_ELMN_LOG_PKT.
08/30/04    gs      Moved definition of macros for channel timing offset from
                    source file to here
                    HSDPA related declarations
08/18/04    yiz     Added ul_asu_cfg_db_copy_cmd() to update ul pointers
                    during ASET update.
11/12/03    m       Added support to suppress end of call spike
08/01/03    gs      Externalized function calc_pre_rm_bits
07/22/03    yiz     Added functions decl to fix re-slam of UL timing on recfg.
04/09/03    scm     Remove FEATURE_WCDMA_COMP_GF.  Feature always supported.
08/22/02    scm     Add ul_teardown_all_active_phch().
08/14/02    sh      Added structure ul_cphy_setup_cmd_type for Uplink
                    channel setup.
07/31/02    gs      Updated the declaration of externalized function
                    ul_compute_gain_factors
07/25/02    scm     Fixing uplink gain factor support during compressed mode.
07/19/02    scm     Uplink gain factor support during compressed mode.
05/29/02    m       Added prototype to support log on demand request for
                    1.  UL_CCTRCH_
                    2.  UL_DPCH
                    3.  UL_RACH
05/18/02    sh      Cell Reselection related changes. Added function
                    ul_cell_trans_drop_done_cmd().
04/12/02    yiz     Added ul_phychan_dpch_log_pkt and ul_log_phychan_dpch_info()
                    to change UL_PHYCHAN_DPCH_LOG_PKT to deferred logging.
01/18/02    gs      1. Externalized the global ul_dpcch_slot_fmt_nm
                    2. Externalized the function ul_set_ciphering_keys
10/29/01    sh      Removed ul_set_mod_req flag.
08/28/01    sk      Changed FEATURE_L1_LOGGING to FEATURE_L1_LOGGING_ENABLED.
08/09/01    sk      Added logging.
06/12/01    sk      Added ul_drop_pending flag.
05/22/01    sk      Added support for SetModTiming command in DL event.
01/12/00    sk      externalized ul_dpcch_slot_fmt.
09/22/00    sk      initial version created.
                    
===========================================================================*/
#include "wcdma_variation.h"
#include "customer.h"
#include "comdef.h"
#include "l1def.h"
#include "l1sapcommon.h"
#include "l1ulcfg.h"
#include "l1rrcif.h"
#include "l1macif.h"
#include "l1task_v.h"
#include "ullog.h"

#include "wl1rfmif.h"

#ifdef FEATURE_WCDMA_MCPM_DRIVER
#include "wmcpmdrv.h"
#endif /* FEATURE_WCDMA_MCPM_DRIVER */

/* DPCH offset */
#define L1_UL_DPCH_OFFSET      4
#define L1_UL_DPCH_OFFSET_Cx8  8192 //4 * 256 * 8 (Tx offset in chipx8)

/* PRACH offsets depending on AICH TT */
#define L1_UL_PRACH_OFFSET0  (-30)
#define L1_UL_PRACH_OFFSET1  (-50)

#define L1_UL_PRACH_DL_REF_OFFSET0  (-30)
#define L1_UL_PRACH_DL_REF_OFFSET1  (-50)
#define L1_UL_DPCCH_DL_REF_OFFSET   (  4)

/*Define the config window for sending DPCCH Cfg Cmd to FW */
#define L1_UL_DPCCH_CFG_WINDOW_START 20
#define L1_UL_DPCCH_CFG_WINDOW_END 120

/* UL Reconfiguration state */
extern boolean ul_reconfig;

/*This variable notes whether there is any mDSP UL DPCCH Config command is sent  during
  * the current frame and prevent sending any other DPCCH cfg cmd in the same frame. */
extern boolean ul_dpcch_cfg_send;

/* Contains parameters required for issuing mDSP SetModTiming command */
typedef struct
{
  boolean slam;    //indicates of DSP should slowly slew or slam to target
  uint8   hw_chan; // active DL h/w chan
  uint32 txPnPositioncx8; // absolute tx Pn Position in Chipx8 units
  int8    dl_ref_offset; // DL ref channel offset DPCCH = 4, RACH: AICH TT 0 = -30, AICH TT 0 = -50
} ul_set_mod_timing_struct_type;

extern boolean ul_tx_evt_disable_in_progress;

extern boolean wl1_ul_call_decfg_tx_flag;

/* UL Set Mod Timing parameters */
extern ul_set_mod_timing_struct_type ul_set_mod_timing;
/* UL DPCCH CFG done flag */
extern boolean ul_dpcch_cfg_done;
/* UL Phychan drop pending flag */
extern boolean ul_drop_pending;
/* log packets */
extern UL_PHYCHAN_DPCH_LOG_PKT_type   ul_phychan_dpch_log_pkt;
extern UL_RACH_PROC_LOG_PKT_type      ul_rach_proc_log_pkt;
#ifdef FEATURE_WCDMA_HS_RACH
extern UL_HSRACH_CONFIG_LOG_PKT_type           ul_hsrach_config_log_pkt;
extern UL_HSRACH_ALLOC_EDCH_RSRC_LOG_PKT_type  ul_hsrach_alloc_edch_rsrc_log_pkt;
extern UL_HSRACH_CHAN_TIMING_LOG_PKT_type      ul_hsrach_chan_timing_log_pkt;
extern UL_HSRACH_PREAMBLE_STATUS_LOG_PKT_type  ul_hsrach_preamble_status_log_pkt;
#endif /* FEATURE_WCDMA_HS_RACH */

/* variable to check if RFM Init failed during Channel Add */
extern boolean wl1_rfm_init_failed;

extern UL_TFC_PWR_ELMN_LOG_PKT_type ul_tfc_pwr_elmn_log_pkt;
extern UL_DPCH_PWR_INFO_LOG_PKT_type  ul_dpch_pwr_info_log_pkt;

/*Mutex between UL DCH ISR and UL reconfig in WL1 task*/
extern rex_crit_sect_type l1_ul_dch_reconfig_isr_mutex;

/* Local Command for UL Physical Channel Setup */
typedef struct
{
  l1_local_cmd_hdr_type           hdr;

  l1_ul_phychan_enum_type         phychan_type;

} ul_cphy_setup_local_cmd_type;

/* DPCCH slot format */
extern uint8 ul_dpcch_slot_fmt;
extern uint8 ul_dpcch_slot_fmt_nm;

/* Indicate whether to keep DPCCH TX power in Sync Proc A */
extern boolean ul_maintain_dpch_tx_pwr;

/* UL TFCS table */
extern mac_ul_tfcs_type  *ul_tfcs_ptr;

/* Special value used to make ul_compute_gain_factors() more efficient when
** called multiple times back to back.  -1 means this is the first time it's
** being called on this TFCS configuration and can assume nothing about the
** past.  Other values indicate the reference TFC used the last time
** ul_compute_gain_factors() was called.  Some computations can be avoided
** if the reference TFC remains unchanged. */
extern int32 ul_previous_ref_tfci;

#if (defined(FEATURE_UMTS_CIPHERING_ENABLE) && defined(FEATURE_WCDMA_HFN_UPDATE))
extern boolean ul_hfn_update_evt_enabled;
#endif /* FEATURE_UMTS_CIPHERING_ENABLE && defined(FEATURE_WCDMA_HFN_UPDATE */

extern const uint8 Npilot_c_for_slot_fmt[],Npilot_n_for_slot_fmt[];
/* UL_DATA_DUMP_LOG_PKT global variables declaration */

typedef enum
{
  /* Logs R99 UL DCH DATA before writing to ERAMA. This cmd 
   * is sent from wenc.c write_erama_data handler */
  UL_DATA_DUMP_LOG_PKT_CMD,
  /* Logs WCDMA RACH parameters from FW interfaces */
  UL_FW_RACH_DEBUG_LOG_PKT_CMD,
  /* Logs WCDMA RACH cfg and status parameters. */
  UL_RACH_DEBUG_LOG_PKT_CMD,
#ifdef FEATURE_WCDMA_HS_RACH
  HS_RACH_CONFIG_LOG_PKT_CMD,
  UL_RACH_ALLOC_EDCH_RES_LOG_PKT_CMD,
  UL_RACH_CHAN_TIMING_LOG_PKT_CMD,
  UL_RACH_PROC_PARAMS_LOG_PKT_CMD,
#endif /* FEATURE_WCDMA_HS_RACH */
  UL_LOG_CMD_NUM_TYPES
}ul_logging_cmd_enum_type;

/* The structure to accomodate all R99 UL Log packets 
 * that are to be submitted in task context */
typedef struct
{
  l1_local_cmd_hdr_type hdr;
  ul_logging_cmd_enum_type log_pkt_type;
} ul_logging_local_cmd_type;

#ifdef FEATURE_WCDMA_DC_HSUPA
/*For RF retune the below struct will be used to skip
  ** PDU building during interruption */
typedef struct
{
  uint8 num_pdu_built_before_interruption;
  uint8 num_subfn_till_interrption_ends;
}eul_skip_pdu_build_type;

extern eul_skip_pdu_build_type eul_skip_pdu_build;
#endif

/* The log packet ptr to record UL_DATA written to ERAMA */
extern UL_DATA_DUMP_LOG_PKT_type  *ul_data_dump_log_pkt_ptr;

/* Flag to enable/disable UL_DATA_DUMP Logging through QXDM  */
extern boolean ul_data_log_qxdm_config_flag;

/* Flag to indicate if the log buffer is available to record new data */
extern boolean ul_data_log_buf_empty_flag;

/* RACH_DEBUG_LOG_PKT structure to record log samples*/
extern UL_FW_RACH_DEBUG_LOG_PKT_type ul_fw_rach_debug_log_pkt;

/*debug flag to indicate if FW trigger a interrupt done cmd to SW*/
extern boolean debug_sec_teardown_flag;

/* Flag to indicate if rach fw debug log buffer is full or empty 
 * Flag is TRUE: Buf is empty; Flag is FALSE: Buf is not empty */
extern boolean ul_fw_rach_debug_buf_empty_flag;

/* RACH_DEBUG_LOG_PKT structure to record log samples*/
extern UL_RACH_DEBUG_LOG_PKT_type ul_rach_debug_log_pkt;

/* Flag to indicate if rach debug log buffer is full or empty */
extern boolean ul_rach_debug_buf_empty_flag;

extern uint8 ul_phychan_db_idx;

#ifdef FEATURE_WCDMA_REL6
extern boolean DEBUG_EUL_FIX_HS_DPCCH;
#endif

/* indicate encoder UL HS beta table computaion is required */
extern boolean enchs_beta_table_prep_reqd;


#ifdef FEATURE_L1_LOG_ON_DEMAND
/* following two variables could not be initialized in any of the
 * functions in this file. Hence they will be left as it is. Since
 * they are global varibles they will be initialized to 0 anyway
 */
 extern ul_phychan_dpch_log_on_demand_struct_type   ul_dpch_log_pkt;
 extern ul_phychan_prach_log_on_demand_struct_type  ul_prach_log_pkt;
#endif

/* Read TX TTI at time of slamming mod timing to FW */
#define UL_FW_RACH_DEBUG_SET_MOD_TIMING_TTI(value) ((ul_fw_rach_debug_log_pkt.ul_rach_params_info).tx_tti = value)

extern timer_type wl1_ul_dpch_drop_intr_timeout_timer;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)

#define INVALID_SUB_FRAME_NUM 0xFFFF
#define INVALID_SLOT_NUM 0xFFFF

/* QTA state which will indicate if the QTA is active or not */
typedef enum
{
  UL_QTA_INACTIVE, /* QTA is not active */
  UL_QTA_ACTIVE_IN_FUTURE, /* QTA is going to get activated in future */
  UL_QTA_ACTIVE,    /* QTA is active */
  UL_QTA_ERROR   /* Invalid state which is initialized state */
}ul_qta_state_enum_type;

typedef struct {
  /* Current state of the QTA machine */
  ul_qta_state_enum_type qta_state;
  /* Start CFN of the QTA which is returned by the QTA manager */
  uint16 qta_start_cfn;
  /* Start slot of the QTA which is returned by the QTA manager */
  uint16 qta_start_slot;
  /* EUL Subframe number where the QTA can be started for EUL*/
  uint16 qta_eul_active_subfn;
} wl1_ul_qta_struct_type;

extern wl1_ul_qta_struct_type wl1_ul_qta_params;

extern boolean wl1_ul_qta_skip_maint_evt_hdlr;
extern boolean wl1_ul_eul_qta_active;

/* Flag for MAC trigger in FACH QTA */
extern boolean wl1_ul_rach_trigger_during_qta;
#endif /* FEATURE_QTA */

/******************************************************************************
*******************************************************************************
              COMMAND HANDLERS FOR PHYSICAL CHANNEL MANAGEMENT
*******************************************************************************
******************************************************************************/
/*--------------------------------------------------------------------------
                  CMD STRUCTURE: UL_CPHY_SETUP_CMD_TYPE
                  
This is the generic command structure used for Uplink physical channel add,
cfg, and drop command.
--------------------------------------------------------------------------*/

typedef struct
{
  /* request mask */
  uint32 req_mask;

  /* data for CPHY_UL_CCTRCH_REQ */
  l1_ul_cctrch_cfg_struct_type*  ul_cctrch_info;


  /* data for CPHY_UL_PHYCHAN_DROP_REQ */
  l1_ul_phychan_enum_type       drop_ul_phychan;

  /*
   * data for CPHY_UL_PHYCHAN_CFG_REQ/CPHY_UL_PHYCHAN_ADD_REQ.  cfg_ul_phychan is the uplink physical
   * channel to be reconfigured.  It is assumed that it will be reconfigured
   * to use the CCTrCH referred to in the CPHY_UL_CCTRCH_REQ also included
   * in the command packet
   */
  l1_ul_phychan_enum_type        ul_phychan;
  l1_ul_phychan_cfg_struct_type* ul_phychan_db;

  mac_ul_tfcs_type  *tfcs_ptr;

  boolean prep_hs_beta_table;

  l1_e_sec_dpcch_info_struct_type sec_dpcch_info;

} ul_cphy_setup_cmd_type;

/*--------------------------------------------------------------------------
                  STRUCTURE: UL_PHYCHAN_ADD_CMD_TYPE
                  
This structure contains the parameters for the ul_phychan_add_cmd().
--------------------------------------------------------------------------*/
typedef ul_cphy_setup_cmd_type ul_phychan_add_type;

extern boolean get_bit_pos(uint16 bit_str, uint8  avail_idx, uint8  *bit_pos);

/*===========================================================================
FUNCTION        UL_PHYCHAN_ADD_VALIDATE

DESCRIPTION     This function validates that the addition of the physical
                channel as specified by the parameters referenced by the
                command can be carried out successfully, i.e. that hardware
                resources are available and the parameters are valid.

DEPENDENCIES    The physical channel database and command parameters

RETURN VALUE    TRUE if the command can be carried out, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
extern boolean ul_phychan_add_validate
(
  ul_phychan_add_type *cmd
);

/*===========================================================================
FUNCTION        UL_CCTRCH_RECONFIG_TFC_PL

DESCRIPTION     This function adjusts the rateAdj table by clearing away the
                histories of rateAdjs not corresponding to any existing TFCs

DEPENDENCIES    ul_tfc_pwr_elmn_state == UL_TFC_PWR_ELMN_STATE_PAUSE
                The CCTrCh should have been reconfigured

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void ul_cctrch_reconfig_tfc_pl( void );

/*===========================================================================
FUNCTION        UL_ENABLE_FRAME_BDRY_EVENT

DESCRIPTION     This function is a call back function for mDSP command to
                enable frame boundry event

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void ul_enable_frame_bdry_event(
  /* mDSP command status */
  uint16 status);

/*===========================================================================
FUNCTION        UL_FRAME_BOUNDRY_EVENT_HANDLER

DESCRIPTION     This function is call back function for UL frame boundry event

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void ul_frame_boundry_event_handler(void);

/*===========================================================================
FUNCTION        UL_SET_MOD_TIMING_POST_CB

DESCRIPTION     This function is a post call back function for post set mod
                timing. It sends UL set done local command if delay flag is
                set to TRUE. Delay flag is cleared after this call back
                function is done.

DEPENDENCIES    The command parameters

RETURN VALUE    None

SIDE EFFECTS    The uplink module will start the process of adding the
                specified physical channel.
===========================================================================*/
extern void ul_set_mod_timing_post_cb(
  /* status of mDSP set mod timing command */
  uint16 status);

/*===========================================================================
FUNCTION        UL_PHYCHAN_ADD_CMD

DESCRIPTION     This function starts the procedure for adding a physical
                channel.

DEPENDENCIES    The command parameters

RETURN VALUE    None

SIDE EFFECTS    The uplink module will start the process of adding the
                specified physical channel.
===========================================================================*/
extern void ul_phychan_add_cmd
(
  ul_phychan_add_type *cmd
);

/*--------------------------------------------------------------------------
                  STRUCTURE: UL_CCTRCH_CFG_TYPE
                  
This structure contains the parameters for ul_cctrch_cfg
--------------------------------------------------------------------------*/
typedef ul_cphy_setup_cmd_type ul_cctrch_cfg_type;

/*===========================================================================
FUNCTION        UL_CCTRCH_CFG_VALIDATE

DESCRIPTION     This function validates that the reconfiguration of the 
                CCTrCH as specified by the parameters referenced
                in the command can be carried out successfully.

DEPENDENCIES    The CCTrCH parameters

RETURN VALUE    TRUE if the command can be carried out, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
extern boolean ul_cctrch_cfg_validate
(
  ul_cctrch_cfg_type *cmd,
  l1_dl_phychan_db_struct_type *dl_phychan_db
);

/*=========================================================================

FUNCTION calc_pre_rm_bits

DESCRIPTION
  This function calculates pre-rate matched bits for a given transport
  channel. The number of pre-rate matched bits depends on the CCTrCH
  configuration for the transport channel and TFC.

  This function is based heavily upon ul_pre_rm_bits() and is virtually
  identical.  The difference is this function does not manipulate the
  global array ul_trch_coding_parms[].

DEPENDENCIES
  None.

PARAMETERS
  uint8  trch_id - Transport channel id.
  uint16 tb_size - Transport block size in bits.
  uint8  crc     - CRC size.
  uint8  num_tb  - Number of transport blocks.
  l1_ul_coding_enum_type  - code_type,
  l1_tti_enum_type        - tti

RETURN VALUE
  Number of pre-rate matched bits.

SIDE EFFECTS
  None.

=========================================================================*/
extern uint32 calc_pre_rm_bits(uint8  trch_id, 
                               uint16 tb_size, 
                               uint8  crc,
                               uint8  num_tb,
                               l1_ul_coding_enum_type code_type,
                               l1_tti_enum_type tti);
/*===========================================================================

FUNCTION ul_compute_gain_factors

DESCRIPTION
  This function computes the control and data gain factors Bc_j and Bd_j
  as described in 3GPP TS 25.214 V3.7.0 (2001-06), 5.1.2.5.3 Computed gain
  factors.

  These computed gain factors for the passed-in TFC use the signalled gain
  factors of a reference TFC in the computation.  A TFCS may contain up to
  4 reference TFCs, as shown in 3GPP TS 25.331 V3.7.0 (2001-06),
  10.3.5.8 Power Offset Information.

DEPENDENCIES
  This function shall only be called for the following three scenarios:
  - Normal (Non-Compressed) Mode / Computed Gain Factors
  - Compressed Mode / Signalled Gain Factors
  - Compressed Mode / Computed Gain Factors

  It shall NOT be called for:
  - Normal (Non-Compressed) Mode / Signalled Gain Factors

PARAMETERS
  uint32   ui32_tfci - TFC to compute gain factors on.
  uint32  *ui32_Bc_j - Returns Bc_j (control gain factor) to caller.  1 to 15.
  uint32  *ui32_Bd_j - Returns Bd_j (data gain factor) to caller.  0 to 15.
  boolean  f_compressed_mode - Whether or not to compute compressed mode gain factors.
                               Only used with FEATURE_CM_SUPPORTED defined.
  uint32  *deltaPc   - Returns the power control rate Adj In delta. Valid
                       only for power control
  l1_ul_cctrch_cfg_struct_type* cfg_tab_ptr - the ccTrCh CFG table
  mac_ul_tfcs_type*             tfcs_tab_ptr - the UL TFCS table
  int32  *i32_previous_ref_tfci - cache for faster calc

RETURN VALUE
  None.

SIDE EFFECTS
  May update "Aj_table_adjusted" array if necessary.

===========================================================================*/
extern void ul_compute_gain_factors(uint32 ui32_tfci, uint32 *ui32_Bc_j, uint32 *ui32_Bd_j,
                                    boolean f_compressed_mode, uint32 *deltaPc,
                                    l1_ul_cctrch_cfg_struct_type*  cfg_tab_ptr,
                                    mac_ul_tfcs_type*              tfcs_tab_ptr,
                                    int32 *i32_previous_ref_tfci);

/*===========================================================================
FUNCTION        UL_CCTRCH_CFG_CMD

DESCRIPTION     This function starts the process for reconfiguring an
                active CCTrCH.

DEPENDENCIES    The command parameters

RETURN VALUE    None

SIDE EFFECTS    The uplink module will start the process of reconfiguring
                the specified CCTrCH.
===========================================================================*/
extern void ul_cctrch_cfg_cmd
(
  ul_cctrch_cfg_type *cmd
);


/*--------------------------------------------------------------------------
                  STRUCTURE: UL_PHYCHAN_CFG_TYPE
                  
This structure contains the parameters for ul_phychan_cfg
--------------------------------------------------------------------------*/
typedef ul_cphy_setup_cmd_type ul_phychan_cfg_type;

/*===========================================================================
FUNCTION        UL_PHYCHAN_CFG_VALIDATE

DESCRIPTION     This function validates that the reconfiguration of the 
                physical channel as specified by the parameters referenced
                in the command can be carried out successfully, i.e. that
                the channel is currently active and that the parameters are
                valid.

DEPENDENCIES    The physical channel database and command parameters

RETURN VALUE    TRUE if the command can be carried out, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
extern boolean ul_phychan_cfg_validate
(
  ul_phychan_cfg_type *cmd
);



/*===========================================================================
FUNCTION        UL_PHYCHAN_CFG_CMD

DESCRIPTION     This function starts the process for reconfiguring an
                active physical channel.

DEPENDENCIES    The command parameters

RETURN VALUE    None

SIDE EFFECTS    The uplink module will start the process of reconfiguring
                the specified physical channel.
===========================================================================*/
extern void ul_phychan_cfg_cmd
(
  ul_phychan_cfg_type *cmd
);

/*--------------------------------------------------------------------------
                  STRUCTURE: UL_PHYCHAN_DROP_CMD_TYPE
                  
This structure contains the parameters for the physical channel drop                  
--------------------------------------------------------------------------*/
typedef ul_cphy_setup_cmd_type ul_phychan_drop_type;


/*===========================================================================
FUNCTION        UL_PHYCHAN_DROP_VALIDATE

DESCRIPTION     This function validates that the drop of the physical 
                channel as specified by the parameters referenced by the
                command can be carried out successfully, i.e. that the 
                channel is currently active.
                
DEPENDENCIES    The physical channel database and command parameters

RETURN VALUE    TRUE if the command can be carried out, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
extern boolean ul_phychan_drop_validate
(
  ul_phychan_drop_type *cmd
);


/*===========================================================================
FUNCTION        UL_PHYCHAN_DROP_CMD

DESCRIPTION     This function starts the process of dropping an active 
                physical channel.
                
DEPENDENCIES    The command parameters

RETURN VALUE    None

SIDE EFFECTS    The uplink module will start the process of dropping the
                specified physical channel.
===========================================================================*/
extern void ul_phychan_drop_cmd
(
  ul_phychan_drop_type *cmd
);

/*===========================================================================
FUNCTION        UL_TEARDOWN_ALL_ACTIVE_PHCH

DESCRIPTION
  This function tears down all active uplink physical channels.  It is
  called in response to the CPHY_SUSPEND_WCDMA_MODE_REQ primitive from RRC.

DEPENDENCIES
  Uplink state and active uplink physical channels.

RETURN VALUE
  None.

SIDE EFFECTS
  The process of tearing down all of the active uplink physical channels
  is started.
===========================================================================*/
extern void ul_teardown_all_active_phch(void);
/*===========================================================================
FUNCTION        UL_CELL_TRANS_DROP_CMD

DESCRIPTION     This function starts the process of dropping a PRACH
                physical channel. This is called when performing a
                cell transition.

DEPENDENCIES    The command parameters

RETURN VALUE    Boolean if a physical channel is being dropped.
                False otherwise.

SIDE EFFECTS    The uplink module will start the process of dropping the
                PRACH if it is already set up.
===========================================================================*/
boolean ul_cell_trans_drop_cmd( void );
/******************************************************************************
                       COMMAND: UL_TX_CONTROL_CMD
******************************************************************************/
/*--------------------------------------------------------------------------
                       STRUCTURE: UL_TX_CONTROL_CMD_TYPE
                  
This structure contains the parameters for the uplink PRACH transmit control.

--------------------------------------------------------------------------*/
typedef l1_ul_tx_ctrl_req_type ul_tx_control_cmd_type;

/*===========================================================================
FUNCTION        UL_TX_CONTROL_CMD

DESCRIPTION     This function starts the process of enabling or disabling
                the specified physical channel - PRACH/PCPCH.
                
DEPENDENCIES    The command parameters

RETURN VALUE    None

SIDE EFFECTS    The uplink module will start the process of dropping the
                specified physical channel.
===========================================================================*/
extern void ul_tx_control_cmd
(
  ul_tx_control_cmd_type *cmd
);

/*===========================================================================
FUNCTION UL_SET_CIPHERING_KEYS

DESCRIPTION
  This function validates and sets the ciphering key information for uplink
  ciphering operation.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE validate and update result.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean ul_set_ciphering_keys(
  /* DL ciphering info structure */
  l1_cipher_key_info_struct_type *ul_cipher_info);

/*===========================================================================
FUNCTION UL_VALIDATE_CIPHERING_KEY_REQ

DESCRIPTION
  This function validates the ciphering key information for uplink
  ciphering operation.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE validate result.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean ul_validate_ciphering_key_req(
  /* DL ciphering info structure to validate the information */
  l1_cipher_key_info_struct_type *ul_cipher_info);

/*===========================================================================
FUNCTION        UL_LOG_PHYCHAN_DPCH_INFO

DESCRIPTION     This function logs DPCH info.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void ul_log_phychan_dpch_info
(
  void
);

/*===========================================================================
FUNCTION        UL_LOG_PRACH_INFO

DESCRIPTION     This function logs PRACH info.

DEPENDENCIES    The command parameters

RETURN VALUE    None

SIDE EFFECTS    The uplink module will start the process of dropping the
                specified physical channel.
===========================================================================*/
void ul_log_prach_info
(
  void
);

#ifdef FEATURE_L1_LOG_ON_DEMAND
/*===========================================================================
FUNCTION        ul_process_cctrch_log_on_demand

DESCRIPTION     This function collects all the items required for UL CCTrCH
                logging and submits the log packet to QXDM.
  
DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void ul_process_cctrch_log_on_demand(void);
#endif

#ifdef FEATURE_L1_LOG_ON_DEMAND
/*===========================================================================
FUNCTION        UL_PROCESS_DPCH_LOG_ON_DEMAND

FILE NAME       ulcmd.c

DESCRIPTION     This funtion creates a DPCH log packet and submits it
                to QXDM

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void ul_process_dpch_log_on_demand(void);
#endif

#ifdef FEATURE_L1_LOG_ON_DEMAND
/*===========================================================================
FUNCTION        UL_PROCESS_RACH_LOG_ON_DEMAND

FILE NAME       ulcmd.c

DESCRIPTION     This funtion creates a RACH log packet and submits it
                to QXDM

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void ul_process_rach_log_on_demand(void);
#endif

void ul_set_tau_dpch_diff_val(uint8 new_val);


/*===========================================================================
FUNCTION        UL_ASU_CFG_DB_COPY_CMD

DESCRIPTION     This function copies UL chan cfg for ASU to avoid re-config

DEPENDENCIES    RRC shall make sure NO change in the chan cfg.
                This function shall be called by L1M to replace ASU caused
                re-config. 

RETURN VALUE    None

SIDE EFFECTS    NONE
===========================================================================*/
void ul_asu_cfg_db_copy_cmd(l1_setup_cmd_type* l1_setup_cmd_ptr);

/*====================================================================
FUNCTION: UL_LOG_TFC_PWR_ELMN_PKT

DESCRIPTION:
  This function sends UL_TFC_PWR_ELMN_LOG_PKT to DIAG 

DEPENDENCIES:
  NONE

RETURN VALUE:
  NONE

SIDE EFFECTS:
  NONE
====================================================================*/
void ul_log_tfc_pwr_elmn_pkt(void);


/*===========================================================================
FUNCTION        UL_LOGGING_CMD_HANDLER

DESCRIPTION     This routine sends R99 uplink module log packets to diag in
                the task context. Based on the type of log pkt, specific log info
                handler will be called.    

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
void ul_logging_cmd_handler(ul_logging_cmd_enum_type log_pkt_type);

/*====================================================================
FUNCTION: UL_LOGGING_PUT_LOCAL_CMD

DESCRIPTION:
  This function puts local cmds to WL1 to submit UL log packets.

DEPENDENCIES:
  NONE

RETURN VALUE:
  NONE

SIDE EFFECTS:
  NONE
====================================================================*/
void ul_logging_put_local_cmd (ul_logging_cmd_enum_type ul_log_local_cmd);

/*===========================================================================
FUNCTION        UL_DATA_DUMP_LOG_INFO_HANDLER

DESCRIPTION     This routine sends R99 DCH DATA recorded in write_erama_data()
                to DIAG

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
void ul_data_dump_log_info_handler(void);

/*===========================================================================
FUNCTION        UL_FW_RACH_DEBUG_LOG_INFO_HANDLER

DESCRIPTION     This routine submits RACH FIRMWARE Log Packet to DIAG. 
                This routine is called from ul_logging_cmd_handler() 
                in task context.  

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
void ul_fw_rach_debug_log_info_handler(void);

/*====================================================================
FUNCTION: UL_LOG_RACH_DEBUG_PUT_LOCAL_CMD

DESCRIPTION: This routine puts UL_RACH_DEBUG_LOG_PKT in the local cmd queue.

DEPENDENCIES:
  NONE

RETURN VALUE:
  NONE

SIDE EFFECTS:
  NONE
====================================================================*/
void ul_log_rach_debug_put_local_cmd(void);

/*====================================================================
FUNCTION: UL_LOG_RACH_DEBUG_INFO_HANDLER

DESCRIPTION: This routine sends UL_RACH_DEBUG_LOG_PKT to DIAG. This routine is
             called from task context.

DEPENDENCIES:
  NONE

RETURN VALUE:
  NONE

SIDE EFFECTS:
  NONE
====================================================================*/
void ul_log_rach_debug_info_handler(void);

#ifdef FEATURE_WCDMA_HS_RACH
/*====================================================================
FUNCTION: hs_rach_config_log_info_handler

DESCRIPTION:
  

DEPENDENCIES:
  NONE

RETURN VALUE:
  NONE

SIDE EFFECTS:
  NONE
====================================================================*/
void hs_rach_config_log_info_handler(void);

/*====================================================================
FUNCTION: ul_rach_alloc_edch_res_log_info_handler

DESCRIPTION:
  

DEPENDENCIES:
  NONE

RETURN VALUE:
  NONE

SIDE EFFECTS:
  NONE
====================================================================*/
void ul_rach_alloc_edch_res_log_info_handler(void);

/*====================================================================
FUNCTION: ul_rach_chan_timing_log_info_handler

DESCRIPTION:
  

DEPENDENCIES:
  NONE

RETURN VALUE:
  NONE

SIDE EFFECTS:
  NONE
====================================================================*/
void ul_rach_chan_timing_log_info_handler(void);

/*====================================================================
FUNCTION: ul_rach_proc_params_log_info_handler

DESCRIPTION:
  

DEPENDENCIES:
  NONE

RETURN VALUE:
  NONE

SIDE EFFECTS:
  NONE
====================================================================*/
void ul_rach_proc_params_log_info_handler(void);
#endif /* FEATURE_WCDMA_HS_RACH */

#ifdef FEATURE_WCDMA_L1_ZI_OPT

/*===========================================================================
FUNCTION        UL_LOG_MEM_ALLOC_DEALLOC

DESCRIPTION     This routine allocates or deallocates dynamic memory
                based on whether log status is enabled/disabled.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void ul_log_mem_alloc_dealloc(log_code_type code, boolean log_status_flag);

#endif

/*====================================================================
FUNCTION: UL_LOG_DPCH_PWR_INFO_PKT

DESCRIPTION:
  This function sends UL_DPCH_PWR_INFO_LOG_PKT to DIAG 

DEPENDENCIES:
  NONE

RETURN VALUE:
  NONE

SIDE EFFECTS:
  NONE
====================================================================*/
void ul_log_dpch_pwr_info_pkt(void);

#ifdef FEATURE_UMTS_CIPHERING_ENABLE
/*===========================================================================
FUNCTION        UL_RESUME_BFI_FOR_CIPHER

FILE NAME       ulcmd.c

DESCRIPTION     This funtion is called by L1M at resume to start dummy Build
                Frame Indication in DCH in ordr to update MAC about CFN/HFN 
                for ciphering before UL starts to request TX data.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    UL will send dummy Build Frame Indication in TX_GEN_GET_DATA
                ISR in DCH until UL starts to request TX data.
===========================================================================*/
void ul_resume_bfi_for_cipher(void);
#endif /* FEATURE_UMTS_CIPHERING_ENABLE */

/*===========================================================================
FUNCTION        UL_DPCCH_PRE_RECONFIG

FILE NAME       ulcmd.c

DESCRIPTION     This funtion is called by SEQ one frame ahead of ACT to
                reconfig UL DPCCH for slot format and scrambling code.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    UL DPCCH will change to new config at ACT
===========================================================================*/
void ul_dpcch_pre_reconfig(l1_ext_cmd_type* ext_cmd);


/*===========================================================================
FUNCTION        UL_GET_DPCH_TPC_STEP_SIZE_DB

FILE NAME       ulcmd.h

DESCRIPTION     This funtion is called to get the value of UL TPC step size
                in dB

DEPENDENCIES    None

RETURN VALUE    UL DPCH TPC step size in dB

SIDE EFFECTS    None
===========================================================================*/
uint8 ul_get_dpch_tpc_step_size_db( void );

/*===========================================================================
FUNCTION        UL_TX_GEN_MOD_TIMING_EVENT_HANDLER

DESCRIPTION
  This is the event handler for the TX Set Modulator Time Event.
  This function enabled the TX System Time in the mDSP.

  This function will also send local command to start initial search
  when DRX command was received and also sends local command to DRX
  on behalf of SRCHZZ when GSM clocks are enabled during a SIB
  modification update.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void ul_tx_gen_set_mod_timing_event_handler( void );

/*===========================================================================
FUNCTION        UL_TX_GEN_MOD_TIMING_POST_CB

DESCRIPTION     This function gets called once after Mod timing set is done and 
                         sets the flag dl_tx_dpch_mod_timing_set_ready to TRUE.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void ul_tx_gen_mod_timing_pre_cb (void);


/*===========================================================================
FUNCTION        UL_TX_GEN_MOD_TIMING_POST_CB

DESCRIPTION     This function gets called once after Mod timing set is done and 
                         sets the flag dl_tx_dpch_mod_timing_set_ready to TRUE.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void ul_tx_gen_mod_timing_post_cb (uint16 status);

/*===========================================================================
FUNCTION        WL1_UL_DPCH_CHANNEL_DROP_INTERRUPT_HANDLER

DESCRIPTION     This function is the interrupt handler for the UL DPCH DROP DONE from FW.
                In this function channel drop confirmation will be send to L1M.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void wl1_ul_dpch_channel_drop_interrupt_handler(void);

/*===========================================================================
FUNCTION    WL1_UL_DPCH_DROP_INTR_TIMEOUT_HANDLER

DESCRIPTION Timeout handler for UL DPCCH drop done interrupt from FW.

DEPENDENCIES DPCCH config disable command should already be issued by SW.

RETURN VALUE None

SIDE EFFECTS Sends FW panic command
===========================================================================*/
extern void wl1_ul_dpch_drop_intr_timeout_handler(timer_cb_data_type data);

/*===========================================================================
FUNCTION  UL_SET_VAR_THROUGH_QXDM

DESCRIPTION
  This function sets the uplink global variables as received
  through diag command. This function is a part of the callback function for 
  diag command dispatch table.
  
DEPENDENCIES
  This function is called when "send_data 75 4 28 0 <val>" is issued from cmd 
  line interface from QXDM

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ul_set_var_through_qxdm( uint8 debug_switch_type, uint32 val );

#ifdef FEATURE_WCDMA_HS_FACH_DRX
/*=======================================================================
FUNCTION        UL_PHYCHAN_PRACH_PRE_MOD_TIMING

DESCRIPTION    This function is called before start of adding the RACH physical channel and again 
               called after the phychan add is done. This is used to indicate the UL DRX entity when 
               the Phychan add is started and when it is completed. The same function is used to 
               indicate the UL DRX entity if we have data/No data through RACH in L1.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
=========================================================================*/
extern void ul_phychan_prach_pre_mod_timing(void);
#endif /* FEATURE_WCDMA_HS_FACH_DRX */

/*===========================================================================
FUNCTION  UL_TX_ENABLE_RFM_CLOCK

DESCRIPTION
  This function sets tx clock and enables RF as part of NikeL bringup.
  This is a temporary function only for brinup. Should be replaced later
  by wl1_enable_tx_and_clk()
DEPENDENCIES
  This function is called at the time of phychan add (DCH)

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ul_tx_enable_rfm_clock(void);

/*===========================================================================
FUNCTION        ul_phychan_is_ul_chan_active

DESCRIPTION     This function returns the state of UL phy channel,
                whether it is active or not.This will be called before 
                turning off Tx Block

DEPENDENCIES    none

RETURN VALUE    TRUE if either PRACH or DPCH channels are active
                FALSE if neither PRACH or DPCH channels are active

SIDE EFFECTS    None
===========================================================================*/
extern boolean ul_phychan_is_ul_chan_active (void);

#ifdef FEATURE_WCDMA_COEXISTENCE_SW
/*=========================================================================
FUNCTION wl1ulmcmgr_register_freq_id_through_cxm(carr_idx, chan_active)

DESCRIPTION  This function is called to register freq id to MCS through CXM

DEPENDENCIES  None

 RETURN VALUE void

SIDE EFFECTS
  None.

=========================================================================*/

void wl1ulmcmgr_register_freq_id_through_cxm(wl1_ul_carr_id_enum_type carr_idx, boolean chan_active);
#endif

#ifdef FEATURE_WCDMA_DC_HSUPA 
/*===========================================================================
FUNCTION        ul_config_dc_mode

DESCRIPTION     This routine sets the RF in DC mode and sends the TxLM cfg cmd to FW to enable DC HSUPA, 
                          followed DPCCH cfg cmd with WaitforScchorder set as FALSE.


DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
void ul_config_dc_mode(void);


/*===========================================================================
FUNCTION        ul_perform_txlm_rf_tune_ops

DESCRIPTION     This routine sets the RF in SC/DC mode and sends TxLM cfg cmd to 
                FW to enable SC/DC HSUPA. This function is called when HS SCCH
                order with activate or deactivate is received.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/

extern void ul_perform_txlm_rf_tune_ops (boolean);

/*===========================================================================
FUNCTION        ul_order_rcvd_act_deact_sec

DESCRIPTION     This routine indicates to UL that an order came to act/deact 
                DC HSUPA. PDU building can be ignored for the interruption period.

DEPENDENCIES    Called by DL module when HS SCCH order is rcvd to act/deact Sec UL.

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/

extern void ul_order_rcvd_act_deact_sec(boolean is_scch_ord_rcvd);


#endif /*FEATURE_WCDMA_DC_HSUPA */

/*===========================================================================
FUNCTION        ul_set_mod_timing_cmd_from_wplt

DESCRIPTION     This func send FW set modulator timing cmd in WPLT mode from ul_phychan_add
                           and maint event handler.

DEPENDENCIES    Called in WPLT mode only

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/

extern void ul_set_mod_timing_cmd_from_wplt(void);

/*===========================================================================
FUNCTION        ul_send_txlm_disable_cfg_cmd

DESCRIPTION     This func sends the TxLM disable cfg to FW

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void ul_send_txlm_disable_cfg_cmd(void);

/*===========================================================================
FUNCTION        WL1_UL_SKIP_DECFG_TX_CHECK

DESCRIPTION     This function sets the flag to skip wl1_rfm_decfg_tx() in the
                following scenarios.
                1) Forced SyncA
                2) FACH to DCH transition if not in HHO.
                3) INTRA FREQ HHO && HHO SUSPEND NEEDED is FALSE

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    wl1_rfm_decfg_tx() will not be called.
===========================================================================*/
void wl1_ul_skip_decfg_tx_check(void);

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
/*===========================================================================
FUNCTION       WL1_UL_INIT_QTA_VARS_EUL

DESCRIPTION    This function will reset all the QTA related variables and 
               flush EDL results if the flag skip_eul_qta_init is not set. 
  
DEPENDENCIES   None
 
RETURN VALUE   Will reset all the EUL related QTA variables
 
SIDE EFFECTS   None
===========================================================================*/
extern void wl1_ul_init_qta_vars_eul(boolean skip_eul_qta_init);

/*===========================================================================
FUNCTION       WL1_UL_INIT_QTA_VARS_R99

DESCRIPTION    This funciton will reset all the QTA related variables and skip
               freeing of DSM items based on skip_dsm_free flag. MAC will free DSM 
               items in RACH so skip_dsm_free flag will be set to TRUE. In RACH abort case 
               due to QTA or any other reason, WL1 will send L1_PRACH_ABORT indication
               for MAC to free DSM items. For QTA in DCH case, skip_dsm_free flag will
               be set to FALSE since WL1 does DSM item free.
  
DEPENDENCIES   None
 
RETURN VALUE   Will reset all the R99 related QTA variables

SIDE EFFECTS   None
===========================================================================*/
extern void wl1_ul_init_qta_vars_r99(boolean skip_dsm_free);

/*===========================================================================
FUNCTION  WL1_UL_QUERY_QTA_MANAGER

DESCRIPTION This funciton will query the QTA manager and will get the future CFN and the slot number
where the QTA is going to be started
  
DEPENDENCIES None
 
RETURN VALUE  Will update the CFN number and the slot number

SIDE EFFECTS  None
===========================================================================*/
extern void wl1_ul_query_qta_manager(void);

/*===========================================================================
FUNCTION     wl1_ul_frames_to_qta_active

DESCRIPTION  This funciton will tell the QTA manager the number of Frames 
             ahead QTA has to start.This info. is used to add diff. 
             overheads to start QTA:
                1 Frame(10ms)  - EUL (10ms/2ms TTI's)
                2 Frame(20ms)  - R99 
  
DEPENDENCIES None
 
RETURN VALUE Will update the CFN number and the slot number

SIDE EFFECTS None
===========================================================================*/

extern uint8 wl1_ul_frames_to_qta_active(void);

#endif /* FEATURE_QTA */
/*===========================================================================
FUNCTION        ulcmd_rfm_decfg_wcdma_tx

DESCRIPTION     This function Deconfigures the RF TX chain for each carrier 

DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    Tx Chain for the corresponding carrier will be deconfigured 
===========================================================================*/
void ulcmd_rfm_decfg_wcdma_tx(void );

/*===========================================================================
FUNCTION        ulcmd_rfm_init_wcdma_tx_per_carrier

DESCRIPTION     This function initializes the RF TX chain for each carrier 


DEPENDENCIES    It takes the phychan type as input

RETURN VALUE    None

SIDE EFFECTS    Tx Chain for the corresponding carrier will be initialized and Tx chain may be enabled
===========================================================================*/
void ulcmd_rfm_init_wcdma_tx_per_carrier(
  uint32 multi_carrier_index, 
  wl1_rfcom_tx_multi_carrier_cfg_type rf_carr_cfg,
  boolean enable_tx);

#endif  // ULCMD_H
