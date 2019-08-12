/*****************************************************************************
***
*** TITLE
***
***   GPRS RLC Uplink Types
***
***
*** DESCRIPTION
***
***  Definition of RLC Uplink private data types and macros
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcultypes.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03/29/01    hv     Created
*** 08/03/04    hv     Added EGPRS code
*** 05/31/06    hv     GRLC_GRLC.10.00.29
***                    Fixed Segment Stall for EDGE mode. Ensure T3182 re-starting
***                    in EXT_UTBF mode. Corrected BSN of CV 0 for EDGE mode
*** 09/19/12    hv     Added dual data space functionality
*** 08/24/12    hv     CR 378752 - correct wrong puncturing scheme
*** 10/10/13    hv     CR555392
***
*****************************************************************************/

#ifndef INC_GRLCULTYPES_H
#define INC_GRLCULTYPES_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "gprsdef.h"
#include "gprsdef_g.h"
#include "grlci.h"
#include "gllcrlcif.h"
#include "grlcmac.h"
#include "gmacrlc.h"
#include "grlctimer.h"
#include "gmac.h"
#include "l1i.h"

/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

/*==================================================================
** Below are temporary debug switches for the RLC uplink module.
** These are not feature switches
**==================================================================
*/

/* This feature prevents RLC from being left in the AWAIT_ASS state when GRR/MAC has
** forgotten RLC requests for a TBF. Default on
*/
#if (!((defined(DEBUG_GSM_GPRS_DS_UNIT_TEST)) || (defined(DEBUG_GSM_EGPRS_DS_UNIT_TEST)) || (defined(PERLUTF))))
#endif  /* DEBUG_GSM_GPRS_DS_UNIT_TEST || DEBUG_GSM_EGPRS_DS_UNIT_TEST / PERLUTF */

/* Turn on to show info on all blocks sent to the PL1 FIFO. Default off.
** Must turn specific switches below to show specific block types
*/
#undef  GSM_GPRS_GRLU_SHOW_BLOCK_SENT_TO_FIFO

/* Turn on specific switches. Default off
*/
#undef  GSM_GPRS_GRLU_SHOW_PTX
#undef  GSM_GPRS_GRLU_SHOW_PACK
#undef  GSM_GPRS_GRLU_SHOW_NACK
#undef  GSM_GPRS_GRLU_SHOW_UNACK

/* Turn on to show PTX blocks sent to the FIFO only when CV is active.
** Must turn GSM_GPRS_GRLU_SHOW_BLOCK_SENT_TO_FIFO to on and GSM_GPRS_GRLU_SHOW_PTX to
** off
*/
#define GSM_GPRS_GRLU_SHOW_PTX_CV_ACTIVE

/* Turn on to show PL1 confirmations of all blocks (except PACK) transmitted by NPL1.
** Default off
*/
#define GSM_GPRS_GRLU_CONFIRM_TX

/* Turn this on to show PL1 confirmations of only PACK blocks transmitted. Default off
*/
#define GSM_GPRS_GRLU_CONFIRM_PACK_TX

/* Turn on to show transmit window variables. Default off
*/
#define GSM_GPRS_GRLU_SHOW_TX_VARS

/* Turn on to show data transfer in progress. Default off
*/
#define GSM_GPRS_GRLU_SHOW_DATA_XFER

/* Turn on to show transfer of PACK blocks. Default off
*/
#undef  GSM_GPRS_GRLU_SHOW_E_TX_PTX_PACK_BLKS

/* Turn on to show PDU Grouping operations. Default off
*/
#undef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_GENERAL

/* Turn on to show PDU Grouping operations relating transferring the number of
** blocks to TBC. Default off
*/
#undef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_XFER

/* Turn this on to show PDU Grouping Adjust operations. Default off
*/
#undef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_ADJ

/* Turn this on to show PDU Grouping status. Default off
*/
#undef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_STATUS

/* Turn this on to show Ungrouping Operations. Default off
*/
#undef  GSM_GPRS_GRLU_SHOW_PDU_UNGROUPING

/* Turn on to show EPUAN interpretation. Default off as it is very intensive
*/
#define GSM_GPRS_ERLU_SHOW_EPUAN

/* Turn on this to show T3182 activites. Default on
*/
#define GSM_GPRS_GRLU_SHOW_T3182

/* Turn on this to show contention resolution activites. Default off
*/
#define GSM_GPRS_GRLU_SHOW_CRES

/* Turn on to show Stall Indicator activities. Default on
*/
#define GSM_GPRS_GRLU_SHOW_SI

/* Turn on this to show N3102 activites. Default on
*/
#define GSM_GPRS_GRLU_SHOW_N3102

/* Turn on this to show N3104 activites. Default on
*/
#define GSM_GPRS_GRLU_SHOW_N3104

/* Turn on to show last_psn being assigned. Default off
*/
#define  GSM_GPRS_SHOW_FIRST_LAST_PSNS

/* Turn on to show every PDU being freed. Default off
*/
#undef  GSM_GPRS_GRLU_SHOW_FREE_PDU

/* Turn on to show the number of free dup items. Default off
*/
#define  GSM_GPRS_SHOW_FREE_DUP_IN_ENQUEUE

/* Turn on to show when there are no blocks to send in un-ack mode. Default off
*/
#undef  GSM_GPRS_GRLU_SHOW_NO_TX_UNACK

/* Turn on to show the more info on the PDUs enqueued by LLC. Default on
*/
#define GSM_GPRS_GRLU_LLC_PDU_TYPE

/* See GSM_GPRS_GRLU_DUMP_BLOCK in grlculfifo.c
** Turn on to show block's contents. Use sparingly. Default off
*/

/* Turn on to show resource reallocation activities. Default off
*/
#undef GSM_GPRS_GRLU_SHOW_REALLOC

/* Turn on to show short access reallocation acitivities. Default off
*/
#define GSM_GPRS_GRLU_SHOW_ACC_PHASE

/* Turn on to show segmentation process. Default off
*/
#undef GSM_GPRS_GRLU_SHOW_SEGMENT

/* Turn on to show operations relating to PAQ (pending ack queue). Default off
*/
#undef GSM_GPRS_GRLU_SHOW_PAQ

/* Turn on to show FIFO unput operations. Default off
*/
#undef  GSM_GPRS_GRLU_SHOW_FIFO_UNPUT

/*==================================================================
** End of temporary debug switches
**==================================================================
*/

/*****************************************************************************
***
***     Public Constant Definitions
***
*****************************************************************************/

/* Maximum number of data blocks segmented ahead of the block being transmitted.
*/
  #define MAX_SEGMENT_DISTANCE    10
  #define E_MAX_SEGMENT_DISTANCE  20

/* This counter is used to detect mis-acknowledgement of PTX block sent to the FIFO.
** The mis-acknowledgement causes the VB state to stay at PTX and hence can not
** be re-transmitted. This count value specifies the number of times a stuck PTX
** block is being assessed by the transmit function. When the number of times exceeds
** this count the PTX block is converted to PACK for re-transmission.
** It also applies to NACK blocks that are stuck as TXA_PENDING_PACK in the FIFO
*/
#define MAX_MISS_CNT      15

/* This count value specifies the maximum of number continuous "rlc no data" events
** reported by PL1. When this threshold is reached it is considered a genuine
** "rlc no data" condition. If there is any intervening sucessful get by PL1 then
** the counter is reset.
*/
#define GRLC_UL_NO_DATA_THRESHOLD   256


/*--------------------------------------------------------------
** Maximum number of attempts at re-sending the last data block
** with CV = 0 in unack mode
**--------------------------------------------------------------
*/
#define UNACK_RE_TX_MAX   4

/* Invalid BSN number for EGPRS RLC */
#define E_GRLC_INVALID_BSN 0xffff

/* Invalid BSN number for GPRS RLC */
#define GRLC_INVALID_BSN 0xff

/* 48 bits (6 bytes) of "0"s that are needed for padding MCS-8
** blocks when re-txed as MCS-6/3 blocks.
*/
#define E_GRLC_PADDING_BYTES 6


/*-----------------------------------------------------------------------
** Maximum number of blocks to segment at any time is 1 greater than the
** capacity of the UL FIFO.
**-----------------------------------------------------------------------
*/
#define MAX_BLOCKS_TO_SEGMENT               (RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE+1)

/*-----------------------------------------------------------------------
** The number of blocks produced will match the FIFO's depth.
**-----------------------------------------------------------------------
*/
#define MAX_BLOCKS_TO_SEND                  RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE

/* Pancturing Scheme (PS) literal values
*/
#define GRLC_PS_1   0
#define GRLC_PS_2   1
#define GRLC_PS_3   2

/* Macro to return TRUE if NW_EXT_TBF is supported and is active
*/
#define IS_EXT_UTBF_ACTIVE(gas_id)	(sysinfo[gas_id].ext_utbf == 1)

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/*----------------------------
** GPRS Suspend/Resume States
**----------------------------
*/
typedef enum
{
  /* In Resume state, ie no suspension
  */
  GRLC_RESUME,

  /* In Suspend state after receiving GRR_SUSPEND_REQ from LLC. All existing
  ** data PDUs are put aside in grlc_ul_old_llc_q.
  */
  GRLC_SUSPEND

} grlc_suspend_resume_status_t;

/*--------------------------------
** Status of segmentation process
**--------------------------------
*/
typedef enum
{
  /* in-active
  */
  SEGMENT_NULL,

  /* Have segmented the number of blocks requested
  */
  SEGMENT_REACHED_NUM_BLOCKS,

  /* Segmentation of current pdu is complete
  */
  SEGMENT_DONE

} segment_status_t;

/*---------------------------------------------------------------------------
** Definitions for Uplink RLC Windows: Transmit Window, Attach Window and
** Un-ack Transmit Window.
**
** The max size of the Transmit Window is WS (fixed at 64 for GPRS, greater for EGPRS).
**
** The Current Transmit Window comprises the blocks between V(A) the oldest
** non-positively acknowledged block, and V(S) the index to where the
** next block is to be transmitted. When this value reaches WS,
** the window is full or stalled.
**
** The Attach Window comprises the blocks between V(S) and to the seg_ind
** which is where the next (segmented) radio block is to be attached or
** queued up for transmit.
**
**     <----------- SNS-blocks transmit workspace -------------->
** .......X......................X......................X............
**        ^                      ^                   ^
**        V(A)                   V(S)                   seg_ind
**        <-- cur tx window ---->
**                               <----attach window---->
**
** The Un-ack Transmit Window starts from V(S) and runs to before seg_ind.
**
**     <----------- SNS-block transmit workspace --------------->
** ..............................X......................X............
**                               ^                  ^
**                               V(S)                  seg_ind
**                               <-- unack tx window -->
**
**---------------------------------------------------------------------------
*/

/* pdu number type
*/
typedef uint8 pdu_num_t;

/*-------------------------------------------------------------------
** Resource allocation substates during transfer and release states.
** The state is reset to RALS_NULL when a new pdu is created.
**-------------------------------------------------------------------
*/
typedef enum
{
  /* realloc null state. Re-allocation has not been done
  */
  RALS_NULL,

  /* case (2), reallocate at tx complete but before getting ack from n/w
  ** during the release state
  */
  RALS_AWAIT_TX_COMPLETE,

  /* RALS_CURRENT_PDU_TX_COMPLETED: this new state allows the post_tx_actions()
  ** function to detect the following conditions:
  ** - realloc_state is RALS_AWAIT_TX_COMPLETE
  ** - last PTX block sent has a boundary set LI/M/E
  ** When the above are true, realloc_state shall be set to RALS_CURRENT_PDU_TX_COMPLETED
  ** This allows the state machine to pick up this state and send the PRR for case (2)
  ** above
  */
  RALS_CURRENT_PDU_TX_COMPLETED,

  /* done, was in AWAIT_RESP state and now got assignment for current and
  ** next pdu.
  */
  RALS_COMPLETE

} realloc_state_t;

/*----------------------------------------------------------------------------
** Uplink events type.
** 4 types: L1, MAC, LLC and TIMER. These can occur at the same time but only
** one is serviced at any time. When an event occurs the the event variable
** shall be set to that event, say, UE_PL1. This helps identify to the
** rlc_ul_state_machine as to event source.
**----------------------------------------------------------------------------
*/
typedef enum
{
  UE_NULL,           /* not valid, pre-init        */

  UE_PL1,            /* uplink event from Layer 1  */
  UE_MAC,            /* uplink event from MAC      */
  UE_TMR,            /* uplink event from timer    */
  UE_LLC             /* uplink event from LLC      */

} rlc_ul_event_t;

/*-------------------------------------------------
** RLC uplink operational state (US: Uplink State)
**-------------------------------------------------
*/
typedef enum
{
  /*----------------------------------*/

  /* Transmit idle state. Ready to receive uplink assignment
  */
  US_NULL,

  /*----------------------------------*/

  /* Received PDU(s) from LLC. Sent off TBF establishment request.
  ** Wait for assignment from the n/w via MAC
  */
  US_AWAIT_ASS,

  /*----------------------------------*/

  /* TBF is established and transfer of current PDU in ack mode is in
  ** progress. Resource reallocation activities for next PDU, if required,
  ** is in progress.
  */
  US_ACK_XFER,


  /*----------------------------------*/

  /* GPRS transparent transfer protocol state where the current LLC PDU has been sent to
  ** GRR to be delivered via L2. In this state RLC UL awaits the confirmation of that
  ** transfer
  */
  US_GTTP_XFER,

  /*----------------------------------*/

  /* TBF is established and transfer of current PDU in un-acknowledge mode
  ** is in progress. Resource reallocation activities for next PDU, if
  ** required, is in progress.
  */
  US_UNACK_XFER,

  /*----------------------------------*/

  /* ETSI Test Mode state. TBF is established and in un-acknowledge mode
  ** RLC just loops back DL data blocks from PL1. No UL LLC PDUs in this mode.
  */
  US_TEST_MODE,

  /*----------------------------------*/

  /* Inactive state waiting for reset signal from MAC
  */
  US_AWAIT_RECOVERY

  /*----------------------------------*/

} rlc_ul_state_t;

/*----------------------------
** GPRS/EGPRS  Reprocess cause
**----------------------------
*/
typedef enum
{
  /* reprocess when change of CS/MCS
  */
  GRLC_CHANGE_OF_CS_MCS,

  /* reprocess when contention res
  */
  GRLC_REPROCESS_CON_RES,

  /* reprocess for other reasons
  */
  GRLC_REPROCESS_OTHER

} grlc_reprocess_cause_t;

/*-------------------------------------------------------------------------
** This structure contains all the transmit related info of a radio block.
** The transmit workspace is an array of SNS of these blocks.
** The transmit window lies within this area and its maximum size is 64.
**-------------------------------------------------------------------------
*/
/*---------------------------------------------------------------------------
** EGPRS tranmit workspace contains 2048 elements. To minimise static memory
** usage only a maximum of 1028 elements plus a margin of 10 elements are used.
** This physical space is then mapped into the virtual space of SNS (2048)
** elements.
**
** Each element contains all transmit related information of a radio block
**-------------------------------------------------------------------------
*/
typedef struct
{
  /* tx ack status V(B). Note that this is not an array but a single
  ** element within an array of ul_xfer_t structs.
  */
  tx_ack_state_t        vb;

  /* block count value after the block is transmitted plus the expression
  ** MAX(BS_CV_MAX,1)-1
  */
  uint16                blk_cnt;

  /* data block in internal format
  */
  grlc_ul_data_block_t  blk;

} grlc_ul_tx_array_t;


/*--------------------------------------------------------------------------
** Structure for holding segmentation-type information to allow accurate
** calculation of the Total Block Count (TBC) used in working out Countdown
** Values.
**--------------------------------------------------------------------------
*/
typedef struct
{
  uint16  octets;         /* total number of octets */
  uint16  num_blks;       /* total number of blks */
  uint16  partial_octets; /* number of octets remained in the (last) partial block */
  uint16  pad_octets;     /* octets remained in partial block which may include the
                          ** ext octet
                          */
  boolean b_ext_octet;    /* TRUE if an Extension Octet (octet with LI field) exists */

} grlc_ul_tbc_info_t;


/*----------------------------------------------------------------------------
**
**----------------------------------------------------------------------------
*/
typedef enum
{
  /* Initial state where no freein gis required, eg. before the first E/PUAN
  */
  NONE_TO_FREE,

  /* Freeing of curent PDU was avoided because psn_to_free_inclusive covers
  ** cur_pdu_ptr[gas_id]->psn. So freeing was done upto and including the PDU previous
  ** to psn_to_free_inclusive
  */
  FREE_CURRENT_PDU,

  /* All freeing has been done, in e/update_ack_state_array
  */
  DONE_FREEING

} pdu_freeing_state_t;


/*----------------------------------------------------------------------------
** Definition of PDU group of the same type. Part of the enqueue_pdu()
** functionality is to group together contiguous PDUs of the same type.
** Same type is defined as having the same radio priority and peak-throughput
** class. This is to aid the Dynamic Open-ended TBF reallocation process such
** that PDUs of the same group (mostly data) are delivered with the minimum
** number of resource requests. This is specially true for very short PDUs
** where the number of blocks in a PDU amounts to less the the minimum
** (BS_CV_MAX) blocks.
**----------------------------------------------------------------------------
*/
typedef struct
{
  /* PDU Sequence Number (global)
  */
  uint16              psn;

  /* Data structure to enable calculation of total block count for a PDU group
  */
  grlc_ul_tbc_info_t  tbci;

} grlc_ul_pdu_group_t;

typedef enum
{
  /* initial state
  */
  GUARD_OFF,

  /* block with CV = zero has just been transmitted
  */
  CV_ZERO,

  /* blocks with CV=0 transmitted and Va = Vsegment
  */
  GUARD_ON,

} ext_utbf_guard_timer_t;

/*-------------------------------------------------------
** Main data structure required for rlc uplink operation
**-------------------------------------------------------
*/
typedef struct
{
  /*------------------------
  ** Uplink RLC sub-states
  **------------------------
  */

  /* hold the current segmentation status, of the current PDU
  */
  segment_status_t    segment_state;

  /* set to TRUE after one time initialisation of UL has been done.
  */
  boolean             b_done_ul_one_time_init;

  /*-------------------------------------
  ** uplink RLC state machine variables
  **-------------------------------------
  */

  /* current rlc ul state
  */
  rlc_ul_state_t                   state;

  /* current rlc ul event
  */
  rlc_ul_event_t      event;

  /*--------------------------------------------------------------------------
  ** Flag set when LLC sends GPRS Suspend signal. Cleared when LLC sends GPRS
  ** Resume signal and at power up
  **--------------------------------------------------------------------------
  */
  grlc_suspend_resume_status_t  suspend_state;

  /*--------------------------------
  ** MAC header information fields
  **--------------------------------
  */

  /* Stall Indicator field value.
  */
  uint8               si;

  /*---------------------------------------
  ** RLC uplink header information fields
  **---------------------------------------
  */

  /* Block sequence number
  */
  uint16              bsn;

  /*------------------------------------------------------
  ** transmit flags. Only meaningful during transmission
  **------------------------------------------------------
  */

  /*---------------------
  ** transmit variables
  **---------------------
  */

  /* segment index variable.
  ** Index to the next free location for a radio block to be attached
  ** to the transmit array. It is always greater than or equal to V(S).
  */
  uint16 seg_ind;


  /* V(S) send state variable.
  ** Denotes the sequence number of the next-in-seq RLC data block to be sent.
  ** Set to zero initially and incremented after each transmission of a
  ** PENDING_TRANSMIT (PTX) data block which has not been transmitted before.
  ** The present of the FIFO means this variable is only incremented after PL1
  ** has performed a Get-access on a data block.
  **
  ** This variable controls the PTX blocks.
  ** The range is between V(A) and In_index.
  */
  uint16 vs;


  /* V(S) is incremented in post_tx_actions() in ISR time. During processing of
  ** NACK, PTX and PACK blocks V(S) is read into inst_vsf and used throughout
  ** without reading V(S) again. This ensures consistent value for the tx-blks()
  ** to work with
  */
  uint16 inst_vs;


  /* V(S)F send state variable which holds the BSN of the next block to be sent
  ** to the RLC-PL1 UL FIFO. This is different to V(S) since V(S) is only
  ** incremented when PL1 performs a Get-access whereas this is incremented
  ** after a block is sent to the FIFO.
  **
  ** The range is between V(S) and seg_ind
  */
  uint16 vsf;

  /* V(A) acknowledge state.
  ** In acknowledged mode it holds the BSN value of the oldest RLC data block
  ** that has not been positively ack'ed (NACK or PACK)
  **
  ** In unack mode, it marks the oldest data block sent after a paknak is
  ** received. In this way, we can check the distance between V(A) and V(S)
  ** to detect 'stall' condition so that post_tx_actions() knows when to
  ** start T3182.
  */
  uint16 va;

  /* V(A)-old or V(A)-post-CV-zero
  ** This vaiable marks the value of V(A) at the time a BSN of CV=zero was transmitted.
  ** This value is used to detect V(A) advancing following PUAN received from NW
  */
  uint16 va_old;

  /* Send state variable for the pending ack block.
  ** This variable holds the BSN of the next PACK block to be sent to the FIFO.
  ** This is required for RLC to keep track of the next PACK block to send to
  ** the FIFO on different invocations.
  **
  ** This variable covers the un-ack blocks which includes the NACK blocks
  ** hence the range is between V(A) and V(S). When RLC has no new NACK blocks
  ** or PTX blocks to send it re-sends the PACK blocks. When pack_vsf passes V(S)
  ** it wraps back to V(A) again.
  **
  ** Since a PUAN could result in V(A) being advanced, on every invocation
  ** pack_vsf is updated to be in its correct range
  */
  uint16 pack_vsf;


  /* Send state variable for the NACK blocks.
  ** This variable holds the BSN of block where the search for the next NACK
  ** block to send to the FIFO starts
  **
  ** The range is between V(A) and V(S).
  */
  uint16 nack_vsf;


  /* Starting sequence number (SSN) in the packet ack/nack msg from the n/w.
  */
  uint16 ssn;

  /* Re-transmit count for the last block (CV = 0). In unack mode,
  ** when waiting for the FAI to set after sending the last block with
  ** CV = 0, RLC keeps a count of the number of re-transmissions. When the count
  ** reaches 4, no more re-tx takes place and the timer t3182 will expire
  ** causing an abnormal release with random access.
  */

  uint8           re_tx_cv_0_cnt;

  /*-------------------------------
  ** operational status variables
  **-------------------------------
  */

  /* Boolean indicating access condition. FALSE at power up and on receipt of
  ** MR_NO_PS_ACCESS_IND. TRUE on receipt of MR_PS_ACCESS_REQ
  */
  boolean             b_access;

  /* uplink release cause as indicated by MAC
  */
  rm_tbf_release_t    ul_release_cause;

  /* Current ul rlc mode
  */
  uint8               rlc_mode;

  /* Current number of ts assigned.
  */

  /* pre_emptive_tx for egprs
  */
  uint8               pre_emptive_tx;

  /* In one-phase access mode, this value should be assigned the coding scheme
  ** as follows:
  ** if alloc_cnf.tlli_block_cs = 0, tlli_cs = RM_CS_1
  ** if alloc_cnf.tlli_block_cs = 1, tlli_cs = alloc_cnf.cs
  **
  ** For EGPRS
  ** if alloc_cnf.tlli_block_cs = 0, tlli_cs = RM_MCS_1
  ** if alloc_cnf.tlli_block_cs = 1, tlli_cs = alloc_cnf.mcs
  */
  uint8               tlli_cs;

  /* coding scheme of the last correctly received data block
  */
  uint8               last_valid_coding_scheme;

  /* MCS of the last correctly received data block
  */
  uint8               last_valid_mcs;

  /* contention resolution status of uplink access.
  ** Only used during one-phase access. When unresolved the tlli field must
  ** be included in rlc uplink data blocks. When resolved the tlli field is
  ** omitted.
  **
  ** Default is unresolved
  */
  uint8               contention_status;

  /* Flag to indicate a block has been transmitted by PL1. This is set to FALSE
  ** at the start of a TBF. It is set by the post_tx_action() function to trigger
  ** RLC to start sending request to re-allocate. Because when RLC is in transfer
  ** MAC and PL1 may not tey reached that state
  */
  boolean             b_first_blk_sent;

  /* flag to indicate to L1 to start checking the contents of the UL FIFO
  ** for data blocks. This is set to TRUE at the beginning of a TBF and
  ** after a coding change takes place.
  **
  ** The transmit function loads up the FIFO with data blocks and checks
  ** this flag. If set, it will send L1 the PH_DATA_REQ and clears the
  ** flag. So that L1 only receives the primitive once for the duration
  ** of a TBF, unless there is a coding scheme change.
  */
  boolean             b_first_fifo_load;

  /*--------------------------------
  ** countdown procedure variables
  **--------------------------------
  */

  /* current CV (Countdown Value). This value is pre-calculated and loaded
  ** into radio block during segmentation.
  */
  uint8           cv;

  /* Indicate when a forced release of a TBF is required, eg. NORMAL_RELEASE
  */
  boolean         b_forced_release;

  /* bsn of the block with CV value being 0. This is used to index the
  ** block to re-send block with CV = 0 to the peer during release state
  */
  uint8           bsn_of_cv_0_block;

  uint16          e_bsn_of_cv_0_block;

  /* flag to indicate RLC data block with CV = 0 has been sent
  */
  boolean         b_cv_0;

  /* CV value of the previously transmitted PTX block
  */
  uint8           ptx_cv;

  /* CV value of the previously transmitted PACK block (for un-ack mode)
  */
  uint8           pak_cv;

  /*-----------------------------------
  ** acknowledgement status variables
  **-----------------------------------
  */

  /* the packet uplink acknack last received from the peer entity
  */
  rm_acknack_desc_t    ul_acknack;
  egprs_ack_nack_des_t egprs_acknack_desc;

  /* Used to flush Pack FIFO when there is abort_mode from L1
  ** during PUAN processing.  CR: 125334
  */
  boolean              b_puan;

  /* This is the block count value associated with the latest ul_acknack above
  */
  uint16              acknack_blk_cnt;

  /*------------------------------
  ** RLC uplink operation timers
  **------------------------------
  */

  /* MS Counter for handling cell-reselection. This is a signed value
  */
  int8                n3102;

  /* Since sysinfo.pan_max is a 3-bit coded value, this variables is required
  ** to hold the decoded value. Use signed value to compare with n3102
  */
  int8                pan_max_value;

  /* Contention resolution counter
  */
  uint8               n3104;

  /* wait for ack. Used to define when to stop waiting for temporary
  ** packet uplink acknack after the last RLC data block has been sent
  ** for the current window or the entire tbf.
  ** Start when block with CV = 0 is transmitted or when the the tx
  ** window has stalled. Stopped when a packet uplink acknack is received
  ** or when the window becomes un-stalled.
  */
  grlc_timer_t        timer_t3182;

  /* Extended uplink TBF guard timer, expiry count and sub-state variable
  */
  grlc_timer_t             timer_await_ul_acknack;
  uint16                   timer_await_ul_acknack_expiry_cnt;
  ext_utbf_guard_timer_t   guard_timer_state;

  /* stat of t3182 expiry
  */
  uint16              timer_t3182_expiry_cnt;

  /* Guards against being left in Await assignment
  */
  grlc_timer_t        timer_await_ass_guard;

  /* Count the number of times this timer has been activated since the last power up
  */
  uint16              timer_await_ass_guard_expiry_cnt;

  /* Awaiting MAC response timeout.
  ** Applies to all MAC messages.
  */
  grlc_timer_t        timer_mac_ul_resp;


  /* Delete any stale UL PDUs in RLC buffers on expiry of stale_pdu_delete
  ** timer.
  */
  grlc_timer_t        timer_stale_pdu_delete;

  /* Number of times stale pdu delete timer has expired since power up.
  */
  uint16              timer_stale_pdu_delete_expiry_cnt;

  boolean             b_ul_pdu_delete_after_stale_tmr_expiry;


  /* ! other timers ..
  */

  /*----------------------------------------
  ** transmit resource related information
  **----------------------------------------
  */

  /* MAC tbf est cnf info is stored in this struct
  ** Instead of plucking out individual items the whole
  ** struct is copied into this structure.
  */
  mr_ul_alloc_cnf_t       cur_res;

  /* the current reallocation sub state within the xfer state
  */
  realloc_state_t         realloc_state;

  /* Number of octets to send in this TBF belonging to the current PDU or
  ** the remained of the current PDU and the next PDU.
  ** This value decreases on every un-transmitted data block being sent.
  ** retransmissions do not alter this value.
  ** This value does not include MAC headers.
  ** ! check if this value should include the RLC headers length.
  */
  uint16        octets_req;

  /* After an asignment
  */
  uint16        octets_rem;

  /* GET_PL1_FREE_SPACE
  */
  uint16        free_space;
  uint16        max_blks;

  /* the equivalence of octets_req in unit of blocks.
  */
  uint16        blocks_req;

  /* Number of blocks remained to transmit.
  ** Assigned the value of blocks_req after a TBF is established or extended
  */
  uint16        blocks_rem;

  /* the equivalence of the octets granted in blocks. Assumed to be greater
  ** than blocks_req
  */
  uint16        blocks_granted;

  /* transient value which is the sum of the number of octets remained to
  ** send in current pdu plus the length of the next pdu. This value is used
  ** for reallocation in both fixed and dynamic modes.
  **
  ** ! reviewed. It now indicates the number of octets present in the next PDU
  */
  uint16        realloc_octets_req;

  /* Realloc blocks requested (calc)
  */
  uint16        realloc_blocks_req;

  /* Pointer to PDU whose parameters are to be sent on the next PRR
  */
  gprs_grr_ul_mes_t  *realloc_pdu_ptr;

  /* PDU sequence number where PRR type 2 should be sent
  */
  uint16        realloc_psn;

  /* PDU sequence number of PDU when a reallcation request was made.
  ** So this value holds the PSN of the previously realocated PDU.
  ** Also prior to a realloction request this value is checked against the PDU
  ** to be re-allocated. If there were the same then no reallocation will be sent
  */
  uint16        prev_realloc_psn;

  /* total number of blocks to transmit in this TBF. Derived from blocks_req
  ** (assumed blocks_granted is greater). Effects CV values directly. This
  ** value is refreshed when resource is re-allocated
  */
  uint16        tbc;

  /* Absolute block sequence number (BSN'S) with range from 0 to (TBC-1)
  **
  ** Does not relate to the BSN (0..127) value of a block. It is simply a
  ** counter from 0 of the number of blocks sent to the peer. When
  ** TBC - ABS_BSN = 1 then the block should have a CV value of 0, regardless
  ** of its BSN value.
  ** BSN' This value should be reset to 0 at the start of a TBF.
  ** BSN' should be unchanged if more blocks are added to TBC.
  */
  uint16        abs_bsn;

  /* at reallocation, the current resource vars are added to the length of
  ** the next pdu to arrive at the 'realloc_octets_req'. When the resource
  ** comes back, the current resource status would have moved on due to further
  ** transmission of blocks. Therefore..
  */
  uint16        next_pdu_octets;

  /* when a PDU ends with a partial block the space remained is considered as
  ** pad_octets. If the TBF ends there then the number of padded octets would
  ** equal this value. This values already accounts for any extension octet.
  ** The pad_octets is the size of the block less data octets and less
  ** extension octet(s)
  */
  uint16        pad_octets;

  /* When a PDU ends with a partial block, the number of data octets (not
  ** including any extension octets) is held here
  */
  uint16        partial_octets;

  /* This flag is set when a data block starts with its LI field set to 0 t
  ** indicate the previous block was a perfect-fit block. This is cleared after
  ** e_pg_calc_initial_tbc() has detected and acted on this condition
  */
  boolean       b_li_0_exists;

  /* flag to indicate if the last block of the current pdu in the TBF already
  ** contains an extension octet. This is set to TRUE when the last block is
  ** partial which demands a boundary extension octet and one already exists,
  ** or the extension is omitted as in the case of B.3. It is set to FALSE
  ** when the final block is filled and the TBF is assumed to end with that pdu
  */
  boolean       b_ext_octet;

  /* hold the number of blocks to be sent in the current TBF. This value changes
  ** after reallocation where more blocks are added. The total block count (TBC)
  ** is a copy of this variable.
  */
  uint16        num_blks;

  /* When NW supports PFC feature, i.e. pfc_feature_mode is TRUE, 'pfi' then contains
  ** the last PFI value sent to the NW for UL TBF establishment/re-establishment.
  ** Note that PFI is initialized to RM_PFI_INVALID = 0xff to indicate that no valid
  ** PFI has been send to NW.
  */
  uint8         pfi;

  /* flag to indicate to the segmentation process to attach and send off
  ** the last data block whose CV value is 0. This is required in reallocation
  ** at tx-complete mode where the last block must be sent before reallocation
  ** can take place.
  ** This flag is cleared at the start of a reallocation assessment, and the
  ** the segmentation process (user) and it is set on entering reallocation
  ** at tx-complete.
  */
  boolean       b_to_send_last_block;

  /* Number of llc pdu octets received and have not been delivered with
  ** acknowledgement.
  */
  uint16        llc_pdus;
  uint32        llc_octets;
  uint16        susp_state_data_pdus;

  /* Logging counters for each type of blocks transmitted during an uplink TBF
  */
  uint16        nack_cnt;
  uint16        ptx_cnt;
  uint16        pack_cnt;

  uint16        num_ests;

  /* Flag is set when an error has occurred which requires RLC UL to recover
  ** gracefully by flushing its queue and starting afresh. At present, only
  ** dup item exhaustion will lead to this sort of recovery
  */
  boolean       b_error_recovery_required;

  /* Pending ack queue stores pointers to the LLC PDU message that are in xfer and
  ** awaiting acknowledgement from the NW. When a PDU is loaded into 'cur_pdu_ptr',
  ** a copy is entered into this queue. When a PDU is acknowledged by the NW it is
  ** removed from this queue.
  */
  #define GRLC_UL_MAX_PAQ_SIZE  256
  #define NUM_LLC_PDUS_HIGH     (120)
  #define NUM_LLC_PDUS_LOW      (NUM_LLC_PDUS_HIGH - 20)
  uint16              paq_in;
  uint16              paq_out;
  uint16              paq_ctr;
  uint16              paq_ctr_max;
  gprs_grr_ul_mes_t*  paq_ptr[GRLC_UL_MAX_PAQ_SIZE];

  /* Additional usage in GAN access mode: points to the OLDEST un-confirmed
  ** PSN (by GAN)
  */
  uint16              first_psn_to_free;

  /*
  */
  uint16              psn_to_free_inclusive;
  pdu_freeing_state_t pdu_freeing_state;

  /* Flag to indicate if LLC is flow-controlled (1)
  */
  uint16              flow_control_suspend;

  /* Structure containing params in the last UL TBF establishment
  */
  rm_ul_est_tbf_req_t last_req;

  boolean             anite_gcf_flg;

  /* ETSI test mode */
  test_mode_T         test_mode;

  /* Used in ETSI Test Mode. Set to TRUE when a PUAN with FAI set is expected to release the UL TBF
  ** after receiving UL TBF release indication with cause normal release.
  */
  boolean             test_mode_ul_tbf_rel_after_fai;

  /* Set only in EXT_UTBF mode to indicate call to pg_calc_initial_tbc() is required
  ** instead of pg_adjust_tbc()
  */
  boolean             b_ext_utbf_init_group;

  /* Counts the number of panic resets requested by MAC from power up
  */
  uint16              panic_reset_cnt;

#ifdef  FEATURE_GPRS_PS_HANDOVER
  boolean             b_psho_active;

  /* Coming out of PSHO_COMPLETE_IND cur_pdu_ptr may not be loaded.
  ** this could mostly happen in WtoG PSHO or when only DL active.
  ** Set when cur_pdu_ptr is NULL in XFER during PSHO
  */
  boolean             b_psho_llc_pdu_absent;
#endif /* FEATURE_GPRS_PS_HANDOVER  */


 /* used to delete incoming signalling pdu under no service case
 */
 delete_pdu_cause_t  no_ps_access_del_pdu_cause;

#define USF_EVALUATION_INTERVAL_MS       (1000)  /* every 1 second */
#define USF_EVALUATION_INTERVAL_MAX      (USF_EVALUATION_INTERVAL_MS/18)
#define MAX_UINT32                       (65536L*65536L-1L)

 /* When TX Blanking is above the maximum threshold below, a reduction is applied to the
 ** Uplink low watermark and high watermark
 */
#define TX_BLANKING_PERCENTAGE_MAX       (50)

#define IS_TX_BLANKING_EXCEEDED_THRESHOLD(gas_id)           \
        ((ul[gas_id].usf_info.blanking_percentage_average) > TX_BLANKING_PERCENTAGE_MAX)

 /* USF Utilisation evaluation and calculation variables
 */
 uint16                     usf_evaluation_interval_cnt;    /* in units of 18.45 miliseconds */

 /* data structure from L1 regarding USF usage, tx blankings info etc..
 */
 l1_usf_utilisation_info_t  usf_info;

 /* Set when an additional flow control event is needed to adjust flow control based on tx blankings
 */
 boolean                    b_adj_flow_control;

 /* The current octet count in the uplink temporary queue. This value and the ''llc_octets' together
 ** determines the flow control
 */
 uint32                     temp_queue_octet_cnt;
 uint16                     temp_queue_pdu_cnt;

 /* last PSN with success indicated
 */
 uint16                     psn_success_ind;

} grlc_ul_t;

/*----------------------------------------------------------------------------
** This structure holds the BSN and the ackowledgement status of a data block
** that is to be acknowledged as transmitted by L1
**----------------------------------------------------------------------------
*/
typedef struct
{
  uint16          bsn;
  tx_ack_state_t  ack_state;

} post_tx_blk_info_t;


/*-----------------------------------------------------------------------------
** This structure holds the FIFO and the array of structures whose elements
** represent an array of BSN values and acknowledgement status of all the data
** blocks that are to be acknowledged as transmitted by PL1.
**-----------------------------------------------------------------------------
*/
typedef struct
{
  uint16  in_ind;
  uint16  out_ind;
  uint16  ctr;

  /* Enough items for MSC-12 and then some
  */
  #define POST_TX_SHOW_ITEM_MAX   24

  post_tx_blk_info_t  show_fifo[POST_TX_SHOW_ITEM_MAX];

} post_tx_blk_fifo_t;

/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/*-----------------------------------------------
** The Spare field of the RLC header is set to 0
**-----------------------------------------------
*/
#define RLC_SPARE   0

/*--------------------------------------------------------------------------
** Macros to test various CV status
**
** Countdown procedure is considered started if the current CV value of
** the last data block sent off (to the RLC/PL1 UL FIFO) is less than or
** equal to BS_CV_MAX. When dealing with a short PDU where the number of
** data blocks requested is less than or equal to BS_CV_MAX. As soon as the
** resources are granted and before a block is transmitted, CV should be
** considered to have been started.
**--------------------------------------------------------------------------
*/

#define IS_CV_ACTIVE(gas_id)    (ul[gas_id].cv != BS_CV_CONST)
#define IS_CV_INACTIVE(gas_id)  (!IS_CV_ACTIVE(gas_id))
#define IS_CV_NOT_ZERO(gas_id)  (ul[gas_id].cv != 0)

/*-------------------------------------------------------------------------
** Transmit countdown value is considered ACTIVE when the previously
** transmitted data block's CV value is NOT 15. This applies to PTX blocks
** in ack mode and PACK blocks in un-ack mode
**-------------------------------------------------------------------------
*/
#define IS_TX_CV_ACTIVE(gas_id)    (ul[gas_id].ptx_cv != BS_CV_CONST || ul[gas_id].pak_cv != BS_CV_CONST)
#define IS_TX_CV_INACTIVE(gas_id)  (!IS_TX_CV_ACTIVE(gas_id))
#define IS_TX_CV_ZERO(gas_id)      (ul[gas_id].ptx_cv == 0 || ul[gas_id].pak_cv == 0)
#define IS_TX_CV_NOT_ZERO(gas_id)  (!IS_TX_CV_ZERO(gas_id))

/*--------------------------------------------------------------
** Macro to return TRUE if the network supports PFC procedures.
**--------------------------------------------------------------
*/
#define IS_PFC_NW_SUPPORTED(gas_id)	(sysinfo[gas_id].pfc_feature_mode != 0)

/*------------------------------------------------------------------------------
** Table of Parameters which determine resource reallocation actions
**   Keys
**    H : new PDU has
**
**   When RLC Mode is the same for current and next PDU
**   RP   PT     PFI    Action
**    H    H(x)   S(x)     T1
**    H    H(x)   D(x)     T1
**    H    L(x)   S(x)     T1
**    H    L(x)   D(x)     T1
**    L    H(x)   S(x)      T2
**    L    H(x)   D(x)      T2
**    L    L(x)   S(x)      T2
**    L    L(x)   D(x)      T2
**--------------------------------------
**    H    S(x)   S(x)     T1
**    H    S(x)   D(x)     T1
**    L    S(x)   S(x)      T2
**    L    S(x)   D(x)      T2
**--------------------------------------
**    S    H      S(x)     T1
**    S    H      D(x)     T1
**    S    L      S(x)      T2
**    S    L      D(x)      T2
**--------------------------------------
**    S    S      S        No action
**    S    S      D        T1
**--------------------------------------
**   When RLC Mode is different for current and next PDU
**   RP   PT     PFI    Action
**    H    H(x)   S(x)     T3
**    H    H(x)   D(x)     T3
**    H    L(x)   S(x)     T3
**    H    L(x)   D(x)     T3
**--------------------------------------
**    H    S(x)   S(x)     T3
**    H    S(x)   D(x)     T3
**------------------------------------------------------------------------------
*/



/*--------------------------------------------------------------------------
** Macros to test for reallocation type 1 or type 2 according to respective
** radio priority and peak throughput class the the current PDU and the
** incoming PDU (not necessarily the next PDU). Type 3 is the same as type 1
** except rlc_mode is different. This is only applicable to EXT_UTBF being
** active
**
** Note: radio priority 0 denotes the highest priority
**--------------------------------------------------------------------------
*/


  #define IS_REALLOC_TYPE_1(cur_p, new_p)                       \
    (                                                           \
      ( IS_PFC_NW_SUPPORTED(gas_id) &&                          \
        (new_p)->pfi != (cur_p)->pfi &&                         \
        (new_p)->radio_priority == (cur_p)->radio_priority &&   \
        (new_p)->peak_throughput == (cur_p)->peak_throughput    \
      ) ||                                                      \
      (                                                         \
        /* rlc mode must be the same in view of EXT_UTBF */     \
        (cur_p)->rlc_mode == (new_p)->rlc_mode &&               \
                                                                \
        /* RP of new PDU is HIGHER than current PDU's */        \
        ((new_p)->radio_priority < (cur_p)->radio_priority ||   \
         (                                                      \
          (new_p)->radio_priority == (cur_p)->radio_priority && \
                                                                \
          /* PTC of new PDU is HIGHER than current PDU's */     \
          (new_p)->peak_throughput > (cur_p)->peak_throughput   \
         )                                                      \
        )                                                       \
      )                                                         \
    )


#define IS_REALLOC_TYPE_2(cur_p, new_p)                           \
  (                                                               \
    /* rlc mode must be the same in view of EXT_UTBF */           \
    ((cur_p)->rlc_mode == (new_p)->rlc_mode) &&                   \
                                                                  \
    /* RP of new PDU is LOWER than current PDU's */               \
    ( (new_p)->radio_priority > (cur_p)->radio_priority  ||       \
      (                                                           \
        ((new_p)->radio_priority == (cur_p)->radio_priority ) &&  \
                                                                  \
        /* PTC of new PDU is LOWER than current PDU's */          \
        ((new_p)->peak_throughput < (cur_p)->peak_throughput)     \
      )                                                           \
    )                                                             \
  )


/* IS_EXT_UTBF_ACTIVE - to ensure RLC sends RM_REALLOC_REQ
** at the end of the TBF even when RLC mode changes
*/

#define IS_REALLOC_TYPE_3(cur_p, new_p)                         \
  (                                                             \
    /* rlc mode must be different in view of EXT_UTBF */        \
    (cur_p)->rlc_mode != (new_p)->rlc_mode &&                   \
                                                                \
    /* RP of new PDU is HIGHER than current PDU's */            \
    ( IS_EXT_UTBF_ACTIVE(gas_id) ||                             \
      (new_p)->radio_priority < (cur_p)->radio_priority         \
    )                                                           \
  )


/*--------------------------------------------------------------------------------
** 8.1.1.1.2 .. paragraph 5:
** If the new upper layer PDU does not have the same RLC mode as the current
** uplink TBF but has a higher radio priority, the mobile station shall complete
** the transmission of the current upper layer PDU using the countdown procedure
** including acknowledgement from the network, if in RLC acknowledged mode.
** If the TBF is operated in non-extended uplink TBF mode, the mobile station
** shall then release the TBF and establish a new uplink TBF for transmission
** of the new upper layer PDU. If the TBF is operated in extended uplink TBF mode
** (see sub-clause 9.3.1b), the mobile station shall use the procedure in
** sub-clause 8.1.1.6 for changing the RLC mode. When the sending of upper layer
** PDUs with a higher radio priority is completed using the countdown procedure,
** including acknowledgement from the network if in RLC acknowledged mode, the
** mobile station shall try to establish an uplink TBF for the transmission of
** any remaining upper layer PDU(s).
**
** This macro returns TRUE if:
** (Assuming neither pointers are NULL)
**
** 1. current PDU is NOT the last PDU, ie. there are PDUs behind the current PDU
**    besides the PDU that has just arrived, AND
** 2. RLC mode of current PDU is different to that of the incoming PDU, AND
** 3. new PDU has higher radio priority than current PDU
**
** For PSHO the conditions are the same for 1. and 3., condition 2 is modified to:
** 2. (new_p).type != LAY3_DATA
**--------------------------------------------------------------------------------
*/
#ifdef  FEATURE_GPRS_PS_HANDOVER

#define IS_SWAP_REQUIRED(cur_p, new_p)                          \
  (                                                             \
    /* rlc mode must be different regardless of EXT_UTBF */     \
    ( ((cur_p)->rlc_mode != (new_p)->rlc_mode) ||               \
       /* new PDU is a signalling PDU */                        \
      ((new_p)->cause != GRR_CAUSE_LAY3_DATA)                   \
    ) &&                                                        \
                                                                \
    /* RP of new PDU is HIGHER than current PDU's */            \
    ((new_p)->radio_priority < (cur_p)->radio_priority)         \
  )
#else

#define IS_SWAP_REQUIRED(cur_p, new_p)                          \
  (                                                             \
    /* current psn is not the last psn */                       \
    (pg[gas_id].psn > (cur_p)->psn) &&                          \
                                                                \
    /* rlc mode must be different regardless of EXT_UTBF */     \
    ((cur_p)->rlc_mode != (new_p)->rlc_mode) &&                 \
                                                                \
    /* RP of new PDU is HIGHER than current PDU's */            \
    ((new_p)->radio_priority < (cur_p)->radio_priority)         \
  )

#endif /* FEATURE_GPRS_PS_HANDOVER */

/*--------------------------------------------------------------------------
** Extract BSN1 from EGPRS RLC uplink header type 1,2 and 3 (same position)
**
** Expect ptr to point to a uint8 representing an EGPRS RLC uplink header.
** No range checking is performed
**--------------------------------------------------------------------------
*/
#define E_GET_RAW_BSN(ptr) ((uint16)(((((uint8 *)ptr)[1])>>3) & 0x001f)  | \
                            (uint16)(((((uint8 *)ptr)[2])<<5) & 0x07e0)    \
                           )

#define E_GET_RAW_BSN2(ptr) ((uint16)(((((uint8 *)ptr)[2])>>6) & 0x0003)  | \
                             (uint16)(((((uint8 *)ptr)[3])<<2) & 0x03fc)    \
                            )

/*-----------------------------------------------------------------
** Macro to format and send the resource re-allocation req to MAC.
** The mptr is of type: rlc_mac_msg_t*
**-----------------------------------------------------------------
*/
#define SEND_REALLOC_REQ(gas_id, mptr, pdu_ptr, num_octets, cv_status)                  \
  {                                                                                     \
    (mptr)->msg.ul_realloc_req.sig = RM_UL_REALLOC_REQ;                                 \
    (mptr)->msg.ul_realloc_req.gas_id =(gas_id);                                        \
    (mptr)->msg.ul_realloc_req.chan_req_desc.rlc_octet_cnt =(num_octets);               \
    (mptr)->msg.ul_realloc_req.chan_req_desc.peak_thr_class=(pdu_ptr)->peak_throughput; \
    (mptr)->msg.ul_realloc_req.chan_req_desc.radio_priority=(pdu_ptr)->radio_priority;  \
    (mptr)->msg.ul_realloc_req.chan_req_desc.llc_pdu_type = (pdu_ptr)->llc_pdu_type;    \
    (mptr)->msg.ul_realloc_req.chan_req_desc.rlc_mode = pdu_ptr->rlc_mode;              \
    (mptr)->msg.ul_realloc_req.b_cv_zero = (cv_status);                                 \
    ul[gas_id].prev_realloc_psn = (pdu_ptr)->psn;                                       \
    TIMER_START_MAC_UL_RESP(gas_id);                                                    \
    mac_put_cmd_rlc(gas_id, mptr);                                                      \
    cur_pdu_ptr[gas_id]->realloc_action = RA_NULL;                                      \
  }

/*-----------------------------------------------------
** Macro to increment the send state var vs modulo SNS
**-----------------------------------------------------
*/
#define INC_MODSNS(x)        INC_MOD((x),SNS)

#define INC_VS(gas_id)       INC_MODSNS(ul[gas_id].vs)

#define E_INC_VS(gas_id)     INC_MOD((ul[gas_id].vs),ESNS)

/*---------------------------------------------------------------------
** Return the number of extension octets required for an ul rlc block
** given the number of pdus in the ul rlc data queue and etc..
**---------------------------------------------------------------------
*/

/*----------------------------------------------------------------
** Assign global Absolute BSN value to the working data block and
** post-increment the Absolute BSN value
**----------------------------------------------------------------
*/
#define INC_ABS_BSN(gas_id)                    \
        {                                      \
          bp->abs_bsn = ul[gas_id].abs_bsn++;  \
        }


/* Set the block count value to the global grlc_ul_block_counter value plus the
** limit defined by BS_CV_MAX. The check for NACK later on will only involved
** comparing this value with the block counter held by the Packet Uplink Acknack
** message.
** ie. NACK is disregarded if acknack_blk_cnt < (blk_cnt+MAX(BS_CV_MAX,1)-1)
*/
/* Corrected the delta (bs_cv_max - 1) to be (bs_cv_max) as spec indicates the beginning
** of the block period when the PUAN is sent over the air, yet our implementation,
** which does not do partial decoding, mark the PUAN 1 block later (2nd burst)
*/

/* DEBUG_GSM_GPRS_DS_UNIT_TEST
** In host environment we test as per the specs requirement and do not do minus
** one otherwise requires lot of changes in Host Test cases.
*/
#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST

#define SET_BLOCK_COUNT_LIMIT(gas_id, bsn)                                                                \
        {                                                                                                 \
          TX_ARR(gas_id,bsn).blk_cnt = grlc_ul_block_counter[gas_id]+(uint16)(MAX(BS_CV_MAX(gas_id),1));  \
        }

#define E_SET_BLOCK_COUNT_LIMIT(gas_id, bsn)                                                               \
        {                                                                                                  \
          TX_ARR(gas_id,(bsn)).blk_cnt = grlc_ul_block_counter[gas_id]+(uint16)(MAX(BS_CV_MAX(gas_id),1)); \
        }

#else
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

/*-------------------------------------------------------------------------
** Macros to start or re-start a timer. If a timer is already running then
** it is re-started
**-------------------------------------------------------------------------
*/
#ifndef DEBUG_GSM_GPRS_RLC_UL_DISABLE_TIMERS

  #define TIMER_START_T3182(gas_id)                                                            \
          grlc_ul_start_timer_T3182(gas_id)

  #define TIMER_START_AWAIT_UL_ACKNACK(gas_id)                                                                \
          {                                                                                                   \
            (void)rex_set_timer(&(ul[gas_id].timer_await_ul_acknack.rex_timer), RM_TMR_AWAIT_UL_ACKNACK_DUR); \
            ul[gas_id].timer_await_ul_acknack.state = TMR_ACTIVE;                                             \
          }

#else
#error code not present
#endif /* #ifndef DEBUG_GSM_GPRS_RLC_UL_DISABLE_TIMERS */

/*-------------------------------------------------------------------------
** If timeout check feature for MAC and L1 not turned on then do not start
** timer when macros are invoked
**-------------------------------------------------------------------------
*/
#ifdef DEBUG_GSM_GPRS_RLC_UL_MAC_PL1_TIMEOUT_CHECK
#error code not present
#else

  #define TIMER_START_MAC_UL_RESP(gas_id)
  #define TIMER_STOP_MAC_UL_RESP(gas_id)

#endif /* DEBUG_GSM_GPRS_RLC_UL_MAC_PL1_TIMEOUT_CHECK */

/*-----------------------------------------------------------------------
** Macros to stop a timer and make it inactive if it is running (active)
** Note: Macro parameter 'x' refers to the GERAN_DATA_SPACE_INDEX
**-----------------------------------------------------------------------
*/
#define   TIMER_STOP_T3182(gas_id)                                                        \
          {                                                                               \
            if (ul[gas_id].timer_t3182.state == TMR_ACTIVE)                               \
            {	                                                                          \
              (void)rex_clr_timer(&(ul[gas_id].timer_t3182.rex_timer));                   \
              ul[gas_id].timer_t3182.state = TMR_INACTIVE;                                \
            }                                                                             \
          }

#define   TIMER_STOP_AWAIT_UL_ACKNACK(gas_id)                                             \
          {                                                                               \
            if (ul[gas_id].timer_await_ul_acknack.state == TMR_ACTIVE)                    \
            {                                                                             \
              (void)rex_clr_timer(&(ul[gas_id].timer_await_ul_acknack.rex_timer));        \
              ul[gas_id].timer_await_ul_acknack.state = TMR_INACTIVE;                     \
            }                                                                             \
          }

#define   TIMER_START_AWAIT_ASS_GUARD(gas_id)                                        \
          {                                                                          \
            (void)rex_set_timer(&(ul[gas_id].timer_await_ass_guard.rex_timer),       \
                                  RM_TMR_AWAIT_ASS_GUARD_DUR);                       \
            ul[gas_id].timer_await_ass_guard.state = TMR_ACTIVE;                     \
            MSG_GRLC_MED_0_G(GRLC_F3_MSG_ENUM_797, "GRLU start await_ass_guard timer");                   \
          }


#define   TIMER_STOP_AWAIT_ASS_GUARD(gas_id)                                         \
          {                                                                          \
            if (ul[gas_id].timer_await_ass_guard.state == TMR_ACTIVE)                \
            {                                                                        \
              (void)rex_clr_timer(&(ul[gas_id].timer_await_ass_guard.rex_timer));    \
              ul[gas_id].timer_await_ass_guard.state = TMR_INACTIVE;                 \
              MSG_GRLC_MED_0_G(GRLC_F3_MSG_ENUM_798, "GRLU stop await_ass_guard timer");                  \
            }                                                                        \
          }

  #define   TIMER_START_STALE_PDU_DELETE(gas_id)                                     \
            {                                                                        \
              if ( (ul[gas_id].timer_stale_pdu_delete.state == TMR_INACTIVE) &&      \
                   (!ul[gas_id].b_ul_pdu_delete_after_stale_tmr_expiry)              \
                 )                                                                   \
              {                                                                      \
                (void)rex_set_timer(&(ul[gas_id].timer_stale_pdu_delete.rex_timer),  \
                                      RM_TMR_STALE_PDU_DELETE_DUR);                  \
                ul[gas_id].timer_stale_pdu_delete.state = TMR_ACTIVE;                \
                MSG_GRLC_MED_0_G(GRLC_F3_MSG_ENUM_799, "GRLU stale_tmr started");                         \
              }                                                                      \
              else                                                                   \
              {                                                                      \
                MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_800, "GRLU stale_tmr not started, state: %d, flag: %d", \
                         ul[gas_id].timer_stale_pdu_delete.state,                    \
                         ul[gas_id].b_ul_pdu_delete_after_stale_tmr_expiry);         \
              }                                                                      \
            }


  #define   TIMER_STOP_STALE_PDU_DELETE(gas_id)                                      \
            {                                                                        \
              if (ul[gas_id].timer_stale_pdu_delete.state == TMR_ACTIVE)             \
              {                                                                      \
                (void)rex_clr_timer(&(ul[gas_id].timer_stale_pdu_delete.rex_timer)); \
                ul[gas_id].timer_stale_pdu_delete.state = TMR_INACTIVE;              \
                MSG_GRLC_MED_0_G(GRLC_F3_MSG_ENUM_801, "GRLU stale_tmr stopped");                         \
              }                                                                      \
              else                                                                   \
              {                                                                      \
                MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_802, "GRLU stale_tmr already stopped/expired, flag:%d", \
                         ul[gas_id].b_ul_pdu_delete_after_stale_tmr_expiry);         \
              }                                                                      \
            }


/*-------------------------------------------------------------
** Re-transmit block with cv = 0 during Await_final_ack state.
** The TXA_UNACK indicates the un-ack mode operation
**-------------------------------------------------------------
*/
#define SEND_BLOCK_CV_0(gas_id)   (void)send_block_to_pl1_fifo(gas_id, ul[gas_id].bsn_of_cv_0_block, TXA_UNACK)

/*-------------------------------------------------------------
** Re-transmit block with cv = 0 during Await_final_ack state.
** The TXA_UNACK indicates the un-ack mode operation
**-------------------------------------------------------------
*/
#define E_SEND_BLOCK_CV_0(gas_id) (void)e_send_block_to_pl1_fifo(   \
                                      gas_id, ul[gas_id].e_bsn_of_cv_0_block, 0xffff, TXA_UNACK)

/*------------------------------------------------------------------
** Macro to load MAC header with the CV, SI and Retry field values
** bp is of type: grlc_ul_data_block_t*
**------------------------------------------------------------------
*/
#define PUT_UL_MAC_HDR(bp,c,s,r)                    \
        {                                           \
          (bp)->mac_hdr.payload = MAC_PAYLOAD_DATA; \
          (bp)->mac_hdr.type.ul_data.cv = (c);      \
          (bp)->mac_hdr.type.ul_data.si = (s);      \
          (bp)->mac_hdr.type.ul_data.retry = (r);   \
        }

/*------------------------------------------------------------------------------
** Macro to load RLC header Octet-1 with values of the Spare, TFI and TI fields
** bp is of type: grlc_ul_data_block_t*
**------------------------------------------------------------------------------
*/

#define PUT_UL_RLC_HDR_OCTET_1(bp,sp,tf)                                                 \
        {                                                                                \
          (bp)->tfi = (tf);                                                              \
          (bp)->ti = (IS_TLLI_REQUIRED(gas_id) ? RM_TLLI_PRESENT : RM_TLLI_NOT_PRESENT); \
          (bp)->pi = (IS_PFI_REQUIRED(gas_id) ? RM_PFI_PRESENT : RM_PFI_NOT_PRESENT);    \
        }

/*------------------------------------------------------------------------------
** Macro to load RLC header Octet-2 with values of the BSN and EXTension fields
** bp is of type: grlc_ul_data_block_t*
**------------------------------------------------------------------------------
*/
#define PUT_UL_RLC_HDR_OCTET_2(bp,bs,ex)            \
        {                                           \
          (bp)->bsn = (bs);                         \
          ul[gas_id].bsn = (bs);                    \
          (bp)->ext = (ex);                         \
        }

/*-----------------------------------------------------------------
** Macro to initialise the current data block.
**
** Input to this macro are:
** 1. If IS_TLLI_REQUIRED():
**      ul.cur_res.access_phase
**      ul.cur_res.ti == RM_TLLI_PRESENT
**      ul.contention_status
**      cur_pdu_ptr[gas_id]->tlli (passed in by LLC)
** 2. The current coding scheme held in ul.cur_res.cs
**
** Output:
** 1. rem_cnt: size of payload less TLLI size if TLLI is present
** 2. TI and TLLI fields of current block
** scheme in use and whether the TLLI field is present.
**
** bp (points to current block) is of type: grlc_ul_data_block_t*
**-----------------------------------------------------------------
*/
#define NEW_DATA_BLOCK(gas_id, bp)      grlc_ul_init_gprs_data_block(gas_id, bp)

/*-----------------------------------------------------------------
** Macro to initialise the current data block.
**
** Input to this macro are:
** 1. If IS_TLLI_REQUIRED():
**      ul.cur_res.access_phase
**      ul.cur_res.ti == RM_TLLI_PRESENT
**      ul.contention_status
**      cur_pdu_ptr[gas_id]->tlli (passed in by LLC)
** 2. The current coding scheme held in ul.cur_res.cs
**
** Output:
** 1. rem_cnt: size of payload less TLLI size if TLLI is present
** 2. TI and TLLI fields of current block
** scheme in use and whether the TLLI field is present.
**
** bp (points to current block) is of type: grlc_ul_data_block_t*
**-----------------------------------------------------------------
*/
#define E_NEW_DATA_BLOCK(gas_id,bp)  grlc_ul_init_egprs_data_block(gas_id,bp)

#define IS_E_DATA_BLOCK_PRECISE_FIT(gas_id,bsn)                    \
        (                                                          \
          TX_ARR(gas_id,(bsn)).blk.num_ext_octets >= 1 &&          \
          TX_ARR(gas_id,(bsn)).blk.ext == RM_EXT_OCTET_FOLLOWS &&  \
          TX_ARR(gas_id,(bsn)).blk.ext_octet[0].egprs.li == 0      \
        )

#define E_RESTORE_DATA_BLOCK_PRECISE_FIT(gas_id)                              \
        {                                                                     \
          cur_blk[gas_id].num_ext_octets = 1;                                 \
          cur_blk[gas_id].ext = RM_EXT_OCTET_FOLLOWS;                         \
          cur_blk[gas_id].ext_octet[0].egprs.ext = RM_EXT_NO_OCTET_FOLLOWS;   \
          cur_blk[gas_id].ext_octet[0].egprs.li = 0;                          \
          cur_blk[gas_id].rem_cnt--;                                          \
          ul[gas_id].b_li_0_exists = TRUE;                                    \
        }

#define DEC_MOD_PS(mcs,ps)                                      \
    {                                                           \
      if( (mcs < RM_MCS_3)   ||                                 \
          ( (mcs > RM_MCS_4) && (mcs < RM_MCS_7) ) )            \
      {                                                         \
        DEC_MOD( (ps), (GRLC_PS_2 + 1) );                       \
      }                                                         \
      else                                                      \
      {                                                         \
        DEC_MOD( (ps), (GRLC_PS_3 + 1) );                       \
      }                                                         \
    }

/*-----------------------------------------------------------------------
** Macro to return the new PS for block re-tx after MCS change.
** new_ps indicates the new pancturing scheme. the See table 9.3.2.1.1
**-----------------------------------------------------------------------
*/
#define ROTATE_PS_MCS_CHANGE( old_mcs, new_mcs, old_ps, p_new_ps )  \
        grlc_ul_rotate_ps_mcs_change(old_mcs, new_mcs, old_ps, p_new_ps)


/*-----------------------------------------------------------------------
** Macro to return the new PS for data block init-tx/re-tx without MCS change.
** Note that MSC 1/2/5/6 only use PS1 and PS2.
** new_ps indicates the new puncturing scheme.
**-----------------------------------------------------------------------
*/
#define ROTATE_PS_NO_MCS_CHANGE( mcs, old_ps, new_ps )      \
        {                                                   \
          switch(mcs)                                       \
          {                                                 \
            case RM_MCS_1:                                  \
            case RM_MCS_2:                                  \
            case RM_MCS_5:                                  \
            case RM_MCS_6:                                  \
              (new_ps) = ((old_ps) + 1) % (GRLC_PS_2 + 1);  \
              if ( new_ps > GRLC_PS_2 )                     \
              {                                             \
                ERR_FATAL("ERLU Old PS %d, New PS %d, MCS %d",old_ps,new_ps,mcs); \
              }                                             \
              break;                                        \
            case RM_MCS_3:                                  \
            case RM_MCS_4:                                  \
            case RM_MCS_7:                                  \
            case RM_MCS_8:                                  \
            case RM_MCS_9:                                  \
              (new_ps) = ((old_ps) + 1) % (GRLC_PS_3 + 1);  \
              if ( new_ps > GRLC_PS_3 )                     \
              {                                             \
                ERR_FATAL("ERLU Old PS %d, New PS %d, MCS %d",old_ps,new_ps,mcs); \
              }                                             \
              break;                                        \
            default:                                        \
              MSG_GRLC_ERROR_1(GRLC_F3_MSG_ENUM_803, "GRLU Invalid MCS %d",(mcs));   \
              break;                                        \
          }                                                 \
        }


#define E_GET_RAW_CPS_HDR2(ptr) ((uint16)(((((uint8 *)ptr)[2])>>6) & 0x0003) | \
                                 (uint16)(((((uint8 *)ptr)[3])<<2) & 0x0004)   \
                                )

/*-----------------------------------------------------------------------
** Macros to return the CPS for the three header types.
**-----------------------------------------------------------------------
*/
#define GET_CPS(gas_id, frst_bsn, scnd_bsn, cps )                 \
        {                                                         \
          if( TX_ARR(gas_id,frst_bsn).blk.cs <= RM_MCS_4 )        \
          {                                                       \
            GET_CPS_HDR_3(gas_id, (frst_bsn), (cps) );            \
          }                                                       \
          else if( TX_ARR(gas_id,frst_bsn).blk.cs <= RM_MCS_6 )   \
          {                                                       \
            GET_CPS_HDR_2(gas_id, (frst_bsn), (cps) );            \
          }                                                       \
          else if( TX_ARR(gas_id,frst_bsn).blk.cs <= RM_MCS_9 )   \
          {                                                       \
            if( (scnd_bsn) == 0xffff )                            \
            {                                                     \
              MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_804, "GRLU Invalid MCS %d or 2nd BSN %d",  \
               TX_ARR(gas_id,frst_bsn).blk.cs, (scnd_bsn));       \
              (scnd_bsn) = (frst_bsn);                            \
            }                                                     \
            GET_CPS_HDR_1(gas_id, (frst_bsn), (scnd_bsn), (cps) );\
          }                                                       \
          else                                                    \
          {                                                       \
            MSG_GRLC_ERROR_1_G(GRLC_F3_MSG_ENUM_805, "GRLU Invalid MCS %d",            \
                TX_ARR(gas_id,frst_bsn).blk.cs);                  \
          }                                                       \
        }


/* Uses table E_GRLC_HDR1_CPS[] to index the right CPS value.
*/
#define GET_CPS_HDR_1(gas_id, frst_bsn, scnd_bsn, cps )                            \
        {                                                                          \
          grlc_ul_data_block_t *tx_blk_frst_bsn_p = &TX_ARR(gas_id, frst_bsn).blk; \
          grlc_ul_data_block_t *tx_blk_scnd_bsn_p = &TX_ARR(gas_id, scnd_bsn).blk; \
                                                                                   \
          if( (tx_blk_frst_bsn_p->cs  <= RM_MCS_9)  &&                             \
              (tx_blk_frst_bsn_p->cs  >= RM_MCS_7)  &&                             \
              (tx_blk_frst_bsn_p->ps  <= GRLC_PS_3) &&                             \
              (tx_blk_scnd_bsn_p->ps  <= GRLC_PS_3)                                \
            )                                                                      \
          {                                                                        \
            (cps) = E_GRLC_HDR1_CPS                                                \
                    [                                                              \
                      ((RM_MCS_9 - tx_blk_frst_bsn_p->cs) * 9) +                   \
                      (3 * tx_blk_frst_bsn_p->ps) + tx_blk_scnd_bsn_p->ps          \
                    ];                                                             \
         }                                                                         \
         else                                                                      \
         {                                                                         \
           MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_806, "GRLU Invalid MCS %d or 1st/2nd PS %d/%d ",         \
             tx_blk_frst_bsn_p->cs,tx_blk_frst_bsn_p->ps,tx_blk_scnd_bsn_p->ps);   \
           ERR_GERAN_FATAL_0_G("GRLU Invalid MCS or PS");                          \
         }                                                                         \
        }

/* Uses table E_GRLC_HDR2_CPS[] to index the right CPS value.
*/                                      
#define GET_CPS_HDR_2( gas_id, bsn, cps )                                                              \
        {                                                                                              \
          grlc_ul_data_block_t *tx_blk_p = &TX_ARR(gas_id, bsn).blk;                                   \
          if (((tx_blk_p->cs == RM_MCS_6)  || (tx_blk_p->cs == RM_MCS_5)) &&                           \
              (tx_blk_p->ps <= GRLC_PS_2))                                                             \
          {                                                                                            \
            if ((tx_blk_p->cs == RM_MCS_6) && (tx_blk_p->init_mcs == RM_MCS_8))                        \
            {                                                                                          \
              (cps) = E_GRLC_HDR2_CPS[MCS_6_P1_PAD_MSC_8_RETX + tx_blk_p->ps];                         \
            }                                                                                          \
            else                                                                                       \
            {                                                                                          \
              (cps) = E_GRLC_HDR2_CPS[((RM_MCS_6 - tx_blk_p->cs) * 4) + tx_blk_p->ps];                 \
            }                                                                                          \
          }                                                                                            \
          else                                                                                         \
          {                                                                                            \
            ROTATE_PS_MCS_CHANGE(ul[gas_id].cur_res.cs, tx_blk_p->cs, tx_blk_p->ps, &(tx_blk_p->ps));  \
            MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_807, "EGRLU cmd_mcs %d, new_mcs %d force PS to %d",                           \
              ul[gas_id].cur_res.cs, tx_blk_p->cs, tx_blk_p->ps);                                      \
          }                                                                                            \
        }

/* Uses table E_GRLC_HDR3_CPS[] to index the right CPS value.
*/
#define GET_CPS_HDR_3( gas_id, bsn, cps )                                             \
        {                                                                             \
          grlc_ul_data_block_t *tx_blk_p = &TX_ARR(gas_id, bsn).blk;                  \
          if (((tx_blk_p->cs == RM_MCS_3) || (tx_blk_p->cs == RM_MCS_4)) &&           \
              (tx_blk_p->ps  <= GRLC_PS_3))                                           \
          {                                                                           \
            if ((tx_blk_p->cs == RM_MCS_3) && (tx_blk_p->init_mcs == RM_MCS_8))       \
            {                                                                         \
              (cps) = E_GRLC_HDR3_CPS[MCS_3_P1_PAD_MCS_8_RETX + tx_blk_p->ps];        \
            }                                                                         \
            else                                                                      \
            {  /* MCS-3 (!init MCS-8) or MCS-4 */                                     \
              (cps) = E_GRLC_HDR3_CPS[((RM_MCS_4 - tx_blk_p->cs) * 3) + tx_blk_p->ps];\
            }                                                                         \
          }                                                                           \
          else if ((tx_blk_p->cs < RM_MCS_3) && (tx_blk_p->ps < GRLC_PS_3))           \
          {                                                                           \
            if (tx_blk_p->cs == RM_MCS_2)                                             \
              (cps) = E_GRLC_HDR3_CPS[ MCS_2_P1 + tx_blk_p->ps];                      \
            else                                                                      \
              (cps) = E_GRLC_HDR3_CPS[ MCS_1_P1 + tx_blk_p->ps];                      \
          }                                                                           \
          else                                                                        \
          {                                                                           \
            MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_808, "GRLU Invalid MCS %d or PS %d",  \
              tx_blk_p->cs,tx_blk_p->ps);                                   \
            ERR_GERAN_FATAL_0_G("GRLU Invalid MCS or PS");                            \
          }                                                                           \
        }

/*--------------------------------------------------------------------------
** Padding is required when the initial MCS is MCS-8 and the derived MCS is
** MCS-3 or MCS-6
**--------------------------------------------------------------------------
*/
#define IS_PADDING_REQUIRED(init_mcs, new_mcs)                          \
        ((init_mcs)==RM_MCS_8 &&                                        \
         ((new_mcs)==RM_MCS_6 || (new_mcs)==RM_MCS_3)                   \
        )

/*-----------------------------------------------------------------------------
** Splitting block is required if the Resegment bit is set and the initial MCS
** and commanded MCS falls into the following combinations:
**
** MCS_9 or MCS_6 into MCS_3 OR
** MCS_8 or MCS_6 into MCS_3 OR
** MCS_7 or MCS_5 into MCS_2 OR
** MCS_4 into MCS_1
**-----------------------------------------------------------------------------
*/
#define IS_SPLIT_REQUIRED(init_mcs, new_mcs, resegment)                               \
        ((resegment) &&                                                               \
         ((((init_mcs)==RM_MCS_9 || (init_mcs)==RM_MCS_6) && (new_mcs)==RM_MCS_3) ||  \
          (((init_mcs)==RM_MCS_8 || (init_mcs)==RM_MCS_6) && (new_mcs)==RM_MCS_3) ||  \
          (((init_mcs)==RM_MCS_7 || (init_mcs)==RM_MCS_5) && (new_mcs)==RM_MCS_2) ||  \
          ((                        (init_mcs)==RM_MCS_4) && (new_mcs)==RM_MCS_1)     \
         )                                                                            \
        )

/*---------------------------------------------------------------------
** Evaluate to TRUE if initial MCS is promoted into a dual-payload MCS
**---------------------------------------------------------------------
*/
#define IS_FORMING_DUAL_PAYLOAD_REQUIRED(new_mcs)   \
                      ( (new_mcs)==RM_MCS_7 ||      \
                        (new_mcs)==RM_MCS_8 ||      \
                        (new_mcs)==RM_MCS_9         \
                      )

/*------------------------------------------------------------------------------
** Macro to obtain the derived MCS to use given the value of the Resegment bit,
** initial MCS and the new commanded MCS, using tables 8.1.1.1/2 of TS 04.60
**------------------------------------------------------------------------------
*/
#define GET_MCS_FOR_RETX(mcs,cmd_mcs,resegment)                                     \
        ( (resegment) ?                                                             \
          (E_GRLC_MCS_RESEGMENT_SET[(RM_MCS_9 - mcs)][(RM_MCS_6_9 - cmd_mcs)]) :    \
          (E_GRLC_MCS_RESEGMENT_CLR[(RM_MCS_9 - mcs)][(RM_MCS_6_9 - cmd_mcs)])      \
        )

/*------------------------------------------------------------------------------
** Macro to obtain the MCS to use for initial transmission given the value of the
** commanded MCS i.e. ul.mcs
**------------------------------------------------------------------------------
*/
#define SET_MCS_FOR_TX(mcs, cmd_mcs)          \
        {                                     \
          if( cmd_mcs == RM_MCS_5_7 )         \
            (mcs) = RM_MCS_5;                 \
          else if( (cmd_mcs) == RM_MCS_6_9 )  \
            (mcs) = RM_MCS_6;                 \
          else                                \
            (mcs) = (cmd_mcs);                \
        }

/*------------------------------------------------------------------------------
** Macro to downgrade a dual payload MCS i.e. MCS-9/8/7 to MCS-5 or MCS-6 when
** there are no enough blocks in the Tx array to form a dual payload block
** during original transmission or re-transmission. See TS 04.60 clause 8.1.1
**------------------------------------------------------------------------------
*/
#define DEMOTE_MCS_FOR_TX( mcs )                      \
  {                                                   \
    if( ((mcs) == RM_MCS_9) || ((mcs) == RM_MCS_8) )  \
    {                                                 \
      (mcs) = RM_MCS_6;                               \
    }                                                 \
    else if ( (mcs) == RM_MCS_7 )                     \
    {                                                 \
      (mcs) = RM_MCS_5;                               \
    }                                                 \
  }


/*----------------------------------------------------------------------
** Macro to return the size of the transmit window defined by the
** distance between V(A) and V(S) taking into account wrapround effect.
**----------------------------------------------------------------------
*/
#define GET_TX_WINDOW_SIZE(gas_id)    MOD_DIF(ul[gas_id].vs, ul[gas_id].va, SNS)
#define GET_E_TX_WINDOW_SIZE(gas_id)  MOD_DIF_ESNS(ul[gas_id].vs, ul[gas_id].va)

/*-------------------------------------------------------------
** Macro to return TRUE if the transmit window is full (stall)
**-------------------------------------------------------------
*/
#define IS_TX_WINDOW_FULL(gas_id)  IS_GE_MODULO_COMP(ul[gas_id].vs, (ul[gas_id].va+RM_RLC_WINDOW_SIZE), SNS)


/*-------------------------------------------------------------------------
** Macros to set the global TBF_MODE variable ul.cur_res.tbf_mode to GPRS mode and
** EGPRS mode
**-------------------------------------------------------------------------
*/
#define SET_TBF_MODE_EGPRS(gas_id)                                 \
        {                                                          \
          ul[gas_id].cur_res.tbf_mode = TBF_MODE_EGPRS;            \
          grlc_ul_fifo[gas_id].tbf_mode = TBF_MODE_EGPRS;          \
          MSG_GRLC_MED_0_G(GRLC_F3_MSG_ENUM_809, "ERLU GPRS to EGPRS");                 \
        }

#define SET_TBF_MODE_GPRS(gas_id)                                  \
        {                                                          \
          ul[gas_id].cur_res.tbf_mode = TBF_MODE_GPRS;             \
          grlc_ul_fifo[gas_id].tbf_mode = TBF_MODE_GPRS;           \
          MSG_GRLC_MED_0_G(GRLC_F3_MSG_ENUM_810, "ERLU EGPRS to GPRS");                 \
        }

/*-------------------------------------------------------------
** Macro to return TRUE if the transmit window is full (stall)
**-------------------------------------------------------------
*/
#define IS_E_TX_WINDOW_FULL(gas_id)	(MOD_DIF_ESNS(ul[gas_id].vs, ul[gas_id].va) >= ul[gas_id].cur_res.ws)

/*--------------------------------------------------------------------------
** Macro to return the size of the attach window, defined by the distance
** between V(A) and the element where the next radio block will be attached
** to in the transmit array.
**--------------------------------------------------------------------------
*/
#define GET_ATTACH_WINDOW_SIZE(gas_id)      MOD_DIF(ul[gas_id].seg_ind, ul[gas_id].va, SNS)

/*-----------------------------------------------------------------------
** The unack tx window starts from V(S) and ends just before seg_ind.
** V(S) indexes the next block to be transmitted. seg_ind indexes block
** where the next segmented block is to be attached
**-----------------------------------------------------------------------
*/
#define GET_UNACK_TX_WINDOW_SIZE(gas_id)    MOD_DIF(ul[gas_id].seg_ind, ul[gas_id].vs, SNS)

#define GET_UNACK_TX_WINDOW_SPACE(gas_id)   (RM_RLC_WINDOW_SIZE - GET_UNACK_TX_WINDOW_SIZE(gas_id))

/*-------------------------------------------------------------
** Macro to return the remaining space in the attach window.
** Limit the attach window to RM_RLC_WINDOW_SIZE which is 64.
** Increasing this value simply lets the segmentation runs too
** far ahead of the transmission.
**-------------------------------------------------------------
*/
#define GET_ATTACH_WINDOW_SPACE(gas_id)	(uint8)(RM_RLC_WINDOW_SIZE - GET_ATTACH_WINDOW_SIZE(gas_id))

#define E_GET_ATTACH_WINDOW_SPACE(gas_id)	(ul[gas_id].cur_res.ws - MOD_DIF_ESNS(ul[gas_id].seg_ind, ul[gas_id].va))

#define E_GET_UNACK_WINDOW_SPACE(gas_id)	(ul[gas_id].cur_res.ws - MOD_DIF_ESNS(ul[gas_id].seg_ind, ul[gas_id].vs))

/*-----------------------------------------------------------------------
** Macro to return TRUE when all pending tx blocks have been transmitted
** ie. segmentation is complete and the attach window is empty. There
** might still be un-ack blocks around !
**-----------------------------------------------------------------------
*/
#define IS_TX_COMPLETE(gas_id)							\
          (ul[gas_id].segment_state == SEGMENT_DONE &&	\
           ul[gas_id].vs == ul[gas_id].seg_ind			\
          )

/*-------------------------------------------------------------------
** Macro to determine if a bsn is valid according to the inequality:
** V(A) <= BSN < V(S) as per 04.60 9.1.8. This checks if the bsn
** value falls in between the V(A) and (VS).
**
** To make use of the IS_GT..() and IS_GE..() macros, the expression
** above becomes:
**
** BSN >= V(A) and V(S) > BSN
**
** Note that this macro does not check the distance between V(S) and
** V(A), ie. if the windows size k is exceeded or not.
**-------------------------------------------------------------------
*/
#define IS_UL_BSN_VALID(bsn,vs,va)              \
        (                                       \
          IS_GE_MODULO_COMP((bsn),(va),SNS) &&  \
          IS_GE_MODULO_COMP((vs),(bsn),SNS)     \
        )


/*------------------------------------------------------------------------
** Macro to determine if a bsn is valid according to the inequality:
** V(A) <= BSN < V(S) as per 04.60 9.1.8.2. This checks if the bsn
** value falls in between the V(A) and (VS).
**
** To make use of the IS_GT..() and IS_GE..() macros, the expression
** above becomes:
**
** BSN >= V(A) and V(S) > BSN
**
** Note that this macro does not check the distance between V(S) and
** V(A), ie. if the windows size WS is exceeded or not.
** See EGPRS RLC LLD.
** This macro depends on the following vars: ul.va, ul.vs, ul.cur_res.ws and ESNS
**------------------------------------------------------------------------
*/
#define IS_UL_BSN_VALID_IN_WINDOW(gas_id,bsn)                                             \
          (IS_GE_MOD((bsn),(ul[gas_id].va),(ul[gas_id].cur_res.ws)) &&                    \
            IS_GE_MOD((MOD_ESNS(ul[gas_id].vs + ESNS -1)),(bsn),(ul[gas_id].cur_res.ws)))


/*--------------------------------------------------------------
** Macro to return TRUE if the TLLI field needs to be included
** in the current data block.
**--------------------------------------------------------------
*/
#define IS_TLLI_REQUIRED(gas_id)                                        \
          (ul[gas_id].cur_res.access_phase == GMAC_ONE_PHASE_ACCESS &&  \
           ul[gas_id].cur_res.ti == RM_TLLI_PRESENT &&                  \
           ul[gas_id].contention_status == RM_CONTENTION_UNRESOLVED     \
          )

/*------------------------------------------------------------------
** Macro to return TRUE if next pdu PFI differs from current pdu PFI.
**------------------------------------------------------------------
*/
#define IS_NEXT_PDU_PFI_DIFF_TO_CURRENT(gas_id,bp)                                          \
        (                                                                                   \
          ((bp)->pi   == RM_PFI_PRESENT) &&                                                 \
          (((bp)->pfi != ul[gas_id].pfi) && (ul[gas_id].pfi != RM_PFI_INVALID))  &&         \
          ((next_pdu_ptr[gas_id] != NULL) && (next_pdu_ptr[gas_id]->pfi == ul[gas_id].pfi)) \
        )

#define IS_NEXT_EGPRS_PDU_PFI_DIFF_TO_CURRENT(gas_id,bp)                                    \
        (                                                                                   \
          ((bp)->hdr.pi == RM_PFI_PRESENT)  &&                                              \
          (((bp)->pfi != ul[gas_id].pfi) && (ul[gas_id].pfi != RM_PFI_INVALID)) &&          \
          ((next_pdu_ptr[gas_id] != NULL) && (next_pdu_ptr[gas_id]->pfi == ul[gas_id].pfi)) \
        )

/*--------------------------------------------------------------
** Macro to return TRUE if the PFI field needs to be included
** in the current data block.
**--------------------------------------------------------------
*/
#define IS_PFI_REQUIRED(gas_id)                                     \
        (                                                           \
          IS_TLLI_REQUIRED(gas_id) && IS_PFC_NW_SUPPORTED(gas_id)   \
        )

/*-------------------------------------------------
** Macros to test for the current allocation modes
**-------------------------------------------------
*/

#define IS_DYNAMIC_OPEN(gas_id)                                   \
        (                                                         \
          (ul[gas_id].cur_res.access_mode == RM_DYNAMIC_ALLOC ||  \
           ul[gas_id].cur_res.access_mode == RM_EXT_DYNAMIC_ALLOC \
          ) &&                                                    \
          ul[gas_id].cur_res.tbf_type == RM_OPEN_ENDED_TBF        \
        )

#define IS_DYNAMIC_CLOSE(gas_id) ((ul[gas_id].cur_res.access_mode == RM_DYNAMIC_ALLOC ||  \
                                ul[gas_id].cur_res.access_mode == RM_EXT_DYNAMIC_ALLOC    \
                               ) &&                                                       \
                               ul[gas_id].cur_res.tbf_type == RM_CLOSE_ENDED_TBF          \
                              )

/*------------------------------------------------------------
** Macro to return the number of free data blocks in L1 FIFO
**------------------------------------------------------------
*/
#define GET_PL1_FREE_SPACE(gas_id)	(uint8)(RLC_PL1_UL_TOTAL_FIFO_SIZE - grlc_ul_fifo_get_cnt(gas_id))


/*============================================
**  definitions and macros for N3102 counter
**============================================
*/
/*---------------------------------
** Define n3102 counter parameters
**---------------------------------
*/
#define PAN_INC(gas_id)	sysinfo[gas_id].pan_inc
#define PAN_DEC(gas_id)	sysinfo[gas_id].pan_dec
#define PAN_MAX(gas_id)	sysinfo[gas_id].pan_max

/*------------------------------------------------------------------------
** If any of the below parameters are 0 then N3102 operation is disabled
**------------------------------------------------------------------------
*/
#define IS_N3102_ENABLED(gas_id)		(PAN_INC(gas_id) != 0 && PAN_DEC(gas_id) != 0)

/*-------------------------------------------------------------------------
** Macro to decode the PAN_MAX parameter into a value to complare with the
** the n3102 counter. PAN_MAX parameter is a 3-bit coded value from 0
** to 7 representing a pan_max_value of 4 to 32 in step of 4, as follows:
**
** PAN_MAX pan_max_value
**  0       4
**  1       8
**  2      12
**  3      16
**  4      20
**  5      24
**  6      28
**  7      32
** ie. pan_max_value = 4 * (PAN_MAX+1)
**-------------------------------------------------------------------------
*/
#define DECODE_PAN_MAX(gas_id)    { ul[gas_id].pan_max_value = (int8)(4*(PAN_MAX(gas_id)+1));  }

/*--------------------------------------------------------------------------
** Macro to set the n3102 counter to PAN_MAX at each cell-reselection event
**--------------------------------------------------------------------------
*/
#define SET_N3102(gas_id)                               \
        {                                               \
          DECODE_PAN_MAX(gas_id);                       \
          ul[gas_id].n3102 = ul[gas_id].pan_max_value;  \
          UPD_UL_N3102(gas_id);                         \
        }

/*------------------------------------------------------------------------
** Macro to increment n3102 by PAN_INC if N3102 is enabled AND the result
** will not exceed pan_max_value
**------------------------------------------------------------------------
*/
#define INC_N3102(gas_id)                                                     \
        {                                                                     \
          if (IS_N3102_ENABLED(gas_id) &&                                     \
              (ul[gas_id].n3102+PAN_INC(gas_id)) <= ul[gas_id].pan_max_value  \
             )                                                                \
          {                                                                   \
            ul[gas_id].n3102 += (int8)PAN_INC(gas_id);                        \
            UPD_UL_N3102(gas_id);                                             \
          }                                                                   \
        }

/*------------------------------------------------------------------
** Macro to decrement n3102 by PAN_DEC. Check to ensure it does not
** underflow and roll over to positive value
**------------------------------------------------------------------
*/
#define DEC_N3102(gas_id)                              \
        {                                              \
          if (ul[gas_id].n3102 > 0)                    \
          {                                            \
            ul[gas_id].n3102 -= (int8)PAN_DEC(gas_id); \
            UPD_UL_N3102(gas_id);                      \
          }                                            \
        }

/*--------------------------------------------------------------
** Macro to test if the n3102 has reached 0 or a negative value
**--------------------------------------------------------------
*/
#define IS_N3102_EXPIRED(gas_id) (ul[gas_id].n3102 <= 0)

/*====================================================================
** Definitions and macros for N3104 counter
**
** During one phase access RLC:
** - sets n3104 to 1 when sending the first block, if contention has
** not been resolved.
** - increments n3104 on every block sent when contention has not
** been resolved
** - sends RM_UL_N3104_EXPIRY_IND when n3104 reaches its max value.
** n3104 is reset to 1. Suspends data transmission whilst MAC retries
** 4 times.
**
**====================================================================
*/
/*-------------------------------
** N3104_MAX as defined in 13.3
**-------------------------------
*/
#define N3104_MAX(gas_id)	(3 * (BS_CV_MAX(gas_id) + 3) * ul[gas_id].cur_res.num_timeslots)

/* Has n3104 reached its maximum defined value
*/
#define IS_N3104_EXPIRED(gas_id)		(ul[gas_id].n3104 >= N3104_MAX(gas_id))

/* When contention resolution starts or when it fails the n3104 counter is
** reset.
*/
#define RESET_N3104(gas_id)            \
        {                              \
          ul[gas_id].n3104 = 0;        \
          UPD_UL_N3104(gas_id);        \
        }

/* Increment after a data block (with TLLI) is transmitted. This should only
** be called when IS_TLLI_REQUIRED() returns TRUE
*/
#define INC_N3104(gas_id)              \
        {                              \
          ul[gas_id].n3104++;          \
          UPD_UL_N3104(gas_id);        \
		}

/*----------------------------------------------------------------------
** Macros to transmit a number of data blocks in GPRS and EGPRS ack mode.
** The number of blocks is the minimum value of the free space in the
** L1 FIFO, or the blocks remained in the current PDU or the remained
**  resource granted,which ever is the smallest.
**----------------------------------------------------------------------
*/
#define TX_BLKS(gas_id)        tx_blks_shell(gas_id)

/*------------------------------------------------------------------------
** Macros to extract fields in RLC data block in raw/over-the-air format.
** These are used in place of the full xlate..() functions.
**------------------------------------------------------------------------
*/
#define GET_RAW_BSN(ptr)  ((((uint8 *)ptr)[2] >> 1) & 0x7f)
#define GET_RAW_CV(ptr)   ((((uint8 *)ptr)[0] >> 2) & 0x0f)

/* Diagnostic related macros
** gas_id: the GERAN DATA SPACE INDEX
*/
#ifndef DEBUG_GSM_GPRS_RLC_TEST
  #define UPD_UL_STATE(gas_id)    {ul_diag_stats_st[gas_id].grlc_ul_state = (uint32)ul[gas_id].state;            \
                                (void)rlc_log_ul_statistics(gas_id, FALSE);}
  #define UPD_UL_SUBSTATE(gas_id) {ul_diag_stats_st[gas_id].grlc_ul_substate = (uint32)ul[gas_id].realloc_state; \
                                (void)rlc_log_ul_statistics(gas_id, FALSE);}
#else
  #error code not present
#endif

#define UPD_UL_VA(gas_id)			ul_diag_stats_st[gas_id].ul_seq_va = (uint8)ul[gas_id].va;
#define UPD_UL_VS(gas_id)			ul_diag_stats_st[gas_id].ul_seq_vs = (uint8)ul[gas_id].vs;
#define UPD_UL_NSEG(gas_id)		ul_diag_stats_st[gas_id].ul_seq_nseg = (uint8)ul[gas_id].seg_ind;

#define UPD_UL_VSF(gas_id)			ul_diag_stats_st[gas_id].ul_seq_vsf = (uint8)ul[gas_id].vsf;
#define UPD_UL_PACK_VSF(gas_id)	ul_diag_stats_st[gas_id].ul_seq_pack_vsf = (uint8)ul[gas_id].pack_vsf;
#define UPD_UL_NACK_VSF(gas_id)	ul_diag_stats_st[gas_id].ul_seq_nack_vsf = (uint8)ul[gas_id].nack_vsf;

#define UPD_UL_N3102(gas_id)		ul_diag_stats_st[gas_id].ul_n3102 = (int8)ul[gas_id].n3102;
#define UPD_UL_N3104(gas_id)		ul_diag_stats_st[gas_id].ul_n3104 = (int8)ul[gas_id].n3104;

#endif /* INC_GRLCULTYPES_H */

/*** EOF: don't remove! ***/
