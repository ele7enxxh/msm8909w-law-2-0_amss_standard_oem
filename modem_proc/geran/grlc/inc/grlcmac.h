/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC TO MAC INTERFACE
***
*** DESCRIPTION
***
***  Definition of data structures and functions provided by RLC for MAC
***  to send messages to RLC
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/inc/grlcmac.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10/12/12   hv      CR426461
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 03/29/01    hv
***
*****************************************************************************/

#ifndef INC_GRLCMAC_H
#define INC_GRLCMAC_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "gprsdef.h"
#include "gprsdef_g.h"
#include "csn.h"
#include "grlcmaccmn.h"
#include "grlci.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/*----------------------------------------------------------------
** Definitions of all signals sent from RLC to MAC on both uplink
** and downlink operations. The MR_ prefix denotes the direction
** the message.
**----------------------------------------------------------------
*/
typedef enum
{

  /*----------- UPLINK ------------*/

  /* MAC confirms reallocation is granted */
  MR_UL_ALLOC_CNF = 0x80,

  /* MAC informs RLC of packet access reject by peer */
  MR_UL_ACC_REJ_IND = 0x81,

  /* MAC indicates change of tlli resolution */
  MR_UL_CONTENTION_IND = 0x82,

  /* MAC indicates to RLC of receipt of packet uplink ack/nack */
  MR_UL_PKT_ACKNACK_IND = 0x83,

  /* ! inform RLC of timer status */
  MR_UL_TIMER_IND = 0x84,

  /* MAC indicates uplink release to RLC */
  MR_UL_RELEASE_IND = 0x85,

  /* MAC indicates cell reselection status (suc/fail) to RLC */
  MR_UL_CELL_RESEL_IND = 0x86,

  /* MAC is requesting a complete RLC UL reset */
  MR_UL_RESET_REQ = 0x87,

  /* MAC indicates of ps access so that rlc can ask for ul tbf req */
  MR_PS_ACCESS_REQ = 0x88,
  /* MAC indicates of no ps access so that rlc will block further tbf est req */
  MR_NO_PS_ACCESS_IND = 0x89,
#ifdef FEATURE_GPRS_PS_HANDOVER
  /* MAC indicates UL PSHO start ind to GRLC */
  MR_UL_PSHO_START_IND = 0x90,
  /* MAC indicates UL PSHO complete ind to GRLC */
  MR_UL_PSHO_COMPLETE_IND = 0x91,
#endif /*FEATURE_GPRS_PS_HANDOVER*/
  /*----------- DOWNLINK -----------*/

  /* MAC indicates to RLC of DL ASS */
  MR_DL_ASS_IND = 0xc0,

  /* ! inform RLC of timer status */
  MR_DL_TIMER_IND = 0xc1,

  /* ! MAC indicates downlink release to RLC */
  MR_DL_RELEASE_IND = 0xc2,

  /* MAC indicates downlink test mode to RLC */
  MR_DL_TEST_MODE_IND = 0xc3

  ,
  /* MAC is requesting a complete RLC DL reset */
  MR_DL_RESET_REQ = 0xc4
#ifdef FEATURE_GPRS_PS_HANDOVER
  ,
  /* MAC indicates DL PSHO start ind to GRLC */
  MR_DL_PSHO_START_IND = 0xc5,
  /* MAC indicates DL PSHO complete ind to GRLC */
  MR_DL_PSHO_COMPLETE_IND = 0xc6
#endif /*FEATURE_GPRS_PS_HANDOVER*/

} mr_sig_t;

/*----------------------------------------------------------------
** Structure for MAC to confirm to RLC of a TBF being established
** or solicited or unsolicited resource assignment
**----------------------------------------------------------------
*/
typedef struct
{
  /* signal id: MR_UL_ALLOC_CNF
  */
  mr_sig_t    sig;

  /*---------------------------------------------------------
  ** MAC informs RLC of the tbf_mode assigned by the network
  **---------------------------------------------------------
  */
  tbf_mode_T  tbf_mode;       /* TBF_MODE_GPRS/TBF_MODE_EGPRS */

#ifdef  FEATURE_GSM_DTM
  /*-------------------------------------------------------------------------
  ** Clear when the uplink assignment is for GPRS/EGPRS. Set when the uplink
  ** assignment is for DTM
  **-------------------------------------------------------------------------
  */
  boolean     dtm_access;
#endif /* FEATURE_GSM_DTM */

  /*---------------------------------------------------------
  ** Extra EGPRS params
  **---------------------------------------------------------
  */
  uint8       mcs;
  uint8       resegment;
  uint16      ws;             /* decoded into actual window size by MAC */

  /* indication from MAC the type of allocation granted:
  ** fixed, dynamic or extended dynamic.
  ** If dynamic, the usf_granularity field exists.
  */
  uint8       access_mode;

  /* access type (one phase, two phase or short access)
  ** The n/w may not grants RLC the access phase it specified
  */
  uint8       access_phase;

  /* tbf type: open-ended tbf or close-ended tbf.
  ** If access_mode is Dynamic then MAC must specifies the tbf-type
  ** here. If Dynamic and close-ended then blocks_granted field must
  ** be non-zero.
  ** If access_mode is fixed alloc, then tbf_type shall be the same
  ** as the value RLC specifies in its rm_ul_est_tbf_req_t struct
  */
  uint8       tbf_type;

  /* coding scheme for RLC to use instructed by the network
  */
  uint8       cs;

  /* Only applicable to one phase or short access mode where RLC is
  ** required to include a TLLI field to help contention resolution
  ** (when ti = RM_TLLI_PRESENT). This field is overidden by the a
  ** change-of-tlli-message from MAC later when contention has been
  ** resolved. When this happens RLC will stop including TLLI in its
  ** data blocks
  */
  uint8       ti;

  /* tlli for RLC to use when 'ti' above is RM_TLLI_PRESENT
  */
  uint32      tlli;

  /* tlli block coding scheme field specifies whether blocks with
  ** tlli field should be coded using coding scheme 1 or the coding
  ** scheme as per channel_coding_cmd field.
  */
  uint8       tlli_block_cs;

  /* how many times MAC Chan Req message were sent. This will help RLC
  ** to format the MAC header
  */
  uint8       retry;

  /* uplink TFI, again part of MAC header
  */
  uint8       ul_tfi;

  /* number of timeslots assigned to uplink tbf.
  *  Depends on the Service Class supported.
  *  Required by RLC to calculate its CV.
  */
  uint8       num_timeslots;

  /* the number of rlc data blocks granted. MAC is to convert the coded
  ** value in PUA or PTRecnf message to a straight value for RLC. For
  ** example, in fixed allocation mode, 9 must be added to
  ** RLC_DATA_BLOCKS_GRANTEDby MAC to give the apparent number of blocks
  ** RLC can transmit. In Dynamic mode, MAC must interpret the
  ** ALLOCATION_BITMAP and fills this variable with the number of blocks
  ** RLC allowed to transmit.
  */
  uint16      blocks_granted;

  /* final allocation indication from the nw.
  ** Only applicable to fixed allocation mode.
  */
  uint8       final_alloc;

  /* USF granularity for use in dynamic allocation mode
  */
  uint8       usf_granularity;

  gas_id_t    gas_id;

} mr_ul_alloc_cnf_t;

/*-----------------------------------------------------------------
** Causes of Access Reject
**-----------------------------------------------------------------
*/
typedef enum
{
  /* Internal state
  */
  ACC_REJ_NULL,

  /* RLC is to hold on to the current PDU AND request uplink TBF
  ** establishment right away.
  */
  ACC_REJ_NO_WAIT_INDICATION,

  /* RLC to delete the current PDU and if there is a next PDU then RLC can
  ** request uplink TBF establishment right away.
  ** Was ACC_REJ_NO_WAIT_INDICATION
  */
  ACC_REJ_NO_WAIT_WITH_DELETE,

  /* RLC is to hold on to the current PDU until MAC sends the MR_UL_RELEASE_IND
  ** with the REL_UL_MAC_IDLE cause before RLC can request for uplink TBF
  */
  ACC_REJ_WAIT_INDICATION,

  /* RLC is to delete the current PDU and attempt to load the next PDU and then
  ** enter AWAIT_MAC_IDLE. When MR_UL_RELEASE_IND is received with the
  ** REL_UL_MAC_IDLE, RLC can then request for uplink TBF again
  */
  ACC_REJ_WAIT_WITH_DELETE,

  /* RLC is prohibited from requesting TBF establishment request unless the
  ** PDU is of Cell Update order from GMM (via LLC)
  */
  ACC_REJ_CELL_RESEL

} mr_acc_rej_cause_t;

/*------------------------------------------------------------------
** Structure for MAC to inform RLC of the packet access reject as a
** response to the recent tbf est request. There is no cause for
** the reject.
**------------------------------------------------------------------
*/
typedef struct
{
  /* signal id: MR_UL_ACC_REJ_IND
  */
  mr_sig_t    sig;

  /* The cause of the access reject
  */
  mr_acc_rej_cause_t    cause;

  gas_id_t              gas_id;

} mr_ul_acc_rej_ind_t;

/*------------------------------------------------------------------
** Structure for MAC to inform RLC when contention has been resolved
**------------------------------------------------------------------
*/
typedef struct
{
  /* signal id: MR_UL_CONTENTION_IND
  */
  mr_sig_t    sig;

  gas_id_t    gas_id;

} mr_ul_cont_ind_t;


/*-------------------------------------------------------------------
** Structure for MAC to inform RLC of the change of tlli status.
** After contention is resolved the tlli field is no longer required
** in the UL RLC/MAC block.
**-------------------------------------------------------------------
*/
typedef struct
{
  /* signal id: MR_UL_CONTENTION_IND
  */
  mr_sig_t      sig;

  /*-------------------------------------------------
  ** Extra EGPRS Parameters
  **-------------------------------------------------
  */

  /* Flag set to TRUE if contention res has been completed on reception of a PUAN
  ** set to FALSE is contention res was completed on reception of PUA
  */
  boolean               ack_nack_info_flg;

  uint8                 mcs;
  uint8                 resegment;
  uint8                 pre_emptive_tx;
  egprs_ack_nack_des_t  egprs_acknack_desc;

  /* contention resolution flag
  */
  uint8         contention_res;

  /* coding scheme for RLC to use
  */
  uint8         cs;

  /* Acknack description: includes fai, ssn & rbb[]
  */
  rm_acknack_desc_t   acknack_desc;

  /* Do we need this field?
  */
  uint8         tfi;

  /* indicate whether RRBP is valid so RLC can proceed to send
  ** Packet Control Acknowledgement message as part of its TBF
  ** release procedure
  */
  uint8         sp;

  /* MAC assigns the global grlc_ul_block_counter value to this field prior to
  ** sending the acknack message to RLC
  */
  uint16              blk_cnt;

  gas_id_t            gas_id;

} mr_ul_tlli_ind_t;

/*-----------------------------------------------------------------
** Structure for MAC to deliver Packet UL Acknack message to RLC
**
** Since P.AckNack can contain fixed realloc struct. MAC is
** expected to split such packet into two separate signals:
** - mr_ul_pkt_acknack_ind_t to hold the acknack_desc and
** - mr_ul_alloc_cnf_t to hold the realloc indication.
** Note that Packnack contains the channel coding scheme which may
** change
**-----------------------------------------------------------------
*/
typedef struct
{
  /* signal id: MR_UL_PKT_ACKNACK_IND
  */
  mr_sig_t            sig;

  /*-------------------------------------------------
  ** Extra EGPRS Parameters
  **--------------------------------------------------
  */
  uint8                   mcs;
  uint8                   resegment;
  uint8                   pre_emptive_tx;
  egprs_ack_nack_des_t    egprs_acknack_desc;


  /* coding scheme for RLC to use
  */
  uint8               cs;

  /* Acknack description: includes fai, ssn & rbb[]
  */
  rm_acknack_desc_t   acknack_desc;

  /* ?
  */
  uint8               tfi;

  /* indicate whether RRBP is valid so RLC can proceed to send
  ** Packet Control Acknowledgement message as part of its TBF
  ** release procedure
  */
  uint8               sp;

  /* MAC assigns the global grlc_ul_block_counter value to this field prior to
  ** sending the acknack message to RLC
  */
  uint16              blk_cnt;

  gas_id_t            gas_id;

} mr_ul_pkt_acknack_ind_t;


/*----------------------------------------------------
** Structure for MAC to inform RLC of UL timer status
**----------------------------------------------------
*/
typedef struct
{
  /* signal id: MR_UL_TIMER_IND
  */
  mr_sig_t          sig;

  /* timer id
  */

  rm_timer_t        id;

  /* timer status
  */
  rm_timer_state_t  state;

  gas_id_t          gas_id;

} mr_ul_timer_ind_t;

/*-------------------------------------------------------------------
** Structure for MAC to inform RLC of TBF release either as a result
** of network controlled release or a MAC failure release
**-------------------------------------------------------------------
*/
typedef struct
{
  /* signal id: MR_UL_RELEASE_IND
  */
  mr_sig_t          sig;

  /* cause for ul release
  */
  rm_tbf_release_t  cause;

  gas_id_t          gas_id;

} mr_ul_release_t;

/*-----------------------------------------------------------------
** Structure for MAC to inform RLC of a succesful cell reselection
** (by GRR) so that RLC can reset its N3102 counter
**-----------------------------------------------------------------
*/
typedef struct
{
  /* signal id: MR_UL_CELL_RESEL_IND
  */
  mr_sig_t                sig;

  rm_cell_resel_status_t  cell_resel_status;

  gas_id_t                gas_id;

} mr_ul_cell_resel_ind_t;

/*---------------------------------------------------------------
** Structure to carry a downlink assigment either during uplink
** transfer or idle mode.
**---------------------------------------------------------------
*/
typedef struct
{
  /* signal id: MR_DL_ASS_IND
  */
  mr_sig_t    sig;

  /*-----------------------------------------------------
  ** Extra EGPRS Parameters
  **-----------------------------------------------------
  */
  tbf_mode_T  tbf_mode;

  /* Actual window size value decoded by MAC
  */
  uint16      ws;

  /* RLC mode
  */
  uint8       rlc_mode;

  /* TLLI
  */
  uint32      tlli;

  /* DL TS*/
  uint8       num_dl_ts;

  /* CTRL_ACK for NEW TBF */
  boolean     ctrl_ack;

  gas_id_t    gas_id;

} mr_dl_ass_ind_t;

/*-----------------------------------------------------
** Structure for MAC to inform RLC of DL timer status
**-----------------------------------------------------
*/
typedef struct
{
  /* signal id: MR_DL_TIMER_IND
  */
  mr_sig_t          sig;

  /* timer id
  */

  rm_timer_t        id;

  /* timer status
  */
  rm_timer_state_t  state;

  gas_id_t          gas_id;

} mr_dl_timer_ind_t;

/*----------------------------------------------------------------------
** Structure for MAC to command RLC to release the current downlink TBF
** On receipt of this signal RLC abort the current TBF and goes back to
** RX_NULL state.
**----------------------------------------------------------------------
*/
typedef struct
{
  /* signal id: MR_DL_RELEASE_IND
  */
  mr_sig_t      sig;

  /* no cause
  */
  gas_id_t gas_id;

} mr_dl_release_t;

typedef struct
{
  /* signal id: MR_UL_RESET_REQ
  */
  mr_sig_t      sig;

  /* no cause
  */
  gas_id_t gas_id;

} mr_ul_reset_t;


typedef struct
{
  /* signal id: MR_DL_RESET_REQ
  */
  mr_sig_t      sig;

  /* no cause
  */
  gas_id_t gas_id;

} mr_dl_reset_t;


/*------------------------------------------------------------------------
** Structure for MAC to indicate to RLC that test mode has been activated
** This signal is applicable to both GPRS mode and EGPRS mode
**------------------------------------------------------------------------
*/
typedef struct
{
  /* signal id: MR_DL_TEST_MODE_IND
  */
  mr_sig_t      sig;

  test_mode_T   test_mode;

  /* no cause
  */
  gas_id_t gas_id;

} mr_dl_test_mode_t;

/********************************************************
*               NEW STRUCTURES FOR IPA                  *
********************************************************/
typedef struct
{
  /* signal id: MR_NO_PS_ACCESS_IND
  */
  mr_sig_t            sig;

  delete_pdu_cause_t  delete_pdu_flag;

  gas_id_t            gas_id;

} mr_no_ps_access_ind_t;

typedef struct
{
  /* signal id: MR_PS_ACCESS_REQ
  */
  mr_sig_t              sig;

  action_on_ps_access_t action;

  gas_id_t gas_id;

} mr_ps_access_req_t;

#ifdef FEATURE_GPRS_PS_HANDOVER
typedef struct
{
  /* signal id: MR_UL_PSHO_START_IND
  */
  mr_sig_t    sig;

  gas_id_t    gas_id;

} mr_ul_psho_start_ind_t;

typedef struct
{
  /* signal id: MR_DL_PSHO_START_IND
  */
  mr_sig_t    sig;

  tbf_mode_T  tbf_mode;

  gas_id_t gas_id;

} mr_dl_psho_start_ind_t;

typedef struct
{
  /* signal id: MR_UL_PSHO_COMPLETE_IND
  */
  mr_sig_t                sig;
  psho_status_t           psho_status;/* This will tell whether PSHO success or failed*/
  psho_type_t             psho_type;/* This will tell what type of PSHO is succeeded*/
  mr_ul_alloc_cnf_t       ul_alloc_cnf;/*if PS HO is successful and PSHO type is GTOG
                                         or WTOG then this structure will
                                         have ul resources otherwise not*/
  uint8                   ul_rlc_mode;
  uint8                   reset_flag;/* This is the reset flag received in PSHO message*/
  uint8                   nas_container_present;/* This will represent whether PSHO
                                                   command has nas container or not*/
  gas_id_t                gas_id;
} mr_ul_psho_complete_ind_t;

typedef struct
{
  /* signal id: MR_DL_PSHO_COMPLETE_IND
  */
  mr_sig_t                sig;
  psho_status_t           psho_status;/* This will tell whether PSHO success or failed*/
  psho_type_t             psho_type;/* This will tell what type of PSHO is succeeded*/
  uint8                   dl_ass_ind_present;/*This will tell whether dl_ass_ind
                                                structure is valid or not*/
  mr_dl_ass_ind_t         dl_ass_ind;
  uint8                   reset_flag;/* This is the reset flag received in PSHO message*/
  uint8                   nas_container_present;/* This will represent whether PSHO
                                                  command has nas container or not*/
  gas_id_t                gas_id;
} mr_dl_psho_complete_ind_t;
#endif
/*-------------------------------
** MAC to RLC message definition
**-------------------------------
*/
typedef struct
{
  q_link_type        link;

  /*---------------------------------------
  ** Union of all messages from MAC to RLC
  **---------------------------------------
  */
  union
  {
    mr_sig_t                    sig;

    /* uplink operation
    */
    mr_ul_alloc_cnf_t           ul_alloc_cnf;
    mr_ul_tlli_ind_t            ul_tlli_ind;
    mr_ul_pkt_acknack_ind_t     ul_pkt_acknack_ind;
    mr_ul_timer_ind_t           ul_timer_ind;
    mr_ul_cell_resel_ind_t      ul_cell_resel_ind;
    mr_ul_release_t             ul_release_ind;

    mr_ul_acc_rej_ind_t         ul_acc_rej_ind;

    mr_ps_access_req_t          ps_access_req;
    mr_no_ps_access_ind_t       no_ps_access_ind;

#ifdef FEATURE_GPRS_PS_HANDOVER
    mr_ul_psho_start_ind_t      ul_psho_start_ind;
    mr_ul_psho_complete_ind_t   ul_psho_complete_ind;
#endif

    mr_ul_reset_t               ul_reset_req;
    mr_ul_cont_ind_t            ul_cont_ind;

    /* downlink operation
    */
    mr_dl_ass_ind_t             dl_ass_ind;
    mr_dl_timer_ind_t           dl_timer_ind;
    mr_dl_release_t             dl_release_ind;
    mr_dl_test_mode_t           dl_test_mode_ind;

    mr_dl_reset_t               dl_reset_req;
#ifdef FEATURE_GPRS_PS_HANDOVER
    mr_dl_psho_start_ind_t      dl_psho_start_ind;
    mr_dl_psho_complete_ind_t   dl_psho_complete_ind;
#endif
  } msg;

} mac_rlc_msg_t;



/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/*===========================================================================
===
===  MACRO         xxx
===
===  DESCRIPTION
===
===    xxx.
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
===========================================================================*/



/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      RLC_GET_CMD_BUF_MAC()
===
===  DESCRIPTION
===
===   Return a pointer to a buffer for MAC to build a message destined
===   for RLC
===
===
===  DEPENDENCIES
===
===    none
===
===  RETURN VALUE
===
===    pointer to a buffer
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
mac_rlc_msg_t *grlc_get_cmd_buf_mac (void);


/*===========================================================================
===
===  FUNCTION      RLC_PUT_CMD_MAC()
===
===  DESCRIPTION
===
===   Return a buffer for MAC to build a message destined for RLC
===
===
===  DEPENDENCIES
===
===    none
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
void grlc_put_cmd_mac (gas_id_t gas_id, mac_rlc_msg_t *msg_ptr);


/*===========================================================================
===
===  FUNCTION      GRLC_DL_T3192_STATUS()
===
===  DESCRIPTION
===
===    Returns Timer T3192 status. This function is used by MAC to find whether
===    T3192 has been started by PL1.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    rm_timer_state_t
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
extern rm_timer_state_t grlc_dl_t3192_status (gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION      GRLC_DL_START_T3190()
===
===  DESCRIPTION
===
===    Start or re-start the T3190 timer. This timer is started/re-started
===    when: a) a DL assignment is received, b) a DL data block is received
===    or c) a timeslot reconfigure message is received (by MAC and not seen
===    by RLC).
===
===    This is provided such that MAC can re-start this timer when it receives
===    a DL timeslot Reconfigure message while RLC DL is active.
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
extern void grlc_dl_start_t3190 (gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      GRLC_UL_IS_CV_ACTIVE()
===
===  DESCRIPTION
===
===   This access function is intended for MAC to check if RLC is in transfer
===   and counting down in E-DTM operation.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===   Return TRUE if RLC is in uplink transfer mode and transmission has
===   reached a data block with an active countdown value, ie values other
===   than 15. Return FALSE otherwise
===
===  SIDE EFFECTS
===
===   none
===
===========================================================================*/
boolean grlc_ul_is_cv_active (gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      GRLC_UL_RESTART_AWAIT_ASS_GUARD()
===
===  DESCRIPTION
===
===   This access function is intended for MAC to restart RLC_AWAIT_ASS_GUARD
===   by the time in milliseconds of the start time rcvd in UL assignment msg.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===   none
===
===========================================================================*/
void grlc_ul_restart_await_ass_guard (gas_id_t gas_id, uint32 time_ms);

/*===========================================================================
===
===  FUNCTION      GRLC_UL_STOP_AWAIT_ASS_GUARD()
===
===  DESCRIPTION
===
===   This access function is intended for MAC to stop RLC_AWAIT_ASS_GUARD
===   when it is waiting for a downlink acknack message to piggy back the uplink
===   request. Since a PDAN may not exist, the timer may expire yet no request
===   had been sent
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===   none
===
===========================================================================*/
extern void grlc_ul_stop_await_ass_guard (gas_id_t gas_id);


#endif /* INC_GRLCMAC_H */

/*** EOF: don't remove! ***/
