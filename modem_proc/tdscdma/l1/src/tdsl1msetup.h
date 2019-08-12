#ifndef TDSL1MSETUP_H
#define TDSL1MSETUP_H
/*===========================================================================

      L 1 M   P H Y S I C A L    C H A N N E L    S E T U P    D E C L S

GENERAL DESCRIPTION
  This file contains the declarations and function prototypes for
  the Layer 1 Manager physical channel setup procedure, the state
  machine for Layer 1 and procedures for cell transition.

EXTERNALIZED FUNCTIONS

  tdsl1msetup_cphy_setup_req_cmd
    This function handles the TDSL1_CPHY_SETUP_REQ primitive from L2/L3 and is
    called by L1M when it receives a TDSL1_CPHY_SETUP_REQ command. This function
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

    The tdsl1m_global.l1_state variable is changed to the new state if there is a state
    transition possible. The cleanup routine of the current state is called
    followed by the initialization routine of the new state.

  tdsl1msetup_send_cphy_setup_cnf_to_rrc
    This function sends the TDSRRC_CPHY_SETUP_CNF primitive to RRC, which
    includes status on each operation performed and an overall status.

    This function is called from tdsl1msetup_cphy_setup_next_cmd_handler(), in the
    TDSL1_CPHY_SETUP_DONE case, if MAC is not processing a corresponding config
    request next, which happens in the case of a MAC command with an
    activation time.  It is also called from l1m_cphy_mac_config_cnf(),
    which means MAC has finished processing its config request as well as L1.

  tdsl1msetup_cphy_setup_next_cmd_handler
    This function determines the next command to be handled that is part
    of the physical channel setup command. It is called first from the
    physical channel setup command handler when the TDSL1_CPHY_SETUP_REQ command
    is first received by L1M, and then subsequently by the Local Command
    Handler that sequences the physical channel setup procedure.

    This function determines the next command to be handled in the physical
    channel setup procedure and then calls the corresponding handler.

  tdsl1msetup_cphy_cell_transition_req_cmd
    This function handles the TDSL1_CPHY_CELL_TRANSITION_REQ primitive.
    This function saves the cell transition request and then sends
    an ASET update command to Search module. The rest of the cell
    transition command is processed after search completes the ASET
    update and sends a local command to L1M.

  tdsl1msetup_phychan_aset_done
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
    for a TDS_MODULE_CLEANUP_DONE_CMD from the modules before performing
    a state transition.

  tdsl1msetup_cell_trans_next_cmd_handler
    This function determines the next command to be handled that is part
    of the cell transition request physical channel setups. This function
    is called after we perform a state transition and we drop all existing
    channels.
    After all the commands have been handled, a confirmation is sent to
    RRC indicating the overall status of the cell transition request.

  tdsl1msetup_dl_cctrch_setup_done_cmd
    This function handles the next step after the DL CCTrCh is set up.
    If the DL CCTrCh setup fails, it sends a confirmation to RRC, otherwise
    it handles the next command in the TDSL1_CPHY_SETUP_REQ command.

  tdsl1msetup_ul_phychan_drop_done_cmd
    This function handles the next step after the UL Physical Channel
    was dropped. If a PRACH was dropped, we proceed to drop the DL
    AICH also, otherwise the next command in the TDSL1_CPHY_SETUP_REQ command
    is processed.

  tdsl1msetup_dl_phychan_add_done_cmd
    This function handles the next step after a DL physical channel was
    added. If an AICH physical channel was added, we proceed to add the
    UL PRACH, otherwise the next command in the TDSL1_CPHY_SETUP_REQ command
    is processed.

INTERNALIZED FUNCTIONS

  tdsl1msetup_validate_state_trans
    This function is called during the physical channel setup command
    (TDSL1_CPHY_SETUP_REQ). This function will check to see if the physical
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

    The actual state transition is made after the TDSL1_CPHY_SETUP_REQ command
    validation completes successfully.

  tdsl1msetup_get_next_cphy_setup_cmd_id
    This function returns the ID of the next command to be processed as
    part of the TDSL1_CPHY_SETUP_REQ command.

    The request mask in the tdsl1m_global.cphy_setup_cmd packet has the following format:

    Bit #  Command
    -----  -------
    0      TDSL1_CPHY_DL_CCTRCH_REQ: setup DL CCTRCH
    1      TDSL1_CPHY_UL_CCTRCH_REQ: setup UL CCTRCH
    2      TDSL1_CPHY_UL_PHYCHAN_DROP_REQ: drop UL physical channel
    3      TDSL1_CPHY_DL_PHYCHAN_DROP_REQ: drop DL physical channel
    4      CPHY_DL_PHYCHAN_CFG_REQ: config DL physical channel
    5      TDSL1_CPHY_UL_PHYCHAN_CFG_REQ: config UL physical channel
    6      TDSL1_CPHY_DL_PHYCHAN_ADD_REQ: add DL physical channel
    7      TDSL1_CPHY_UL_PHYCHAN_ADD_REQ: add UL physical channel
    8-15   Reserved

  save_cphy_setup_req
    This function copies contents of cmd into tdsl1m_global.cphy_setup_cmd. Pointer values
    are saved as is, instead of copying all the contents of the pointer. The
    assumption is that L2/L3 will not overwrite those pointers until they
    receive a confirm or error indication for a command.

  tdsl1msetup_validate_cell_trans
    This function validates the channel setup parameters that are sent
    as part of the cell transition request command. The request mask
    in the command is used to validate each of the sub-primitives in
    the command. The request mask is the same as the one used for
    TDSL1_CPHY_SETUP_REQ commands.

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

  tdsl1msetup_dl_cctrch_validate
    This function validates the Downlink CCTrCh setup request.

  tdsl1msetup_dl_cctrch_setup
    This function calls the Downlink module to set up the DL CCTrCh.

  tdsl1msetup_ul_cctrch_setup
    This function calls the uplink module to set up the UL CCTrCh.
    If the request is to set up an UL PRACH, then we first set up
    the DL AICH.

  tdsl1msetup_ul_cctrch_setup_cell_trans
    This function calls the uplink module to set up the UL CCTrCh.
    If the request is to set up an UL PRACH, then we first set up
    the DL AICH.
    This function is only called during channel setup during
    a cell transition after a cell reselection.

  tdsl1msetup_setup_next_cmd
    This function handles the next command after a step in the physical
    channel setup procedure. Appropriate functions are called depending
    on whether we are setting up physical channels as a result of
    TDSL1_CPHY_CELL_TRANSITION_REQ or as a result of TDSL1_CPHY_SETUP_REQ.

  tdsl1msetup_cphy_setup_cmd_post_process
    This function is called before the start of the physical
    channel setups. This function will perform state
    transition if it needs to be done, otherwise it starts
    the channel setup process.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2000-2010 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsl1msetup.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/18/11   cxd     Modified event sequence for CPHY_SETUP_REQ to remove dependency between UL/DL
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "tdsl1rrcif.h"
#include "tdsl1m.h"
#include "customer.h"


/*===========================================================================

                        TYPE DECLARATIONS

===========================================================================*/
typedef uint32           TDSL1MSETUP_STMR_TX_TIME_STATUS_MICRO_TYPE;

typedef enum
{
  TDSL1MSETUP_MDSP_SYNC_CMD_PRE_CB = 0,
  TDSL1MSETUP_MDSP_SYNC_CMD_POST_CB,
  TDSL1MSETUP_NUM_MDSP_SYNC_CMD_CB
} tdsl1msetup_mdsp_sync_cmd_cb_enum_type;

typedef struct
{
  uint8                          current_cfn;
  uint16                         current_tick;
  uint16                         current_status;
  TDSL1MSETUP_STMR_TX_TIME_STATUS_MICRO_TYPE tx_slot_count;
  TDSL1MSETUP_STMR_TX_TIME_STATUS_MICRO_TYPE tx_tti_count;
  TDSL1MSETUP_STMR_TX_TIME_STATUS_MICRO_TYPE tx_frame_count;
} tdsl1msetup_phychan_cfg_dbg_struct_type;

#define TDSL1MSETUP_MAX_SLAMS_DURING_HHO_CONFIG 20

typedef enum
{
  TDSL1MSETUP_HHO_CFN_ON_OLD_CFG = 0,
  TDSL1MSETUP_HHO_CM_SUSPEND_DUR_ON_OLD_CFG,
  TDSL1MSETUP_HHO_MAC_SUSPEND_DUR_ON_OLD_CFG
} tdsl1msetup_hho_compute_oldcfg_timeline_enum_type;

typedef enum
{
  TDSL1MSETUP_NEXT_REQ_MODE_ANY = 0,
  TDSL1MSETUP_NEXT_REQ_MODE_DL,
  TDSL1MSETUP_NEXT_REQ_MODE_UL
} tdsl1msetup_next_req_mode_enum_type;

typedef enum
{
  TDSL1MSETUP_ASET_UPDATE_NONE_BHO = 0,
  TDSL1MSETUP_ASET_UPDATE_BHO_UL,
  TDSL1MSETUP_ASET_UPDATE_BHO_DL
} tdsl1msetup_aset_update_enum_type;


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define TDSL1MSETUP_CPHY_HHO_VALIDATE_MASK \
  (TDSL1_CPHY_HHO_INTERF_TI_HHO_INCL| \
   TDSL1_CPHY_HHO_INTERF_TM_HHO_INCL| \
   TDSL1_CPHY_HHO_INTRAF_TI_HHO_INCL| \
   TDSL1_CPHY_HHO_INTRAF_TM_HHO_INCL)

#define TDSL1MSETUP_CPHY_HHO_CFG_REQ_IS_CFG_INCL(mask, cfg) \
  (((mask) & TDSL1_CPHY_HHO_##cfg##_INCL) ? TRUE : FALSE)
  
/* This macro returns TRUE if the following conditions are not satisfied
   -- If more than one HHO bits are not set in the input argument
   -- and if revert back to old config is present in the input argument, 
      it will check the setup is HHO */
#define TDSL1MSETUP_CPHY_VALIDATE_HHO_CFG(mask) \
  (((((uint16)((mask&TDSL1MSETUP_CPHY_HHO_VALIDATE_MASK)-1) & \
      (uint16)(mask&TDSL1MSETUP_CPHY_HHO_VALIDATE_MASK)) == 0) && \
    ((mask&TDSL1_CPHY_HHO_RESUME_TO_OLD_CONFIG_HHO_INCL != 0 ) && \
     ((mask&(TDSL1MSETUP_CPHY_HHO_VALIDATE_MASK)))))?TRUE:FALSE)

/*--------------------------------------------------------------------------
  The following bit masks define the set of Transport Channel types that
  are valid in each of the L1 states. The bit fields for each of the
  Transport Channels are defined in the file tdsl1dlcctrchcfg.h.

  ------------------------------------
   L1        Transport Channel Types
  State    SBCH NBCH PCH FACH DCH DSCH
  ------------------------------------
  BCH       1    0    0   0    0    0
  PCH       x    0    1   0    0    0
  FACH      x    x    x   1    0    0
  DCH       x    x    x   x    1    x
---------------------------------------------------------------------------*/
#define TDSL1MSETUP_BCH_STATE_VALID_MASK ( TDSL1_DL_TRCH_REFTYPE_SBCH_BF )


#define TDSL1MSETUP_PCH_STATE_VALID_MASK ( TDSL1_DL_TRCH_REFTYPE_PCH_BF | \
                                   TDSL1_DL_TRCH_REFTYPE_SBCH_BF | \
                                   TDSL1_DL_TRCH_REFTYPE_NBCH_BF )

#define TDSL1MSETUP_FACH_STATE_VALID_MASK ( TDSL1_DL_TRCH_REFTYPE_FACH_BF | \
                                    TDSL1_DL_TRCH_REFTYPE_PCH_BF  | \
                                    TDSL1_DL_TRCH_REFTYPE_SBCH_BF | \
                                    TDSL1_DL_TRCH_REFTYPE_NBCH_BF )


#define TDSL1MSETUP_DCH_STATE_VALID_MASK ( TDSL1_DL_TRCH_REFTYPE_DCH_BF  | \
                                   TDSL1_DL_TRCH_REFTYPE_DSCH_BF | \
                                   TDSL1_DL_TRCH_REFTYPE_FACH_BF | \
                                   TDSL1_DL_TRCH_REFTYPE_PCH_BF  | \
                                   TDSL1_DL_TRCH_REFTYPE_SBCH_BF | \
                                   TDSL1_DL_TRCH_REFTYPE_NBCH_BF )


#define TDSL1MSETUP_SYNC_STATE_MASK 0x00

/*--------------------------------------------------------------------------
  Macros to check if the next state is valid from the current state.
  Each of the macro is passed a mask of the TrCh's for the next state.
---------------------------------------------------------------------------*/

/* This macro checks to see if the bit mask of TrCh's for the BCH state
 * is valid. The passed bit mask should be a sub-set of the valid state
 * mask for the BCH state and the TrCh bit for SBCH should be set.
 */
#define TDSL1MSETUP_BCH_STATE_VALID( next_state_mask )         \
  ( ( ( (next_state_mask) | TDSL1MSETUP_BCH_STATE_VALID_MASK ) \
       == TDSL1MSETUP_BCH_STATE_VALID_MASK ) &&                \
       (next_state_mask & TDSL1_DL_TRCH_REFTYPE_SBCH_BF) )

/* This macro checks to see if the bit mask of TrCh's for the PCH state
 * is valid. The passed bit mask should be a sub-set of the valid state
 * mask for the PCH state and the TrCh bit for PCH should be set.
 */
#define TDSL1MSETUP_PCH_STATE_VALID( next_state_mask )         \
  ( ( ( (next_state_mask) | TDSL1MSETUP_PCH_STATE_VALID_MASK ) \
       == TDSL1MSETUP_PCH_STATE_VALID_MASK ) &&                \
       (next_state_mask & TDSL1_DL_TRCH_REFTYPE_PCH_BF) )

/* This macro checks to see if the bit mask of TrCh's for the FACH state
 * is valid. The passed bit mask should be a sub-set of the valid state
 * mask for the FACH state and the TrCh bit for FACH should be set.
 */
#define TDSL1MSETUP_FACH_STATE_VALID( next_state_mask )         \
  ( ( ( (next_state_mask) | TDSL1MSETUP_FACH_STATE_VALID_MASK ) \
       == TDSL1MSETUP_FACH_STATE_VALID_MASK ) &&                \
       (next_state_mask & TDSL1_DL_TRCH_REFTYPE_FACH_BF) )

/* This macro checks to see if the bit mask of TrCh's for the DCH state
 * is valid. The passed bit mask should be a sub-set of the valid state
 * mask for the DCH state and the TrCh bit for DCH should be set.
 */
#define TDSL1MSETUP_DCH_STATE_VALID( next_state_mask )         \
  ( ( ( (next_state_mask) | TDSL1MSETUP_DCH_STATE_VALID_MASK ) \
       == TDSL1MSETUP_DCH_STATE_VALID_MASK ) &&                \
       (next_state_mask & TDSL1_DL_TRCH_REFTYPE_DCH_BF) )

/* Check to see if the next state mask is 0, which implies we have
 * dropped all channels, with no channels to add, and we will be
 * transitioning to TDSL1M_IDLE state.
 */
#define TDSL1MSETUP_SYNC_STATE_VALID( next_state_mask ) \
 ( next_state_mask == TDSL1MSETUP_SYNC_STATE_MASK )


#define TDSL1MSETUP_RESET_CELL_TRANS_PROGRESS() do                                    \
                                        {                                     \
                                          tdsl1m_global.cell_trans_in_progress = FALSE; \
                                        } while (0)

/* This flag is set by L1M when UL drops a physical channel */
#define TDSL1MSETUP_UL_PHYCHAN_DROP_MASK 0x1
/* This flag is set by L1M when DL drops a physical channel */
#define TDSL1MSETUP_PHYCHAN_DROP_MASK 0x2
/* This flag is set when UL and DL need to be dropped together */
#define TDSL1MSETUP_UL_DL_PHYCHAN_DROP_MASK 0x4

/* Whether L1 should wait for both UL/DL drop before proceeding */
#define TDSL1MSETUP_IS_WAIT_UL_DL_DROP() \
  (tdsl1m_global.phychan_drop_mask & TDSL1MSETUP_UL_DL_PHYCHAN_DROP_MASK)

/* Clear wait UL/DL drop flag */
#define TDSL1MSETUP_CLEAR_WAIT_UL_DL_DROP() \
  (tdsl1m_global.phychan_drop_mask = 0)

#define TDSL1MSETUP_UL_DL_DROP_IS_FINISHED() \
  (tdsl1m_global.phychan_drop_mask == 0)

/* Whether UL phychan drop has been completed */
#define TDSL1MSETUP_IS_UL_PHYCHAN_DROP_DONE() \
  (tdsl1m_global.phychan_drop_mask & TDSL1MSETUP_UL_PHYCHAN_DROP_MASK)

/* Whether DL phychan drop has been completed */
#define TDSL1MSETUP_IS_DL_PHYCHAN_DROP_DONE() \
  (tdsl1m_global.phychan_drop_mask & TDSL1MSETUP_PHYCHAN_DROP_MASK)

/* Whether UL/DL phychan drops have been completed */
#define TDSL1MSETUP_IS_UL_DL_PHYCHAN_DROP_DONE() \
  ((tdsl1m_global.phychan_drop_mask & TDSL1MSETUP_PHYCHAN_DROP_MASK) && (tdsl1m_global.phychan_drop_mask & TDSL1MSETUP_UL_PHYCHAN_DROP_MASK))

/* Set UL drop done */
#define TDSL1MSETUP_UL_PHYCHAN_DROP_DONE() \
  (tdsl1m_global.phychan_drop_mask |= TDSL1MSETUP_UL_PHYCHAN_DROP_MASK)

/* Set DL drop done */
#define TDSL1MSETUP_DL_PHYCHAN_DROP_DONE() \
  (tdsl1m_global.phychan_drop_mask |= TDSL1MSETUP_PHYCHAN_DROP_MASK)

#ifdef FEATURE_TDSCDMA_HSUPA
#define TDSL1MSETUP_IS_EUL_TO_STOP_AT_RECFG() \
  ((tdsl1m_global.cphy_setup_cmd.e_req_mask & TDSL1_EUL_STOP) == TDSL1_EUL_STOP)
#endif

#ifdef FEATURE_TDSCDMA_GPS_CELL_ID_READING
  #define TDSL1MSETUP_IS_CPHY_SETUP_FOR_FOR_CELL_ID_BCH_ADD(l1_cphy_setup_cmd) ((tdsl1m_global.l1_state == TDSL1M_DCH)  \
    && (TDSL1_CPHY_SETUP_REQ_IS_DL_PHYCHAN_ADD_INCL(l1_cphy_setup_cmd.req_mask))  \
    && (l1_cphy_setup_cmd.add_dl_phychan == TDSL1_DL_PHYCHAN_PCCPCH_S))
#endif

#define TDSL1MSETUP_IS_CPHY_SETUP_REQ_INCLUDE_DPCH_CFG(l1_cphy_setup_cmd) (  \
    ((TDSL1_CPHY_SETUP_REQ_IS_DL_PHYCHAN_ADD_INCL(l1_cphy_setup_cmd.req_mask)) &&   \
     (l1_cphy_setup_cmd.add_dl_phychan == TDSL1_DL_PHYCHAN_DPCH)) || \
    ((TDSL1_CPHY_SETUP_REQ_IS_DL_PHYCHAN_CFG_INCL(l1_cphy_setup_cmd.req_mask)) &&   \
        (l1_cphy_setup_cmd.cfg_dl_phychan == TDSL1_DL_PHYCHAN_DPCH)) || \
    ((TDSL1_CPHY_SETUP_REQ_IS_DL_PHYCHAN_DROP_INCL(l1_cphy_setup_cmd.req_mask)) &&   \
        (l1_cphy_setup_cmd.drop_dl_phychan == TDSL1_DL_PHYCHAN_DPCH)))

#define TDSL1MSETUP_ACT_FRAME_DELTA      1
#define TDSL1MSETUP_MISSED_ACT_TIME_WINDOW_SIZE 8
/*===========================================================================
FUNCTION        tdsl1msetup_send_rrc_failure_confirmation

DESCRIPTION
  This function sends the TDSRRC_CPHY_SETUP_CNF primitive with a status of
  FAILURE, obviously meaning something bad happened while processing
  the TDSL1_CPHY_SETUP_REQ command.

PARAMETERS
  uint8 seq_num - If the TDSL1_CPHY_SETUP_REQ had an activation time, this is the
                  sequence number of the command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sends TDSRRC_CPHY_SETUP_CNF with FAILURE status to RRC.
===========================================================================*/
extern void tdsl1msetup_send_rrc_failure_confirmation(uint8 seq_num, tdsl1_setup_cnf_status_enum_type status_type);

/*===========================================================================
FUNCTION        tdsl1msetup_cphy_setup_req_cmd

FILE NAME       l1m_setup.c

DESCRIPTION
  This function handles the TDSL1_CPHY_SETUP_REQ primitive from L2/L3 and is called
  by L1M when it receives a TDSL1_CPHY_SETUP_REQ command. This function will first
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
  The tdsl1m_global.l1_state variable is changed to the new state if there is a state
  transition possible. The cleanup routine of the current state is called
  followed by the initialization routine of the new state.

===========================================================================*/
extern void tdsl1msetup_cphy_setup_req_cmd
(
  /* command parameters struct pointer */
  tdsl1_ext_cmd_type *cmd
);

/*===========================================================================
FUNCTION        tdsl1msetup_cphy_setup_req_wrapper

DESCRIPTION
    This function handles the TDSL1_CPHY_SETUP_REQ primitive 
    and try to request band change if necessary 

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
 

===========================================================================*/

extern void tdsl1msetup_cphy_setup_req_wrapper( tdsl1_ext_cmd_type *cmd );


/*===========================================================================
FUNCTION         tdsl1msetup_validate_cphy_setup_req_parms

DESCRIPTION
  This function validates each of the parameters in the TDSL1_CPHY_SETUP_REQ
  command. The validate routines are called based on what actions are
  being performed in the command. If any validations fails a CPHY_SETUP_CNF
  command is sent to RRC with a fail status.

DEPENDENCIES

RETURN VALUE
  Boolean. True if all validation passes, False if any validation fails.

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsl1msetup_validate_cphy_setup_req_parms
(
  tdsl1_ext_cmd_type *cmd
);

/*===========================================================================
FUNCTION         tdsl1msetup_identify_dch_handover

DESCRIPTION
  This function use information in TDSL1_CPHY_SETUP_REQ to identify the DCH 
  setup category handover, recover or regular.  

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_identify_dch_handover
(
  tdsl1_ext_cmd_type *cmd
);

/*===========================================================================
FUNCTION         tdsl1msetup_convert_target_act_time

DESCRIPTION
  This function use to convert active time of TDSL1_CPHY_SETUP_REQ that based on 
  target cell CFN to activation time based on serving cell CFN. This happens during
  Handover scenarios in L1 DCH.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsl1msetup_convert_target_act_time
(
  tdsl1_ext_cmd_type *cmd
);

/*===========================================================================
FUNCTION         tdsl1msetup_tune_rf_freq

DESCRIPTION
  This function use to tune RF frequency.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsl1msetup_tune_rf_freq
(
  uint16 freq
);

//#ifdef FEATURE_TDSCDMA_L2TD_PS_HANDOVER 
/*===========================================================================
FUNCTION         tdsl1msetup_dch_handover_handler

DESCRIPTION
  This function is used to handle the handover for TDL1_CPHY_SETUP_REQ in 
  DCH state.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_dch_handover_handler
(
  tdsl1_setup_cmd_type *setup_cmd_ptr
);
//#endif

/*===========================================================================
FUNCTION         tdsl1msetup_dch_x2t_psho_handler

DESCRIPTION
  This function is used to handle the X2T PSHO for TDL1_CPHY_SETUP_REQ in 
  DCH state.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_dch_x2t_psho_handler
(
  tdsl1_setup_cmd_type *setup_cmd_ptr
);

/*===========================================================================
FUNCTION         tdsl1msetup_cphy_setup_cmd_post_process

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
extern void tdsl1msetup_cphy_setup_cmd_post_process( void );

/*===========================================================================
FUNCTION         tdsl1msetup_drop_all_channel

DESCRIPTION
  This function is called to drop all L1 channels

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1msetup_drop_all_channel( tdsl1m_drop_all_cause_enum_type casue );

/*===========================================================================
FUNCTION         tdsl1msetup_hspa_drop_process

DESCRIPTION
  This function is called to drop all HSDPA/HSUPA channels

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdsl1msetup_hspa_drop_process( void );

/*===========================================================================
FUNCTION         tdsl1msetup_hsdpa_process_after_drop

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1msetup_hsdpa_process_after_drop(void);

/*===========================================================================
FUNCTION         tdsl1msetup_hsupa_process_after_drop

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1msetup_hsupa_process_after_drop( void );

/*===========================================================================
FUNCTION         tdsl1msetup_send_drop_all_cnf

DESCRIPTION
  This function is called to send confirmation to RRC, 
  IDLE_CNF or SETUP_CNF

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1msetup_send_drop_all_cnf( tdsl1m_drop_all_cause_enum_type cause );

/*===========================================================================

FUNCTION tdsl1msetup_fail_remaining_cphy_setup_cmd_steps

DESCRIPTION
  This function systematically and without concern for others fails any
  and all steps remaining to be processed in the current TDSL1_CPHY_SETUP_REQ
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
extern void tdsl1msetup_fail_remaining_cphy_setup_cmd_steps(void);

/*=========================================================================

FUNCTION tdsl1msetup_send_cphy_setup_cnf_to_rrc

DESCRIPTION
  This function sends the TDSRRC_CPHY_SETUP_CNF primitive to RRC, which
  includes status on each operation performed and an overall status.

  This function is called from tdsl1msetup_cphy_setup_next_cmd_handler(), in the
  TDSL1_CPHY_SETUP_DONE case, if MAC is not processing a corresponding config
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
  Sends TDSRRC_CPHY_SETUP_CNF primitive with status to RRC.

=========================================================================*/
extern void tdsl1msetup_send_cphy_setup_cnf_to_rrc(void);

/*===========================================================================

FUNCTION tdsl1msetup_cphy_setup_next_cmd_handler

DESCRIPTION
  This function determines the next command to be handled that is part
  of the physical channel setup command. It is called first from the
  physical channel setup command handler when the TDSL1_CPHY_SETUP_REQ command
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
void tdsl1msetup_cphy_setup_next_cmd_handler
(
   tdsl1msetup_next_req_mode_enum_type next_req_mode
);

/*===========================================================================

FUNCTION tdsl1msetup_channel_setup_start

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_channel_setup_start(void);

/*===========================================================================

FUNCTION tdsl1msetup_channel_setup_post_cmd

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_channel_setup_post_cmd(uint16 subfn, uint32 cfg_db_idx);

/*===========================================================================

FUNCTION tdsl1msetup_channel_disable_meas_schedule

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_channel_disable_meas_schedule(uint16 subfn, uint32 cfg_db_idx);

/*===========================================================================

FUNCTION tdsl1msetup_channel_restore_meas_schedule

DESCRIPTION  only used in CPHY_CMD_CANCEL
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_channel_restore_meas_schedule(void);

/*===========================================================================

FUNCTION tdsl1msetup_register_channel_setup_cb

DESCRIPTION Register callback function to start channel setup at next frame
            boundary
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_register_channel_setup_cb(void);

/*===========================================================================

FUNCTION tdsl1msetup_register_disable_meas_schedule_cb

DESCRIPTION Register callback function to disable meas scheduling at act_time - 2 
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_register_disable_meas_schedule_cb(uint8 act_time);

/*===========================================================================

FUNCTION tdsl1msetup_register_align_cphy_setup_add_cb

DESCRIPTION Register callback function to align CPHY_SETUP or CELL TRANSITION at next frame boundary
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_register_align_cphy_setup_add_cb(uint32 req_id);

/*===========================================================================

FUNCTION tdsl1msetup_pre_act_time_adjust

DESCRIPTION adjust SEQUENCER tdsseq_dispatch_pending_cmds delta_frame_act
  

DEPENDENCIES
  None

RETURN VALUE
  uint8 delta_frame_act

SIDE EFFECTS
  None
===========================================================================*/
uint8 tdsl1msetup_pre_act_time_adjust(uint8 old_delta_frame_act);


/*===========================================================================

FUNCTION tdsl1msetup_validate_state_trans

DESCRIPTION
  This function is called during the physical channel setup command
  (TDSL1_CPHY_SETUP_REQ). This function will check to see if the physical
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

  The actual state transition is made after the TDSL1_CPHY_SETUP_REQ command
  validation completes successfully.

DEPENDENCIES
  None

RETURN VALUE
  This function returns TRUE if the TDSL1_CPHY_SETUP_REQ results in a valid state
  transition, or FALSE otherwise.

SIDE EFFECTS
  If there are any CCTrCh Add and/or Drops, the tdsl1m_global.next_trch_setup_mask is
  modified to reflect the new set of CCTrChs. If there is a valid state
  transition possible, tdsl1m_global.l1_next_state is set to the new state.

===========================================================================*/
boolean tdsl1msetup_validate_state_trans
(
  tdsl1_setup_cmd_type* cmd,
  tdsl1m_state_enum_type* l1_next_state_ptr,
  uint8* next_trch_setup_mask_ptr
);

/*===========================================================================
FUNCTION        tdsl1msetup_do_state_transition

FILE NAME       l1msetup.c

DESCRIPTION     This function performs a L1 state transition. When the argument
                normal is set to TRUE, the function assumes that the transition
                is part of normal operation. Otherwise, the function assumes an
                erroneous or abnormal scenario under which it is called, and it
                sets up flags/commands to facilitate applicable and available
                error recovery procedures.

DEPENDENCIES    The variable tdsl1m_global.l1_next_state

RETURN VALUE    None

SIDE EFFECTS    The L1 state will be changed from tdsl1m_global.l1_state to tdsl1m_global.l1_next_state
===========================================================================*/
void tdsl1msetup_do_state_transition( void );

/*===========================================================================
FUNCTION        tdsl1msetup_save_cphy_setup_req

FILE NAME

DESCRIPTION
  This function copies contents of cmd into tdsl1m_global.cphy_setup_cmd. Pointer values
  are saved as is, instead of copying all the contents of the pointer. The
  assumption is that L2/L3 will not overwrite those pointers until they
  receive a confirm or error indication for a command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Saves the contents of the cphy setup request into the variable tdsl1m_global.cphy_setup_cmd.
  This function only does a shallow copy of all the fields in the cmd packets.
  Any pointers to buffers are assumed valid until the CPHY_SETUP_CNF is
  sent from L1.

===========================================================================*/
void tdsl1msetup_save_cphy_setup_req( tdsl1_ext_cmd_type *cmd );

/*===========================================================================

FUNCTION tdsl1msetup_get_next_cphy_setup_cmd_id

DESCRIPTION
  This function returns the ID of the next command to be processed as
  part of the TDSL1_CPHY_SETUP_REQ command.

  The request mask in the tdsl1m_global.cphy_setup_cmd packet has the following format:

  Bit #  Command
  -----  -------
  0      TDSL1_CPHY_DL_CCTRCH_REQ: setup DL CCTRCH
  1      TDSL1_CPHY_UL_CCTRCH_REQ: setup UL CCTRCH
  2      TDSL1_CPHY_UL_PHYCHAN_DROP_REQ: drop UL physical channel
  3      TDSL1_CPHY_DL_PHYCHAN_DROP_REQ: drop DL physical channel
  4      CPHY_DL_PHYCHAN_CFG_REQ: config DL physical channel
  5      TDSL1_CPHY_UL_PHYCHAN_CFG_REQ: config UL physical channel
  6      TDSL1_CPHY_DL_PHYCHAN_ADD_REQ: add DL physical channel
  7      TDSL1_CPHY_UL_PHYCHAN_ADD_REQ: add UL physical channel
  8-15   Reserved

DEPENDENCIES
  None

RETURN VALUE
  tdsl1_cphy_setup_cmd_id_enum_type value that corresponds to the next command
  to be processed in the physical channel setup request command.

SIDE EFFECTS
  If a bit in the request mask is set that corresponds to a command, that
  bit is turned off in the tdsl1m_global.cphy_setup_req_mask so that it will not be
  processed again.

===========================================================================*/
tdsl1_cphy_setup_cmd_id_enum_type tdsl1msetup_get_next_cphy_setup_cmd_id( uint16* mask, tdsl1msetup_next_req_mode_enum_type next_req_mode );

/*===========================================================================

FUNCTION      tdsl1msetup_dl_cctrch_validate

DESCRIPTION
  This function validates the Downlink CCTrCh setup request.

DEPENDENCIES
  None

RETURN VALUE
  Boolean: TRUE if validation succeeds, false otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsl1msetup_dl_cctrch_validate( tdsl1_setup_cmd_type* cmd );

/*===========================================================================

FUNCTION      tdsl1msetup_dl_cctrch_setup

DESCRIPTION
  This function calls the Downlink module to set up the DL CCTrCh.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_dl_cctrch_setup( tdsl1_setup_cmd_type* setup );

/*==============================================================a=============

FUNCTION      tdsl1msetup_ul_cctrch_setup

DESCRIPTION
  This function calls the uplink module to set up the UL CCTrCh.
  If the request is to set up an UL PRACH, then we first set up
  the DL AICH.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_ul_cctrch_setup( tdsl1_setup_cmd_type* setup );

/*==============================================================a=============

FUNCTION      tdsl1msetup_ul_cctrch_setup_cell_trans

DESCRIPTION
  This function calls the uplink module to set up the UL CCTrCh.
  If the request is to set up an UL PRACH, then we first set up
  the DL AICH.
  This function is only called during channel setup during
  a cell transition after a cell reselection.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_ul_cctrch_setup_cell_trans( tdsl1_cell_transition_cmd_type* setup );

/*===========================================================================

FUNCTION     tdsl1msetup_dl_cctrch_setup_done_cmd

DESCRIPTION
  This function handles the next step after the DL CCTrCh is set up.
  If the DL CCTrCh setup fails, it sends a confirmation to RRC, otherwise
  it handles the next command in the TDSL1_CPHY_SETUP_REQ command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A confirmation is sent to RRC if the DL CCTrCh set up failed.
===========================================================================*/
extern void tdsl1msetup_dl_cctrch_setup_done_cmd
(
  tdsdlchmgr_dl_cphy_setup_local_cmd_type* cmd_ptr
);

/*===========================================================================

FUNCTION     tdsl1msetup_ul_phychan_drop_done_cmd

DESCRIPTION
  This function handles the next step after the UL Physical Channel
  was dropped. If a PRACH was dropped, we proceed to drop the DL
  AICH also, otherwise the next command in the TDSL1_CPHY_SETUP_REQ command
  is processed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1msetup_ul_phychan_drop_done_cmd
(
  tdsulcmd_cphy_setup_local_cmd_type* cmd_ptr
);

/*===========================================================================

FUNCTION     tdsl1msetup_ul_phychan_cfg_done_cmd

DESCRIPTION
  This function handles the next step after the UL Physical Channel
  was configured. Need to handle BHO DL for T=80ms, internally hold 
  DL CCTRCH setup and DL physical channel CFG until act time + 7

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1msetup_ul_phychan_cfg_done_cmd(void);

/*===========================================================================

FUNCTION     tdsl1msetup_dl_phychan_drop_done_cmd

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1msetup_dl_phychan_drop_done_cmd
(
  tdsdlchmgr_dl_cphy_setup_local_cmd_type* cmd_ptr
);

/*===========================================================================

FUNCTION     tdsl1msetup_dl_phychan_add_done_cmd

DESCRIPTION
  This function handles the next step after a DL physical channel was
  added. If an AICH physical channel was added, we proceed to add the
  UL PRACH, otherwise the next command in the TDSL1_CPHY_SETUP_REQ command
  is processed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1msetup_dl_phychan_add_done_cmd
(
  tdsdlchmgr_dl_cphy_setup_local_cmd_type* cmd_ptr
);

/*===========================================================================

FUNCTION     tdsl1msetup_ul_resync_success_ind

DESCRIPTION
  This function handles the next step after UL re-sync procedure succeed. RF
  tune to working frequency if needed, then proceed to the next command in the 
  TDSL1_CPHY_SETUP_REQ command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1msetup_ul_resync_success_ind
(
  void
);

/*===========================================================================

FUNCTION     tdsl1msetup_ul_resync_fail_ind

DESCRIPTION
  This function handles the next step after UL re-sync procedure failed. 
  TDSRRC_CPHY_SETUP_CNF is sent to RRC with status fail. Transit L1M to
  TDSL1M_SYNC state, waiting for next TDSL1_CPHY_SETUP_REQ to restore the 
  old configuration. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1msetup_ul_resync_fail_ind
(
  void
);

/*===========================================================================

FUNCTION     tdsl1msetup_setup_next_cmd

DESCRIPTION
  This function handles the next command after a step in the physical
  channel setup procedure. Appropriate functions are called depending
  on whether we are setting up physical channels as a result of
  TDSL1_CPHY_CELL_TRANSITION_REQ or as a result of TDSL1_CPHY_SETUP_REQ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1msetup_setup_next_cmd( tdsl1msetup_next_req_mode_enum_type next_req_mode );

/*===========================================================================

FUNCTION    tdsl1msetup_send_aset_update_cmd

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void tdsl1msetup_send_aset_update_cmd
(
  tdsl1_setup_cmd_type* cmd
);

/* Cell Transition related functions */

/*===========================================================================
FUNCTION   tdsl1msetup_cphy_cell_transition_req_cmd

FILE NAME  l1msetup.c

DESCRIPTION
  This function handles the TDSL1_CPHY_CELL_TRANSITION_REQ primitive.
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
extern void tdsl1msetup_cphy_cell_transition_req_cmd
(
  tdsl1_cell_transition_cmd_type *cmd
);

/*===========================================================================
FUNCTION        tdsl1msetup_cphy_cell_transition_req_wrapper

DESCRIPTION
  This function is a wrapper function for tdsl1msetup_cphy_cell_transition_req_cmd.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tdsl1msetup_cphy_cell_transition_req_wrapper(tdsl1_ext_cmd_type *cmd);

/*===========================================================================
FUNCTION        tdsl1msetup_cell_trans_update_nbr_lists

DESCRIPTION
  In order to maintain continuous JDS, intra-f cell measurement and inter-f cell measurement will continue. 
  GSM neighbor cells and LTE neighbor cells will be cleared until we get new TDSL1_CPHY_MEASUREMENT_REQ.


PARAMETERS
  None.  

DEPENDENCIES
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsl1msetup_cell_trans_update_nbr_lists( void );

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
void tdsl1msetup_save_cphy_cell_trans_req( tdsl1_cell_transition_cmd_type* cmd );

/*===========================================================================

FUNCTION     tdsl1msetup_validate_cell_trans

DESCRIPTION
  This function validates the channel setup parameters that are sent
  as part of the cell transition request command. The request mask
  in the command is used to validate each of the sub-primitives in
  the command. The request mask is the same as the one used for
  TDSL1_CPHY_SETUP_REQ commands.

  In the event of a failed validation, a confirmation is sent to RRC
  with the status set to FALSE.

  If the validation passes, we first drop the UL channels and then
  drop the DL channels. The DL module will post a local command
  after all the DL physical channels are dropped.

DEPENDENCIES
  None

RETURN VALUE
  boolean, TRUE if validation passed, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdsl1msetup_validate_cell_trans( void );

/*===========================================================================

FUNCTION        tdsl1msetup_cell_trans_cmd_post_process

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
void tdsl1msetup_cell_trans_cmd_post_process( void );

/*===========================================================================

FUNCTION        tdsl1msetup_cell_trans_rf_tune_and_aset_update

DESCRIPTION
  This function will be called during the cell transition to tune RF if 
  primary frequency is changed and do aset update if cell is changed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Cleans up current state and initializes the next state.
 
===========================================================================*/
void tdsl1msetup_cell_trans_rf_tune_and_aset_update( uint16 freq, uint8 cpi );

/*===========================================================================

FUNCTION     tdsl1msetup_cell_trans_next_cmd_handler

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
void tdsl1msetup_cell_trans_next_cmd_handler( tdsl1msetup_next_req_mode_enum_type next_req_mode );

/*===========================================================================

FUNCTION tdsl1msetup_ul_phychan_drop_cmd_handler

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
void tdsl1msetup_ul_phychan_drop_cmd_handler
(
  tdsl1_setup_cmd_type* cmd
);

/*===========================================================================

FUNCTION     tdsl1msetup_save_interf_hho_cmd

DESCRIPTION
  Saves the parameters passed by RRC for CPHY_SETUP_CMD

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1msetup_save_interf_hho_cmd(void);

/*===========================================================================

FUNCTION     tdsl1msetup_is_hho_timing_ind

DESCRIPTION
  Returns  Timing Indication Type based on  the  HHO  request is  a  timing
  initialized or timing maintained case.

DEPENDENCIES
  None

RETURN VALUE
  TDSL1_DL_PHYCFG_TIMING_IND_MAINTAIN - If Timing maintained
  TDSL1_DL_PHYCFG_TIMING_IND_INIT  - for all other cases

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_dl_phycfg_timing_ind_enum_type tdsl1msetup_is_hho_timing_ind
(
  tdsl1_dl_phycfg_phychan_db_struct_type *dl_phychan_db_struct  /* dl phychan db ptr */
);

/*===========================================================================

FUNCTION     tdsl1msetup_dl_indicate_phychan_status

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_dl_indicate_phychan_status(boolean status);

/*===========================================================================

FUNCTION     tdsl1msetup_cleanup_failed_dl_setup

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
void tdsl1msetup_cleanup_failed_dl_setup
(
  tdsl1_dl_phychan_enum_type dl_phychan,
  uint8                   cctrch_index
);

#ifdef FEATURE_TDSCDMA_HSUPA
/*===========================================================================
FUNCTION     tdsl1msetup_eul_cphy_setup_cnf_put_status

DESCRIPTION
  This function is used by the EUL(E_DL and E_UL) modules to 
  update the tdsl1m_global.cphy_setup_status buffer in the course of 
  performing the steps of a TDSL1_CPHY_SETUP_REQ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The l1_cphy_status buffer is updated accordingly
===========================================================================*/
extern void tdsl1msetup_eul_cphy_setup_cnf_put_status( boolean pass );

/*===========================================================================
FUNCTION        tdsl1msetup_eul_send_setup_done_to_l1m

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
extern void tdsl1msetup_eul_send_setup_done_to_l1m(
  boolean status,
  tdsl1def_l1_local_cmd_enum_type  l1_cmd_code
);
#endif /* FEATURE_TDSCDMA_HSUPA */

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
extern void tdsl1msetup_send_phychan_establish_ind(boolean phy_est_ind_result);

/*===========================================================================
FUNCTION        tdsl1msetup_cphy_setup_cnf_set_status_word

FILE NAME       l1m.c

DESCRIPTION     This function sets a passed status word according to the
                passed offset, operation, and status

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The passed status word is updated accordingly
===========================================================================*/
void tdsl1msetup_cphy_setup_cnf_set_status_word(uint32 *stat_word,uint8 offset,
                                       tdsl1_setup_ops_enum_type op, boolean status);

/*===========================================================================

FUNCTION       tdsl1msetup_get_ul_dl_max_tti

DESCRIPTION    This function gets the maximum TTI among the 
               various transport channels that comprise the CCTrChs.

DEPENDENCIES   None

RETURN VALUE   Returns the number of frames in the max TTI value.

SIDE EFFECTS   None

===========================================================================*/
extern uint8 tdsl1msetup_get_ul_dl_max_tti(tdsl1_ext_cmd_type* ext_cmd);

/*===========================================================================
FUNCTION    tdsl1msetup_align_act_time_with_max_tti

DESCRIPTION
  This function generates teh activation tine for TDSL1_CPHY_SETUP_REQ.

DEPENDENCIES
  None

RETURN VALUE
  Activation tine

SIDE EFFECTS
  None
===========================================================================*/
uint16 tdsl1msetup_align_act_time_with_max_tti(
  /* Pointer to external command with cphy setup request */
  tdsl1_ext_cmd_type* ext_cmd);

/*===========================================================================
FUNCTION        tdsl1msetup_act_time_adjust

DESCRIPTION
  Adjust activation time in TDSL1_CPHY_SETUP_REQ

DEPENDENCIES

RETURN VALUE
  None
  
SIDE EFFECTS
 

===========================================================================*/
void tdsl1msetup_act_time_adjust(tdsl1_ext_cmd_type *cmd);

/*===========================================================================
FUNCTION        tdsl1msetup_act_time_adjust_for_hho_add

DESCRIPTION
  Adjust activation time in TDSL1_CPHY_SETUP_REQ for hho add, after UL re-sync

DEPENDENCIES

RETURN VALUE
  None
  
SIDE EFFECTS
 

===========================================================================*/
void tdsl1msetup_act_time_adjust_for_hho_add(tdsl1_ext_cmd_type *cmd);

/*===========================================================================
FUNCTION        tdsl1msetup_act_time_adjust_for_cell_trans

DESCRIPTION
  Adjust activation time in TDSL1_CPHY_CELL_TRANSITION_REQ to frame boundary

DEPENDENCIES

RETURN VALUE
  None
  
SIDE EFFECTS
 

===========================================================================*/
void tdsl1msetup_act_time_adjust_for_cell_trans(tdsl1_ext_cmd_type *cmd);

/*===========================================================================
FUNCTION        tdsl1msetup_act_time_adjust_for_drx_req

DESCRIPTION
  Adjust activation time in TDSL1_CPHY_DRX_REQ to frame boundary

DEPENDENCIES

RETURN VALUE
  None
  
SIDE EFFECTS
 

===========================================================================*/
void tdsl1msetup_act_time_adjust_for_drx_req(tdsl1_ext_cmd_type *cmd);

/*===========================================================================
FUNCTION        tdsl1msetup_act_time_adjust_for_bho_act_plus_8

DESCRIPTION
  Adjust activation time in TDSL1_CPHY_SETUP_REQ, for BHO act_time + 8

DEPENDENCIES

RETURN VALUE
  None
  
SIDE EFFECTS
 

===========================================================================*/
void tdsl1msetup_act_time_adjust_for_bho_act_plus_8(tdsl1_ext_cmd_type *cmd);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_hho

DESCRIPTION
  Whether it is a hard handover. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
  
===========================================================================*/
boolean tdsl1msetup_mobility_is_hho(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_hho_in_progress

DESCRIPTION
  Whether it is a hard handover. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
  
===========================================================================*/
boolean tdsl1msetup_mobility_hho_in_progress(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_intraf_hho

DESCRIPTION
  Whether it is a hard handover. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
  
===========================================================================*/
boolean tdsl1msetup_mobility_is_intraf_hho(void);
/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_hho_d2d

DESCRIPTION
  Whether it is a hard handover DCH->DCH

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
  
===========================================================================*/
boolean tdsl1msetup_mobility_is_hho_d2d(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_bho

DESCRIPTION
  Whether it is a baton handover. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
   
===========================================================================*/
boolean tdsl1msetup_mobility_is_bho(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_bho_d2d

DESCRIPTION
  Whether it is a DCH->DCH baton handover. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
   
===========================================================================*/
boolean tdsl1msetup_mobility_is_bho_d2d(void);

/*===========================================================================
FUNCTION    tdsl1msetup_is_regular_freqchange_d2d

DESCRIPTION
  Whether it is a DCH->DCH regular reconfigure or frequency change reconfigure. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
   
===========================================================================*/
boolean tdsl1msetup_is_regular_freqchange_d2d(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_bho_f2d

DESCRIPTION
  Whether it is a FACH->DCH baton handover. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
   
===========================================================================*/
boolean tdsl1msetup_mobility_is_bho_f2d(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_bho_x2d

DESCRIPTION
  Whether it is a SYNC->DCH baton handover. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
   
===========================================================================*/
boolean tdsl1msetup_mobility_is_bho_x2d(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_bho_t80

DESCRIPTION
  Whether it is a baton handover T = 80ms

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
   
===========================================================================*/
boolean tdsl1msetup_mobility_is_bho_t80(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_bho_interf

DESCRIPTION
  Whether it is a inter-freq baton handover 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
   
===========================================================================*/
boolean tdsl1msetup_mobility_is_bho_interf(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_bho_intraf

DESCRIPTION
  Whether it is a intra-freq baton handover 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
   
===========================================================================*/
boolean tdsl1msetup_mobility_is_bho_intraf(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_handover

DESCRIPTION
  Whether it is a handover. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
   
===========================================================================*/
boolean tdsl1msetup_mobility_is_handover(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_same_cell_working_freq_change

DESCRIPTION
  Whether it is a working freq change. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
   
===========================================================================*/
boolean tdsl1msetup_mobility_is_same_cell_working_freq_change(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_same_cell_working_freq_change_d2d

DESCRIPTION
  Whether it is a working freq change. DCH->DCH

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
   
===========================================================================*/
boolean tdsl1msetup_mobility_is_same_cell_working_freq_change_d2d(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_same_cell_working_freq_change_x2d

DESCRIPTION
  Whether it is a working freq change. FACH->DCH or SYNC->DCH

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
   
===========================================================================*/
boolean tdsl1msetup_mobility_is_same_cell_working_freq_change_x2d(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_handover_recover

DESCRIPTION
  Whether it is a working freq change. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsl1msetup_mobility_is_handover_recover(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_bho_recover

DESCRIPTION
  Whether it is a bho recover. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsl1msetup_mobility_is_bho_recover(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_x2d_working_freq_change

DESCRIPTION
  Whether it is a X2D working freq change. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsl1msetup_mobility_is_x2d_working_freq_change(void);


#ifdef FEATURE_TDSCDMA_L2TD_PS_HANDOVER 
/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_x2t_ps_hho

DESCRIPTION
  Whether it is a PS HHO. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsl1msetup_mobility_is_x2t_ps_hho(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_x2t_ps_bho

DESCRIPTION
  Whether it is a PS BHO. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsl1msetup_mobility_is_x2t_ps_bho(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_x2t_psho

DESCRIPTION
  Whether it is a PSHO. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsl1msetup_mobility_is_x2t_psho(void);
#endif

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_dch_working_freq_and_aset_update

DESCRIPTION
  Do aset update if needed, Tune working freq if needed. If need both, tune
  freq first.  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void tdsl1msetup_mobility_dch_working_freq_and_aset_update(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_none_dch_freq_and_aset_update

DESCRIPTION
  Do aset update if needed, Tune primary freq if current FW is on different freq.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void tdsl1msetup_mobility_none_dch_freq_and_aset_update(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_d2x_freq_and_aset_update

DESCRIPTION
  For DCH->FACH, DCH->PCH. D2X
  Do aset update if needed, Tune working freq if needed. If need both, tune
  freq first.  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void tdsl1msetup_mobility_d2x_freq_and_aset_update(uint16 primary_freq);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_dch_aset_update

DESCRIPTION
  Aset update is needed, do aset update for DCH

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void tdsl1msetup_mobility_dch_aset_update(tdsl1msetup_aset_update_enum_type type);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_dch_restore_measurements

DESCRIPTION
  Aset update is needed, do aset update for DCH

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void tdsl1msetup_mobility_dch_restore_measurements(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_convert_add_drop_to_recfg

DESCRIPTION
  Internally convert TDSL1_CPHY_SETUP_REQ add/drop to recfg

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void tdsl1msetup_mobility_convert_add_drop_to_recfg(tdsl1_setup_cmd_type* setup);


/*===========================================================================
FUNCTION    tdsl1msetup_mobility_convert_recfg_to_add_drop

DESCRIPTION
  Internally convert TDSL1_CPHY_SETUP_REQ recfg to add/drop

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void tdsl1msetup_mobility_convert_recfg_to_add_drop(tdsl1_setup_cmd_type* setup);


/*===========================================================================
FUNCTION    tdsl1m_mobility_fw_bho_rsp_handler

DESCRIPTION
  Handle MSG TDSCDMA_FW_BATON_HANDOVER_RSP from FW

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void tdsl1m_mobility_fw_bho_rsp_handler(tfw_baton_handover_rsp_t *fw_bho_rsp);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_is_t2g_failure_recovery

DESCRIPTION
  Whether it is a T2G failure recovery 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
   
===========================================================================*/
boolean tdsl1msetup_mobility_is_t2g_failure_recovery(void);

/*===========================================================================
FUNCTION    tdsl1msetup_ho_or_interf_recfg_is_during_dl_sync

DESCRIPTION
  Whether it is during dl sync of HO and interf recfg in a cell. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
   
===========================================================================*/
boolean tdsl1msetup_ho_or_interf_recfg_is_during_dl_sync(void);

/*===========================================================================
FUNCTION    tdsl1msetup_freq_scan_is_finished

DESCRIPTION
  if TDSRRC_CPHY_FREQ_SCAN_CNF is sent by SRCH, set the flag. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
   
===========================================================================*/
void tdsl1msetup_freq_scan_is_finished(void);

/*===========================================================================
FUNCTION    tdsl1msetup_set_timing_lost_ind

DESCRIPTION
  when OOS/RLF/DL WEAK IND, set the flag and L1M go into IDLE after drop all 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
   
===========================================================================*/
void tdsl1msetup_set_timing_lost_ind(void);
/*===========================================================================
FUNCTION    tdsl1msetup_set_bch_dl_weak_ind

DESCRIPTION
  when DL WEAK IND, set the flag and L1M will do real ACQ in SYNC state 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
   
===========================================================================*/
void tdsl1msetup_set_bch_dl_weak_ind(void);
/*===========================================================================
FUNCTION    tdsl1msetup_set_to_next_frame_boundary

DESCRIPTION
  Package the callback of tdsseq_registered_sub_frame_cb

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
   
===========================================================================*/
void tdsl1msetup_set_to_next_frame_boundary(uint16 subsfn, uint32 cfg_id);

/*===========================================================================
FUNCTION    tdsl1msetup_rach_resume_done_cmd

DESCRIPTION
  After drop N_PCCPCH DONE, waiting DL RESUME RACH/FACH, then send CPHY_SETUP_CNF or CELL_RESELECTION_CNF to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
   
===========================================================================*/
void tdsl1msetup_rach_resume_done_cmd(void);
/*===========================================================================
FUNCTION    tdsl1msetup_report_fw_err_to_rrc

DESCRIPTION
  when receive TDSCDMA_FW_ERROR_DETECTED_IND, L1M report RLF/OOS or establish_fail to RRC, and after rrc drop the channel, L1M turn to IDLE state and reset FW and MCPM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
   
===========================================================================*/
#ifdef FEATURE_TDSCDMA_L1M_SELF_RESET_FW
void tdsl1msetup_report_fw_err_to_rrc(void);
#endif

/*=========================================================================

FUNCTION : TDSL1_GET_ORDERED_CONFIG_ACT_TIME

DESCRIPTION : This is for RRC to get calculated activation time of ordered 
configuration.
In order to cancel ordered config that is sent to L1 but not
activated, RRC need to know the exact activation time.

DEPENDENCIES : RRC invokes this function when a CPHY_SETUP_REQ is pending or
being performed.

INPUT PARAMETERS :
act_time - the calculated activation time (0~255).

RETURN VALUE : 
boolean: TRUE if L1 has an ongoing CPHY_SETUP_REQ for dedicated channels;
FALSE otherwise.

SIDE EFFECTS : 
N/A
==========================================================================*/
boolean tdsl1_get_ordered_config_act_time(int16 *act_time);

/*===========================================================================
FUNCTION    tdsl1msetup_save_dmo_config

DESCRIPTION
  save DMO configuration in gap manager

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
   
===========================================================================*/
#ifdef FEATURE_TDSCDMA_DMO_SUPPORT
void tdsl1msetup_save_dmo_config(tdsl1_meas_occasion_info_struct_type * dmo_para_ptr, uint16 cphy_setup_activation_time);
#endif

/*===========================================================================

FUNCTION tdsl1msetup_build_rf_script_for_bho

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_build_rf_script_for_bho(void);

/*===========================================================================
FUNCTION    tdsl1msetup_l1m_disable_heartbeat_evt

DESCRIPTION
  offer to SRCH to indicate if L1M disabled MEAS

DEPENDENCIES
  None

RETURN VALUE
  TRUE: L1M disabled MEAS heartbeat event
  FALSE: L1M doesn't disable MEAS heartbeat event

SIDE EFFECTS
  None
   
===========================================================================*/
boolean tdsl1msetup_l1m_disable_heartbeat_evt(void);

/*===========================================================================

FUNCTION tdsl1msetup_trigger_build_rf_script_for_bho

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_trigger_build_rf_script_for_bho(uint16 subfn, uint32 cfg_db_idx);

#ifdef FEATURE_TDSCDMA_DSDA
/*===========================================================================
FUNCTION    tdsl1msetup_init_dch_ts_allocation

DESCRIPTION
  Initialize ts allocation arrary for DCH.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_init_dch_ts_allocation(void);
#endif

/*===========================================================================
FUNCTION    tdsl1msetup_infom_drx_to_sleep

DESCRIPTION
  after L1M finishes CMD processing, inform DRX L1M work is done and can go to sleep now

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_infom_drx_to_sleep(void);

/*===========================================================================
FUNCTION    tdsl1msetup_cmd_process_done

DESCRIPTION
  after L1M finishes CMD processing, trigger a local cmd to inform DRX L1M work is done and can go to sleep now

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_cmd_process_done(void);

/*===========================================================================
FUNCTION    tdsl1msetup_mobility_aset_update_only

DESCRIPTION
  only do Force ASET_UPDATE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1msetup_mobility_aset_update_only(void);

#endif /* TDSL1MSETUP_H */

