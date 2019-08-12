#ifndef DLPHCH_H
#define DLPHCH_H

/*===========================================================================
                 WCDMA L1 DOWNLINK PHYSICAL CHANNEL CONFIGURATION

GENERAL DESCRIPTION
  This file contains the code for handling validation, configuration and
  management of downlink physical channels 
  (PCCPCH, SCCPCH,  DPCH, AICH, PICH and MICH).

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2007 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dlcmd.c_v   1.109   02 Jul 2002 18:11:42   cemani  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/dlphch.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who      what, where, why
--------   ---      -------------------------------------------------------- 
08/18/14    vr      Cleanup code for HSRACH 'conventional' seamless recfg.
06/23/14    sad     Moving DC-HSUPA specific code under FEATURE_WCDMA_DC_HSUPA flag
06/12/14    mit     Inform MCPM driver of a change in PCCPCH_N status 
06/04/14    hk      Corrected the positioning of Maint event for HS-RACH FDPCH
02/14/14    pr      Enable stmr events in the post callback of mod timing.
03/18/14    mit     F3 cleanup, enabling via NV's
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
09/14/13    pr      VSTMR Changes for BOLT
12/04/13    pr      Added new api for checking if reslam is pending for x2w hho.
11/07/2013  nd      Added support for QMI Extension for Detailed Protocol Stack Information
10/29/13    sad     Inital check in for Secondary ASET update changes
10/14/13    pr/sr   Channel config optimizations
07/09/13    pr      Fix the UL DPCCH sequence for resuming back to WCDMA cases
05/21/13    sad     Added DC HSUPA code changes under FEATURE_WCDMA_DC_HSUPA 
04/01/13    hk      Mainlined FEATURE_WCDMA_EFDPCH
10/22/12    kcm     Reducing F3 prints in Idle Mode scenarios
11/22/12    hk      added prototype for function "DL_PHCH_GET_DOFF"
09/12/12    sr       Changes to handle FDPCH deactivation cmd while executing
                    FDPCH activation.
08/07/12    dp      W2L PSHO time improvements
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/22/12    pr      STMR changes for Dime
07/05/12    pv      APIs for Offline PICH.
06/13/12    kcm     TPC rejection algorithm ehancements
06/19/12    vr      HSRACH code update
06/08/12    gv      Feature HS RACH Initial code check in
06/07/12    vs       MBMS feature cleanup.
05/23/12    pr       Fixed compilation warnings.
02/15/12    hk      Timeline alignment changes for HHO & W2G Failure recovery
02/13/12    dm      Nikel feature cleanup
02/10/12    rgn     Nikel feature cleanup
02/10/12    hk      Nikel feature cleanup
01/26/12    vs      Nikel feature cleanup.Mainline the feature FEATURE_OLPC_TX_DIV_OFFSET
                    & removed the feature FEATURE_WCDMA_EQ_CONTROLLER
01/19/12    zr      Mainlined FEATURE_WCDMA_GENESIS_SW
08/29/11    kcm     Added dl_phch_resume_ch_ops_on_cm_suspend function for resuming 
                    DPCH reconfig/drop after CM is suspended/frozen
07/20/10    ks      Added dl_phch_hw_tpc_init prototype
05/31/11    geg     Changes for RRC and L2 api files clean-up wcdma dependencies
10/11/10    vb      Added suport for offline pich optimization
10/08/10    vb      Added support to make fast pich configuration for pich 
                    suspend and resume operations
08/17/10    ks      Updatde dl_phch_update_pich_trans_time_offset prototype
07/20/10    ks      Added prototype for dl_phch_is_ssc_enabled
06/17/10    sup     Added protoype for dl_phch_is_fdpch_enabled() to resolve compiler warning 
04/29/10    ks      Added function to return CLTD timing mode
04/09/10    ks      Corrected rl per hw tpc bmsk width
04/06/10    ks      Added Enhanced FDPCH support
09/30/09    hk      Replaced INTLOCK with Mutex
08/08/09    hk      Update align cell info when sfn_wrc diff is calculated 
                    for ref cell
07/16/09    vsr     Added APIs returning ordered TxD and s-cpich status for 
                    given DPCH RL
07/16/09    hk      Reorganized the code to remove INC/SRC violations
07/15/09    rvs     Added QICE controller header file.
05/25/09    hk      Fixed compiler warnings
04/22/09    gnk     Fixed compiler warnings
04/20/09    rvs     Remove TXD change cb type and reg/dereg functions.
04/14/09    rgn     Fixed a compiler warning
03/12/09    rc      Added HS callback function to update HS RL TD status. 
02/26/09    rgn     Use Align type from phch cfg db instead of querying channel
                    manager
12/24/08    vsr     Added cell_cfg_processing_pending to dl_phch_cfg_db to avoid
                    race conditions due to event re-use
12/22/08    ks      Mainlining FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT
12/10/08    ks      Mainlining FEATURE_WCDMA_DL_ENHANCED
11/19/08    vsr     Changes to support Compressed Mode
10/27/08    hk      Lint Fixes
10/22/08    hk      Externed the allocate/deallocate stmr ctr functions
10/03/08    hk      Bringing in 7k Mailine fixes
09/12/08    rgn     Added SHO support
09/03/08    vsr     Added dl_phch_align_to_frc() - Fix for stmr combiner not getting 
                    freed
08/27/08    vsr     Added CME capability under feature FEATURE_WCDMA_CME_CONTROLLER
07/22/08    vc      Added function to export stmr counter info for TA logging.
06/16/08    vsr     Offline modem MCAL DL code cleanup
01/25/08    vsr     Checking in Enhanced DL files onto mainline
01/23/08    vsr     Added dl_phch_any_active_channels_on_cell()
01/22/08    hk      Moved the dl_phch_cfg_db_struct_type structure from dlphchi.h
01/21/08    vsr     Added API for WCB size calculation
01/18/08    vsr     Moved dl_aset_update_cmd() to dlchmgr.h
01/15/08    vp      Moved the definition of dl_phch_cfg_db_struct_type and 
                    associated types from dlphchi.h to dlphch.h.
08/23/07    vsr     Moved in variables related to removal of dlcmd.* files
08/21/07    vsr     Corrected macros
08/20/07    vsr     Moved in functions and variables from dldem files
08/08/07    gv/vp   Added support for MCAL_EDL and consolidated APIs in dlphch
                    to get rl_offset.
07/12/07    vp      Added function-dl_phch_is_phch_cfg_in_progress()
07/11/07    vsr     Removed unwanted function definition.
07/05/07    vsr     modified return type for synchronize function
06/18/07    rgn     Added support for HSDPA and added apis to retrieve various phch params
06/08/07    vsr     MCAL_DL code cleanup changes
05/23/07    vsr     Added support for combiner log packet
05/08/07    vsr     Corrected macro
05/02/07    rgn     Added support for compressed mode
04/20/07    vsr     Lint changes
04/05/07    vb      finger ta logging field support
03/27/07    vsr     Added support for ASET UPD procedure
02/21/07    vp/scm  Added support for the dlchmgr, new drx mgr and change i/f with
                    tlm.
01/26/06    vb      support for suspend/resume i/f changes
10/17/06    vp/vb   Created this module

===========================================================================*/


/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"

#include "l1rrcif.h"
#include "l1const.h"
#include "l1def.h"
#include "mcalwcdma_demod.h"
#include "mcalwcdma_dl.h"
#include "l1dlphychancfg.h"


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#define DL_PHCH_INVALID_CFG_ID 0xFF
#define DL_PHCH_INVALID_RL_ID NO_RL_IDX
#define DL_PHCH_INVALID_COMB_CH_ID (NO_COMBINER_ID)
#define DL_PHCH_INVALID_CELL_DB_ID DL_CELL_INVALID_CELL_DB_ID
#define DL_PHCH_INVALID_FREQ 0xFFFF
#define DL_PHCH_INVALID_PSC 0xFFFF
#define DL_PHCH_INVALID_TTO 0xFF
#define DL_PHCH_INVALID_FR_OFF 0xFFFF

#define DL_PHCH_INVALID_SLOT_FORMAT 0xFF

#define DL_PHCH_INVALID_BITS_PER_IND 0x0
#define DL_PHCH_INVALID_INTERFACE_BUF_ID 0xFF

#define DL_PHCH_NUM_STMR_CTRS STMR_SUPPORTED_COMBINERS

#define DL_PHCH_STMR_FRC_MARGIN (6 * CHIP_PER_STMR_TICK * CHIPX8_PER_CHIP)

#define DL_PHCH_MAX_DPCH_RL L1_ASET_MAX
#define DL_PHCH_MAX_SCCPCH_RL 1

#define DL_PHCH_MAX_PCCPCH_RL 2
#define DL_PHCH_MAX_AICH_RL 1
#define DL_PHCH_MAX_PICH_RL 1

#ifdef FEATURE_WCDMA_DC_HSUPA
#define DL_PHCH_MAX_SEC_FDPCH_RL  4
#define DL_PHCH_MAX_SEC_PCCPCH_RL  1
#else
#define DL_PHCH_MAX_SEC_FDPCH_RL  0
#define DL_PHCH_MAX_SEC_PCCPCH_RL  0
#endif

#define DL_PHCH_MAX_RL DL_PHCH_MAX_DPCH_RL \
                                          + DL_PHCH_MAX_SCCPCH_RL \
                                          + DL_PHCH_MAX_PCCPCH_RL \
                                          + DL_PHCH_MAX_AICH_RL \
                                          + DL_PHCH_MAX_PICH_RL \
                                          + DL_PHCH_MAX_SEC_FDPCH_RL \
                                          + DL_PHCH_MAX_SEC_PCCPCH_RL


#define DL_MAX_MBMS_RL 5  /* have it defined from MBMS module */

#define DL_PHCH_MAX_RL_PER_PHCH DL_PHCH_MAX_DPCH_RL

#define DL_PHCH_MAX_NUM_CELL DL_MAX_NUM_CELLS

#define DL_PHCH_MAX_NUM_COMB_CHAN MCALWCDMA_DL_NUM_MAX_SW_COMBINERS

#define DL_PHCH_MAX_RL_CFG 2*(DL_PHCH_MAX_RL)

#define DL_PHCH_MAX_NUM_COMB_CHAN_CFG 2*(DL_PHCH_MAX_NUM_COMB_CHAN)

#define DL_PHCH_IS_PHCH_CFG_DB_IDX_INVALID(phch_cfg_db_idx)    \
    (phch_cfg_db_idx >= DL_PHCH_MAX_NUM_COMB_CHAN_CFG)

#define DL_PHCH_IS_COMB_CH_DB_IDX_INVALID(comb_ch_db_idx)    \
    (comb_ch_db_idx >= DL_PHCH_MAX_NUM_COMB_CHAN)

#define DL_PHCH_IS_CELL_DB_IDX_INVALID(cell_db_idx)    \
    (cell_db_idx >= DL_PHCH_MAX_NUM_CELL)

#define DL_PHCH_IS_RL_IDX_INVALID(rl_idx)    \
    (rl_idx >= DL_PHCH_MAX_RL_CFG)

#define DL_PHCH_CFG_ON_PROGRESS(comb_id) \
  (dl_phch_comb_ch_db[comb_id].cfg_on_progress)

#define DL_PHCH_GET_MCAL_FR_OFF_FROM_RRC_DOFF(doff_rrc)  \
    (doff_rrc/CHIP_PER_FRAME)

#define EFDPCH_TIMING_MODE_1_MIN_SF   1
#define EFDPCH_TIMING_MODE_1_MAX_SF   8

#define EFDPCH_TIMING_MODE(sf) ((((sf)>=EFDPCH_TIMING_MODE_1_MIN_SF) || ((sf)<=EFDPCH_TIMING_MODE_1_MAX_SF))?(1):(0))

typedef uint8 dl_phch_cfg_handle_type;

/*--------------------------------------------------------------------------
                  STRUCTURE: DL_PHYCHAN_ADD_TYPE

This structure contains the parameters for the dl_phychan_add().
--------------------------------------------------------------------------*/
typedef struct {
  /* the physical channel */
  l1_dl_phychan_enum_type phch;

  /* Indicate if physical channel DPCH type is of type F-DPCH */
  boolean dpch_chan_is_fdpch;

  /* the CCTrCH table index chosen by dl_dec for this channel */
  uint8 cctrch_index;

  /* align type for this channel */
  seq_align_type align_type;
 
  /* pointer to the dl phychan ordered config database */
  l1_dl_phychan_db_struct_type *phychan_db;

  /* frequency on which config goes on */
  uint16 rf_chan;

  /*Indicate if we are to wait for order to start UL Tx on sec*/
  boolean waitOnScchOrder;
} dl_phychan_add_type;

typedef dl_phychan_add_type dl_phychan_recfg_type;

/* array to maintain ref phch per hwch/combiner. This is needed for triage */
extern l1_dl_phychan_enum_type dl_phch_ref_phch_per_hwch[DL_PHCH_MAX_NUM_COMB_CHAN];

// cfitoz
//typedef uint8 dl_phch_cfg_handle_type;

typedef enum
{
  DL_PHCH_STATE_INACTIVE,
  DL_PHCH_STATE_SUSPEND,
  DL_PHCH_STATE_ACTIVE
} dl_phch_state_enum_type;

typedef uint8 dl_phch_comb_ch_db_handle_type;

typedef uint8 dl_phch_rl_handle_type;

typedef void DL_PHCH_CFG_DONE_CB_FUNC_TYPE(
  uint8 cctrch_idx,
  l1_dl_phychan_enum_type phch_type,
  dl_phch_cfg_handle_type phch_handle,
  boolean status);

typedef void (*DL_PHCH_CFG_DONE_CB_FUNC_PTR_TYPE)( uint8 cctrch_idx, l1_dl_phychan_enum_type phch_type, dl_phch_cfg_handle_type phch_handle,  boolean status);

typedef  void DL_PHCH_CFG_SUSPEND_RESUME_FUNC_TYPE(
  dl_phch_cfg_handle_type phch_cfg_db_idx,
  boolean fast_cfg,
  DL_PHCH_CFG_DONE_CB_FUNC_PTR_TYPE phch_suspend_done_cb_ptr);


typedef void DL_PHCH_CFG_SET_MOD_TIMING_POST_CB_FUNC_TYPE(uint16 status);

/*--------------------------------------------------------------------------
                 STRUCTURE: DL_PHCH_CELL_DB_STRUCT_TYPE

This structure contains all of the various parameters for RL physical channel
configuration process
--------------------------------------------------------------------------*/
typedef struct {

  /* reference finger */
  uint8 ref_fing;

  /* active hwch mask */
  uint16 active_hwch_mask;

  /* This cell is ref for comb channels */
  uint16 ref_for_comb_ch_mask; 

  /* holds rl phch handle for activated hwch */
  dl_phch_rl_handle_type rl_idx[DL_PHCH_MAX_NUM_COMB_CHAN];
  
}dl_phch_cell_db_struct_type;

/*--------------------------------------------------------------------------
                  STRUCTURE: DL_PHYCHAN_ADD_TYPE

This structure contains the parameters for the dl_phychan_add().
--------------------------------------------------------------------------*/

typedef struct
{
  l1_local_cmd_hdr_type hdr;

  /* the combiner channel that is under configuration */
  uint8 comb_id;

  /* cfn at which configuration is applied */
  uint8 ccfn;
  
  /* status of the configuration */
  boolean status;
}dl_phch_cfg_done_cmd_type;

/*--------------------------------------------------------------------------
                 STRUCTURE: DL_PHCH_CFG_DB_STRUCT_TYPE

This structure contains all of the various parameters for physical channel
configuration process
--------------------------------------------------------------------------*/
typedef struct {
  /* comb_id that is used for this configuration */
  uint8 comb_id;

  /* Event ID for cell config event */
  uint8 cell_config_event_id;
  
  /* Flag to indicate cell cfg pending */
  boolean cell_cfg_processing_pending;

  /* state of the physical channel */
  dl_phch_state_enum_type state;

  /* notification phychan type */
  l1_dl_phychan_enum_type notify_phychan_type;

  /* configuration cfn at which the configuration is activated/deactivated */
  uint8 ccfn;

  /* pilot bits */
  uint8 pilot_bits;
  
  /* spreading factor */
  l1_sf_enum_type spread_factor;

  /* index points to the old config of dl_phch_cfg_db array */
  dl_phch_cfg_handle_type old_phch_cfg_idx;
  
  /* Number of radio links that contributes to the physical channel */
  uint8 num_rl;

  /* align type for this phychan*/
  seq_align_type align_type;


  /* Actions before notification */
  /* 
      Bit0 resume triage
      Bit1 unrerve combiner id
      Bit2 enable cctrch
      Bit3 free cctrch
      Bit4 free phch db
   */
  uint32 actions_bmsk;

  /* physical channel config parameters to MCAL DL Module which typically holds
      phychan type, slot format, cctrch_idx, transmit time offset, num_frame offset, 
      ref finger */
  mcalwcdma_dl_combiner_cfg_parms_struct_type cfg_params;

  /* Index to the rl_info table that holds */
  dl_phch_rl_handle_type rl_idx[DL_PHCH_MAX_RL_PER_PHCH];

  /*Structure to hold the TFN_WRC difference calculated during DPCH drop of FW TI HHO
    This will be used to calculate the parameters(SFN, CFN) needed for tlm_set_align*/
  tlm_tfn_struct_type tfn_wrc_diff;

  /*Indicate if we are to wait for order to start UL Tx on sec*/
  boolean waitOnScchOrder;
}dl_phch_cfg_db_struct_type;

extern boolean dl_phch_no_pich_fast_cfg_support;

extern boolean pccpch_setup_in_dpch_state;


extern boolean dl_first_dpch_setup;


extern uint16 dl_ref_cell_idx;

/* will be set to TRUE if any RL enabled with CLTD during DPCH setup, that need
  synchronization procedure A */
extern boolean dl_first_cltd_dpch_setup;

/* This variable provides enough room to get the filter energy above Q_IN
after we pass the CLTD Init mode. It maintains how many sync indications 
are bypassed to notice the UL got synchronized at BTS through 
FBI behavior on DL. This count is incremented up to threshold specified by 
This count is incremented up phase2 dpcch est filter length
starting from zero during CLTD init mode, once in every frame 
while evaluating DPCCH sir est */
extern uint8   dl_bypass_sync_ind_count_for_cltd_dpch_est;

/* will be set to TRUE when phase difference between the weight factors of the 
CLTD enabled RL is with in specified limits around 135 degrees and corresponding 
link CPICH engery is above specified threshold to make sure that FBI computation 
is reliable and this variable is used to bypass the DL   sychronization to establish 
DPCH, having CLTD enabled RL when it is set to TRUE */
extern boolean dl_bypass_sync_criteria_for_cltd_dpch_est;

extern l1_tx_div_mode_enum_type dl_saved_common_txdiv;

extern uint8 dl_phch_empty_cfg_id_cnt;

typedef struct
{
  /* Bmsk to keep track of avaliable free TPC combiners. 1: Free 0: Busy*/
  uint8 hw_tpc_avaliable_bmsk;
  /* HW TPC bmsk mapped on OTA TPC .Pls note one OTA TPC can have multiple HW TPC */
  uint8 hw_tpc_bmsk_per_ota_tpc_id[MCALWCDMA_MAX_TPC_INDEX];
  /* RLs bmsk mapped on HW TPC idx*/
  uint32 active_rl_bmsk_per_hw_tpc_id[MCALWCDMA_MAX_TPC_INDEX];

} dl_phch_tpc_db_struct_type;

extern rex_crit_sect_type dl_phch_pend_cfg_crit_sect;

/*===========================================================================
FUNCTION dl_phch_init

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dl_phch_init(void);


/*--------------------------------------------------------------------------
                 Operation Handler Functions
                 1. dl_phch_add
                 2. dl_phch_recfg
                 3. dl_phch_drop
                 4. dl_phch_suspend
                 5. dl_phch_resume
                 6. dl_phch_aset_upd
--------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION dl_phch_add

DESCRIPTION
  Adds a physical channel to put it in either suspended state or active state as specified in next state. Once after 
  physical channel is added call back function call will be executed

DEPENDENCIES
  None

RETURN VALUE
  physical channel configuration handle, which should be used for all 
  future communication for reconfig/suspend/resume/drop operations. Returns
  Invalid physical channeld handle if operation fails
  
SIDE EFFECTS
  None
===========================================================================*/

extern dl_phch_cfg_handle_type dl_phch_add(
  /* next state after this configuration */
  dl_phch_state_enum_type phch_next_state,

  /* add type phychan confguration */
  dl_phychan_add_type *phychan_add,

  /* call back function pointer after add operation is done */
  DL_PHCH_CFG_DONE_CB_FUNC_TYPE *phch_add_done_cb_ptr);

/*===========================================================================
FUNCTION dl_phch_recfg

DESCRIPTION
  Reconfigures a physical channel to put it in either suspended state or active state as specified in next state. Once after 
  physical channel is reconfigured call back function call will be executed

DEPENDENCIES
  None

RETURN VALUE
  physical channel configuration handle, which should be used for all 
  future communication for reconfig/suspend/resume/drop operations. Returns
  Invalid physical channeld handle if operation fails
  
SIDE EFFECTS
  None
===========================================================================*/

extern dl_phch_cfg_handle_type dl_phch_recfg(
  /* phch cfg handle that is to be reconfigured */
  dl_phch_cfg_handle_type phch_cfg_db_idx,

  /* next state after this configuration */
  dl_phch_state_enum_type phch_next_state,

  /* add type phychan confguration */
  dl_phychan_recfg_type *phychan_recfg,

  /* call back function pointer after reconfigure operation is done */
  DL_PHCH_CFG_DONE_CB_FUNC_TYPE *phch_recfg_done_cb_ptr);

/*===========================================================================
FUNCTION dl_phch_drop

DESCRIPTION
  Drops a physical channel to put it in inactive state. Once after 
  physical channel is dropped call back function call will be executed

DEPENDENCIES
  None

RETURN VALUE
  Returns Boolean to specify if drop operation successful or not. 
  
SIDE EFFECTS
  None
===========================================================================*/

extern boolean dl_phch_drop(
  /* phch cfg handle that is to be dropped */
  dl_phch_cfg_handle_type phch_cfg_db_idx,
  
  /* call back function pointer after drop operation is done */
  DL_PHCH_CFG_DONE_CB_FUNC_TYPE *phch_drop_done_cb_ptr);

/*===========================================================================
FUNCTION dl_phch_reslam

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

boolean dl_phch_reslam(
  /* phch cfg handle that is to be reconfigured */
  dl_phch_cfg_handle_type phch_cfg_db_idx,
  
  /* call back function pointer after reconfigure operation is done */
  DL_PHCH_CFG_DONE_CB_FUNC_TYPE *phch_reslam_done_cb_ptr);

/*===========================================================================
FUNCTION dl_phch_suspend

DESCRIPTION
  Suspends a physical channel to put it in suspended state. Once after 
  physical channel is suspended call back function call will be executed

DEPENDENCIES
  None

RETURN VALUE
  physical channel configuration handle, which should be used for all 
  future communication for reconfig/resume/drop operations. Returns
  Invalid physical channeld handle if operation fails
  
SIDE EFFECTS
  None
===========================================================================*/

extern boolean dl_phch_suspend(
  /* phch cfg handle that is to be suspended */
  dl_phch_cfg_handle_type phch_cfg_db_idx,

  /* fast suspend flag */
  boolean fast_suspend,

  /* call back function pointer after suspend operation is done */
  DL_PHCH_CFG_DONE_CB_FUNC_TYPE *phch_suspend_done_cb_ptr);

/*===========================================================================
FUNCTION dl_phch_resume

DESCRIPTION
  Resumes a physical channel to put it in active state. Once after 
  physical channel is resumed call back function call will be executed

DEPENDENCIES
  None

RETURN VALUE
  physical channel configuration handle, which should be used for all 
  future communication for reconfig/suspend/drop operations. Returns
  Invalid physical channeld handle if operation fails
  
SIDE EFFECTS
  None
===========================================================================*/

extern boolean dl_phch_resume(
  /* phch cfg handle that is to be resumed */
  dl_phch_cfg_handle_type phch_cfg_db_idx,
  
  /* fast resume flag */
  boolean fast_resume,

  /* call back function pointer after resume operation is done */
  DL_PHCH_CFG_DONE_CB_FUNC_TYPE *phch_resume_done_cb_ptr);

/*--------------------------------------------------------------------------
                 DL PHCH MCAL DL CFG Handler Call Back Functions defined in MCAL DL module
                 1. mcalwcdma_dl_phychan_cfg_sent_to_dsp_ind 
                 2. mcalwcdma_dl_phychan_done_ind
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                 DL PHCH CFG Handler Functions
                 1. dl_phch_cfg_done_handler
--------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION dl_phch_cfg_done_handler

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

extern void dl_phch_cfg_done_handler(
  dl_phch_cfg_done_cmd_type* cmd);

/*--------------------------------------------------------------------------
                 DL PHCH Update Fing Phch and Combiner tracking Functions
                 1. dl_phch_upd_fing_phch
                 2. dl_phch_handle_ref_fing_change
--------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION        DL_PHCH_ADD_ASET_RADIO_LINKS

DESCRIPTION     

DEPENDENCIES   

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern boolean dl_phch_add_aset_radio_links(
    /* to which cfg db location the information should be copied */
    dl_phch_cfg_handle_type phch_cfg_db_idx,
  
    /* add type phychan confguration */ 
    dl_phychan_add_type *aset_upd_info,
    
  /* call back function pointer after add operation is done */
  DL_PHCH_CFG_DONE_CB_FUNC_TYPE *phch_aset_upd_done_cb_ptr);

/*===========================================================================
FUNCTION        DL_PHCH_REMOVE_ASET_RADIO_LINKS

DESCRIPTION     

DEPENDENCIES   

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern boolean dl_phch_remove_aset_radio_links(
    /* cell db ids of the cells that are being dropped */
    uint16 dropped_cell_bmsk,
    /* cfg done cb*/
    DL_PHCH_CFG_DONE_CB_FUNC_TYPE *phch_aset_upd_done_cb_ptr,
    /*Associated channel on RL */
    l1_dl_phychan_enum_type phch
);

/*===========================================================================
FUNCTION        DL_PHCH_ALLOCATE_STMR_CTR

DESCRIPTION     

DEPENDENCIES   

RETURN VALUE    stmr counter index

SIDE EFFECTS    
===========================================================================*/
extern stmr_counter_source_enum_type dl_phch_allocate_stmr_ctr(void);

/*===========================================================================
FUNCTION        DL_PHCH_DEALLOCATE_STMR_CTR

DESCRIPTION     

DEPENDENCIES   

RETURN VALUE    status of deallocate operation

SIDE EFFECTS    
===========================================================================*/
extern boolean dl_phch_deallocate_stmr_ctr(
  /*stmr ctr that needs to be deallocated*/
  uint16 stmr_cntr_id);

/*--------------------------------------------------------------------------
                 DL PHCH Pending Config Handler Functions
                 1. dl_phch_pend_cfg_handler
--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
                 DL PHCH Holding Command Handler Functions
                 1. dl_phch_hold_cmd_handler
--------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION dl_phch_pend_cmd_handler

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dl_phch_hold_cmd_handler(
  /* combiner channel for which holding command handling needed */
  dl_phch_comb_ch_db_handle_type comb_ch_idx);

/*===========================================================================
FUNCTION dl_phch_update_pich_trans_time_offset

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dl_phch_update_pich_trans_time_offset(dl_phch_cfg_handle_type phch_cfg_db_idx, uint16 pi_pn_count, uint16 pi_offset_from_pich);

extern void dl_phch_handle_align_change(
dl_phch_cfg_handle_type phch_cfg_idx, 
seq_align_type align_type
);


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
extern uint8 dl_phch_get_hwch_from_handle(dl_phch_cfg_handle_type phch_handle);

/*===========================================================================
FUNCTION dl_phch_get_hwch_from_handle

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint8 dl_phch_get_cctrch_idx(dl_phch_cfg_handle_type phch_handle);

/*===========================================================================
FUNCTION dl_phch_get_slot_format_idx

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint8 dl_phch_get_slot_format_idx(dl_phch_cfg_handle_type phch_handle);

/*===========================================================================
FUNCTION dl_phch_get_pilot_bits

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint8 dl_phch_get_pilot_bits(dl_phch_cfg_handle_type phch_handle);

/*===========================================================================
FUNCTION dl_phch_get_spread_factor

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern l1_sf_enum_type dl_phch_get_spread_factor(dl_phch_cfg_handle_type phch_handle);

/*===========================================================================
FUNCTION dl_phch_get_cfg_finger

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint8 dl_phch_get_cfg_finger(dl_phch_cfg_handle_type phch_handle);

/*===========================================================================
FUNCTION dl_phch_get_transmit_time_offset

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint8 dl_phch_get_phch_tto(dl_phch_cfg_handle_type phch_handle);

/*===========================================================================
FUNCTION dl_phch_get_active_phch_handle_mask

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint16 dl_phch_get_active_phch_handle_mask(void);

/*===========================================================================
FUNCTION dl_phch_get_suspended_phch_handle_mask

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint16 dl_phch_get_suspended_phch_handle_mask(void);

/*===========================================================================
FUNCTION dl_phch_is_rl_configured_with_cltd

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_phch_is_rl_configured_with_cltd(uint8 cell_db_idx, uint8 comb_id);

/*===========================================================================
FUNCTION dl_channel_get_psc

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

extern uint16 dl_channel_get_psc(
  dl_phch_cfg_handle_type phch_handle);

/*===========================================================================
FUNCTION dl_phch_get_cell_info_from_handle

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dl_phch_get_cell_info_from_handle(
  dl_phch_cfg_handle_type phch_handle, uint16 *freq, uint16 *psc);

/*===========================================================================
FUNCTION dl_phch_get_ch_off_from_handle

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dl_phch_get_ch_off_from_handle(dl_phch_cfg_handle_type phch_handle,
  uint16 *fr_off, uint32 *chip_off);

/*===========================================================================
FUNCTION dl_phch_is_phch_suspended

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_phch_is_phch_suspended(dl_phch_cfg_handle_type phch_hdle);

/*===========================================================================
FUNCTION dl_phch_is_phch_suspended

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_phch_is_phch_inactive(dl_phch_cfg_handle_type phch_hdle);

/*===========================================================================
FUNCTION dl_phch_is_phch_suspended

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_phch_is_phch_active(dl_phch_cfg_handle_type phch_hdle);

/*===========================================================================
FUNCTION        dl_phch_aset_update

DESCRIPTION     This function will be used to update the rl db for all RRC 
                initiated aset update procedure

DEPENDENCIES    None

RETURN VALUE
                TRUE: aset update was successful
                FALSE: aset update was failure

SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_phch_aset_update(dl_phch_cfg_handle_type phch_cfg_db_idx, dl_phychan_add_type *aset_upd_info);

/*===========================================================================

FUNCTION        DL_PHCH_GET_TTO_FROM_CELL_PHCH_IDX

DESCRIPTION     This function gets the TTO information from the cell phch index

                
DEPENDENCIES    Unknown

RETURN VALUE    The TTO information of the channel whose phch handle is passed

SIDE EFFECTS    Unknown

===========================================================================*/
extern uint16 dl_phch_get_rl_tto(dl_phch_cfg_handle_type phch_hdle, uint8 cell_db_idx);

/*===========================================================================
FUNCTION        dl_phch_handle_hs_fing_cfg_post_aset

DESCRIPTION     This function will handle hs finger config following an aset
                update

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dl_phch_handle_hs_fing_cfg_post_aset(void);
/*===========================================================================
FUNCTION        dl_phch_get_txdiv_info_for_cell

DESCRIPTION     This function returns the tx diversity mode for a given dpch_handle 
                and cell db idx
                
DEPENDENCIES    dpch_handle & cell db idx should be valid 

RETURN VALUE    tx diversity present for the given phch handle

SIDE EFFECTS    None
===========================================================================*/
extern mcalwcdma_demod_tx_div_mode_enum_type dl_phch_get_txdiv_info_for_cell(
  /*handle to access the dl_phch_cfg_db*/
  dl_phch_cfg_handle_type dpch_handle,
  /*cell db index*/
  uint8 cell_db_idx);


/*===========================================================================
FUNCTION        dl_phch_get_txdiv_status_for_hs

DESCRIPTION     This function returns the tx diversity for dpch. It would be used
                for HS
                                
DEPENDENCIES    dpch handle should exist(in other words dpch should have an entry 
                in the dl _phch_cfg_db

RETURN VALUE    a mask containing the txdiv status

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_phch_get_txdiv_status_for_hs(void);
/*===========================================================================
FUNCTION        dl_phch_query_rl_idx_for_psc

DESCRIPTION     This function returns the rl idx given a psc
                
DEPENDENCIES    None

RETURN VALUE    Valid rl idx if the psc is found

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_phch_query_rl_idx_for_psc(uint16 psc);

/*===========================================================================
FUNCTION        dl_phch_get_hwch_for_phch

DESCRIPTION     This function returns the hwch/combiner for a given physical channel
                
DEPENDENCIES    None

RETURN VALUE    Valid comb id if fund
SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_phch_get_hwch_for_phch(l1_dl_phychan_enum_type phch_type);

/*===========================================================================
FUNCTION dl_phch_get_phch_handle_from_hwch
 
DESCRIPTION
  None
 
DEPENDENCIES
  None
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern dl_phch_cfg_handle_type dl_phch_get_phch_handle_from_hwch(uint8 hwch);

/*===========================================================================
FUNCTION        dl_phch_hho_retrive_phch_params_info

DESCRIPTION     This function retrives necessary phch params which may be used for
                resuming back to old config upon HHO failure.

DEPENDENCIES    This function should be called only if we are resuming to old config
                upon failure of dch-dch HHO

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dl_phch_hho_retrive_phch_params_info(void);

/*===========================================================================
FUNCTION        DL_CALC_SLOT_FORMAT

DESCRIPTION     This function computes the slot format index as needed by
                the mDSP interface for setting up the SCCPCH, DPCH, and
                PDSCH physical channels.

DEPENDENCIES    None

RETURN VALUE    The slot format index based on the channel type, numboer
                of or existence of pilot bits, tfci existence, and the
                spreading factor.

SIDE EFFECTS    None.
===========================================================================*/
extern uint8 dl_calc_slot_format
(
  /* the channel that the slot format is being computed for.  Valid
     channels are the S-CCPCH, DPCH, and PDSCH */
  l1_dl_phychan_enum_type ch,

  /* spreading factor */
  l1_sf_enum_type sf,

  /* tfci existence */
  boolean tfci_exists,

  /* number of pilot bits */
  uint8 num_pilot_bits
);


/*===========================================================================
FUNCTION        DL_CALC_SCCPCH_SLOT_FORMAT

DESCRIPTION     This function computes the slot format for the SCCPCH as
                needed by the mDSP interface.

DEPENDENCIES    None

RETURN VALUE    The slot format index

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_calc_sccpch_slot_format
(
  l1_sf_enum_type sf,

  boolean tfci_exists,

  boolean pilot_exists
);

/*===========================================================================
FUNCTION        DL_CALC_DPCH_SLOT_FORMAT

DESCRIPTION     This function computes the slot format for the DPCH as
                needed by the mDSP interface.

DEPENDENCIES    None

RETURN VALUE    The slot format index

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_calc_dpch_slot_format
(
  l1_sf_enum_type sf,

  boolean tfci_exists,

  uint8 num_pilot_bits
);

/*===========================================================================
FUNCTION        DL_GET_SF_FROM_CMD

DESCRIPTION     This function returns the spreading factor for a physical
                channel referred to in the passed command.

DEPENDENCIES    RRC phychan ordered config database

RETURN VALUE    The spreading factor enum

SIDE EFFECTS    None
===========================================================================*/
extern l1_sf_enum_type dl_get_sf_from_ord_cfg(
  /* the physical channel being added */
  l1_dl_phychan_enum_type phch,

  /* pointer to the dl_phychan_db part of ordered config */
  l1_dl_phychan_db_struct_type *phychan_db
);


/*===========================================================================
FUNCTION        DL_GET_DPCH_TPC_BITS

FILE NAME       dlphch.c

DESCRIPTION     This function returns the number of TPC bits used on a
                passed in DPCH physical channel.

DEPENDENCIES    The phychan database.  Note the channel must be in the
                PHYCHAN_ACTIVE state in order to return the correct value.

RETURN VALUE    Either the number of TPC bits used in DPCH channel or
                0xFF indicating a failure.

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_get_dpch_tpc_bits(void);


/*===========================================================================
FUNCTION        DL_PHCH_GET_WCB_SIZE_FROM_ORD_CFG

DESCRIPTION     returns WCB size for the given physical channel type and configuration parameters.

DEPENDENCIES    None

RETURN VALUE    WCB size

SIDE EFFECTS    None
===========================================================================*/
extern uint32 dl_phch_get_wcb_size_from_ord_cfg(
  /* the type of operation, ADD or CFG */
  l1_setup_ops_enum_type op,

  /* which physical channel is being referred to */
  l1_dl_phychan_enum_type phch,

  /* pointer to the RRC ordered config */
  l1_dl_phychan_db_struct_type *phychan_db
);


/*===========================================================================
FUNCTION        dl_phch_any_active_channels_on_cell

DESCRIPTION     This function checks if there are any active channels on the given cell_db_index

DEPENDENCIES    None

RETURN VALUE    TRUE if there is an active channel on the given cell_db_index. Else returns FALSE.

SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_phch_any_active_channels_on_cell(uint8 cell_db_index);

/*===========================================================================
FUNCTION        DL_CHECK_CLTD_RL_CPICH_ENG_ABOVE_THRESH

FILE NAME       dldem.c

DESCRIPTION     This function returns TRUE if, any CLTD enabled RL in ASET has 
                combined cpich energy(primary+diversity) exceeds the 
                DL_CLTD_RL_CPICH_ENG_THRESH.
                
DEPENDENCIES    None

RETURN VALUE    Boolean

SIDE EFFECTS    None
===========================================================================*/

extern boolean dl_check_cltd_rl_cpich_eng_above_thresh(void);

/*===========================================================================
FUNCTION        DL_CHECK_ALL_RL_DIV_ENABLED

FILE NAME       dlphch.c

DESCRIPTION     This function checks to see if all the RL have some kind
                of (either STTD or CLTD) diversity enabled

DEPENDENCIES    Assumes the check is only for active RLs only

RETURN VALUE    TRUE if all RL have div enabled
                FALSE otherwise (ie atleast one RL is not div enabled)

SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_check_all_rl_div_enabled(void);

#ifdef FEATURE_L1_LOG_ON_DEMAND
/*===========================================================================

FUNCTION     DL_PROCESS_COMMON_PHYCHAN_LOG_ON_DEMAND

DESCRIPTION
	Performs log on demand for down link common physical channels

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void dl_process_common_phychan_log_on_demand(void);
#endif

#ifdef FEATURE_L1_LOG_ON_DEMAND
/*===========================================================================

FUNCTION     DL_PROCESS_DED_PHYCHAN_LOG_ON_DEMAND

DESCRIPTION
	Performs log on demand for down link dedicated physical channel

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void dl_process_ded_phychan_log_on_demand(void);
#endif

/*===========================================================================
FUNCTION        DL_PHCH_ALIGN_TO_FRC

FILE NAME       dlphch.c

DESCRIPTION     

DEPENDENCIES    None

RETURN VALUE    

SIDE EFFECTS    None
===========================================================================*/
extern void dl_phch_align_to_frc(void);

/*===========================================================================
FUNCTION        DL_PHCH_GET_ORDERED_SCPICH_STATUS

FILE NAME       dlphch.c

DESCRIPTION     This function returns the SPICH status for given RL (PSC)
                from the ordered config if available. Else it checks the cell
                pointer and returns the SPICH status found.
                
DEPENDENCIES    None

RETURN VALUE   None

SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_phch_get_ordered_scpich_status(
                   /* CPHY_SETUP_REQ command pointer */
                  l1_setup_cmd_type *cmd, 
                  uint16 psc);

/*===========================================================================
FUNCTION        DL_PHCH_GET_ORDERED_TD_STATUS

FILE NAME       dlphch.c

DESCRIPTION     This function returns the TD status for given RL (PSC)
                from the ordered config if available. Else it checks DL db 
                for this RL and returns the TD status found.
                
DEPENDENCIES    None

RETURN VALUE   None

SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_phch_get_ordered_td_status(
                 /* CPHY_SETUP_REQ command pointer */
                l1_setup_cmd_type *cmd,
                /* PSC */
                uint16 psc);

/*===========================================================================
FUNCTION        DL_PHCH_UPDATE_CLTD_ENABLE_FLAG

DESCRIPTION     This function is used to update the global flag indicating the enable/disable status of cltd. It also
                flushes the cltd logging buffer if there is change in cltd status from enable->disable

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dl_phch_update_cltd_enable_flag(void);

/*===========================================================================
FUNCTION        DL_PHCH_GET_CLTD_ENABLE_STATUS

DESCRIPTION     This function is used to return the global flag indicating the enable/disable status of cltd.

DEPENDENCIES    None

RETURN VALUE    TRUE if cltd is enabled, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
boolean dl_phch_get_cltd_enable_status(void);

/*===========================================================================
FUNCTION        dl_phch_log_strm_ctr_info

DESCRIPTION     This function exports the stmr counter information. The information
                is formatted to support the TA logging.

DEPENDENCIES   

RETURN VALUE    copy the stmr counter info for logging for all the combiners.

SIDE EFFECTS    
===========================================================================*/
extern void dl_phch_log_strm_ctr_info(uint32 *stmr_ctr_info_ptr);

/*===========================================================================
FUNCTION        DL_PHCH_GET_DPCH_TIMING

FILE NAME       dlphch.c

DESCRIPTION     This function is used to return the DPCH channel timing in cx8

DEPENDENCIES    None

RETURN VALUE   DPCH channel timing in cx8

SIDE EFFECTS    None
===========================================================================*/
extern uint32 dl_phch_get_dpch_timing(void);

/*===========================================================================
FUNCTION        DL_GET_DATA_BITS_PER_FRAME

FILE NAME       dlphch.c

DESCRIPTION     This function returns how many bits are available per frame
                in the physical channel referenced by the passed command.

                If an error condition exists, i.e.
                No channel is referenced in the command; or
                A channel is referenced as a cfg channel but that channel
                  was not active
                then 0 is returned and it is left to the caller to further
                handle the error.


DEPENDENCIES    The physical channel database

RETURN VALUE    Either the number of bits available per frame or 0 if an
                error condition is present.

SIDE EFFECTS    None
===========================================================================*/
uint16 dl_get_data_bits_per_frame
(
  /* the type of operation, ADD or CFG */
  l1_setup_ops_enum_type op,

  /* which physical channel is being referred to */
  l1_dl_phychan_enum_type,

  /* pointer to the RRC ordered config */
  l1_dl_phychan_db_struct_type *phychan_db
);

/*===========================================================================
FUNCTION        DL_PHCH_UPDATE_ALIGN_CELL_INFO

FILE NAME       dlphch.c

DESCRIPTION     This function is used to update the Aligned cell's chip offset (fr, cx8)

DEPENDENCIES    None

RETURN VALUE   None

SIDE EFFECTS    None
===========================================================================*/
extern void dl_phch_update_align_cell_info(tlm_cell_info_struct_type *cell_info);

/*===========================================================================
FUNCTION dl_phch_get_cltd_slot_timing_mode

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
uint8 dl_phch_get_cltd_slot_timing_mode(uint8 cell_db_idx);

/*===========================================================================
FUNCTION        DL_PHCH_IS_FDPCH

DESCRIPTION     This function reports whether the channel is associated with FDPCH

DEPENDENCIES    None

RETURN VALUE    TRUE if channel is associated with FDPCH, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
boolean dl_phch_is_fdpch_enabled(void);

/*===========================================================================
FUNCTION        DL_PHCH_IS_SSC_ENABLED

DESCRIPTION     This function reports whether SSC is in use ?

DEPENDENCIES    None

RETURN VALUE    TRUE if SSC is enabled FDPCH, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
boolean dl_phch_is_ssc_enabled(void);

/*===========================================================================
FUNCTION    dl_phch_set_demod_config_pending_during_pich_setup

DESCRIPTION This function sets/resets demod cfg pending bit

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
extern void dl_phch_set_demod_config_pending_during_pich_setup(boolean set_cfg_pending);

/*===========================================================================
FUNCTION    dl_phch_send_pending_demod_config_cmd

DESCRIPTION This function send pending comit if needed

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
extern void dl_phch_send_pending_demod_config_cmd(void);

/*===========================================================================
FUNCTION    dl_phch_pich_fast_configure()

DESCRIPTION This function configures PICH in ASAP mode

DEPENDENCIES None

RETURN VALUE TRUE if it is successful configuration

SIDE EFFECTS None
===========================================================================*/
extern boolean dl_phch_pich_fast_configure(
  /* phch cfg handle that is to be resumed */
  dl_phch_cfg_handle_type phch_cfg_db_idx,
  /* TRUE for enable or FALSE for disable */
  boolean pich_enable, 
  /* send explicit comit incase of failure setup */
  boolean cell_cfg_needs_comit,
  /* call back function pointer after suspend operation is done */
  DL_PHCH_CFG_DONE_CB_FUNC_TYPE *pich_configure_cb_ptr  
);

/*===========================================================================
FUNCTION        DL_PHCH_GET_DPCH_SF

DESCRIPTION     This function returns the spread factor used for the DPCH   
                channel.

DEPENDENCIES    The phychan database.  Note the channel must be in the
                PHYCHAN_ACTIVE state in order to return the correct value.

RETURN VALUE    Either the spreading factor used in DPCH channel or
                L1_NUM_SF (value of 8) indicating a failure.

SIDE EFFECTS    None
===========================================================================*/
extern l1_sf_enum_type dl_phch_get_dpch_sf(void);

/*===========================================================================
FUNCTION        DL_PHCH_GET_DPCH_NUM_TFCI_BITS

DESCRIPTION     This function returns the spread factor used for the DPCH   
                channel.

DEPENDENCIES    The phychan database.  Note the channel must be in the
                PHYCHAN_ACTIVE state in order to return the correct value.

RETURN VALUE    Either the spreading factor used in DPCH channel or
                L1_NUM_SF (value of 8) indicating a failure.

SIDE EFFECTS    None
===========================================================================*/

extern uint8 dl_phch_get_dpch_num_tfci_bits(void);

/*===========================================================================
FUNCTION        DL_PHCH_GET_DPCH_NUM_PILOT_BITS

DESCRIPTION     This function returns the pilot bits used for the DPCH   
                channel.

DEPENDENCIES    The phychan database.  Note the channel must be in the
                PHYCHAN_ACTIVE state in order to return the correct value.

RETURN VALUE    Either the num pilot bits used in DPCH channel or
                0xFF indicating a failure.

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_phch_get_dpch_num_pilot_bits(void);

/*===========================================================================
FUNCTION        dl_phch_hw_tpc_init

DESCRIPTION     This function Initializes TPC HW db

DEPENDENCIES   

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
void dl_phch_hw_tpc_init(void);


/*===========================================================================
FUNCTION    dlphch_set_pich_auto_sccpch_bmsk

DESCRIPTION  This function sets the auto sccpch bmsk

DEPENDENCIES Use only for PICH and SCCPCH0

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
extern void dlphch_set_pich_auto_sccpch_bmsk(
  dl_phch_cfg_handle_type phch_cfg_db_idx);

/*===========================================================================
FUNCTION    dlphch_clear_pich_auto_sccpch_bmsk

DESCRIPTION  This function clears the auto sccpch bmsk

DEPENDENCIES Use only for PICH and SCCPCH0

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
extern void dlphch_clear_pich_auto_sccpch_bmsk(
  dl_phch_cfg_handle_type phch_cfg_db_idx);

/*===========================================================================
FUNCTION        DL_PHCH_STMR_CTR_TT_HANDLER

DESCRIPTION     Called by maintenance event handler every frame for STMR   
                time tracking 

DEPENDENCIES   

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void dl_phch_stmr_ctr_tt_handler(void);

#ifdef FEATURE_WCDMA_HS_RACH
/*===========================================================================
FUNCTION    dl_phch_fill_decoded_hsrach_fpdch_params

This function copies OVSF and TTO inferred from the EDCH common resource index 
allocated to the UE.

DESCRIPTION  

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
extern boolean dl_phch_fill_decoded_hsrach_fpdch_params(
  uint16 ovsf, 
  uint32 trans_time_offset
  );

/*===========================================================================
FUNCTION    dl_phch_is_hsrach_fdpch_active

This function returns TRUE if HS RACH FDPCH is active at FW.

DESCRIPTION  

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
extern boolean dl_phch_is_hsrach_fdpch_active(void);

/*===========================================================================
FUNCTION    dlphch_activate_hs_rach_fdpch

DESCRIPTION  

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
extern void dlphch_activate_hs_rach_fdpch(void);

/*===========================================================================
FUNCTION    dlphch_activate_hs_rach_fdpch_evt_handler
This performs the necessary actions needed once HS RACH FPDCH is actve at FW.

DESCRIPTION  

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
extern void dlphch_activate_hs_rach_fdpch_evt_handler(void);

extern void dlphch_reset_hsrach_fpdch_status(void);
#endif /* FEATURE_WCDMA_HS_RACH */

/*===========================================================================
FUNCTION    dl_phch_enable_dbg_msgs
Controls the enable/disable of dbg f3s

DESCRIPTION  

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
extern void dl_phch_enable_dbg_msgs(boolean enable);

/*===========================================================================
FUNCTION    dl_set_dl_phch_nv_values
  Function controls enabling of debug F3s for dl_phch

DESCRIPTION  

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
extern void dl_set_dl_phch_nv_values(void);

/*===========================================================================
FUNCTION        DL_PHCH_GET_DOFF

FILE NAME       dlphch.c

DESCRIPTION     This function returns the DOFF based on the DPCH/F-DPCH channel

DEPENDENCIES    None

RETURN VALUE   TRUE: If DPCH/F-DPCH is active
               FALSE: Otherwise

SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_phch_get_doff(tlm_cfn_struct_type *doff);

#ifdef FEATURE_WCDMA_DC_HSUPA
/*===========================================================================
FUNCTION        dl_phch_scch_order_sec_fdpch_ops_handler

FILE NAME       dlphch.c

DESCRIPTION     This function calls the dlphch API's to perform DL operations
                upon reception of hs scch order to activate/deactivate DC-HSUPA                     


DEPENDENCIES    None

RETURN VALUE    None
               
SIDE EFFECTS    None
===========================================================================*/
extern void dl_phch_scch_order_sec_fdpch_ops_handler(boolean scch_ord_to_activate);

/*===========================================================================
FUNCTION        dl_phch_get_sec_fdpch_activation_status

FILE NAME       dlphch.c        

DESCRIPTION     This function is used to query secondary FDPCH channel 
                activation status

DEPENDENCIES    None

RETURN VALUE   TRUE if DC-HSUPA is activated
               
SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_phch_get_sec_fdpch_activation_status(void);
#endif /* FEATURE_WCDMA_DC_HSUPA */

/*===========================================================================
FUNCTION        dl_phch_send_mod_timing

FILE NAME       dlphch.c

DESCRIPTION     This function is used to send the mod timing command to FW for DPCH cases only

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dl_phch_send_mod_timing(dl_phch_cfg_handle_type phch_cfg_idx, seq_align_type align_type);

/*===========================================================================
FUNCTION        dl_phch_get_ch_stmr_action_time

FILE NAME       dlphch.c

DESCRIPTION     This function is used to calculate the stmr action time(wrc) based on the start cfn and action bpg

DEPENDENCIES    None

RETURN VALUE    STMR Action time in WRC

SIDE EFFECTS    None
===========================================================================*/
extern uint32 dl_phch_get_ch_stmr_action_time(uint8 start_cfn, uint8 action_bpg);

/*===========================================================================
FUNCTION        dl_phch_get_ch_stmr_action_time

FILE NAME       dlphch.c

DESCRIPTION     This function is used to calculate the stmr action time(wrc) based on the start cfn and action bpg

DEPENDENCIES    None

RETURN VALUE    STMR Action time in WRC

SIDE EFFECTS    None
===========================================================================*/
extern int8 dl_phch_calculate_rg_stmr_bpg_adj(uint32 rgchactiontime, uint32 hichactiontime);

/*===========================================================================
FUNCTION    l1_get_phy_chan_info

DESCRIPTION  API to provide Physical channel related parameters to RRC

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
extern void l1_get_phy_chan_info(l1_active_set_info_type* l1_active_set_info);

/*===========================================================================
FUNCTION        dl_phch_is_reslam_pending_for_x2w_hho

FILE NAME       dlphch.c

DESCRIPTION     This function returns the status of dpch reslam pending in x2w hho cases

DEPENDENCIES    None

RETURN VALUE    TRUE if reslam is pending, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/

extern boolean dl_phch_is_reslam_pending(void);

/*===========================================================================
FUNCTION        dl_phch_query_rl_idx_for_psc

DESCRIPTION     This function returns the rl idx given a psc
                
DEPENDENCIES    None

RETURN VALUE    Valid rl idx if the psc is found

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_phch_query_sec_rl_idx_for_psc(uint16 psc);

/*===========================================================================
FUNCTION        dl_phch_set_mod_timing_post_cb

FILE NAME       dlphch.c

DESCRIPTION     This is the post cb for the set mod timing cmd sent to FW. 
                It will enable the Tx stmr events disabled as part of set align for Tx TL.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dl_phch_set_mod_timing_post_cb(uint16 status);

/*===========================================================================
FUNCTION dl_phch_get_phch_handle

DESCRIPTION
 This function finds the phch-handle(i.e dl_phch_cfg_db index) of the channel 
                identified by physical channel type.

DEPENDENCIES    This function should only be called for channels that can have 
                a maximum of one instantiation at any point of time(eg: PICH, AICH).
                It should not be called for channels that can have more than 
                one instantiation at a time(eg: SCCPCH)

RETURN VALUE
                dyn-db-idx: The dynamic db idx of the channel

SIDE EFFECTS
  None
===========================================================================*/
extern dl_phch_cfg_handle_type dl_phch_get_phch_handle(l1_dl_phychan_enum_type phch_type);
#endif /* DLPHCH_H */
