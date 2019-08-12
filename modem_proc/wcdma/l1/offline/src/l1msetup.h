#ifndef L1MSETUP_H
#define L1MSETUP_H
/*===========================================================================

      L 1 M   P H Y S I C A L    C H A N N E L    S E T U P    D E C L S

GENERAL DESCRIPTION
  This file contains the declarations and function prototypes for
  the Layer 1 Manager physical channel setup procedure, the state
  machine for Layer 1 and procedures for cell transition.

EXTERNALIZED FUNCTIONS

  l1m_cphy_setup_req_cmd
    This function handles the CPHY_SETUP_REQ primitive from L2/L3 and is
    called by L1M when it receives a CPHY_SETUP_REQ command. This function
    will first save the request and validates any state transition that is
    possible as a result of this request. If the state transition is valid,
    it performs parameter validation. Each of the Downlink and Uplink modules
    are called inturn to validate the parameters. The modules are required to
    validate the parameters and also make sure they have enough resources to
    complete the request. The request mask is then saved and the current
    state of L1 is changed to the new state (if the request results in a
    state transition). The commands in the request mask are then executed in
    order.

    The state validation function return depends on the physical channels
    currently setup and also depends on the new physical channel adds/drops
    in the command packet.

    The l1m_state variable is changed to the new state if there is a state
    transition possible. The cleanup routine of the current state is called
    followed by the initialization routine of the new state.

  l1m_send_cphy_setup_cnf_to_rrc
    This function sends the RRC_CPHY_SETUP_CNF primitive to RRC, which
    includes status on each operation performed and an overall status.

    This function is called from l1m_cphy_setup_next_cmd_handler(), in the
    CPHY_SETUP_DONE case, if MAC is not processing a corresponding config
    request next, which happens in the case of a MAC command with an
    activation time.  It is also called from l1m_cphy_mac_config_cnf(),
    which means MAC has finished processing its config request as well as L1.

  l1m_cphy_setup_next_cmd_handler
    This function determines the next command to be handled that is part
    of the physical channel setup command. It is called first from the
    physical channel setup command handler when the CPHY_SETUP_REQ command
    is first received by L1M, and then subsequently by the Local Command
    Handler that sequences the physical channel setup procedure.

    This function determines the next command to be handled in the physical
    channel setup procedure and then calls the corresponding handler.

  l1m_cphy_cell_transition_req_cmd
    This function handles the CPHY_CELL_TRANSITION_REQ primitive.
    This function saves the cell transition request and then sends
    an ASET update command to Search module. The rest of the cell
    transition command is processed after search completes the ASET
    update and sends a local command to L1M.

  l1m_phychan_aset_done
    This function is called after an ASET update is done by the Search module.
    Based on the update type, we take the following actions.

    If the update type is cell reselection, it means we have to set up a
    neighbour BCH for RRC.

    If the update type is cell camp, it means we are doing a cell transition.

  l1m_cell_trans_drop_done
    This function is called by L1M after it receives a local command
    from the DL when it completes dropoing all the DL channels.
    This function will clean up the current state of L1 and then perform
    a state transition. If the clean up of the modules complete immediately,
    a state transition is done from this function, otherwise L1M waits
    for a MODULE_CLEANUP_DONE_CMD from the modules before performing
    a state transition.

  l1m_cell_trans_next_cmd_handler
    This function determines the next command to be handled that is part
    of the cell transition request physical channel setups. This function
    is called after we perform a state transition and we drop all existing
    channels.
    After all the commands have been handled, a confirmation is sent to
    RRC indicating the overall status of the cell transition request.

  l1m_dl_cctrch_setup_done_cmd
    This function handles the next step after the DL CCTrCh is set up.
    If the DL CCTrCh setup fails, it sends a confirmation to RRC, otherwise
    it handles the next command in the CPHY_SETUP_REQ command.

  l1m_ul_phychan_drop_done_cmd
    This function handles the next step after the UL Physical Channel
    was dropped. If a PRACH was dropped, we proceed to drop the DL
    AICH also, otherwise the next command in the CPHY_SETUP_REQ command
    is processed.

  l1m_dl_phychan_add_done_cmd
    This function handles the next step after a DL physical channel was
    added. If an AICH physical channel was added, we proceed to add the
    UL PRACH, otherwise the next command in the CPHY_SETUP_REQ command
    is processed.

INTERNALIZED FUNCTIONS

  l1m_validate_state_trans
    This function is called during the physical channel setup command
    (CPHY_SETUP_REQ). This function will check to see if the physical
    channel setup or teardown will result in a state transition.

    A state transition is possible when new CCTrCh's are added in L1, during
    the case of a physical channel add or config.
    A transition is also possible when CCTrCh's are dropped, when a physical
    channel with which it is associated with is dropped.

    The function creates a mask based on CCTrCh "adds" and/or "drops" that
    will determine what the next state is going to be after this physical
    channel setup request completes successfully. Only certain state
    transitions are permitted within a particular L1 state. This function
    determines if the next state mask is within the set of allowed CCTrCh
    combinations for the current state.

    The actual state transition is made after the CPHY_SETUP_REQ command
    validation completes successfully.

  l1m_get_next_cphy_setup_cmd_id
    This function returns the ID of the next command to be processed as
    part of the CPHY_SETUP_REQ command.

    The request mask in the l1_setup_cmd packet has the following format:

    Bit #  Command
    -----  -------
    0      CPHY_DL_CCTRCH_REQ: setup DL CCTRCH
    1      CPHY_UL_CCTRCH_REQ: setup UL CCTRCH
    2      CPHY_UL_PHYCHAN_DROP_REQ: drop UL physical channel
    3      CPHY_DL_PHYCHAN_DROP_REQ: drop DL physical channel
    4      CPHY_DL_PHYCHAN_CFG_REQ: config DL physical channel
    5      CPHY_UL_PHYCHAN_CFG_REQ: config UL physical channel
    6      CPHY_DL_PHYCHAN_ADD_REQ: add DL physical channel
    7      CPHY_UL_PHYCHAN_ADD_REQ: add UL physical channel
    8-15   Reserved

  save_cphy_setup_req
    This function copies contents of cmd into l1_setup_cmd. Pointer values
    are saved as is, instead of copying all the contents of the pointer. The
    assumption is that L2/L3 will not overwrite those pointers until they
    receive a confirm or error indication for a command.

  l1m_validate_cell_trans
    This function validates the channel setup parameters that are sent
    as part of the cell transition request command. The request mask
    in the command is used to validate each of the sub-primitives in
    the command. The request mask is the same as the one used for
    CPHY_SETUP_REQ commands.

    In the event of a failed validation, a confirmation is sent to RRC
    with the status set to FALSE.

    If the validation passes, we first drop the UL channels and then
    drop the DL channels. The DL module will post a local command
    after all the DL physical channels are dropped.

  save_cphy_cell_trans_req
    This function saves the cell transition request command sent by the RRC
    into a global buffer in L1. This saved command is later used after
    the ASET update completes when we need to reconfigure the physical
    channels.

  l1m_dl_cctrch_validate
    This function validates the Downlink CCTrCh setup request.

  l1m_dl_cctrch_setup
    This function calls the Downlink module to set up the DL CCTrCh.

  l1m_ul_cctrch_setup
    This function calls the uplink module to set up the UL CCTrCh.
    If the request is to set up an UL PRACH, then we first set up
    the DL AICH.

  l1m_ul_cctrch_setup_cell_trans
    This function calls the uplink module to set up the UL CCTrCh.
    If the request is to set up an UL PRACH, then we first set up
    the DL AICH.
    This function is only called during channel setup during
    a cell transition after a cell reselection.

  l1m_setup_next_cmd
    This function handles the next command after a step in the physical
    channel setup procedure. Appropriate functions are called depending
    on whether we are setting up physical channels as a result of
    CPHY_CELL_TRANSITION_REQ or as a result of CPHY_SETUP_REQ.

  l1m_start_cphy_setup_cmd
    This function is called before the start of the physical
    channel setups. This function will perform state
    transition if it needs to be done, otherwise it starts
    the channel setup process.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2000-2014 by Qualcomm Technologies, Inc.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/l1msetup.h_v   1.14   19 May 2002 03:25:28   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/l1msetup.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/24/15   ap      Initial check-in for WL1 QSH framework 
02/05/15   nd      Trigger connected mode cleanup while resuming to old
                   config in case of HHO failure.
12/31/14   kr      check for Voice is up from setup mask instead of mcpmdrv mask
11/24/14   gsk     SRCHZZ wakeup cleanup changes
08/21/14   mit     Fixed compiler warnings
08/06/14   sr/rsr  Changes to handle nbr QICE cell drop for cell resel to that cell.
07/31/14   sr      Fixed low level warnings
06/27/14   jhl     HS ASAP Start optimization and recfg removal    
06/23/14   sad     Moving DC-HSUPA specific code under FEATURE_WCDMA_DC_HSUPA flag
06/16/14   ash/pr  Enhance api to check if DL phychan is pending for a specific carr idx
05/28/14   pr      Add MCPM op as part of setup action mask for the prestart cphy setup cmd
05/13/14   ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
07/01/14   amj     Clean up old ASET update code
01/06/14   sr      Suspending periodic ops before CPHY req processing
01/03/13   gsk     Reverting the changes to delay cphy setup until L1M wakeup
10/29/13   sr/gsk  During cell reselection, delaying CPHY setup cnf for NBCH drop untill
                   serv cell is configured and nbr cell is deconfigured,
10/29/13   sad     Inital check in for Secondary ASET update changes
10/14/13   pr/sr   Channel config optimizations
09/12/13   jd      Searcher redesign
08/28/13   pr      Validation check for CPHY paramter's
08/14/13   sad     Changes to state cleanup bitmask handling
07/11/13   sad     Added support to Freeze TxAccum on sec for call spike and 
                   re-configurations
07/09/13   kcm     Adding function to determine if SUSPENDED trans is needed during HHO
07/09/13   sad     Fix to clear NBCH set on cell trans failure
06/27/13   sad     Added support for secondary Tx RL failure/Phychan estb. failure
06/18/13   kcm     Cleaning up few macros and featurization changes for DC HSDPA.
06/07/13   ms      Event reporting changes for L1M redesign
05/21/13    sad     Added DC HSUPA code changes under FEATURE_WCDMA_DC_HSUPA 
05/20/13   sad     Support for Enhanced Layer-1 Manager
03/26/13   sad     Changes to handle new RRC-L1 interface.
09/04/12   vr      Calling L1-MAC API to indicate MAC that there is a cell reselection
                   ongoing and HS RACH being STOPPED and STARTED
08/07/12   dp      W2L PSHO time improvements
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/06/12   vr      HSRACH fixes
06/29/12   kcm     Changes for X2W PSHO
06/13/12   kcm     Trigger DL opts for X2W HHO only for R99 config.
06/11/12   vs      MBMS feature cleanup.
06/08/12   gv      Feature HS RACH Initial code check in
04/19/12   stk     When HSFACH is configured send CELL_TRANS_CNF to RRC only after 
                   MAC_DL_CONFIG_CNF is received.
02/10/12   rgn     Nikel feature cleanup
02/01/12   gnk     Nikel feature cleanup
11/30/11   rc      Added DSR support under FEATURE_WCDMA_DSR_SUPPORT
07/28/11   ms      Externed l1_hsdpa_req_mask
03/11/11   dp      Ported CPC DRX changes to offline Architecture
09/02/10   gv      Code Checkin for CPC DTX
08/08/10   sj      Added unified PLT support
05/03/10   rc      Added function prototype.
03/24/10   sv      Fixed previous check-in error. 
03/23/10   mc      Externed function to return status of ongoing MIMO SCPICH config.
11/09/09   hk      Fixed the compilation error
04/17/09   vp      Maintain HHO related information in seperate flags for CM
                   which are maintained from an early point in the cphy_setup
                   processing.
05/25/09   hk      Fixed compiler warnings
05/08/09   sup     Changing the includes for the renaming of header files  dec.h, enci.h, l1m.h, srch.h, srchi.h, 
                   srchlog.h to wdec.h, wenci.h, wl1m_v.h, wsrch.h, wsrchi.h, wsrchlog.h
02/09/09   mc      HS drop for ASU is handled in DL channel manager.
02/04/09   mc      If HS action is STOP, drop HS before proceeding to work 
                   on ASU
12/09/08   uk      Mainling FEATURE_WCDMA_DL_ENHANCED, FEATURE_ENHANCED_CELL_RESELECTION, 
                   FEATURE_WCDMA_DRX_MANAGER, FEATURE_WCDMA_OFFLINE_SEARCHER 
                   and FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT 
10/03/08   hk      Bringing in 7k Mailine fixes
10/24/07   vsr     Mainlining MBMS and Enhanced DL changes 
08/08/07   vp      Added support for reading cell id of gps bch in dch state.
03/08/07   yiz     Added Macro for checking EUL STOP in e_req_mask
12/22/06   ksr     Rel-6 post-verification, skip sync A changes
10/26/06   cpe     Mainlined a bunch of features
08/14/06   yiz     Mainline HSUPA code
05/29/06   vp      Replaced the mac suspend sfn holder: rev_tm_hho with a 
                   variable that could be used for all HHOs.
05/29/06   vp      Decalre a placeholder to store the MAC suspend instant
                   during a reverse TM HHO.
05/14/06   vb      Code addition to handle CM suspention during HHO for which no acq needed
07/05/05   vb      Code changes to use the macro L1_IS_IN_TIMING_INIT_HHO_TO_KNOWN_CELL for 
                   timing re-initialized HHO by removing it from GCF flag protection
06/08/05   vb      Code changes to support Forced SyncA procedure. 
05/20/05   vb      Code addition to by pass SFN decode for tgt cell procedure
                   for the known tgt cell during Intraf timing init HHO
05/05/05   vb      flag addition to hho store setup to support saved set management
04/11/05   vb      code addition for resuming to old config upon HHO failure. 
                   As a part of it Added functions related to maintenance of 
                   l1_hho_suspend_time_info and added couple of macro to identify 
                   the type of HHO
03/24/05   gs      Externalized function l1m_cphy_setup_req_for_hho
03/20/05   scm     If both PCH/FACH transport blocks enabled, goto FACH state.
11/01/04   vb      Added l1_phychan_cfg_hho_enum_type and prototype 
                   for is_hho_timing_ind() removed the prototypes for 
                   is_intraf_hho_timing_ind(), is_interf_hho_timing_ind()
08/25/04   sh      Defined macro L1M_IDLE_STATE_VALID().
07/22/04   m       Added support for intraf-hho
05/21/04   gs      Externalized global variable l1_interf_hho_fach_to_dch
03/04/04   src     Externed l1_setup_req_mask.
03/03/04   sh/src  Support for error recovery from layer-1 getting stuck while
                   processing a CPHY_SETUP_REQ.
03/01/04   src     Support for error recovery in some cases of mDSP dead/stuck.
01/22/04   m       Added external declarations for IFHHO global variables
12/23/03    src    Defined a new macro L1M_RESET_CELL_TRANS_PROGRESS to reset
                   both l1m_cell_trans_in_progress and srchfach_aset_srch_done
                   to FALSE.
11/25/03   m       Define/use global variable to determine the timing
                   indication for HHO instead of function 
09/09/03   m       Modifications to support Inter Freq HHO
07/24/03   yus     Add typedef of STMR_TX_TIME_STATUS_MICRO_TYPE 
                   for MSM6250.
07/17/03   sh      Externalized l1m_state_transition_in_cell_trans.
07/02/03   src     Added two more typedefs l1_mdsp_sync_cmd_cb_enum_type and
                   l1_phychan_cfg_dbg_struct_type, plus extern-declared a flag
                   l1_phychan_cfg_debug_needed, which serve to put F3 messages
                   from various time-critical physical channel configuration
                   operations disabled until demanded. When the F3 messages are
                   disabled, critical debug information is stored in the struct
                   types.
04/14/03   src     Externalised the variable l1_setup_cmd_seq_num.
03/13/03   scm     Move l1m_compute_suspend_duration() to l1mcmd.h.
03/05/03   scm     Add extern to l1m_compute_suspend_duration().
01/06/03   scm     Add extern to l1m_previous_state.
10/30/02   scm     Remove extern to resuming_wcdma_mode flag.
10/14/02   scm     Add resuming_wcdma_mode flag for re-establishing CM gaps.
10/03/02   scm     Always failing RESUME for now.
09/25/02   scm     Working on SUSPEND with activation time support.
08/28/02   scm     Change l1_pending_acq_cmd, l1_pending_idle_cmd, and
                   l1_pending_fs_cmd from pointers to actual structures,
                   so data is not corrupted when command is freed.
08/14/02   sh      Added l1m_ul_cctrch_setup_cell_trans to set up UL cctrch
                   during cell reselection.
08/08/02   scm     Add prototype for l1m_send_cphy_setup_cnf_to_rrc();
05/18/02   sh      Changed L1M_PCH_STATE_VALID_MASK to include N-BCH.
                   Removed function l1m_cell_trans_drop_done() and added
                   l1m_start_cell_trans_setup().
02/21/02   sh      Added proto l1m_start_cphy_setup_cmd() to be called
                   before physical channel setups.
02/15/02   sh      Removed code for UL/DL simultaneous drop for now. Changed
                   proto for l1m_cphy_setup_req_cmd().
02/12/02   sh      Fixed macro L1_IS_UL_DL_PHYCHAN_DROP_DONE().
02/06/02   sh      Added macros for use during UL/DL simultaneous drop for
                   Activation Times. Added the following new function
                   protos: l1m_validate_cphy_setup_req_parms(),
                   l1m_dl_phychan_drop_done_cmd(), l1m_send_aset_update_cmd().
08/31/01   sh      Added l1_pending_fs_cmd for supporting new state L1M_FS.
06/22/01   sh      Moved cphy_setup_cmd_id_enum_type to l1rrcif.h.
06/14/01   sh      Added functions l1m_dl_cctrch_setup_done_cmd(),
                   l1m_ul_phychan_drop_done_cmd(),
                   l1m_dl_phychan_add_done_cmd(), l1m_dl_cctrch_validate(),
                   l1m_dl_cctrch_setup(), l1m_ul_cctrch_setup(),
                   l1m_setup_next_cmd() for physical channel setup
                   processing.
                   Added support for AICH.
06/05/01   sh      Removed code under flag CPHY_SETUP_UNIT_TEST. Added
                   function l1m_send_invalid_cctrch_cnf().
03/29/01   sh      Added l1m_cphy_cell_transition_req_cmd command handler
                   for CPHY_CELL_TRANS_REQ command and support routines
                   for cell transition validation, setup, and aset update.
03/09/01   sh      Added l1m_validate_cell_trans() function.
01/25/01   sh      Put back function save_cphy_setup_req() to save the
                   CPHY_SETUP_REQ command until we are done with it.
01/25/01   ml      Modified State Machine to take care of cleanups by L1
                   modules. L1M waits until all modules have finished their
                   cleanup procedures and then performs a state transition.
                   Added function l1m_do_state_transition().
12/21/00   sh      Added more documentation.
                   Removed function save_cphy_setup_req().
                   Modifications due to new command interfaces. Deleted
                   l1rrcctrl.h and added l1rrcif.h.
12/14/00   sh/ml   Defined valid bit masks for each state to support code
                   changes to l1m_validate_state_trans function.
10/27/00   sh      Created file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "l1rrcif.h"
#include "wl1m.h"
#include "customer.h"


/*===========================================================================

                        TYPE DECLARATIONS

===========================================================================*/
typedef uint32           STMR_TX_TIME_STATUS_MICRO_TYPE;

typedef enum
{
  L1_MDSP_SYNC_CMD_PRE_CB = 0,
  L1_MDSP_SYNC_CMD_POST_CB,
  NUM_L1_MDSP_SYNC_CMD_CB
} l1_mdsp_sync_cmd_cb_enum_type;

typedef struct
{
  uint8                          current_cfn;
  uint16                         current_tick;
  uint16                         current_status;
  STMR_TX_TIME_STATUS_MICRO_TYPE tx_slot_count;
  STMR_TX_TIME_STATUS_MICRO_TYPE tx_tti_count;
  STMR_TX_TIME_STATUS_MICRO_TYPE tx_frame_count;
} l1_phychan_cfg_dbg_struct_type;

typedef enum
{
  L1_NOT_IN_HHO = 0,
  L1_INTER_FREQ_HHO_TIMING_INIT,
  L1_INTER_FREQ_HHO_TIMING_MAINTAIN,
  L1_INTRA_FREQ_HHO_TIMING_INIT,
  L1_INTRA_FREQ_HHO_TIMING_MAINTAIN,
  L1_NOT_IN_HHO_FORCED_SYNC_A
} l1_phychan_cfg_hho_enum_type;

typedef struct
{
  uint8                  hho_state_mask;
  uint8                  hho_cmd_seq_num;
  uint32                 hho_setup_req_mask;
  boolean                hho_suspend_needed; 
  boolean                hho_cm_suspend_needed;
  boolean                hho_srch_meas_suspend_needed;
  boolean                hho_srch_saved_set_valid;
  boolean                hho_no_acq_needed;
  boolean                inter_freq_hho_in_progress;
  uint16                 known_cell_off;
  uint32                 known_cell_tm;
  l1_setup_cmd_type      hho_setup_cmd;
}l1_hho_store_setup_cmd_type;


#define MAX_SLAMS_DURING_HHO_CONFIG 20
typedef struct
{
  /* maintains current index that old_cell_sfn_timeline_in_ticks array gets filled in. need to reset 
     at the time suspending MAC before any HHO */
  uint8 curr_index;

  /* maintains the start sfn in ticks for cm suspension before HHO*/
  uint32 cm_suspend_sfn_in_tcks;

  /* maintains the start sfn in ticks for mac suspension before HHO*/
  uint32 mac_suspend_sfn_in_tcks;

  /* sfn in resolution of ticks before slamming. This array get filled when 
     l1 is in HHO not because of revert back to old config and when ever there 
     is change in seq align type or computation of CFN on old timeline/MAC 
     suspend time or CM suspend time,  */
  uint32 start_sfn_in_ticks_on_curr_align;

  /* Used to take care of Number of chip duration took for changing timeline alignment */
  uint32 start_chip_before_align;

  /* Sum of delta chips during align changes, these are accumulated delta chips during align changes
     delta chips = (end_chip_after align - start_chip_before_align)%CHIP_PER_STMR_TICK*/
  uint32 sum_of_delta_chips;

  /* old cell (sfn-cfn)%256 and gets filled in when l1 is in HHO and entering to suspended 
     state */
  int32 old_cell_sfn_cfn_diff_in_ticks;

  /* This variable gets incremented when ever we update the sfn_minus with 
     difference of curr_sfn_in_ticks - old_cell_sfn_timeline_in_ticks[curr_index-1] 
     when curr_index > 0, if curr_index is 0 this variable is initialized with 
     curr_sfn_in_ticks */
  uint32 old_cell_sfn_timeline_in_ticks[MAX_SLAMS_DURING_HHO_CONFIG];
}l1_hho_suspend_time_struct_type;

typedef enum
{
  L1_HHO_CFN_ON_OLD_CFG = 0,
  L1_HHO_CM_SUSPEND_DUR_ON_OLD_CFG,
  L1_HHO_MAC_SUSPEND_DUR_ON_OLD_CFG
} l1_hho_compute_oldcfg_timeline_enum_type;

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define CPHY_ACTIONS_DROP_MASK 0x0001
#define CPHY_ACTIONS_PRE_ADD_MASK 0x0002
#define CPHY_ACTIONS_ASU_MASK 0x0004
#define CPHY_ACTIONS_ADD_MASK 0x0008
#define CPHY_ACTIONS_MCPM_REQ_MASK 0x10
#define CPHY_REQ_MSK_LEN 32

#define CPHY_ACTIONS_IS_DROP_INCL() \
  (((l1m_ctrl_db.setup_actions_mask) & CPHY_ACTIONS_DROP_MASK) ? TRUE : FALSE)

#define CPHY_ACTIONS_IS_PRE_ADD_INCL() \
  (((l1m_ctrl_db.setup_actions_mask) & CPHY_ACTIONS_PRE_ADD_MASK) ? TRUE : FALSE)

#define CPHY_ACTIONS_IS_ASU_INCL() \
  (((l1m_ctrl_db.setup_actions_mask) & CPHY_ACTIONS_ASU_MASK) ? TRUE : FALSE)

#define CPHY_ACTIONS_IS_ADD_INCL() \
  (((l1m_ctrl_db.setup_actions_mask) & CPHY_ACTIONS_ADD_MASK) ? TRUE : FALSE)

#define CPHY_ACTIONS_IS_MCPM_REQ_INCL() \
  (((l1m_ctrl_db.setup_actions_mask) & CPHY_ACTIONS_MCPM_REQ_MASK) ? TRUE : FALSE)

#define CPHY_HHO_VALIDATE_MASK \
  (CPHY_HHO_INTERF_TI_HHO_INCL| \
   CPHY_HHO_INTERF_TM_HHO_INCL| \
   CPHY_HHO_INTRAF_TI_HHO_INCL| \
   CPHY_HHO_INTRAF_TM_HHO_INCL)

#define CPHY_HHO_CFG_REQ_IS_CFG_INCL(mask, cfg) \
  (((mask) & CPHY_HHO_##cfg##_HHO_INCL) ? TRUE : FALSE)
  
/* This macro returns TRUE if the following conditions are not satisfied
   -- If more than one HHO bits are not set in the input argument
   -- and if revert back to old config is present in the input argument, 
      it will check the setup is HHO */
#define CPHY_VALIDATE_HHO_CFG(mask) \
  (((((uint16)((mask&CPHY_HHO_VALIDATE_MASK)-1) & \
      (uint16)(mask&CPHY_HHO_VALIDATE_MASK)) == 0) && \
    ((mask&CPHY_HHO_RESUME_TO_OLD_CONFIG_HHO_INCL != 0 ) && \
     ((mask&(CPHY_HHO_VALIDATE_MASK)))))?TRUE:FALSE)

/*--------------------------------------------------------------------------
  Macros for Suspend periodic ops
---------------------------------------------------------------------------*/
#define CPHY_SUSPEND_PERIODIC_OPS_IGNORE_SCCH_ORDER  0x0001
#define CPHY_SUSPEND_PERIODIC_OPS_CM                                 0x0002
#define CPHY_SUSPEND_PERIODIC_OPS_SEARCHER                     0x0004
#define CPHY_SUSPEND_PERIODIC_OPS_FMO                               0x008
#define CPHY_SUSPEND_PERIODIC_OPS_WL1DLDRXCTRL             0x0010
#define CPHY_SUSPEND_PERIODIC_OPS_CME                               0x0020
#define CPHY_SUSPEND_PERIODIC_OPS_TRIAGE                          0x0040
#define CPHY_SUSPEND_PERIODIC_OPS_QSET_OPS               0x0080

#define CPHY_SUSPEND_PERIODIC_OPS_IGNORE_SCCH_ORDER_INCL() \
  (((l1m_ctrl_db.cphy_periodic_suspend_mask) & CPHY_SUSPEND_PERIODIC_OPS_IGNORE_SCCH_ORDER) ? TRUE : FALSE)

#define CPHY_SUSPEND_PERIODIC_OPS_CM_INCL() \
  (((l1m_ctrl_db.cphy_periodic_suspend_mask) & CPHY_SUSPEND_PERIODIC_OPS_CM) ? TRUE : FALSE)

#define CPHY_SUSPEND_PERIODIC_OPS_SEARCHER_INCL() \
  (((l1m_ctrl_db.cphy_periodic_suspend_mask) & CPHY_SUSPEND_PERIODIC_OPS_SEARCHER) ? TRUE : FALSE)

#define CPHY_SUSPEND_PERIODIC_OPS_FMO_INCL() \
  (((l1m_ctrl_db.cphy_periodic_suspend_mask) & CPHY_SUSPEND_PERIODIC_OPS_FMO) ? TRUE : FALSE)

#define CPHY_SUSPEND_PERIODIC_OPS_WL1DLDRXCTRL_INCL() \
  (((l1m_ctrl_db.cphy_periodic_suspend_mask) & CPHY_SUSPEND_PERIODIC_OPS_WL1DLDRXCTRL) ? TRUE : FALSE)

#define CPHY_SUSPEND_PERIODIC_OPS_CME_INCL() \
  (((l1m_ctrl_db.cphy_periodic_suspend_mask) & CPHY_SUSPEND_PERIODIC_OPS_CME) ? TRUE : FALSE)

#define CPHY_SUSPEND_PERIODIC_OPS_TRIAGE_INCL() \
  (((l1m_ctrl_db.cphy_periodic_suspend_mask) & CPHY_SUSPEND_PERIODIC_OPS_TRIAGE) ? TRUE : FALSE)

#define CPHY_SUSPEND_PERIODIC_OPS_DECFG_QSET_CELL_INCL() \
  (((l1m_ctrl_db.cphy_periodic_suspend_mask) & CPHY_SUSPEND_PERIODIC_OPS_QSET_OPS) ? TRUE : FALSE)

#define CPHY_CLR_MODULE_FROM_SUSPEND_PERIODIC_BMASK(module) \
  (l1m_ctrl_db.cphy_periodic_suspend_mask &= (~module))



#define l1m_set_setup_req_mask(mask) \
  l1_setup_req_mask |= mask

/*--------------------------------------------------------------------------
  The following bit masks define the set of Transport Channel types that
  are valid in each of the L1 states. The bit fields for each of the
  Transport Channels are defined in the file l1dlcctrchcfg.h.

  ------------------------------------
   L1        Transport Channel Types
  State    SBCH NBCH PCH FACH DCH DSCH
  ------------------------------------
  BCH       1    0    0   0    0    0
  PCH       x    0    1   0    0    0
  FACH      x    x    x   1    0    0
  DCH       x    x    x   x    1    x
---------------------------------------------------------------------------*/
#define L1M_BCH_STATE_VALID_MASK ( L1_DL_TRCH_REFTYPE_SBCH_BF )

#ifdef FEATURE_UMTS_BMC
/* Will likely mainline this at some point, but for now just for BMC support.
** If we get a CPHY_SETUP_REQ for SCCPCH with both PCH and FACH transport
** blocks enabled (both PCCH and CTCH mapped to the same SCCPCH for example),
** go to L1M_PCH instead of L1M_FACH state. */
#define L1M_PCH_STATE_VALID_MASK ( L1_DL_TRCH_REFTYPE_PCH_BF  | \
                                   L1_DL_TRCH_REFTYPE_FACH_BF | \
                                   L1_DL_TRCH_REFTYPE_SBCH_BF | \
                                   L1_DL_TRCH_REFTYPE_NBCH_BF )

#define L1M_FACH_STATE_VALID_MASK ( L1_DL_TRCH_REFTYPE_FACH_BF | \
                                    L1_DL_TRCH_REFTYPE_SBCH_BF | \
                                    L1_DL_TRCH_REFTYPE_NBCH_BF )
#else
#define L1M_PCH_STATE_VALID_MASK ( L1_DL_TRCH_REFTYPE_PCH_BF | \
                                   L1_DL_TRCH_REFTYPE_SBCH_BF | \
                                   L1_DL_TRCH_REFTYPE_NBCH_BF )

#define L1M_FACH_STATE_VALID_MASK ( L1_DL_TRCH_REFTYPE_FACH_BF | \
                                    L1_DL_TRCH_REFTYPE_PCH_BF  | \
                                    L1_DL_TRCH_REFTYPE_SBCH_BF | \
                                    L1_DL_TRCH_REFTYPE_NBCH_BF )
#endif

#define L1M_DCH_STATE_VALID_MASK ( L1_DL_TRCH_REFTYPE_DCH_BF  | \
                                   L1_DL_TRCH_REFTYPE_DSCH_BF | \
                                   L1_DL_TRCH_REFTYPE_FACH_BF | \
                                   L1_DL_TRCH_REFTYPE_PCH_BF  | \
                                   L1_DL_TRCH_REFTYPE_SBCH_BF | \
                                   L1_DL_TRCH_REFTYPE_NBCH_BF )


#define L1M_IDLE_STATE_MASK 0x00

/*--------------------------------------------------------------------------
  Macros to check if the next state is valid from the current state.
  Each of the macro is passed a mask of the TrCh's for the next state.
---------------------------------------------------------------------------*/

/* This macro checks to see if the bit mask of TrCh's for the BCH state
 * is valid. The passed bit mask should be a sub-set of the valid state
 * mask for the BCH state and the TrCh bit for SBCH should be set.
 */
#define L1M_BCH_STATE_VALID( next_state_mask )         \
  ( ( ( (next_state_mask) | L1M_BCH_STATE_VALID_MASK ) \
       == L1M_BCH_STATE_VALID_MASK ) &&                \
       (next_state_mask & L1_DL_TRCH_REFTYPE_SBCH_BF) )

/* This macro checks to see if the bit mask of TrCh's for the PCH state
 * is valid. The passed bit mask should be a sub-set of the valid state
 * mask for the PCH state and the TrCh bit for PCH should be set.
 */
#define L1M_PCH_STATE_VALID( next_state_mask )         \
  ( ( ( (next_state_mask) | L1M_PCH_STATE_VALID_MASK ) \
       == L1M_PCH_STATE_VALID_MASK ) &&                \
       (next_state_mask & L1_DL_TRCH_REFTYPE_PCH_BF) )

/* This macro checks to see if the bit mask of TrCh's for the FACH state
 * is valid. The passed bit mask should be a sub-set of the valid state
 * mask for the FACH state and the TrCh bit for FACH should be set.
 */
#define L1M_FACH_STATE_VALID( next_state_mask )         \
  ( ( ( (next_state_mask) | L1M_FACH_STATE_VALID_MASK ) \
       == L1M_FACH_STATE_VALID_MASK ) &&                \
       (next_state_mask & L1_DL_TRCH_REFTYPE_FACH_BF) )

/* This macro checks to see if the bit mask of TrCh's for the DCH state
 * is valid. The passed bit mask should be a sub-set of the valid state
 * mask for the DCH state and the TrCh bit for DCH should be set.
 */
#define L1M_DCH_STATE_VALID( next_state_mask )         \
  ( ( ( (next_state_mask) | L1M_DCH_STATE_VALID_MASK ) \
       == L1M_DCH_STATE_VALID_MASK ) &&                \
       (next_state_mask & L1_DL_TRCH_REFTYPE_DCH_BF) )

/* Check to see if the next state mask is 0, which implies we have
 * dropped all channels, with no channels to add, and we will be
 * transitioning to L1M_IDLE state.
 */
#define L1M_IDLE_STATE_VALID( next_state_mask ) \
 ( next_state_mask == L1M_IDLE_STATE_MASK )

typedef struct
{
  /** Bitmask for all cmds that have been issued but have not been completed yet**/
  uint32 pending_cmd_bmsk;

  /** Bitmask to determine which module need clean up **/
  uint32 module_cleanup_pending_bmsk;

  /** Bitmask to determine steps to be carried out in nex cmd handler**/
  uint32 setup_actions_mask;

  /**Drop_req_mask: To evaluate all the drops that are required**/
  uint32 cphy_drop_req_mask;

  /** QXDM Event bmsk */
  uint32 setup_event_bmsk;

  /** Flag to indicate cleanup trigger from HHO */
  boolean hho_cleanup;

  /** Flag to indicate cleanup done for HHO */
  boolean hho_cleanup_needed;

  /** Suspend_req_mask: to suspend all periodic ops befor CPHY processing **/
  uint16 cphy_periodic_suspend_mask;

  /* act time type to indicate if there is activation time from cphy setup */
  l1_act_time_enum_type act_time_type;

  /* activation time from cphy setup used to inform HS module do not start before this time*/
  uint16 act_time;

} l1m_ctrl_db_type;

extern l1m_ctrl_db_type l1m_ctrl_db;

#define WCDMA_EVENT_HS_DSCH_STATUS1_BMSK         0x001
#define WCDMA_EVENT_HS_DSCH_STATUS2_BMSK         0x002
#define WCDMA_EVENT_HSPA_PLUS_CFG_BMSK           0x004
#define WCDMA_EVENT_HSPA_PLUS_DC_BMSK            0x008
#define WCDMA_EVENT_HS_SERVING_CELL_CHANGE_BMSK  0x010
#define WCDMA_EVENT_EUL_RECONFIG_OR_ASU1_BMSK    0x020
#define WCDMA_EVENT_EUL_RECONFIG_OR_ASU2_BMSK    0x040
#define WCDMA_EVENT_EUL_SERVING_CELL_CHANGE_BMSK 0x080
#define WCDMA_EVENT_EUL_TTI_RECONFIG_BMSK        0x100
#define WCDMA_EVENT_CPC_CONFIG_ACTION1_BMSK      0x200
#define WCDMA_EVENT_CPC_CONFIG_ACTION2_BMSK      0x400

#define WCDMA_EVENT_SET_VAL(field)\
    (l1m_ctrl_db.setup_event_bmsk |= (WCDMA_##field##_BMSK))

#define WCDMA_EVENT_GET_VAL(field)\
    (l1m_ctrl_db.setup_event_bmsk & WCDMA_##field##_BMSK)

/* Global variable used to hold the L1 current state as a mask. The
 * mask depends on the types of CCTrCh currently setup by L1.
 */
extern uint8 l1m_current_state_mask;

/* Keeps track of the next state of the L1 State Machine */
extern l1m_state_enum_type l1m_next_state;
extern uint8 l1m_next_state_mask;

/* Keeps track of the previous state of the L1 State Machine */
extern l1m_state_enum_type l1m_previous_state;

/* buffer to hold the cell transition parameters*/
extern l1_cell_trans_params_struct_type l1_cell_trans_params;

/* buffer to hold the current L1 CPHY_SETUP_REQ cmd */
extern l1_setup_cmd_type l1_setup_cmd;


/* global variable that holds the set of CPHY setup
 * operations still remaining.
 */
extern uint32 l1_setup_req_mask;

/* True if we are processing a cell transition request, false otherwise */
extern boolean l1m_state_in_cell_trans;

/* Indicates if we are in a state transition during physical channel setup */
extern boolean l1m_state_transition_in_cphy_setup;

/* Indicates if we are in state transition during cell transition */
extern boolean l1m_state_transition_in_cell_trans;

/* Indicates if a cell transition command is in progress in L1 */
extern boolean l1m_cell_trans_in_progress;

/* Indicates if we have to drop phychans during cell transition */
extern boolean l1m_cell_trans_phychan_drop_pending;

#define L1M_RESET_CELL_TRANS_PROGRESS() (l1m_cell_trans_in_progress = FALSE)

#define L1M_GET_CELL_TRANS_PROGRESS_FLAG() l1m_cell_trans_in_progress

/*--------------------------------------------------------------------------
Static CPHY_SETUP_CNF packet used to keep setup status while handling
a CPHY_SETUP_REQ
--------------------------------------------------------------------------*/
extern l1_setup_cnf_type l1_cphy_setup_status;

/* If CPHY_SETUP_REQ has an activation time, store it in here. */
extern uint8       l1_setup_cmd_seq_num;

/* Pending commands to be used during state transition */
extern l1_acq_cmd_type        l1_pending_acq_cmd;
extern l1_idle_cmd_type       l1_pending_idle_cmd;

extern l1_freq_scan_cmd_type  l1_pending_fs_cmd;

/* Indicates if L1 has set the current command as pending (Activation Time) */
extern boolean l1_cmd_pending;

/* Indicates if a CPHY_SETUP_REQ command is in progress in L1 */
extern boolean l1m_cphy_setup_in_progress;

/* Saved request mask used for cleanup in the case of cphy setup timeout */
extern uint32 l1_setup_req_mask_for_cleanup;

/* Indicates the type of Hard handover inter/intra timing init/maintain */
extern l1_phychan_cfg_hho_enum_type l1_in_hho;

/* Indicates true if the freq info is inclded and PSC are not same 
   snd state trans from FACH to DCH */
extern boolean l1_interf_hho_fach_to_dch;

/* This flag is to indicate L1 is in Forced sync A without performing HHO */
extern boolean l1_in_forced_sync_a;

/* will be set to TRUE/FALSE through l1_setup_req_cmd processing */
extern boolean l1_revert_back_to_old_config;

/* used to store all necessary params related to compute the 
   total suspend time or CFN on olc config timeline */
extern l1_hho_suspend_time_struct_type l1_hho_suspend_time_info;

/* global variable to store the HSDPA request mask from RRC */
extern uint16 l1_hsdpa_req_mask;

/* This flag is set by L1M when UL drops a physical channel */
#define L1_UL_PHYCHAN_DROP_MASK 0x1
/* This flag is set by L1M when DL drops a physical channel */
#define L1_DL_PHYCHAN_DROP_MASK 0x2
/* This flag is set when UL and DL need to be dropped together */
#define L1_UL_DL_PHYCHAN_DROP_MASK 0x4

/* Whether L1 should wait for both UL/DL drop before proceeding */
#define L1_IS_WAIT_UL_DL_DROP() \
  (l1_phychan_drop_mask & L1_UL_DL_PHYCHAN_DROP_MASK)

/* Clear wait UL/DL drop flag */
#define L1_CLEAR_WAIT_UL_DL_DROP() \
  (l1_phychan_drop_mask = 0)

/* Whether UL phychan drop has been completed */
#define L1_IS_UL_PHYCHAN_DROP_DONE() \
  (l1_phychan_drop_mask & L1_UL_PHYCHAN_DROP_MASK)

/* Whether DL phychan drop has been completed */
#define L1_IS_DL_PHYCHAN_DROP_DONE() \
  (l1_phychan_drop_mask & L1_DL_PHYCHAN_DROP_MASK)

/* Whether UL/DL phychan drops have been completed */
#define L1_IS_UL_DL_PHYCHAN_DROP_DONE() \
  ((l1_phychan_drop_mask & L1_DL_PHYCHAN_DROP_MASK) && (l1_phychan_drop_mask & L1_UL_PHYCHAN_DROP_MASK))

/* Set UL drop done */
#define L1_UL_PHYCHAN_DROP_DONE() \
  (l1_phychan_drop_mask |= L1_UL_PHYCHAN_DROP_MASK)

/* Set DL drop done */
#define L1_DL_PHYCHAN_DROP_DONE() \
  (l1_phychan_drop_mask |= L1_DL_PHYCHAN_DROP_MASK)

/* Whether L1 is in inter freq HHO */
#define L1_IS_IN_INTERF_HHO()   \
  ((l1_in_hho == L1_INTER_FREQ_HHO_TIMING_INIT) ||   \
   (l1_in_hho == L1_INTER_FREQ_HHO_TIMING_MAINTAIN))

#define L1_IS_IN_FACH_DCH_INTERF_TI_HHO() \
  (l1_interf_hho_fach_to_dch == TRUE)

#define L1_IS_NOT_IN_HHO() \
  (l1_in_hho == L1_NOT_IN_HHO)

#define L1_IS_IN_HHO() \
  (l1_in_hho != L1_NOT_IN_HHO)

#define L1_IS_IN_INTERF_TIMING_MAINT_HHO() \
  (l1_in_hho == L1_INTER_FREQ_HHO_TIMING_MAINTAIN)

#define L1_IS_IN_DCH_TO_DCH_HHO() \
  ((l1_in_hho != L1_NOT_IN_HHO) &&   \
   (l1_interf_hho_fach_to_dch == FALSE))

#define L1_IS_IN_HHO_RESUMING_TO_OLD_CONFIG() \
  ((l1_revert_back_to_old_config == TRUE) && L1_IS_IN_DCH_TO_DCH_HHO())

#define L1_IS_IN_FORCED_SYNC_A() \
  (l1_in_forced_sync_a == TRUE)

#define L1_SET_FORCED_SYNC_A(flag) \
  (l1_in_forced_sync_a = (flag))

/* Whether L1 is in intra freq HHO */
#define L1_IS_IN_INTRAF_HHO()   \
  ((l1_in_hho == L1_INTRA_FREQ_HHO_TIMING_INIT) ||   \
   (l1_in_hho == L1_INTRA_FREQ_HHO_TIMING_MAINTAIN))

#define L1_IS_IN_INTRAF_TIMING_MAINT_HHO()   \
   (l1_in_hho == L1_INTRA_FREQ_HHO_TIMING_MAINTAIN)

/* Whether L1 is in inter/intra freq Timing initialized HHO */
#define L1_IS_IN_TIMING_INIT_HHO()   \
  ((l1_in_hho == L1_INTER_FREQ_HHO_TIMING_INIT) ||   \
   (l1_in_hho == L1_INTRA_FREQ_HHO_TIMING_INIT))

/* Whether L1 is in inter/intra freq Timing maintained HHO */
#define L1_IS_IN_TIMING_MAINT_HHO()   \
  ((l1_in_hho == L1_INTER_FREQ_HHO_TIMING_MAINTAIN) ||   \
   (l1_in_hho == L1_INTRA_FREQ_HHO_TIMING_MAINTAIN))

#define L1_IS_IN_HHO_SUSPEND_NEEDED() \
  ((l1_in_hho != L1_NOT_IN_HHO) && \
   (l1_in_hho != L1_INTRA_FREQ_HHO_TIMING_MAINTAIN))

#define L1_IS_IN_TIMING_INIT_HHO_TO_KNOWN_CELL() \
  (L1_IS_IN_INTRAF_HHO() && \
   L1_IS_IN_TIMING_INIT_HHO() && \
   (l1m_hho_acq_needed() == FALSE))


#define L1M_IS_EUL_TO_STOP_AT_RECFG() \
  ((l1_setup_cmd.e_req_mask & L1_EUL_STOP) == L1_EUL_STOP)

#ifdef FEATURE_WCDMA_GPS_CELL_ID_READING
  #define L1M_IS_CPHY_SETUP_FOR_FOR_CELL_ID_BCH_ADD(l1_cphy_setup_cmd) ((l1m_state == L1M_DCH)  \
    && (CPHY_SETUP_REQ_IS_DL_PHYCHAN_ADD_INCL(l1_cphy_setup_cmd.req_mask))  \
    && (l1_cphy_setup_cmd.dl_phychan == L1_DL_PHYCHAN_PCCPCH_S))
#endif

#define L1M_IS_CPHY_SETUP_REQ_INCLUDE_DPCH_CFG(l1_cphy_setup_cmd) (  \
    ((CPHY_SETUP_REQ_IS_DL_PHYCHAN_ADD_INCL(l1_cphy_setup_cmd.req_mask)) &&   \
     (l1_cphy_setup_cmd.dl_phychan == L1_DL_PHYCHAN_DPCH)) || \
    ((CPHY_SETUP_REQ_IS_DL_PHYCHAN_CFG_INCL(l1_cphy_setup_cmd.req_mask)) &&   \
        (l1_cphy_setup_cmd.dl_phychan == L1_DL_PHYCHAN_DPCH)) || \
    ((CPHY_SETUP_REQ_IS_DL_PHYCHAN_DROP_INCL(l1_cphy_setup_cmd.req_mask)) &&   \
        (l1_cphy_setup_cmd.drop_dl_phychan == L1_DL_PHYCHAN_DPCH)))

#ifdef FEATURE_WCDMA_DC_HSUPA
#define L1M_IS_CPHY_SETUP_REQ_INCLUDE_SEC_DPCH_CFG(l1_cphy_setup_cmd) (  \
    ((CPHY_SETUP_REQ_IS_SEC_DL_PHYCHAN_ADD_INCL(l1_cphy_setup_cmd.req_mask)) &&   \
     (l1_cphy_setup_cmd.dl_phychan == L1_DL_PHYCHAN_SEC_FDPCH)) || \
    ((CPHY_SETUP_REQ_IS_SEC_DL_PHYCHAN_CFG_INCL(l1_cphy_setup_cmd.req_mask)) &&   \
        (l1_cphy_setup_cmd.dl_phychan == L1_DL_PHYCHAN_SEC_FDPCH)) || \
    ((CPHY_SETUP_REQ_IS_SEC_DL_PHYCHAN_DROP_INCL(l1_cphy_setup_cmd.req_mask)) &&   \
        (l1_cphy_setup_cmd.drop_dl_phychan == L1_DL_PHYCHAN_SEC_FDPCH)))
#endif

#define L1_IS_CPHY_SETUP_IN_PROGRESS() \
  (l1m_cphy_setup_in_progress == TRUE)

/* Mask of physical channels that are being dropped */
extern uint8 l1_phychan_drop_mask;

/* The CCTrCh index returned by the CCTrCh setup command. This is used
 * to config or add a DL Physical Channel.
 */
extern uint8 l1_cctrch_index;

/* Flag to indicate if physical channel configuration steps need debugging.
 * If set to true, this enables a large number of F3 messages to allow the
 * developer the progress of physical channel configuration. Normally, this
 * flag should be turned off to avoid CPU loading issues.
 */
extern boolean l1_phychan_cfg_debug_needed;

extern boolean dc_hsupa_activate_without_order;

extern boolean wl1_is_voice_call_mcvs ;

/*===========================================================================
FUNCTION        L1M_SEND_RRC_FAILURE_CONFIRMATION

DESCRIPTION
  This function sends the RRC_CPHY_SETUP_CNF primitive with a status of
  FAILURE, obviously meaning something bad happened while processing
  the CPHY_SETUP_REQ command.

PARAMETERS
  uint8 seq_num - If the CPHY_SETUP_REQ had an activation time, this is the
                  sequence number of the command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sends RRC_CPHY_SETUP_CNF with FAILURE status to RRC.
===========================================================================*/
extern void l1m_send_rrc_failure_confirmation(uint8 seq_num);

#if defined(FEATURE_WCDMA_MIMO)
/*===========================================================================
FUNCTION l1m_cphy_setup_ord_config_mimo_scpich_status

DESCRIPTION
  This function returns the status of MIMO SCPICH in the ordered config for
  a particular PSC.

DEPENDENCIES
  This assumes that processing of CPHY_SETUP_REQ has already been started

RETURN VALUE
  TRUE - CPHY_SETUP_REQ has MIMO in SCPICH mode
  FALSE - CPHY_SETUP_REQ is not in progress OR
          CPHY_SETUP_REQ does not have MIMO in SCPICH mode

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1m_cphy_setup_ord_config_mimo_scpich_status(
  /* PSC to be checked for MIMO S-CPICH status */
  uint16 psc);
#endif


/*===========================================================================
FUNCTION        l1m_cphy_setup_req_for_hho

DESCRIPTION
  This function checks if this setup command is in HHO and returns the type of 
  HHO. Below is the sequence of execution. 
  X        : don't care
  NotHHO   : L1 not in hard handover
  IntraFTM : Intra Frequency Timing Maintained HHO
  IntraFTI : Intra Frequency Timing Initialized HHO
  IntraFTM : Intra Frequency Timing Maintained HHO
  IntraFTI : Intra Frequency Timing Initialized HHO
  Fach to DCH freq included case is not hard handover, 
  but are using the HHO flaqs in order to go through 
  same HHO acquisition path
  ------------------------------------------------------------
  Freq     Freq      RL         L1 State  Timing   Type of HHO
  incl     change    change     change    Ind
  ------------------------------------------------------------
  No                  X            X       X       NotHHO
  YES      NO        NO        DCH-DCH   Maint     IntraFTM
  YES      NO        NO            X     Init/None NotHHO
  YES      NO        YES       DCH-DCH   Init/None IntraFTI
  YES      NO        YES       DCH-DCH   Maint     IntraFTM
  YES      YES        X        DCH-DCH   Init/None InterFTI
  YES      YES        X        DCH-DCH   Maint     InterFTM
  YES      X          X        FACH-DCH     X      InterFTI
  ------------------------------------------------------------
DEPENDENCIES
  None.

RETURN VALUE
  l1_phychan_cfg_hho_enum_type

SIDE EFFECTS
  None.
===========================================================================*/

extern l1_phychan_cfg_hho_enum_type l1m_cphy_setup_req_for_hho(
  /* CPHY SETUP REQ command pointer */
  l1_setup_cmd_type *setup_cmd);

#ifdef FEATURE_WCDMA_DC_HSUPA 
/*===========================================================================

FUNCTION     l1m_cphy_setup_req_per_carr_hho

DESCRIPTION
  
DEPENDENCIES
  

RETURN VALUE
  NONE

SIDE EFFECTS
  None
===========================================================================*/
extern l1_phychan_cfg_hho_enum_type l1m_cphy_setup_req_per_carr_hho (
  /* CPHY SETUP REQ command pointer */
  l1_setup_cmd_type *setup_cmd,
  boolean carr_idx);
#endif

/*===========================================================================

FUNCTION     l1m_update_timing_of_known_rl

DESCRIPTION
  Updates the hho info  with OFF, Tm, which will be used to compute 
  the timing adjustment  to new cell while configuring dpch on target cell

DEPENDENCIES
  None

RETURN VALUE
  Boolean True if there is RL is known by OFF and Tm

SIDE EFFECTS
  None
===========================================================================*/
boolean l1m_update_timing_of_known_rl(
  /* CPHY SETUP REQ command pointer */
  l1_setup_cmd_type *setup_cmd);

/*===========================================================================

FUNCTION     l1m_hho_tm_of_known_timg_cell

DESCRIPTION
  This function return the tm of the known tgt cell for hho procedures.

DEPENDENCIES
  Should be called in Intra freq timing init procedures

RETURN VALUE
  NONE

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 l1m_hho_tm_of_known_timg_cell(void);

/*===========================================================================

FUNCTION     l1m_hho_acq_needed

DESCRIPTION
  This function returns if hho acq is needed during hho procedures.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1m_hho_acq_needed(void);

/*===========================================================================
FUNCTION        L1M_CPHY_SETUP_REQ_CMD

FILE NAME       l1m_setup.c

DESCRIPTION
  This function handles the CPHY_SETUP_REQ primitive from L2/L3 and is called
  by L1M when it receives a CPHY_SETUP_REQ command. This function will first
  save the request and validates any state transition that is possible as a
  result of this request. If the state transition is valid, it performs
  parameter validation. Each of the Downlink and Uplink modules are called
  inturn to validate the parameters. The modules are required to validate the
  parameters and also make sure they have enough resources to complete
  the request. The request mask is then saved and the current state of L1 is
  changed to the new state (if the request results in a state transition).
  The commands in the request mask are then executed in order.

DEPENDENCIES
  The state validation function return depends on the physical channels
  currently setup and also depends on the new physical channel adds/drops
  in the command packet.

RETURN VALUE
  None

SIDE EFFECTS
  The l1m_state variable is changed to the new state if there is a state
  transition possible. The cleanup routine of the current state is called
  followed by the initialization routine of the new state.

===========================================================================*/
extern void l1m_cphy_setup_req_cmd
(
  /* command parameters struct pointer */
  l1_ext_cmd_type *cmd
);

/*===========================================================================
FUNCTION         L1M_VALIDATE_CPHY_SETUP_REQ_PARMS

DESCRIPTION
  This function validates each of the parameters in the CPHY_SETUP_REQ
  command. The validate routines are called based on what actions are
  being performed in the command. If any validations fails a CPHY_SETUP_CNF
  command is sent to RRC with a fail status.

DEPENDENCIES

RETURN VALUE
  Boolean. True if all validation passes, False if any validation fails.

SIDE EFFECTS
  None
===========================================================================*/
boolean l1m_validate_cphy_setup_req_parms
(
  l1_ext_cmd_type *cmd
);
/*===========================================================================
FUNCTION        l1m_pre_start_cphy_setup_cmd

DESCRIPTION


DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void l1m_pre_start_cphy_setup_cmd
(
  /* command parameters struct pointer */
  l1_ext_cmd_type *cmd
);
/*===========================================================================
FUNCTION         L1M_START_CPHY_SETUP_CMD

DESCRIPTION
  This function is called before the start of the physical
  channel setups. This function will perform state
  transition if it needs to be done, otherwise it starts
  the channel setup process.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1m_start_cphy_setup_cmd( void );

/*===========================================================================

FUNCTION L1M_FAIL_REMAINING_CPHY_SETUP_CMD_STEPS

DESCRIPTION
  This function systematically and without concern for others fails any
  and all steps remaining to be processed in the current CPHY_SETUP_REQ
  from RRC.  This may be desirable if a step fails that, in turn, should
  cause all remaining steps to fail.  It then causes CPHY_SETUP_CNF,
  containing all the failed statuses, to get sent to RRC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1m_fail_remaining_cphy_setup_cmd_steps(void);

/*=========================================================================

FUNCTION l1m_send_cphy_setup_cnf_to_rrc

DESCRIPTION
  This function sends the RRC_CPHY_SETUP_CNF primitive to RRC, which
  includes status on each operation performed and an overall status.

  This function is called from l1m_cphy_setup_next_cmd_handler(), in the
  CPHY_SETUP_DONE case, if MAC is not processing a corresponding config
  request next, which happens in the case of a MAC command with an
  activation time.  It is also called from l1m_cphy_mac_config_cnf(),
  which means MAC has finished processing its config request as well as L1.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sends RRC_CPHY_SETUP_CNF primitive with status to RRC.

=========================================================================*/
extern void l1m_send_cphy_setup_cnf_to_rrc(void);

/*===========================================================================

FUNCTION L1M_CPHY_SETUP_NEXT_CMD_HANDLER_WRM_WRAPPER

DESCRIPTION
  This is a wrapper function for L1M_CPHY_SETUP_NEXT_CMD_HANDLER
  used by wl1carrcfg to pass this as grant call back in wrm_modify_freq()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1m_cphy_setup_next_cmd_handler_wrm_wrapper(boolean grant);

/*===========================================================================

FUNCTION L1M_CPHY_SETUP_NEXT_CMD_HANDLER

DESCRIPTION
  This function determines the next command to be handled that is part
  of the physical channel setup command. It is called first from the
  physical channel setup command handler when the CPHY_SETUP_REQ command
  is first received by L1M, and then subsequently by the Local Command Handler
  that sequences the physical channel setup procedure.

  This function determines the next command to be handled in the physical
  channel setup procedure and then calls the corresponding handler.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1m_cphy_setup_next_cmd_handler
(
  void
);

/*===========================================================================

FUNCTION L1M_VALIDATE_STATE_TRANS

DESCRIPTION
  This function is called during the physical channel setup command
  (CPHY_SETUP_REQ). This function will check to see if the physical
  channel setup or teardown will result in a state transition.

  A state transition is possible when new CCTrCh's are added in L1, during the
  case of a physical channel add or config.
  A transition is also possible when CCTrCh's are dropped, when a physical
  channel with which it is associated with is dropped.

  The function creates a mask based on CCTrCh "adds" and/or "drops" that will
  determine what the next state is going to be after this physical channel
  setup request completes successfully. Only certain state transitions are
  permitted within a particular L1 state. This function determines if the
  next state mask is within the set of allowed CCTrCh combinations for
  the current state.

  The actual state transition is made after the CPHY_SETUP_REQ command
  validation completes successfully.

DEPENDENCIES
  None

RETURN VALUE
  This function returns TRUE if the CPHY_SETUP_REQ results in a valid state
  transition, or FALSE otherwise.

SIDE EFFECTS
  If there are any CCTrCh Add and/or Drops, the l1m_next_state_mask is
  modified to reflect the new set of CCTrChs. If there is a valid state
  transition possible, l1m_next_state is set to the new state.

===========================================================================*/
boolean l1m_validate_state_trans
(
  l1_setup_cmd_type * cmd
);

/*===========================================================================
FUNCTION        L1M_DO_STATE_TRANSITION

FILE NAME       l1msetup.c

DESCRIPTION     This function performs a L1 state transition. When the argument
                normal is set to TRUE, the function assumes that the transition
                is part of normal operation. Otherwise, the function assumes an
                erroneous or abnormal scenario under which it is called, and it
                sets up flags/commands to facilitate applicable and available
                error recovery procedures.

DEPENDENCIES    The variable l1m_next_state

RETURN VALUE    None

SIDE EFFECTS    The L1 state will be changed from l1m_state to l1m_next_state
===========================================================================*/
void l1m_do_state_transition( boolean normal );

/*===========================================================================
FUNCTION        L1M_SAVE_CPHY_SETUP_REQ

FILE NAME

DESCRIPTION
  This function copies contents of cmd into l1_setup_cmd. Pointer values
  are saved as is, instead of copying all the contents of the pointer. The
  assumption is that L2/L3 will not overwrite those pointers until they
  receive a confirm or error indication for a command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Saves the contents of the cphy setup request into the variable l1_setup_cmd.
  This function only does a shallow copy of all the fields in the cmd packets.
  Any pointers to buffers are assumed valid until the CPHY_SETUP_CNF is
  sent from L1.

===========================================================================*/
void l1m_save_cphy_setup_req( l1_ext_cmd_type *cmd );

/*===========================================================================

FUNCTION L1M_GET_NEXT_CPHY_SETUP_DROP_CMD_ID

DESCRIPTION
  This function returns the ID of the next command to be processed as
  part of the DROP request mask.

  The request mask in the l1_setup_cmd and l1_cell_trans_cmd
  packet has the following format:

  Bit #  Command
  -----  -------
  0      CPHY_DL_CCTRCH_REQ: setup DL CCTRCH
  1      CPHY_UL_CCTRCH_REQ: setup UL CCTRCH
  2      CPHY_UL_PHYCHAN_DROP_REQ: drop UL physical channel
  3      CPHY_DL_PHYCHAN_DROP_REQ: drop DL physical channel
  4      CPHY_DL_PHYCHAN_CFG_REQ: config DL physical channel
  5      CPHY_UL_PHYCHAN_CFG_REQ: config UL physical channel
  6      CPHY_DL_PHYCHAN_ADD_REQ: add DL physical channel
  7      CPHY_UL_PHYCHAN_ADD_REQ: add UL physical channel
  8      CPHY_HS_CHAN_CFG_REQ: Configure HS channel
  9-15   Reserved

DEPENDENCIES
  None

RETURN VALUE
  cphy_setup_cmd_id_enum_type value that corresponds to the next command
  to be processed as determined by the request mask.

SIDE EFFECTS
  If a bit in the request mask is set that corresponds to a command, that
  bit is turned off in the req_mask so that the command will not be
  processed again.

===========================================================================*/
extern cphy_setup_cmd_id_enum_type l1m_get_next_cphy_setup_drop_cmd_id
(
  void
);

/*===========================================================================

FUNCTION L1M_GET_NEXT_CPHY_SETUP_ADD_CMD_ID

DESCRIPTION
  This function returns the ID of the next command to be processed as
  part of the ADD request mask.

  The request mask in the l1_setup_cmd 
  packet has the following format:

  Bit #  Command
  -----  -------
  0      CPHY_DL_CCTRCH_REQ: setup DL CCTRCH
  1      CPHY_UL_CCTRCH_REQ: setup UL CCTRCH
  2      CPHY_UL_PHYCHAN_DROP_REQ: drop UL physical channel
  3      CPHY_DL_PHYCHAN_DROP_REQ: drop DL physical channel
  4      CPHY_DL_PHYCHAN_CFG_REQ: config DL physical channel
  5      CPHY_UL_PHYCHAN_CFG_REQ: config UL physical channel
  6      CPHY_DL_PHYCHAN_ADD_REQ: add DL physical channel
  7      CPHY_UL_PHYCHAN_ADD_REQ: add UL physical channel
  8      CPHY_HS_CHAN_CFG_REQ: Configure HS channel
  9-15   Reserved

DEPENDENCIES
  None

RETURN VALUE
  cphy_setup_cmd_id_enum_type value that corresponds to the next command
  to be processed as determined by the request mask.

SIDE EFFECTS
  If a bit in the request mask is set that corresponds to a command, that
  bit is turned off in the req_mask so that the command will not be
  processed again.

===========================================================================*/
cphy_setup_cmd_id_enum_type l1m_get_next_cphy_setup_add_cmd_id( void );

/*===========================================================================

FUNCTION      L1M_DL_CCTRCH_VALIDATE

DESCRIPTION
  This function validates the Downlink CCTrCh setup request.

DEPENDENCIES
  None

RETURN VALUE
  Boolean: TRUE if validation succeeds, false otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean l1m_dl_cctrch_validate( l1_setup_cmd_type* cmd );

/*===========================================================================

FUNCTION      L1M_DL_CCTRCH_SETUP

DESCRIPTION
  This function calls the Downlink module to set up the DL CCTrCh.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1m_dl_cctrch_setup( l1_setup_cmd_type* setup );

/*==============================================================a=============

FUNCTION      L1M_UL_CCTRCH_SETUP

DESCRIPTION
  This function calls the uplink module to set up the UL CCTrCh.
  If the request is to set up an UL PRACH, then we first set up
  the DL AICH.
  This function can alse be called during channel setup during
  a cell transition after a cell reselection.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1m_ul_cctrch_setup( void );
/*===========================================================================

FUNCTION     L1M_DL_CCTRCH_SETUP_DONE_CMD

DESCRIPTION
  This function handles the next step after the DL CCTrCh is set up.
  If the DL CCTrCh setup fails, it sends a confirmation to RRC, otherwise
  it handles the next command in the CPHY_SETUP_REQ command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A confirmation is sent to RRC if the DL CCTrCh set up failed.
===========================================================================*/
extern void l1m_dl_cctrch_setup_done_cmd
(
  dl_cphy_setup_local_cmd_type* cmd_ptr
);

/*===========================================================================

FUNCTION     L1M_UL_PHYCHAN_DROP_DONE_CMD

DESCRIPTION
  This function handles the next step after the UL Physical Channel
  was dropped. If a PRACH was dropped, we proceed to drop the DL
  AICH also, otherwise the next command in the CPHY_SETUP_REQ command
  is processed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1m_ul_phychan_drop_done_cmd
(
  ul_cphy_setup_local_cmd_type* cmd_ptr
);

/*===========================================================================

FUNCTION     L1M_DL_PHYCHAN_DROP_DONE_CMD

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1m_dl_phychan_drop_done_cmd
(
  dl_cphy_setup_local_cmd_type* cmd_ptr
);

/*===========================================================================

FUNCTION     L1M_DL_PHYCHAN_ADD_DONE_CMD

DESCRIPTION
  This function handles the next step after a DL physical channel was
  added. If an AICH physical channel was added, we proceed to add the
  UL PRACH, otherwise the next command in the CPHY_SETUP_REQ command
  is processed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1m_dl_phychan_add_done_cmd
(
  dl_cphy_setup_local_cmd_type* cmd_ptr
);

/*===========================================================================

FUNCTION     L1M_SETUP_NEXT_CMD

DESCRIPTION
  This function handles the next command after a step in the physical
  channel setup procedure. Appropriate functions are called depending
  on whether we are setting up physical channels as a result of
  CPHY_CELL_TRANSITION_REQ or as a result of CPHY_SETUP_REQ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1m_setup_next_cmd( void );

/* Cell Transition related functions */

/*===========================================================================
FUNCTION   L1M_CPHY_CELL_TRANSITION_REQ_CMD

FILE NAME  l1msetup.c

DESCRIPTION
  This function handles the CPHY_CELL_TRANSITION_REQ primitive.
  This function saves the cell transition request and then sends
  an ASET update command to Search module. The rest of the cell
  transition command is processed after search completes the ASET
  update and sends a local command to L1M.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1m_cphy_cell_transition_req_cmd
(
  l1_cell_transition_cmd_type *cmd
);

/*===========================================================================

FUNCTION     SAVE_CPHY_CELL_TRANS_REQ

DESCRIPTION
  This function saves the cell transition request command sent by the RRC
  into a global buffer in L1. This saved command is later used after
  the ASET update completes when we need to reconfigure the physical
  channels.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1m_save_cphy_cell_trans_req( l1_cell_transition_cmd_type* cmd );

/*===========================================================================

FUNCTION     L1M_VALIDATE_CELL_TRANS

DESCRIPTION
  This function validates the channel setup parameters that are sent
  as part of the cell transition request command. The request mask
  in the command is used to validate each of the sub-primitives in
  the command. The request mask is the same as the one used for
  CPHY_SETUP_REQ commands.

  In the event of a failed validation, a confirmation is sent to RRC
  with the status set to FALSE.

  If the validation passes, we first drop the UL channels and then
  drop the DL channels. The DL module will post a local command
  after all the DL physical channels are dropped.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1m_validate_cell_trans( void );

/*===========================================================================

FUNCTION        L1M_START_CELL_TRANS_SETUP

DESCRIPTION
  This function will be called during the cell transition to the new cell
  during cell reselection. This will disable cell reselection until
  a new DRX command is received for the new cell. This function will
  perform any state transition and start performing channel setups
  for the new cell.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Possible state transition.
 
===========================================================================*/
void l1m_start_cell_trans_setup( void );

/*===========================================================================

FUNCTION     L1M_CELL_TRANS_NEXT_CMD_HANDLER

DESCRIPTION
  This function determines the next command to be handled that is part
  of the cell transition request physical channel setups. This function
  is called after we perform a state transition and we drop all existing
  channels.
  After all the commands have been handled, a confirmation is sent to
  RRC indicating the overall status of the cell transition request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1m_cell_trans_next_cmd_handler( void );

/*===========================================================================

FUNCTION L1M_UL_PHYCHAN_DROP_CMD_HANDLER

DESCRIPTION
  This function calls the L1 modules necessary for dropping an Uplink
  Physical Channel. A PHYCHAN_TEARDOWN_DONE local command will be posted
  to L1M at the completion of the teardown.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1m_ul_phychan_drop_cmd_handler
(
  l1_setup_cmd_type* cmd
);

/*===========================================================================

FUNCTION     L1M_SAVE_INTERF_HHO_CMD

DESCRIPTION
  Saves the parameters passed by RRC for CPHY_SETUP_CMD

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1m_save_interf_hho_cmd(void);

/*===========================================================================

FUNCTION     L1M_RESTORE_INTERF_HHO_CMD

DESCRIPTION
  Restores the parameters passed by RRC for CPHY_SETUP_CMD

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1m_restore_interf_hho_cmd(void);

/*===========================================================================

FUNCTION     L1M_START_INTERF_HHO

DESCRIPTION
  Starts the  ball rollling for  implementing Interfreq HHO.  From now
  onwards searcher will  take over. It tunes the RF  to new frequency,
  searches  for pilot  on the  new frequency,  acquires signal  on new
  frequency,  if timing  initialized  case  is used  BCH  is setup  to
  get timing  on new  cell, then  calls l1m_srch_acq_get_timing_done()
  function below to inform the status of the search.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1m_start_interf_hho(void);

/*===========================================================================

FUNCTION     L1M_SRCH_ACQ_GET_TIMING_DONE

DESCRIPTION
  Search module informs CPHY setup procedure to continue after aquiring
  on the new frequency.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1m_srch_acq_get_timing_done(boolean result);

/*===========================================================================

FUNCTION     IS_HHO_TIMING_IND

DESCRIPTION
  Returns  Timing Indication Type based on  the  HHO  request is  a  timing
  initialized or timing maintained case.

DEPENDENCIES
  None

RETURN VALUE
  L1_TIMING_IND_MAINTAIN - If Timing maintained
  L1_TIMING_IND_INIT  - for all other cases

SIDE EFFECTS
  None
===========================================================================*/
l1_timing_ind_enum_type is_hho_timing_ind
(
  l1_dl_phychan_db_struct_type *dl_phychan_db_struct  /* dl phychan db ptr */
);

/*===========================================================================

FUNCTION     DL_INDICATE_PHYCHAN_STATUS

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dl_indicate_phychan_status(boolean status);

/*===========================================================================

FUNCTION     L1_CLEANUP_FAILED_DL_SETUP

DESCRIPTION
  This function cleans up and half-baked DL physical channel setup, including
  freeing any allocated CCTrCH table index.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Physical channel database and CCTrCH table database will get modified.
===========================================================================*/
void l1_cleanup_failed_dl_setup
(
  l1_dl_phychan_enum_type dl_phychan,
  uint8                   cctrch_index
);

/*===========================================================================
FUNCTION     L1_EUL_CPHY_SETUP_CNF_PUT_STATUS

DESCRIPTION
  This function is used by the EUL(E_DL and E_UL) modules to 
  update the l1_cphy_setup_status buffer in the course of 
  performing the steps of a CPHY_SETUP_REQ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The l1_cphy_status buffer is updated accordingly
===========================================================================*/
extern void l1_eul_cphy_setup_cnf_put_status( boolean pass, wl1_ul_carr_id_enum_type carr_idx );


#ifdef FEATURE_WCDMA_DC_HSUPA 
/*===========================================================================
FUNCTION        l1_sec_ul_cphy_setup_cnf_put_status

FILE NAME       l1msetup.c

DESCRIPTION     This function is used by the wl1ulmc modules to 
                update the l1_cphy_setup_status buffer in the course of 
                performing the steps of a CPHY_SETUP_REQ.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The l1_cphy_status buffer is updated accordingly
===========================================================================*/
extern void l1_sec_ul_cphy_setup_cnf_put_status(boolean pass);

/*===========================================================================
FUNCTION        L1_SEC_UL_SEND_SETUP_DONE_TO_L1M

FILE NAME       l1msetup.c

DESCRIPTION     This function sends the done local commands for wl1ulmc 
                modules. The command to be sent is decided based upon the
                input parameter l1_cmd_code.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void l1_sec_ul_send_setup_done_to_l1m( boolean status, cphy_setup_cmd_id_enum_type  setup_cmd_id);
#endif



/*===========================================================================
FUNCTION        L1_EUL_SEND_SETUP_DONE_TO_L1M

DESCRIPTION     This function sends the done local commands for EUL/EDL 
                modules. The command to be sent is decided based upon the
                input parameter l1_cmd_code.

DEPENDENCIES    
  None

RETURN VALUE    
  None

SIDE EFFECTS    
  None
===========================================================================*/
extern void l1_eul_send_setup_done_to_l1m(
  boolean status,
  cphy_setup_cmd_id_enum_type setup_cmd_id
);

/*===========================================================================

FUNCTION     L1_SEND_PHYCAHN_EST_IND

DESCRIPTION
This function sends the PHYCHAN_ESTABLISH_IND.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
extern void l1_send_phychan_establish_ind(wl1_ul_carr_id_enum_type carr_id, boolean phy_est_ind_result);

/*===========================================================================
FUNCTION          l1_update_hho_type_for_cm

DESCRIPTION       This function updates hho related variabled in CM globals.

DEPENDENCIES      None

RETURN VALUE      None.

SIDE EFFECTS      None
===========================================================================*/
extern void l1m_update_hho_type_for_cm(l1_phychan_cfg_hho_enum_type hho_type, uint16 cphy_setup_hho_mask);

/*===========================================================================
FUNCTION          l1_is_l1_in_rev_hho_for_cm

DESCRIPTION       This function finds out whether L1 is in revrese hho based on CM globals.

DEPENDENCIES      None

RETURN VALUE
  TRUE: L1 is in reverse HHO
  FALSE: L1 is not in reverse HHO

SIDE EFFECTS      None
===========================================================================*/
extern boolean l1m_is_l1_in_rev_hho_for_cm(void);

/*===========================================================================
FUNCTION          l1m_reset_hho_type_for_cm

DESCRIPTION       This function updates hho related variables in CM globals.

DEPENDENCIES      None

RETURN VALUE      None.

SIDE EFFECTS      None
===========================================================================*/
extern void l1m_reset_hho_type_for_cm(void);

/*===========================================================================

FUNCTION     l1m_setup_resume_aset_upd_after_rl_ops

DESCRIPTION
This function handles aset update after radio links have been deleted

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
extern void l1m_setup_resume_aset_upd_after_rl_ops(boolean status, l1_dl_phychan_enum_type phch_type);


/*===========================================================================
FUNCTION        L1_CPHY_SETUP_CNF_SET_STATUS_WORD

FILE NAME       l1m.c

DESCRIPTION     This function sets a passed status word according to the
                passed offset, operation, and status

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The passed status word is updated accordingly
===========================================================================*/
void l1_cphy_setup_cnf_set_status_word(uint32 *stat_word,uint8 offset,
                                       l1_setup_ops_enum_type op, boolean status);

/*==============================================================a=============
FUNCTION    WL1UL_HSRACH_AICH_SETUP

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
 void wl1ul_hsrach_aich_setup( l1_dl_phychan_db_struct_type* add_dl_phychan_ptr, 
                                                          uint32 req_mask, l1_dl_phychan_enum_type drop_dl_phychan);

#ifdef FEATURE_WCDMA_PLT_MODE
#error code not present
#endif /* FEATURE_WCDMA_PLT_MODE */

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_SIM_WCDMA_TUNEAWAY)

/*===========================================================================
FUNCTION     wl1_fill_tuneaway_duration

DESCRIPTION
  This is called by other modules to fill tuneaway duration (in frames)
  into our global variable.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None.
===========================================================================*/
extern void wl1_fill_tuneaway_duration(uint16 duration);

/*===========================================================================
FUNCTION     wl1_return_tuneaway_duration

DESCRIPTION
  This function returns duration of tuneaway, in frames to the caller.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
Tuneaway duration, in frames.
===========================================================================*/
extern uint16 wl1_return_tuneaway_duration(void);
#endif  /* #if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_SIM_WCDMA_TUNEAWAY) */

/*===========================================================================

FUNCTION     l1m_voice_call_only_active

DESCRIPTION
  This function indicates if voice call only is active
DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
===========================================================================*/
extern boolean l1m_voice_call_only_active(void);

/*=========================================================================

FUNCTION l1m_send_cphy_cell_trans_cnf_to_rrc

DESCRIPTION
  This function sends the RRC_CPHY_CELL_TRANSITION_CNF primitive to RRC.
  Currently this function is a standalone funciton called only during HSFACH via
  l1m_cphy_mac_dl_config_cnf(), when MAC sends the DL_CONFIG_CNF.
 
  In regular case, RRC_CPHY_CELL_TRANSITION_CNF will be sent via l1m_dl_phychan_drop_done_cmd().
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/
extern void l1m_send_cphy_cell_trans_cnf_to_rrc(boolean, boolean);

/*=========================================================================

FUNCTION l1m_send_setup_done_cmd

DESCRIPTION
  This function is used to post l1m_setup_done_cmd to l1m along with the request that
  has been completed

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/
extern void l1m_send_setup_done_cmd(cphy_setup_cmd_id_enum_type setup_cmd_id);

/*===========================================================================
FUNCTION        l1m_get_wait_on_scch_order_bmsk_dc_hsupa

DESCRIPTION
            
DEPENDENCIES


RETURN VALUE
  Boolean

SIDE EFFECTS

===========================================================================*/
extern boolean l1m_get_wait_on_scch_order_bmsk_dc_hsupa(void);

/*=========================================================================

FUNCTION L1M_IS_HHO_SUSPEND_NEEDED

DESCRIPTION
  This function returns whether suspend in needed in case of an HHO 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None.

=========================================================================*/
boolean l1m_is_hho_suspend_needed( void );

/*=========================================================================

FUNCTION l1m_validate_cphy_gen_params

DESCRIPTION
  This is a generic function which validates certain cphy parameters during channel setup.
  Currently,validation for FBI bits is done when trying to add a channel with CLTD enabled.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None.

=========================================================================*/
extern boolean l1m_validate_cphy_gen_params(l1_setup_cmd_type *cmd, uint32 req_mask);

/*===========================================================================
FUNCTION        l1m_cphy_next_frame_wanted_cb

DESCRIPTION     This function returns the next frame wanted to DRX. As L1M 
                entity doesn't any schedule, it always returns ALWAYS_ACTIVE.

DEPENDENCIES    None

RETURN VALUE    Always TRUE

SIDE EFFECTS    None
===========================================================================*/
boolean l1m_cphy_next_frame_wanted_cb(drx_entity_handle_type          entity_handle,
  drx_manager_sleep_struct_type  *sleep_info_ptr);

/*===========================================================================
FUNCTION        l1m_cphy_sleep_wakeup_cb

DESCRIPTION     No Action needed in this callback
                because L1M doesn't any sleep/wakeup functionality

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void l1m_cphy_sleep_wakeup_cb(drx_entity_handle_type entity_handle);

/*===========================================================================
FUNCTION        l1m_cphy_suspend_periodic_ops

DESCRIPTION     This function suspends all periodic ops before CPHY req is processed

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
boolean l1m_cphy_suspend_all_periodic_ops(void);

/*===========================================================================
FUNCTION        l1m_cphy_resume_suspending_periodic_ops

DESCRIPTION    This is a callback passed to a module to resume CPHY processing

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void l1m_cphy_resume_suspending_periodic_ops(void);

/*===========================================================================
FUNCTION        l1m_cphy_resume_all_periodic_ops

DESCRIPTION     This function resumes all periodic ops after CPHY req is processed

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void l1m_cphy_resume_all_periodic_ops(void);

/*=========================================================================

FUNCTION l1m_is_dl_phychan_cfg_ongoing

DESCRIPTION
  This function returns true if the dl config for the specific carr idx is in progress

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None.

=========================================================================*/ 
extern boolean l1m_is_dl_phychan_cfg_ongoing(wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================================

FUNCTION l1m_get_activation_time

DESCRIPTION
  This function returns the activation time for cphy setup

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  uint16 CFN value

SIDE EFFECTS
  None.

=========================================================================*/ 
extern uint16 l1m_get_activation_time(void);

extern boolean l1m_voice_call_active (void);

#ifdef FEATURE_QSH_DUMP
/*=========================================================================

FUNCTION l1m_qsh_populate_common_dump_info

DESCRIPTION
  This function populates L1M related dump info

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  uint16 CFN value

SIDE EFFECTS
  None.

=========================================================================*/ 
extern void l1m_qsh_populate_common_dump_info(wl1_common_mini_debug_info_struct_type *common_dump_ptr);
#endif /* FEATURE_QSH_DUMP */ 

#endif /* L1MSETUP_H */
