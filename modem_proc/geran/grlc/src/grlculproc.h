/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC UPLINK PROCESSING HEADER
***
***
*** DESCRIPTION
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlculproc.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 11/24/03    hv     Created
*** 08/03/04    hv     Added EGPRS code
*** 09/19/12    hv     Added dual data space functionality
***
*****************************************************************************/

#ifndef INC_GRLCULPROC_H
#define INC_GRLCULPROC_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "geran_multi_sim.h"
#include "grlcultypes.h"
#include "grlcutil.h"
#include "grlcdiag.h"
#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
#include "gllc_v.h"
#include "gllc_v_g.h"
#endif /* ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST  */
#include "grlcllc.h"
#include "grlcllcif.h"
#include "rr_public_store.h"


/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

/*----------------------------------------------------------------------------
** Pointers outside the PDU queue to hold the current pdu being worked on and
** on and the next pdu
**----------------------------------------------------------------------------
*/
extern q_type  grlc_ul_old_llc_q[NUM_GERAN_DATA_SPACES];

extern gprs_grr_ul_mes_t   *cur_pdu_ptr[NUM_GERAN_DATA_SPACES], *next_pdu_ptr[NUM_GERAN_DATA_SPACES];

/* Main structure containing the majority of data related to RLC uplink operations
*/
extern grlc_ul_t	ul[NUM_GERAN_DATA_SPACES];

/* This flag controls whether the data block timers are serviced (in ack mode)
** by the PL1 ISR. When this flag is set, the timer service re-starts the
** timer after serviving it.
**
** This flag is set when RLC first sends PH_DATA_REQ primitive to PL1 and
** and stays on for the duration of the TBF. It is cleared when a TBF terminates
*/
extern boolean b_block_timers_service_active[NUM_GERAN_DATA_SPACES];

/* Rachet counter to detect "rlc no data" condition
*/
extern uint16 grlc_ul_no_data_ctr[NUM_GERAN_DATA_SPACES];

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      PROCESS_GUARD_TIMER()
===
===  DESCRIPTION
===
===    This function maintains the sub-state machine for the Guard Timer in Extended
===    Uplink TBF mode. The sub-states are:
===
===    GUARD_OFF: initial state
===               timer starts when a BSN with CV=0 been sent (run in post_tx_actions)
===               state change to CV_ZERO
===
===    CV_ZERO:   - timer re-started when PUAN is received which advances Va
===               - timer re-started when Vseg = Va, ie. all BSNs been ack'ed (run after PUAN/CRes)
===               state change to GUARD_ON
===               - timer stopped when Vs > va, ie. new blocks arrived
===               state change to GUARD_OFF
===               - timer stopped when sending reallocation request (GCF 41.3.6.10)
===
===    GUARD_ON:  - timer stopped when CV not zero, ie. new blocks arrived
===               state change to GUARD_OFF
===               - timer stopped when sending reallocation request (GCF 41.3.6.10)
===
===    When Guard Timer expires, abnormal release TBF
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void process_guard_timer (gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION      GRLC_UL_ADJUST_FLOW_CONTROL_THRESHOLDS()
===
===  DESCRIPTION
===
===    This function reads the USF usage statistic provided by L1 and determines
===    if a reduction to the current flow control values is required. If so a scale
===    factor (in percentage) is applied, and the current flow control is updated
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void grlc_ul_adjust_flow_control_thresholds (gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      E_UPDATE_ACK_STATE_ARRAY
===
===  DESCRIPTION
===
===    Interpret the EGPRS packet uplink acknack description and update the
===    transmit array acknowledgement states ..
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void e_update_ack_state_array ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      GRLC_UL_INIT_GPRS_DATA_BLOCK()
===
===  DESCRIPTION
===
===    Fill GPRS data block default starting values
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void grlc_ul_init_gprs_data_block (gas_id_t gas_id, grlc_ul_data_block_t *bp);

/*===========================================================================
===
===  FUNCTION      GRLC_UL_INIT_EGPRS_DATA_BLOCK()
===
===  DESCRIPTION
===
===    Fill EGPRS data block default starting values
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void grlc_ul_init_egprs_data_block (gas_id_t gas_id, grlc_ul_data_block_t *bp);

/*===========================================================================
===
===  FUNCTION      CLR_UL_DATA()
===
===  DESCRIPTION
===
===    Clear the uplink data structure
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    return value written to module var cv.
===
===========================================================================*/
void clr_ul_data ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      READ_UL_SYSINFO()
===
===  DESCRIPTION
===
===    Reads the uplink sysinfo from GRR
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void read_ul_sysinfo ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      CLR_LLC_PDU_QUEUE
===
===  DESCRIPTION
===
===    Clear the RLC UL pdu queue by reading each item and deallocate DSM
===    resources for each item.
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===  COMMENTS
===
===    Used for purging LLC PDUs in queue. "is_del_all_active" is set if
===    STATUS_IND is required when purging a SM PDU.
===
===========================================================================*/
void flush_llc_pdu_queue ( gas_id_t gas_id, boolean is_del_all_active );

/*===========================================================================
===
===  FUNCTION      GRLC_CLEAR_REALLOC_VAR
===
===  DESCRIPTION
===
===     Clears reallocation details held in "ul" structure. 
===
===  PARAMS 
=== 
===     None
===
===  DEPENDENCIES
===
===     None
===
===  RETURN VALUE
===
===     None
===
===  SIDE EFFECTS
===
===     None
===
===  COMMENTS
===
===
===========================================================================*/
void grlc_clear_realloc_vars ( gas_id_t gas_id );



/*===========================================================================
===
===  FUNCTION      FLUSH_LLC_SIG_QUEUE
===
===  DESCRIPTION
===
===    Read out all LLC signalling messages and free them
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===  COMMENTS
===
===========================================================================*/
void flush_llc_sig_queue ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      FLUSH_MAC_SIG_QUEUE
===
===  DESCRIPTION
===
===    Read out all MAC messages and free them
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===  COMMENTS
===
===========================================================================*/
void flush_mac_sig_queue ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      FLUSH_TMR_SIG_QUEUE
===
===  DESCRIPTION
===
===    Read out all TIMER messages and free them
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===  COMMENTS
===
===========================================================================*/
void flush_tmr_sig_queue ( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION      DETECT_ERROR_AND_RECOVER()
===
===  DESCRIPTION
===
===    Indicate
===
===    If the pending release flag is set then exit without loading
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    cur_pdu_ptr is returned either NULL or pointing to a valid PDU
===
===  SIDE EFFECTS
===
===========================================================================*/
void detect_error_and_recover ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      UPDATE_ACK_STATE_ARRAY
===
===  DESCRIPTION
===
===    Interpret the Receive Bitmap Block (RBB) & Starting Sequence Number
===    (SSN) in packet uplink acknack msg and update the Ack State Array V(B),
===    and the Ack State V(A).
===
===    The bitmap is arranged as follows (as per 11.0 and 12.3)
===
===    MSB                                LSB
===
===    b64  b63  b62  b61  b60  b59  b58  b57   Octet N + 0  RBB[0]
===    b56  b55  b54  b53  b52  b51  b50  b49   Octet N + 1
===    b48  b47  b46  b45  b44  b43  b42  b41   Octet N + 2
===    b40  b39  b38  b37  b36  b35  b34  b33   Octet N + 3
===
===    b32  b31  b30  b29  b28  b27  b26  b25   Octet N + 4
===    b24  b23  b22  b21  b20  b19  b18  b17   Octet N + 5
===    b16  b15  b14  b13  b12  b11  b10  b09   Octet N + 6
===    b08  b07  b06  b05  b04  b03  b02  b01   Octet N + 7  RBB[7]
===
===    Given a bitpos (1..64) the octet number is:
===    octet = 7 - ((bitpos-1)/8)
===
===    Given a bitpos (1..64) the mask is:
===    mask = 1 << ((bitpos-1)%8)
===
===    The BSN is then:
===    bsn = ssn - bitpos;
===
===    -------------------------------------------------------------------
===
===          BSN
===        |  0  |  1  |  2  |  3  |  4  |  5  |
===        |     |     |     |  ^  |     |     |
===                             |
===   bit64  <...  bit2  bit1   |____ssn
===
===  DEPENDENCIES
===
===    input
===    ul_acknack is the latest packet uplink acknack struct from peer entity
===    output
===    ack_state_arr_vb
===    ack_state_va
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void update_ack_state_array ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      HANDLE_CONTENTION_RESOLUTION_FAILURE()
===
===  DESCRIPTION
===
===    This function clears the variables associated with the transfer of the
===    current PDU which has failed Contention Resolution. However, since MAC
===    will retry upto 4 times by sending the previously TBF establishment
===    request RLC had sent earlier. So RLC only needs to jump to AWAIT_ASS
===    state without re-sending TBF request. RLC however needs to re-instate
===    some key variables in order to restore its state to before the
===    Uplink Assignment had been received
===
===  PARAMS
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void handle_contention_resolution_failure (gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      HANDLE_CONTENTION_RESOLUTION_SUCCESS()
===
===  DESCRIPTION
===
===    This function reprocesses the data blocks in the FIFO and those already
===    segmented (in the attached window) when the Contention Resolution
===    Indication is received. Re-processing is required in order to take out
===    the TLLI fields in new data blocks -- for both ACK and UNACK modes.
===
===    ! This function is written to handle the current version of the FIFO where
===    ABORT_ACCESS is not supported. This function needs to be modified when
===    ABORT_ACCESS is supported.
===
===    When RLC receives the MR_UL_CONTENTION_IND msg with RM_CONTENTION_RESOLVED,
===    It disables PL1 access to the FIFO. PL1 also receives the same message from
===    MAC causing it to stop accessing the FIFO. If PL1 does access the FIFO,
===    UL_FIFO_BUSY will be returned. When RLC completes its processing, it
===    re-enables PL1 access and sends PH_DATA_REQ. PL1 should clear its read/get
===    records as RLC will resynchronise the read/get indices of the FIFO.
===
===    Processing involves searching the FIFO to find the oldest un-transmitted
===    data block. The offset of the block is then mapped to a position in the
===    current PDU to allow the PDU to be re-wound. Segmentation then starts
===    again from this point (without TLLI), and new values for V(S), seg_ind,
===    CV, TBC, blocks_rem etc.. are adjusted.
===
===  PARAMS
===
===  DEPENDENCIES
===
===    ul struct
===
===    PL1 should clear its read/get records as RLC will resynchronise the
===    read/get indices of the FIFO
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    Alter PL1-RLC UL FIFO
===
===========================================================================*/
void handle_contention_resolution_success (gas_id_t gas_id, const mac_rlc_msg_t  *mac_msg_ptr);

/*===========================================================================
===
===  FUNCTION      CHANGE_CODING_SCHEME()
===
===  DESCRIPTION
===
===    This function reprocesses some of the data blocks already produced
===    in the old coding scheme and produce the new data blocks with new
===    coding scheme.
===
===    The content of the L1 FIFO is examined to find the oldest un-read data
===    block for each area. This is the point at which reprocessing starts from.
===    Each data block contains an 'offset' value to the beginning of the PDU.
===    This offset value together with the PSN (pdu number) allows the PDU in
===    to be restored to where the offset is. Segmentation then starts again
===    from this point with the new coding scheme, and new values for VS, etc..
===    are adjusted.
===
===    This function also sets a flag to trigger PL1 to start looking at the
===    FIFO again once resegmentation is complete. Since PL1 waits for this
===    signal from RLC when new coding scheme is commanded by the n/w.
===
===    MAC sends coding scheme info to both PL1 and RLC where PL1 gets this
===    info first and hence stops accessing the FIFO before RLC gets to
===    reprocess it.
===
===    As the FIFO contains 3 distinct types of blocks Nack, Pending-tx and
===    Pending-ack. Only the Pending-tx blocks need to be re-segmented and the
===    other two are left untouched and re-transmitted in their original
===    CV and coding scheme. However, PL1 expects the Read Out indices in the
===    Pending-ack and Nack areas to be re-synced.
===
===    The 3 cases referred to below are the states of the FIFO when coding scheme
===    change command is received
===
===  PARAMS
===
===  DEPENDENCIES
===
===    ul struct
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    Alter PL1-RLC UL FIFO
===
===========================================================================*/
void change_coding_scheme (gas_id_t gas_id, const uint8 cs);

/*==========================================================================
===
===  FUNCTION      CHECK_TO_SEND_PRR
===
===  DESCRIPTION
===
===    If state in transfer and both current and next PDU exist then assess Qos
===    of current PDU against next PDU and initiate reallocation action.
===
===  PARAMS
===
===    grr_ul_msg_ptr points to LLC message containing info about the LLC PDU
===    and a pointer to the PDU in DSM pool. It also has space for info internal
===    to RLC when RLC processes it.
===
===  DEPENDENCIES
===
===    cur_pdu_ptr, next_pdu_ptr, ul
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void check_to_send_prr ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      GRR_SUSPEND_REQ()
===
===  DESCRIPTION
===
===    Processes the GPRS Suspend Request from LLC. If RLC is not in IDLE then
===    say error and do nothing. If RLC is already suspended then say error.
===    Otherwise go in to GRLC_SUSPEND and delink by moving the current 'data'
===    into the reserved queue.
===
===  PARAMS
===
===  DEPENDENCIES
===
===    ul_q
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grr_suspend_req ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      GRR_PURGE_SUSPENDED_DATA_REQ()
===
===  DESCRIPTION
===
===    This function purges the suspended data PDU in the reserved queue. This
===    only happens when GRLC in is Suspend state. This is called by GRR following
===    a Detach Request during Suspension. This purging action prevents suspended
===    data to be delivered by RLC after the Detach Sequence
===
===  PARAMS
===
===  DEPENDENCIES
===
===    ul_q
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grr_purge_suspended_data_req ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      GRR_PURGE_DATA_REQ()
===
===  DESCRIPTION
===
===    This function purges the data PDU in the reserve queue and main queue. This
===    only happens when GRLC in asked to delete data after PDP deactivation of a
===    particular SAPI.
===
===  PARAMS
===   sapi - sapi to be deleted.
===
===  DEPENDENCIES
===
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grr_purge_data_req (gas_id_t gas_id, uint8 sapi);

/*===========================================================================
===
===  FUNCTION      GRR_RESUME_REQ()
===
===  DESCRIPTION
===
===    Processes the GPRS Resume Request from LLC. If there is no GMM-PDUs in
===    the ULQ and that Suspend state is active then go straight to Resume
===    state and ULQ is restored. If not then advance to RESUME_PENDING state.
===    When the last GMM-PDU has been delivered and ULQ is empty the Suspend
===    state will change from RESUME_PENDING to RESUME.
===
===
===  PARAMS
===
===  DEPENDENCIES
===
===    ul_q
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grr_resume_req ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      GRR_TEST_MODE_REQ()
===
===  DESCRIPTION
===
===    Processes request from LLC to establish an UL tbf for kick starting
===    the ETSI Test mode B or SRB.
===
===  PARAMS
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===     Note that this function is called from the LLC task thread
===
===========================================================================*/
void grr_test_mode_req( const gprs_grr_ul_mes_t *msg_ptr );

/*===========================================================================
===
===  FUNCTION      ACCEPT_RESOURCE()
===
===  DESCRIPTION
===
===    Accept the resource granted by the n/w and store away the information.
===    Change the realloc_state according to whether resource assigned is:
===     1. to establish a new TBF.
===     2. in response to RLC request to extend the TBF
===     3. un-solicited assignment from the network.
===
===    Check for FINAL_ALLOCATION is done for all 3 cases.
===    Check for coding scheme change is done for cases 2 and 3
===    If tx is in resource exhaust state then case 3 un-halts the tx process
===
===  PARAMS
===         None
===
===  DEPENDENCIES
===
===    ul.state
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void accept_resource ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      REALLOC_FOR_NEXT_PDU()
===
===  DESCRIPTION
===
===    This function sends a reallocation request to MAC with the number
===    octets being the sum of the un-transmitted octets of the current
===    pdu and the length of the next pdu.
===
===    The number of blocks is also worked out and stored in
===    realloc_blocks_req.
===
===  PARAMETERS
===
===    b_cv_zero: indicates (to MAC) whether RLC has sent the block with CV = 0
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void realloc_for_next_pdu
(
  gas_id_t                gas_id,
  const gprs_grr_ul_mes_t *pdu_ptr,
  boolean                 b_cv_zero
);

/*===========================================================================
===
===  FUNCTION      EST_TBF_FROM_NULL()
===
===  DESCRIPTION
===
===    This function establishes an UL TBF given the current PDU by sending
===    RM_UL_EST_TBF_REQ to MAC and entering US_AWAIT_ASS state.
===
===    This function is called from within the state machine. It expects
===    a valid pdu in cur_pdu. No backup is made of the current pdu.
===
===  DEPENDENCIES
===
===    expects cur_pdu to hold a valid LLC PDU in ACK or UNACK mode
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
============================================================================*/
void est_tbf_from_null ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      INDICATE_TO_LLC_RLCMAC_FAILURE()
===
===  DESCRIPTION
===
===    Send status indication to LLC to inform of RLCMAC failure
===    If RLC fails to transmit GMM/SM signaling PDU such that 
===    if LAYER3_TX_CONFIRM is  defined then NAS can perform retry
===    on a selective basis.
===
===  PARAMS
===    gprs_grr_ul_mes_t
===
===  DEPENDENCIES
===
===    ul struct
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/
void indicate_to_llc_rlcmac_failure (gas_id_t gas_id, const gprs_grr_ul_mes_t *pdu_ptr);

/*===========================================================================
===
===  FUNCTION      SEND_RESUME_CNF_TO_LLC
===
===  DESCRIPTION
===
===    Send GRR_RESUME_CNF to LLC
===
===  PARAMS
===    gprs_grr_ul_mes_t
===
===  DEPENDENCIES
===
===    ul struct
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/
void send_resume_cnf_to_llc ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      INDICATE_TO_LLC_RLCMAC_SUCCESS()
===
===  DESCRIPTION
===
===    Send status indication to LLC to inform of GMM/SM PDU tx success at RLC
===
===  PARAMS
===    gprs_grr_ul_mes_t
===
===  DEPENDENCIES
===
===    ul struct
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/
void indicate_to_llc_rlcmac_success (gas_id_t gas_id, const gprs_grr_ul_mes_t *pdu_ptr);




/*===========================================================================
===
===  FUNCTION      GRLC_UL_PURGE_PDU_QUEUE
===
===  DESCRIPTION
===
===    Purge PDUs
===
===  PARAMS
===    None
===
===  DEPENDENCIES
===    ul struct
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_purge_pdu_queue ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      HANDLE_UL_RELEASE_EVENT()
===
===  DESCRIPTION
===
===    Handle uplink release which can be caused by MAC or from within RLC
===
===  PARAMS
===
===    ul_release_cause: cause value
===    b_from_mac: if TRUE indicates the release order came from MAC
===        otherwise the release is caused within RLC in which case RLC will
===        need to inform MAC of the release
===
===  DEPENDENCIES
===
===    ul struct
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/
void handle_ul_release_event
(
  gas_id_t          gas_id,
  rm_tbf_release_t  ul_release_cause,
  boolean           b_from_mac
);


/*===========================================================================
===
===  FUNCTION      REPROCESS()
===
===  DESCRIPTION
===
===    This function reprocesses the pending transmit blocks. This is done to
===    allow grouping of the incoming PDU, ie. to extend the current TBF to
===    include another PDU. In ack mode, the PTX blocks if the FIFO are reprocessed.
===    In un-ack mode, the PAK blocks in the FIFO are reprocessed.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void reprocess(gas_id_t gas_id, grlc_reprocess_cause_t reprocess_cause);

#endif /* INC_GRLCULPROC_H */

/*** EOF: don't remove! ***/
