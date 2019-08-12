/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC/PL1 Uplink FIFO Definition
***
*** DESCRIPTION
***
***  Definition of data structures and functions provided by the FIFO module
***
***  Functions for use by PL1 to access data blocks in the FIFO
***     "gas_id_t gas_id" has been added as the first parameter in most of these functions
***
***     grlc_ul_fifo_get_cnt(gas_id_t)
***
***     grlc_ul_fifo_get_nack_cnt(gas_id_t);
***     grlc_ul_fifo_get_ptx_cnt(gas_id_t);
***     grlc_ul_fifo_get_pack_cnt(gas_id_t);
***
***     grlc_ul_fifo_access_block(gas_id_t)
***
***  Functions for use by RLC to initialise and fills the FIFO
***
***     grlc_ul_fifo_one_time_init(gas_id_t)
***     grlc_ul_fifo_re_init(gas_id_t)
***
***     grlc_ul_fifo_put_block(gas_id_t)
***     grlc_ul_fifo_delete_unread_ptx_blocks(gas_id_t)
***     grlc_ul_fifo_delete_unread_pack_blocks(gas_id_t)
***     grlc_ul_fifo_delete_unread_nack_pack_blocks(gas_id_t)
***
***     grlc_ul_fifo_is_area_full(gas_id_t)
***
***     void grlc_ul_fifo_enable_pl1_access(gas_id_t)
***     void grlc_ul_fifo_disable_pl1_access(gas_id_t)
***
***  Functions to support Back-to-back Target and DSPE tests
***
***     void grlc_ul_fifo_init_mac()
***     void grlc_ul_fifo_put_mac_block
***     mac_rlc_msg_t *grlc_ul_fifo_get_mac_block()
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/inc/grlculfifo.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 06/14/01    hv     Created
*** 07/17/01    hv     Added stall indicator param to grlc_ul_fifo_put_block()
*** 07/26/01    hv     Removed SI. Added grlc_ul_fifo_resync().
*** 09/18/01    hv     Added test features
*** 08/03/04    hv     Added EGPRS code
***
*****************************************************************************/

#ifndef _GRLCULFIFO_H_
#define _GRLCULFIFO_H_

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
#include "grlcmaccmn.h"
#include "geran_multi_sim.h"
#include "grlci.h"
#include "grlcmac.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/*-----------------------------------------------------------------------
** GRLC UL PL1 Radio Block structure comprising the coding scheme and an
** RLC radio block (with MAC header)
**-----------------------------------------------------------------------
*/
typedef struct
{
  /* Coding scheme for this data block
  */
  uint8   cs;

  /* Uplink RLC/MAC block including the MAC header
  ** Space is reserved for the highest coding scheme (CS_4).
  ** A lower coding scheme uses only the first x bytes.
  */
  uint16  filler; /* This forces the following block[] to be word aligned
                     for efficient MDSP mem copying. */
  uint8   block[GRLC_RADIO_BLOCK_MAX_LEN];

} grlc_ul_pl1_data_t;


/* If EGPRS is supported, replace the grlc_ul_pl1_data_t with a super structure
** which contains both existing GPRS info as well as EGPRS specific one. This
** later will be merged back into the GPRS code
*/

/*-----------------------------------------------------------------------
** GRLC UL PL1 Radio Block structure comprising the coding scheme and an
** RLC radio block (with MAC header)
**-----------------------------------------------------------------------
*/


/*-----------------------------------------------------------------------
** EGRLC RLC-UL/PL1 data block interface
**-----------------------------------------------------------------------
*/
typedef struct
{
  uint8   mcs;
  uint8   ps1;
  uint8   ps2;

  uint8   filler;   /* force even alignment */

  uint8   hdr[E_GRLC_HDR_TYPE_MAX_LEN];
  uint8   msg1[E_GRLC_DATA_BLOCK_MAX_LEN];
  uint8   msg2[E_GRLC_DATA_BLOCK_MAX_LEN];

} e_grlc_ul_fifo_element_t;

typedef struct
{
  union
  {
    grlc_ul_pl1_data_t        gprs;
    e_grlc_ul_fifo_element_t  egprs;
  } elm;

  /* pass through PDU priority info from upper layer whether PDU contains
  ** just data or ctrl(tcp/nas signalling)
  */
  geran_pdu_priority_t        pdu_prio;
} grlc_ul_fifo_element_t;


/*-------------------------------------------------------------------------------
** The length of each of the 3 areas is 4 blocks. The total size of the FIFO
** is therefore 12 blocks.
**
** For multislot class 12 where the maximum number of uplink timeslots can be 4.
** The FIFO depth needs to be 8 and the low water mark sets to 4.
**-------------------------------------------------------------------------------
*/
  #ifdef  FEATURE_GSM_GPRS_MSC33
    #define RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE   10
  #else
    #define RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE   8
  #endif /* FEATURE_GSM_GPRS_MSC33 */

#define RLC_PL1_UL_TOTAL_FIFO_SIZE        (3*RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE)

/*------------------------------------
** The RLC-PL1 UL FIFO data structure
**------------------------------------
*/
typedef struct
{
  tbf_mode_T  tbf_mode;   /* TBF_MODE_INVALID/GPRS/EGPRS */

  /* The FIFO comprises 3 areas each holding blocks of a particular acknowledge
  ** status: nacked (nack), pending-transit (ptx) and pending-ack (pack).
  **
  ** blk_arr: data blocks in each area is held in an array of blocks.
  ** in_ind, out_ind: selects the next incoming or outgoing block.
  ** read_out_ind: selects the next block to be read out.
  ** read_ctr: counts the number of read blocks in an area.
  ** ctr: keeps the sum of blocks present in an area
  **
  ** b_validity: internal flag indicating whether the FIFO is busy (TRUE) being
  ** processed by RLC or not busy (FALSE) when PL1 can access the data blocks.
  ** RLC resets this flag at the start of a TBF.
  **
  ** PL1 must ensure READ_MODE accesses and GET_MODE accesses are balanced.
  ** Pl1 is expected to perform an ABORT_ACCESS at the end of every block period
  ** specially when the number READ_MODE and the number of GET_MODE accesses are
  ** different.
  **
  ** When PL1 aborts operation due to a coding scheme change, the FIFO is
  ** re-processes by RLC.
  */

  /*--------------------
  ** Nacked (NACK) area
  **--------------------
  */

  /* Number of blocks read ths far
  */
  uint8               nack_read_ctr;

  /* Index to the next incoming data block
  */
  uint8               nack_in_ind;

  /* Index to the next block to be taken out (GET_ACCESSS)
  */
  uint8               nack_out_ind;

  /* Total number of blocks remained for GET_ACCESS
  */
  uint8               nack_ctr;

  /* Index to the next block to be read
  */
  uint8               nack_read_out_ind;

  /* Array containing x data blocks for this area
  */
  grlc_ul_fifo_element_t  nack_blk_arr[RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE];

  /*-----------------------------
  ** Pending-transmit (PTX) area
  **-----------------------------
  */
  uint8               ptx_read_ctr;
  uint8               ptx_in_ind;
  uint8               ptx_out_ind;
  uint8               ptx_ctr;
  uint8               ptx_read_out_ind;
  grlc_ul_fifo_element_t  ptx_blk_arr[RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE];

  /*-------------------------
  ** Pending-ack (PACK) area
  **-------------------------
  */
  uint8               pack_read_ctr;
  uint8               pack_in_ind;
  uint8               pack_out_ind;
  uint8               pack_ctr;
  uint8               pack_read_out_ind;
  grlc_ul_fifo_element_t  pack_blk_arr[RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE];

  boolean             b_validity;

} grlc_ul_pl1_fifo_t;


/*--------------------------------------------------------------------------
** Access modes definition for PL1.
** READ_MODE: returns block for copying to NPL1 registers without removing.
** GET_MODE: returns block and removes it from FIFO.
**--------------------------------------------------------------------------
*/
typedef enum
{
  /* Return address of a data block without removing data block from the FIFO
  */
  READ_MODE,

  /* Return address of a data block and deletes it from the FIFO.
  */
  GET_MODE,

  /* Reset the read blocks in all three areas
  */
  ABORT_MODE
} grlc_ul_fifo_access_t;


/*-----------------------------------------------------------------------
** Definition of the get-type parameters of grlc_ul_fifo_access_block().
** PL1 uses GET_WITH_PREVIOUS_READ in Dynamic Allocation mode
**-----------------------------------------------------------------------
*/
typedef enum
{
  /* Indicate the current GET_MODE access is paired up with previous
  ** READ_MODE access(es) for use in Dynamic Allocation mode.
  **
  ** For example, PL1 performs 2 READ_MODE accesses and was given 2 blocks
  ** in the ptx area. Two more nack blocks then arrive and PL1 performs 2
  ** GET_MODE accesses with the intention to remove 2 previously read blocks.
  ** By precidence the blocks returned will be the nack blocks which are wrong.
  ** However, by specifying GET_WITH_PREVIOUS_READ together with the ptx status
  ** PL1 shall get 2 blocks previously returned in the ptx area, as intended.
  */
  GET_WITH_PREVIOUS_READ
} grlc_ul_fifo_get_t;


/*------------------------------------------------
** Definition of results of get & put operations
**------------------------------------------------
*/
typedef enum
{
  UL_FIFO_PUT_OK,
  UL_FIFO_FULL,
  UL_FIFO_EMPTY,

  /* This is returned when RLC sets the Validity flag to FALSE during re-processing
  ** of the FIFO whislt handling Contention Resolution
  */
  UL_FIFO_BUSY,

  UL_FIFO_ACCESS_OK,

  /* Out of range access mode
  */
  UL_FIFO_ACCESS_ERROR,

  /* When a block is not available contrary to values of counters
  */
  UL_FIFO_READ_ERROR,

  /* When the put ack status is not in range
  */
  UL_FIFO_PUT_ERROR,

  /* When the ack status requested is not in range
  */
  UL_FIFO_GET_ERROR

} grlc_ul_fifo_op_result_t;



/*-------------------------------------------------------------
** Structure to hold arguments for grlc_ul_fifo_access_block()
**-------------------------------------------------------------
*/
typedef struct
{
  /* RLC returns a pointer to data block comprising coding scheme and RLC
  ** radio block (with MAC header)
  */
  grlc_ul_fifo_element_t    *blk_ptr;

  /* PL1 specifies to RLC the access mode
  */
  grlc_ul_fifo_access_t access_mode;

  /* RLC returns the block's ack state (Nack, Pending_tx or Pending_ack)
  ** to PL1 using this pointer
  */
  tx_ack_state_t        ack_state;

  /* pass through PDU priority info from upper layer whether PDU contains
  ** just data or ctrl(tcp/nas signalling)
  */
  geran_pdu_priority_t  pdu_prio;

  /* If GET_MODE is used, PL1 specifies whether this get pairs up with
  ** a previous READ_MODE access
  */
  grlc_ul_fifo_get_t    get_type;

} grlc_ul_fifo_access_block_t;


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

/*------------------------------------------------------------------------------
** Dedicated uplink FIFO between RLC and PL1. This is owned and managed by GRLC
**------------------------------------------------------------------------------
*/
extern grlc_ul_pl1_fifo_t   grlc_ul_fifo[NUM_GERAN_DATA_SPACES];


/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION        GRLC_UL_FIFO_INIT
===
===  DESCRIPTION
===
===    initialise the rlc ul fifo to empty state.
===
===  PARAMETERS
===
===
===  DEPENDENCIES
===
===    This should be called by rlc ul task.
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
void grlc_ul_fifo_one_time_init (gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION        GRLC_UL_FIFO_RE_INIT
===
===  DESCRIPTION
===
===    Re-initialise the rlc ul fifo to empty state leaving the array of
===     pointers to data blocks unchanged.
===
===  PARAMETERS
===
===
===  DEPENDENCIES
===
===    This should be called by rlc ul task.
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
void grlc_ul_fifo_re_init (gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_GET_COUNT
===
===  DESCRIPTION
===
===    Return the number of rlc uplink data blocks available in the fifo.
===
===  PARAMETERS
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    byte representing the number of ul blocks fifo
===
===  SIDE EFFECTS
===
===
===========================================================================*/
uint8 grlc_ul_fifo_get_cnt (gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_GET_NACK_CNT
===
===  DESCRIPTION
===
===   Return the number of items currently held in the Nack area of the FIFO
===
===  PARAMETERS
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    uint8
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
uint8 grlc_ul_fifo_get_nack_cnt(gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_GET_PTX_CNT
===
===  DESCRIPTION
===
===   Return the number of items currently held in the pending-transmit
===   area of the FIFO
===
===  PARAMETERS
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    uint8
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
uint8 grlc_ul_fifo_get_ptx_cnt(gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_GET_PACK_CNT
===
===  DESCRIPTION
===
===   Return the number of items currently held in the pending-ack area
===   of the FIFO
===
===  PARAMETERS
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    uint8
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
uint8 grlc_ul_fifo_get_pack_cnt(gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION    GRLC_UL_FIFO_PUT_BLOCK()
===
===  DESCRIPTION
===
===    This function takes a pointer to an uplink data block in internal
===     format and translates it into external format and stores it at the
===     FIFO item indexed by the in_index.
===
===  PARAMETERS
===
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    UL_FIFO_PUT_OK, UL_FIFO_FULL
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
grlc_ul_fifo_op_result_t  grlc_ul_fifo_put_block
(
  gas_id_t				      gas_id,
  grlc_ul_data_block_t  *bp,
  tx_ack_state_t        ack_stat
);

/*===========================================================================
===
===  FUNCTION       E_GRLC_UL_FIFO_PUT_BLOCK()
===
===  DESCRIPTION
===
===   This function inserts a data block into the FIFO in area given by the
===   ack status, if that area of the FIFO is not full.
===
====  Blocks with TXA_UNACK are put in the area Pending-ack area.
===
===   This function also invokes xlate_int_ul_data() to convert the payload
===   part of the data block in DSM item into straight buffer.
===
===  PARAMETERS
===
===   *bp1/2: pointers to e_grlc_ul_data_block_t which hold the MCS and
===           an array of octets representing the RLC data block
===
===   ack_state: ack status of the block before it is sent off to the FIFO.
===        The ack state is of tx_ack_state_t but only 4 values are used:
===        TXA_NACK, TXA_PTX, TXA_PA and TXA_UNACK.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    grlc_ul_fifo_op_result_t
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
grlc_ul_fifo_op_result_t  e_grlc_ul_fifo_put_block
(
  gas_id_t				gas_id,
  grlc_ul_data_block_t  *bp1,
  grlc_ul_data_block_t  *bp2,
  tx_ack_state_t         ack_state
);

/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_ACCESS_BLOCK()
===
===  DESCRIPTION
===
===    Retrieve a pointer to a buffer representing an RLC/MAC block from
===    the RLC-PL1 FIFO. There are 3 areas in the FIFO with each holding a
===    particular acknowledgement status which determine the order in
===    which the blocks are retrieved. The 3 ack status are Nack(TXA_NACK),
===    Pending-transmit(TXA_PTX) and Pending-ack(TXA_PACK).
===
===    There are 2 access modes: READ_MODE returns a pointer to a block for
===    copying into the DSP, without taking the block out of the FIFO. After
===    the DSP confirms the block has been transmitted then GET_MODE can be
===    used to remove the block from the FIFO.
===
===    This function also invokes post_tx_actions(). This allows RLC to start
===    the block timer and update its transmit variables.
===
===    In READ_MODE the ack state of the block is returned to PL1 together
===    with the block. When PL1 makes a GET_MODE access it must return the
===    ack state of the block to indicate the area the block came from.
===
===    In GET_MODE access, the get_type param indicates if the GET_MODE is
===    made after a previous READ_MODE access. If get_type is
===    GET_WITH_PREVIOUS_READ, the ack status passed will be used to find
===    the block in the indicated area. If the get_type is
===
===  PARAMETERS
===
===    access_mode: READ_MODE or GET_MODE to be specified by PL1.
===
===    get_type: If access_mode is GET_MODE then PL1 uses:
===          - GET_WITH_PREVIOUS_READ: when in Dynamic Allocation mode
===
===    blk_ptr: pointer to struct representing data block returned to PL1.
===
===    ack_state: Only a subset of tx_ack_state_t is used: TXA_NACK, TXA_PTX
===          or TXA_PACK. In READ_MODE this value is returned to PL1 for
===          keeping track of a series of READ_MODE accesses.
===
===          In GET_MODE PL1 returns this value so the READ/GET can
===          pair up correctly. Since between the READ and GET operations
===          more blocks may have arrived and altered the order of the
===          blocks being returned.
===
===  DEPENDENCIES
===
===    grlc_ul_fifo must be initialised and ready.
===    post_tx_actions() hook function must be defined.
===
===  RETURN VALUE
===
===    grlc_ul_fifo_op_result_t
===
===  SIDE EFFECTS
===
===    grlc_ul_fifo
===    when access mode is GET_MODE signal_tlli_block() is called.
===
===========================================================================*/
grlc_ul_fifo_op_result_t  grlc_ul_fifo_access_block
(
  gas_id_t						gas_id,
  grlc_ul_fifo_access_block_t  *ff_ptr
);


/*===========================================================================
===
===  FUNCTION      xxx
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

/*===========================================================================
===
===  FUNCTION       GRLC_UL_FIFO_IS_AREA_FULL()
===
===  DESCRIPTION
===
===   Return TRUE if the given area in the RLC/PL1 UL FIFO is full.
===   Return FALSE otherwise.
===
===  PARAMETERS
===
===   ack_state: ack status of the block before it is sent off to the FIFO.
===        The ack state is of tx_ack_state_t but only three values are used:
===        TXA_NACK, TXA_PTX, TXA_PACK and TXA_UNACK.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===   boolean
===
===  SIDE EFFECTS
===
===   none
===
===========================================================================*/
boolean grlc_ul_fifo_is_area_full (gas_id_t gas_id, const tx_ack_state_t ack_state);


/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_DELETE_UNREAD_NACK_PACK_BLOCKS()
===
===  DESCRIPTION
===
===    Upon receipt of a Packet Uplink Acknack message, some data blocks intended
===    for re-transmission in the PACK and NACK areas may be marked as received.
===    For those data blocks already been read by PL1 we will leave. But the
===    un-read blocks must be deleted. This function performs those functions as
===    well as winding the transmit variables (pack_vsf and nack_vsf) back to their
===    correct values.
===
===    Actions:
===
===    1. Find the BSN value of the oldest block in the un-read part of the NACK
===       area.
===    2. Rewind the 'next-to-send' (nack_vsf) pointer to the BSN value found.
===    3. Do the same for the PACK area.
===
===    eg. the NACK area contains BSN3,BSN4 and BSN5 and BSN3 has already been read.
===    BSN4 is the oldest un-read block. After processing the area should only hold
===    BSN3 and the transmit variable 'nack_vsf' should point to BSN4.
===
===  PARAMETERS
===
===    nack_vsf_ptr: points to ul.nack_vsf. Updated to the BSN value of the oldest
===                  purged NACK block
===    pack_vsf_ptr: points to ul.pack_vsf. Updated to the BSN value of the oldest
===                  purged PACK block
===
===  DEPENDENCIES
===
===    grlc_ul_fifo must be initialised and ready.
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    grlc_ul_fifo
===
===========================================================================*/
void grlc_ul_fifo_delete_unread_nack_pack_blocks
(
  gas_id_t  gas_id,
  uint16	  *nack_vsf_ptr,
  uint16	  *pack_vsf_ptr
);

/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_ENABLE_PL1_ACCESS()
===
===  DESCRIPTION
===
===    Function for RLC to enable PL1 access to the RLC/PL1 UL FIFO.
===    RLC calls this function at the start of a TBF or after it finishes the
===    processing of the RLC-PL1 UL FIFO
===
===  PARAMETERS
===
===    none
===
===  DEPENDENCIES
===
===    grlc_ul_fifo must be initialised and ready.
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    grlc_ul_fifo
===
===========================================================================*/
void grlc_ul_fifo_enable_pl1_access (gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_DISABLE_PL1_ACCESS()
===
===  DESCRIPTION
===
===    Function for RLC to disable PL1 access to the RLC/PL1 UL FIFO whilst
===    RLC re-processes the FIFO following receipt of a Packet Uplink Acknack
===    message
===
===  PARAMETERS
===
===    none
===
===  DEPENDENCIES
===
===    grlc_ul_fifo must be initialised and ready.
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    grlc_ul_fifo
===
===========================================================================*/
void grlc_ul_fifo_disable_pl1_access (gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_DELETE_UNREAD_PTX_BLOCKS()
===
===  DESCRIPTION
===
===    If the PTX contains any un-read blocks then delete them and return the
===    BSN of the oldest un-read block in bsn_ptr and return TRUE. Otherwise
===    return FALSE
===
===  PARAMETERS
===
===    bsn_ptr: points to the variable holding the result
===
===  DEPENDENCIES
===
===    grlc_ul_fifo must be initialised and ready.
===
===  RETURN VALUE
===
===    TRUE if any un-read PTX blocks are deleted. FALSE otherwise
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
boolean grlc_ul_fifo_delete_unread_ptx_blocks
(
  gas_id_t  gas_id,
  uint16    *bsn_ptr
);

/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_DELETE_UNREAD_PACK_BLOCKS()
===
===  DESCRIPTION
===
===    If the PACK contains any un-read blocks then delete them and return the
===    BSN of the oldest un-read block in bsn_ptr and return TRUE. Otherwise
===    return FALSE
===
===  PARAMETERS
===
===    bsn_ptr: points to the variable holding the result
===
===  DEPENDENCIES
===
===    grlc_ul_fifo must be initialised and ready.
===
===  RETURN VALUE
===
===    TRUE if any un-read PACK blocks are deleted. FALSE otherwise
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
boolean grlc_ul_fifo_delete_unread_pack_blocks
(
  gas_id_t  gas_id,
  uint16    *bsn_ptr
);

#endif /* _GRLCULFIFO_H_ */

/*** EOF: don't remove! ***/
