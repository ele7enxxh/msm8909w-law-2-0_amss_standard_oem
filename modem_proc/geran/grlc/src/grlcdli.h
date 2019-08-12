/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC DOWNLINK INTERNAL HEADER
***
***
*** DESCRIPTION
***
***  This module contains internal definitions common to the downlink module
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcdli.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 11/20/03    hv     Created
***
*****************************************************************************/

#ifndef INC_GRLCDLI_H
#define INC_GRLCDLI_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "gprs_pdu.h"
#include "grlci.h"
#include "gmacrlc.h"
#include "grlcmac.h"

#include "grlcl1.h"
#include "grlcdlfifo.h"
#include "geran_multi_sim.h"


/*-------------------------------------------------
** RLC More & Extension bits interpreted together
**-------------------------------------------------
*/
#define RM_ME_IGNORE                                    0x00
#define RM_ME_NO_PDU_FOLLOWS_CURRENT_NO_EXT_FOLLOWS_01  0x01  /* 01 */
#define RM_ME_PDU_FOLLOWS_EXT_FOLLOWS_10                0x02  /* 10 */
#define RM_ME_PDU_FOLLOWS_NO_EXT_FOLLOWS_11             0x03  /* 11 */

/* maximum length of llc pdu. Any octet received beyond this limit and until
** the next llc pdu boundary shall be discarded. 
** 04.60 v 6.9.0, 9.1.11
*/
#define  GRLC_DL_PDU_MAX_LEN      1560

/*------------------------------------------------------------------------
** Current downlink pdu states.
** cur_dl_pdu is used as a temporary struct which holds the pdu currently 
** being assembled. This pdu is, at times half assembled, waiting for the
** next data block to be appended. Its current state is reflected by the 
** states described.
**------------------------------------------------------------------------
*/
typedef enum
{  
  /* un-initialised downlink pdu struct
  */
  DL_PDU_STATUS_UNINIT,

  /* init (with ack/-unack mode) and ready to receive a data block.
  ** May contain 0 or more data blocks.
  */
  DL_PDU_STATUS_INIT

} dl_pdu_status_t;


/*----------------------
** Downlink pdu struct
**----------------------
*/
typedef struct
{ 
  /* GERAN Data Space Index
  */
  gas_id_t          gas_id;

  /* pdu status
  */
  dl_pdu_status_t   status;

  /* pdu length as the total number of data octets held by dsm item
  ** pointed to by dsm_ptr.
  */
  uint16            len;

  /* pointer to dsm item
  */
  dsm_item_type     *dsm_ptr;

  /* first and last BSN values of the PDU
  */
  uint16            first_bsn;
  uint16            last_bsn;

  /* rlc mode: ack or unack
  */
  uint8             rlc_mode;

} grlc_dl_pdu_t;

/*------------------------------------------------
** values returned by the apppend_pdu() function
**------------------------------------------------
*/
typedef enum
{
  /* octets appended to pdu without exceeding the length limit
  */
  APPEND_PDU_OK,

  /* pdu not able to take the specified number of octets. Take enough
  ** octets to fill pdu and ignore the rest.
  */
  APPEND_PDU_TRUNCATED
                
} append_pdu_t;

/*---------------------------------------------------------------------
** enumeration of downlink events type
** When the master signal is set, the event handler determines the
** origin of a signal, one at a time, and sets the event variable
** accordingly before calling the dl state machine. The state machine
** then drops into its current state and the event variable is
** used to identify and respond to the event.
**
** The event is set to DE_NULL again after the event is serviced.
**---------------------------------------------------------------------
*/
typedef enum
{
  /* No event or invalid event
  */
  DE_NULL,

  /* the normal events downlink events are signals from L1, MAC and
  ** timer services.
  */
  DE_PL1,
  DE_MAC,
  DE_TMR

} dl_event_t;

/*------------------
** Downlink States
**------------------
*/
typedef enum
{
  DS_NULL,                      /* not in receive mode */
  DS_RX_ACK,                    /* ready to receive blocks in ack mode */
  DS_RX_UNACK,                  /* ready to receive blocks in un-ack mode */

  DS_RX_ACK_PENDING_RELEASE,    /* got all blocks, FBI. Sent FAI. Await release */
  DS_RX_UNACK_PENDING_RELEASE ,  /* ! */
  DS_TEST_MODE                  /* ETSI Test Mode B abd SRB */

} dl_state_t;


/*----------------------------------------------------------
** Structure for storing DL Block Polling information
**----------------------------------------------------------
*/

typedef struct
{
  uint8          rrbp;
  uint32         frame_number;
  uint8          time_slot;

} dl_poll_t;


/*------------------------------
** RLC Downlink Data Structure
**------------------------------
*/
typedef struct
{
  /* Determines if MS in GPRS or EGPRS mode
  */
  tbf_mode_T    tbf_mode;

  /* EGPRS Window Size */
  uint16        ws;

  /* current state of rlc downlink state machine
  */
  dl_state_t                state;

  /* Identify where the message comes from. LLC, MAC, L1 or TIMER.
  */
  dl_event_t                event;

  /* V(R) Receive state variable
  **
  ** Denotes the end of the receiver window and hence holds the BSN value of 
  ** the next block to be received.
  ** 
  ** In ack mode V(R) is set to 1+BSN' -- where BSN' is the most recently 
  ** received block -- if the block is error free and the BSN's is still 
  ** within the k window size.
  ** 
  ** In un-ack mode, V(R) is set to 1+BSN'.
  */
  uint16                    vr;

#ifdef FEATURE_GSM_EGPRS_IR_WS_REDUCTION
   /*
   ** set during each start of DL TBF to know what physical 
   ** array size FW is doing EDGE IR.
   */
   uint16                    ir_oper_win_size;
   /* 
   ** Its like max bsn (actual bsn and not bsn +1) to know 
   ** what max bsn we are operating and if this falls in 
   ** operating range , ms_out_of_mem =0 else it is made 1.
   ** 
   */
   uint16                    vr_max;
   /*
   ** Set when bsn falls outside physical memory space required 
   ** by FW for EDGE IR
   */
   boolean                   ms_out_of_mem;
#endif /* FEATURE_GSM_EGPRS_IR_WS_REDUCTION */
   
  /* V(Q) Receive 'window' state variable
  **
  ** Denotes the lowest BSN not yet received and therefore represents the start 
  ** of the receive window.
  ** 
  ** In ack mode, when a block whose BSN is equal to V(Q) is received, V(Q) is 
  ** set to V(R) if all blocks are received OK or it is set to the value of the 
  ** BSN that is closest to V(Q) in the receive window element whose value 
  ** is not RECEIVED.
  **
  ** In un-ack mode, if V(R) - V(Q) > k, ie. V(R) points passed k from V(Q) 
  ** then V(Q) is set to: V(Q) = V(R) - k, ie. the start of the rx window, or
  ** V(Q) is shifted along.
  */
  uint16                    vq;
  
  /* Re-assembly indexes for DL UNACK mode when out of seq BSN are detected in a frame.
  ** ass_start points at the next in seq BSN to be assembled.
  ** ass_end points at the highest rcved BSN in the frame number currently processed.  
  ** When a new DL block is received in a new frame number, then all blocks
  ** between ass_start and ass_end are re-assembled into an LLC pdu.
  */ 
  uint16                    ass_start;
  uint16                    ass_end;
  
  /* struct used for formulating and sending packet downlink acknack 
  ** message to peer entity. Sub-members contain EGPRS related params
  */
  rm_dl_pkt_acknack_req_t   acknack;

  /* Starting sequence number (SSN)
  ** Assigned thevalue of the receive state variable V(R).
  ** This is returned with the receive block bitmap (RBB) in the packet downlink
  ** acknack message.
  */
  uint16                    ssn;

  /* current downlink assignment information
  */
  mr_dl_ass_ind_t           dl_ass;

  /* boolean to indicate if RLC has sent to MAC the RM_DL_FIRST_BLOCK_IND
  ** message so MAC can enter DL packet transfer mode.
  ** This is set to FALSE at assignment and set to TRUE after RLC has received
  ** the 1st data block from the n/w and has sent off the indication to MAC
  */
  boolean                   b_first_block_ind_sent;

  /* current channel coding scheme for gprs or egprs.
  */
  uint8                     cs;

  /* the last valid channel coding scheme
  */
  uint8                     last_valid_channel_coding;

  /* current downlink block sequence number.
  ** It is the last bsn that have been assembled into a pdu.
  */
  uint16                    bsn;

  /* Latch value of S/P field in the current downlink data block (cur_dl_blk[gas_id]).
  ** Required since the assemble function may re-initialise cur_dl_blk[gas_id]
  ** after using it. esp and sp has been combined for gprs and egprs.
  */
  uint8                     sp;

  /* Partial Bitmap Sequence Number */
  uint16                    pbsn;

  /* TFI of the previously received dl data block. This value is used to generate the
  ** next EDPAN -- whether the polling occurs on a data block with a BSN value in the 
  ** window size or out of the window size
  */
  uint8                     tfi;

  /* RRBP
  */
  uint8                     rrbp;

  /* flag to indicate the Final Block Indicator field of a 'current' block
  */
  uint8                     fbi;

  /* flag to indicate a final block has been received
  */
  uint8                     fbi_latch;

  /* flag to indicate when a pdu has been truncated after reaching a length
  ** of 1536 octets as per 9.1.11. When this happens all octets following
  ** the truncation is disregarded until the next identified pdu boundary, ie
  ** any block with the EXT (in the RLC hdr) field set to RM_EXT_OCTET_FOLLOWS.
  */
  boolean                   b_pdu_truncated;

  /* declare downlink timer structures
  */
  grlc_timer_t              timer_t3190;
  grlc_timer_t              timer_t3192;

  /* Flag set when RLC allows PL1 to start Timer T3192 when it sends the next
  ** Packet Downlink Ack/nack
  */
  boolean                   b_allow_t3192_to_start;

  uint16                    timer_t3190_expiry_cnt;
  uint16                    timer_t3192_expiry_cnt;

  /* frame number of the current data block
  */
  uint32                    frame_number;

  /* frame number of the unack data block to be assembled when diff then 
  ** the current frame number
  */
  uint32                    frame_number_unack;

  /* time slot of the current data block
  */
  uint8                     time_slot;
  
  /* count the number of filler blocks made in unack mode during back-to-back 
  ** test
  */
  uint16                    filler_block_cnt;
  
#ifdef FEATURE_GPRS_PS_HANDOVER
  /* Counter for number of blocks during PS Handover.*/
  uint16                    blind_data_blks_ctr;

  tbf_mode_T                psho_tbf_mode;

  boolean                   b_psho_active;
#endif /* FEATURE_GPRS_PS_HANDOVER */

  /* ETSI test mode */
  test_mode_T               test_mode;

  /* ENG MODE block counters*/

  uint16                    eng_mode_rcvd_dl_block_cnt;
  uint16                    eng_mode_rcvd_dup_dl_block_cnt;

  /* Receive block counter. This is a free running counter initialised at
  ** grlc dl task start up.  
  */
  uint32                    rx_blk_cnt;
  
  /* Temporary store for recording DL Block Polling information as DL Blocks
  ** are processed from the DL FIFO from GPL1. An absolute maximum of 8 polls
  ** are provided for within a radio block period.
  */
#define GRLC_DL_MAX_QUEUED_POLLS  8

  uint32                    poll_queue_index;
  dl_poll_t                 poll_queue[GRLC_DL_MAX_QUEUED_POLLS];

#if defined (DEBUG_GSM_GPRS_RLC_TEST) || defined (PERLUTF)
  #error code not present
#endif /* DEBUG_GSM_GPRS_RLC_TEST / PERLUTF */

#ifdef ERLD_DUMP_12BYTES_PLD

#define DUMP_PLD_LEN 12

  /* Set TRUE when a buffer dump is required. This is introduced to avoid
  ** printing information repeatedly for the same blocks.
  */  
  boolean                   dump_pld_once;

  struct
  {
    /* Header of DL block */ 
    uint16                  hdr[3];
    
    /* Buffer to copy the first 12 octets of the DL blocks   */
    uint8                   pld1[DUMP_PLD_LEN];
    uint8                   pld2[DUMP_PLD_LEN]; 
    
  }dump_buff[GRLC_DL_PL1_FIFO_SIZE + 1];

#endif /* ERLD_DUMP_12BYTES_PLD   */

  /* When Tuneaway occurs, tx/rx actitivies are discarded for reading paging on the other subscriber.
  ** This var holds the VETO status for GRLC downlink task, which disables (vote against TuneAway) when
  ** un-ack mode xfer is taking place. This variable is set to FALSE on starting up and when an RLC 
  ** ack-mode downlink TBF is being assigned, and it is set to TRUE on an ack-mode RLC downlink TBF.
  ** This variable will reduce un-necessary voting when many short downlink TBF occurs
  */
  boolean b_block_tuneaway;

} grlc_dl_t;


/* Maximum DL blocks per frame ( 4 or 5 TS dual payload ) */
#define GRLC_MAX_DL_BLCKS_PER_FR(gas_id)  (2 * dl[gas_id].dl_ass.num_dl_ts)


/*****************************************************************************
***
***     Private Macros
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

/*---------------------------------------------------------
** group M bit and E bit of the Extension Octet together.
** Required where it is more convenient to interpret the 
** M & E bits together.
**---------------------------------------------------------
*/
#define GROUP_ME(m,e)  ((((m) << 1) | (e)) & 0x3)

/*-----------------------------------------------------------------------
** ! Downlink timer durations definition
** durations and macros to start and stop downlink T3190 & T3192 timers
**-----------------------------------------------------------------------
*/
#define RM_TMR_T3190_DUR      5000
#define RM_TMR_T3192_DUR(gas_id) (t3192_dur_ms[0x07 & sysinfo[gas_id].t3192_index])


/* default until defined by sys info */

#ifndef DEBUG_GSM_GPRS_RLC_DL_DISABLE_TIMERS

  #define TIMER_START_T3190(gas_id)                                                      \
          {                                                                           \
            (void)rex_set_timer(&(dl[gas_id].timer_t3190.rex_timer), RM_TMR_T3190_DUR);  \
            dl[gas_id].timer_t3190.state = TMR_ACTIVE;                                   \
          }

  #define TIMER_START_T3192(gas_id)                                                          \
          {                                                                               \
            (void)rex_set_timer(&(dl[gas_id].timer_t3192.rex_timer), RM_TMR_T3192_DUR(gas_id)); \
            dl[gas_id].timer_t3192.state = TMR_ACTIVE;                                       \
          }
          
#else
#error code not present
#endif /* #ifndef DEBUG_GSM_GPRS_RLC_DL_DISABLE_TIMERS */

#define TIMER_STOP_T3190(gas_id)                                       \
        {                                                           \
          if (dl[gas_id].timer_t3190.state == TMR_ACTIVE)              \
          {                                                         \
            (void)rex_clr_timer(&(dl[gas_id].timer_t3190.rex_timer));  \
            dl[gas_id].timer_t3190.state = TMR_INACTIVE;               \
          }                                                         \
        }

#define TIMER_STOP_T3192(gas_id)                                       \
        {                                                           \
          if (dl[gas_id].timer_t3192.state == TMR_ACTIVE)              \
          {                                                         \
            (void)rex_clr_timer(&(dl[gas_id].timer_t3192.rex_timer));  \
            dl[gas_id].timer_t3192.state = TMR_INACTIVE;               \
          }                                                         \
          dl[gas_id].b_allow_t3192_to_start = FALSE;                   \
        }

/*======================================================
** definition of the three inequality macros required 
** for RLC downlink operation
**======================================================
*/

/*---------------------------------------------------------------------
** This macro evaluates the expression[V(Q) <= BSN < V(R)] mod SNS
** as per GSM 04.60, 9.1.7.
** This expression checks if the BSN coincides with or falls between 
** the start of the receive window V(Q) and the end of the receive 
** window V(R). If TRUE, the element of the status array V(N) indexed
** by the value of BSN is marked as RECEIVED.
**
** This implementation reads the expression from right to left in order
** to make use of the IS_GE.. and IS_GT macros. ie:
** V(Q) <= BSN < V(R) becomes: BSN >= V(Q) and V(R) > BSN
**
** Due to the way the we implement our modulo comparison the expression
** is modified to:
** V(Q) <= BSN <= (V(R)-1)
**---------------------------------------------------------------------
*/

/*

#define  IS_VQ_LE_BSN_LT_VR(q,b,r)        \
      IS_GE_MODULO_COMP(b,q,SNS)  &&      \
      IS_GT_MODULO_COMP(r,b,SNS)

#define IS_VQ_LE_BSN_LT_VR(vq,bsn,vr)             \
        (                                         \
          IS_GE_MODULO_COMP((bsn),(vq),SNS)  &&   \
          IS_GE_MODULO_COMP(((vr)-1),(bsn),SNS)   \
        )
*/

/*----------------------------------------------------------------------
** This macro evaluates the expression [V(R) <= BSN < V(R)-k] mod SNS
** as per GSM 04.60, 9.1.7.
**
** This checks if the BSN falls outside the receive window which is
** defined relative to V(R).
**
** The implementation reads the expression from right to left in order
** to make use of the IS_GE.. and IS_GT macros. ie:
**
** V(R) <= BSN < (V(R)-k) becomes: BSN >= V(R) and (V(R)-k) >= BSN
**
** Due to the way the we implement our modulo comparison the expression
** is modified to:
** V(R) <= BSN <= (V(R)-WS-1)
**----------------------------------------------------------------------
*/
/*
#define  IS_VR_LE_BSN_LE_VR_MINUS_K(vr,bsn,k)           \
      IS_GE_MODULO_COMP(bsn,vr,SNS)     &&              \
      IS_GT_MODULO_COMP(MOD_SNS((vr)-(k)),bsn,SNS)
*/
#define IS_VR_LE_BSN_LE_VR_MINUS_K(vr,bsn,k)            \
        (                                               \
          IS_GE_MODULO_COMP(bsn,vr,SNS) &&              \
          IS_GT_MODULO_COMP(MOD_SNS((vr)-(k)),bsn,SNS)  \
        )


/*---------------------------------------------------------------------
** This macro evaluates the expression [V(R) <= BSN < V(Q)+k] mod SNS
** as per GSM 04.60, 9.1.5.
**
** This expression checks if the BSN falls inside the receive window
** defined by the start of the window V(Q) and that BSN coincides 
** with or ahead of the end of the receive window V(R). The latter
** condition determines if V(R) needs to be updated to point past
** the last highest item received.
**
** The implementation reads the expression from right to left in order
** to make use of the IS_GE.. and IS_GT. ie:
** V(R) <= BSN < (V(Q)+k) becomes: BSN >= V(R) and (V(Q)+k) > BSN
**
** Consider the scenario where vq=0, bsn=0, k=64 which makes the 
** expression BSN < (V(Q)+K) evaluates to FALSE. Because
** [0 < 64] modulo 128 is FALSE.
** FALSE. Since 64 > 0 is FALSE if modulo comparison is used.
**
**---------------------------------------------------------------------
*/

/*
#define  IS_VR_LE_BSN_LT_VQ_PLUS_WS(vr,bsn,vq,k)      \
      IS_GE_MODULO_COMP(bsn,vr,SNS)    &&     \
      IS_GT_MODULO_COMP(MOD_SNS((vq)+(k)),bsn,SNS)
*/

#define IS_VR_LE_BSN_LT_VQ_PLUS_WS(vr,bsn,vq,ws)              \
        (                                                     \
          IS_GE_MODULO_COMP((bsn),(vr),(SNS))    &&           \
          IS_GE_MODULO_COMP(MOD_SNS((vq)+(ws)-1),(bsn),(SNS)) \
        )

#define IS_VQ_LE_BSN_LT_VQ_PLUS_WS(vq,bsn,ws)           \
        (                                               \
          IS_GE_MODULO_COMP((bsn),(vq),(SNS)) &&        \
          IS_GE_MODULO_COMP(MOD_SNS((vq)+(ws)-1),(bsn),(SNS))   \
        )


#define EGPRS_IS_VR_LE_BSN_LT_VQ_PLUS_WS(vr,bsn,vq,ws)        \
        (                                                     \
          IS_GE_MOD((bsn),(vr),(ws))    &&                    \
          IS_GE_MOD(MOD_ESNS((vq)+(ws)-1),(bsn),(ws))         \
        )

#define EGPRS_IS_VQ_LE_BSN_LT_VQ_PLUS_WS(vq,bsn,ws)     \
        (                                               \
          IS_GE_MOD((bsn),(vq),(ws)) &&                 \
          IS_GE_MOD(MOD_ESNS((vq)+(ws)-1),(bsn),(ws))   \
        )

#define EGPRS_IS_VQ_LE_BSN_LT_VR(vq,bsn,ws,vr)          \
        (                                               \
          IS_GE_MOD((bsn),(vq),(ws)) &&                 \
          IS_GT_MOD((vr),(bsn),(ws))                    \
        )

/*-----------------------------------------------------------------------------
** The Split Block indicator (SPB) is used in header type 3 to indicate if some 
** user data is re-transmitted using 2 block resegmentation
**-----------------------------------------------------------------------------
*/
#define IS_BLOCK_SPLIT(gas_id)                                \
        ( (cur_dl_blk[gas_id].cs <= RM_MCS_4)  &&             \
          ( (cur_dl_blk[gas_id].spb == SPB_RETX_PART_1 ) ||   \
            (cur_dl_blk[gas_id].spb == SPB_RETX_PART_2 )      \
          )                                                \
        )                                                  \

/*---------------------------------------------------------
** Extract BSN1 and BSN2 from a raw DL data block's header
**---------------------------------------------------------
*/
#define E_GET_RAW_DL_BSN1(p)                                \
        (                                                   \
         ((uint16)((((p)[1]) >> 6) & 0x0003)  |             \
          (uint16)((((p)[2]) & 0x00ff) << 2)  |             \
          (uint16)((((p)[3]) & 0x0001) << 10)               \
         ) & 0x07ff                                         \
        )

#define E_GET_RAW_DL_BSN2(p)                                \
        (                                                   \
         ((uint16)((((p)[3]) >> 1) & 0x007f)  |             \
          (uint16)((((p)[4]) & 0x0007) << 7)                \
         ) & 0x03ff                                         \
        )


/* Diagnostic Macros
*/

#if defined(DEBUG_GSM_EGPRS_DS_UNIT_TEST) || defined(PERLUTF)
    #error code not present
#else
  #define UPD_DL_STATE(gas_id)     {                           \
          dl_diag_stats_st[gas_id].grlc_dl_state = (uint32)dl[gas_id].state;\
          (void)rlc_log_dl_statistics(gas_id, FALSE);               \
                                }
#endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST / PERLUTF */

#define UPD_DL_ACKNACK(gas_id)  dl_diag_stats_st[gas_id].ack_nack_req_cnt++;
#define UPD_DL_VR(gas_id)       dl_diag_stats_st[gas_id].dl_seq_vr = (uint8)dl[gas_id].vr;
#define UPD_DL_VQ(gas_id)       dl_diag_stats_st[gas_id].dl_seq_vq = (uint8)dl[gas_id].vq;
#define UPD_DL_SSN(gas_id)      dl_diag_stats_st[gas_id].dl_seq_ssn = (uint8)dl[gas_id].ssn;


#endif /* INC_GRLCDLI_H */

/*** EOF: don't remove! ***/
