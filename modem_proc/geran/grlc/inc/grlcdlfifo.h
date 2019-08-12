/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC/L1 Downlink FIFO Definition
***
*** DESCRIPTION
***
***  Definition of functions to implement the downlink interface between
***  RLC and L1
***
***  grlc_dl_fifo_one_time_init()
***  grlc_dl_fifo_clear()
***  grlc_dl_fifo_get_cnt()
***  grlc_dl_fifo_get_block()
***  grlc_dl_fifo_put_block()
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/inc/grlcdlfifo.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 05/16/01    hv     Created
*** 07/10/01    hv     Added absolute frame number to data from L1
***
*****************************************************************************/

#ifndef _GRLCDLFIFO_H_
#define _GRLCDLFIFO_H_

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
#include "gprs_pdu.h"
#include "grlci.h"
#include "geran_multi_sim.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

#define GRLC_DL_PL1_FIFO_SIZE   16
#ifdef  FEATURE_GSM_GPRS_MSC33
  #define GRLC_DL_FIFO_HIGH_WATER  10 /* Currently set at a low level to allow
                                      sufficient time to respond to poll bit
                                      in time to meet RRBP */
#else
  #define GRLC_DL_FIFO_HIGH_WATER  4 /* Currently set at a low level to allow
                                      sufficient time to respond to poll bit
                                      in time to meet RRBP */
#endif /* FEATURE_GSM_GPRS_MSC33 */
/*----------------------------------------------------------------
** The PL1 FIFO holds an array of PL1 Radio Block Headers and an
** associated array of pointers to DSM items containing the PL1
** Radio Block payloads.
**----------------------------------------------------------------
*/
typedef struct
{
  /* index the next item to go into the fifo
  */
  uint8             in_ind;

  /* index the next item to be taken out of the fifo
  */
  uint8             out_ind;

  /* the number of items currently held in the fifo
  */
  uint8             ctr;

  /* Array of Radio Block PL1 Headers and the associated Radio Block
  ** Payload pointers.
  */
  pl1_radio_block_header_t pl1_blk_hdr[GRLC_DL_PL1_FIFO_SIZE];
  dsm_item_type            *pl1_pyld_ptrs[GRLC_DL_PL1_FIFO_SIZE];

  e_pl1_radio_block_t arr_elm[GRLC_DL_PL1_FIFO_SIZE];

} grlc_dl_pl1_fifo_t;

/*-----------------------------------------------------------
** enumeration of results of fifo get, put & ref operations
**-----------------------------------------------------------
*/
typedef enum
{
  DL_FIFO_NOT_USED,
  DL_FIFO_PUT_OK,
  DL_FIFO_GET_OK,
  DL_FIFO_REF_OK,
  DL_FIFO_FULL,
  DL_FIFO_EMPTY,
  DL_FIFO_WRONG_TBF_MODE          /* guard against putting in wrong TBF mode */

} grlc_dl_fifo_op_result_t;

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
extern grlc_dl_pl1_fifo_t  grlc_dl_fifo[NUM_GERAN_DATA_SPACES];

#ifdef  ERLD_SHOW_BLOCK
extern grlc_dl_pl1_fifo_t  grlc_dl_fifo[NUM_GERAN_DATA_SPACES];
#endif

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
===  FUNCTION        GRLC_DL_FIFO_INIT
===
===  DESCRIPTION
===
===    initialise the rlc dl fifo to empty state.
===
===  PARAMETERS
===
===
===  DEPENDENCIES
===
===    This should be called by rlc dl task.
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
void grlc_dl_fifo_one_time_init ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      GRLC_DL_FIFO_CLEAR
===
===  DESCRIPTION
===
===    Read out all remaining items in FIFO. For each value returned, if the
===    DSM pointer is not NULL then free the DSM item.
===
===    Also re-init the DL FIFO indices and counter
===
===
===  PARAMETERS
===
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
void grlc_dl_fifo_clear ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      GRLC_DL_FIFO_GET_COUNT
===
===  DESCRIPTION
===
===    Return the number of rlc downlink data blocks available in the fifo.
===
===  PARAMETERS
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    uint8 representing the number of dl blocks fifo
===
===  SIDE EFFECTS
===
===
===========================================================================*/
uint8 grlc_dl_fifo_get_cnt ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION    GRLC_DL_FIFO_PUT_BLOCK()
===
===  DESCRIPTION
===
===    This function inserts a PL1 Radio Block Header and a pointer to a
===    DSM item containing the PL1 Radio Block payload into the FIFO.
===
===    If sucessful, the in_index and ctr of the FIFO are adjusted.
===
===    After one or more blocks are copied to the fifo, L1 should send a
===    a PH_DATA_IND signal to RLC to indicate there are data block(s)
===    available in the fifo.
===
===  PARAMETERS
===
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    DL_FIFO_PUT_OK, DL_FIFO_FULL
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
grlc_dl_fifo_op_result_t  grlc_dl_fifo_put_block
(
  gas_id_t                        gas_id,
  const pl1_radio_block_header_t  *pl1_blk_hdr_ptr_ptr,
  dsm_item_type                   *pl1_blk_pyld_ptr_ptr
);

/*===========================================================================
===
===  FUNCTION      GRLC_DL_FIFO_GET_BLOCK
===
===  DESCRIPTION
===
===    This function retrieves a pointer to a PL1 Radio lock Header and a
===    pointer to a DSM item containing a PL1 Radio Block Header representing a portable
===    layer 1 radio block.
===
===    If succesful, the block is copied to the designated block pointer and
===    the function adjust the out index and counter and return DL_FIFO_GET_OK.
===    If the fifo is empty then DL_FIFO_EMPTY is returned.
===
===  PARAMETERS
===
===
===  DEPENDENCIES
===
===    grlc_dl_fifo must be initialised and ready.
===
===  RETURN VALUE
===
===    DL_FIFO_GET_OK, DL_FIFO_EMPTY
===
===  SIDE EFFECTS
===
===    grlc_dl_fifo
===
===========================================================================*/
grlc_dl_fifo_op_result_t  grlc_dl_fifo_get_block
(
  gas_id_t                  gas_id,
  pl1_radio_block_header_t  **pl1_blk_hdr_ptr_ptr,
  dsm_item_type             **pl1_blk_pyld_ptr_ptr
);

/*===========================================================================
===
===  FUNCTION      GRLC_DL_FIFO_REF_BLK_HDR
===
===  DESCRIPTION
===
===    Reference the next downlink data block header in the rlc dl fifo
===    without removing the element from the FIFO.
===
===    If succesfull the designated block pointer is made to point to the
===    block and the function returns DL_FIFO_REF_OK. If the fifo is empty
===    then DL_FIFO_EMPTY is returned.
===
===  PARAMETERS
===
===
===  DEPENDENCIES
===
===    grlc_dl_fifo must be initialised and ready.
===
===  RETURN VALUE
===
===    grlc_dl_fifo_op_result_t type
===
===  SIDE EFFECTS
===
===    grlc_dl_fifo
===
===========================================================================*/
grlc_dl_fifo_op_result_t grlc_dl_fifo_ref_blk_hdr
(
  gas_id_t                  gas_id,
  pl1_radio_block_header_t  **pl1_blk_hdr_ptr_ptr
);

/*===========================================================================
===
===  FUNCTION      GRLC_IS_RLC_DL_REQUIRED_CONTEXT()
===
===  DESCRIPTION
===
===    This function is called by PL1 after a radio block is put into the
===    DL FIFO. It returns a boolean value which when TRUE requires PL1 to
===    call the grlc_send_ph_data_ind() to give RLC context to process and
===    flush the DL FIFO. This function ensures RLC DL is only gains context
===    when it needs it in order to reduce context switches from PL1 to RLC DL.
===
===    This function returns TRUE when one of the conditions below is TRUE:
===    1. FBI bit of the DL radio block is set: this indicates the data block
===       is the last of the current PDU or that an acknowledgement is requested
===       by the NW.
===    2. Ext bit is clear: this indicates the presence of a PDU boundary which
===       means the data block may be the last of the current PDU.
===    3. If the number of radio blocks in the DL FIFO reaches either the
===       high watermark or full.
===
===  PARAMETERS
===
===    data_block_ptr: pointer to the RLC data block which includes the MAC
===                    header followed by RLC DL header and payload
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    boolean: TRUE when PL1 should call grlc_send_ph_data_ind()
===             FALSE when PL1 does not need to call grlc_ph_data_ind()
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
boolean grlc_is_rlc_dl_required_context
(
  gas_id_t    gas_id,
  const uint8 *data_block_ptr
);

/*===========================================================================
===
===  FUNCTION       E_GRLC_DL_FIFO_PUT_BLOCK()
===
===  DESCRIPTION
===
===   this function allows L1 to copy a downlink data block to the fifo.
===     After one or more blocks are copied to the fifo, L1 should send a
===
===
===  PARAMETERS
===
===    pl1_blk_ptr: points to PL1 structure which represents an element of
===                 the DL FIFO
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
grlc_dl_fifo_op_result_t  e_grlc_dl_fifo_put_block
(
  gas_id_t                  gas_id,
  const e_pl1_radio_block_t *pl1_blk_ptr
);

/*===========================================================================
===
===  FUNCTION      E_GRLC_DL_FIFO_GET_BLOCK
===
===  DESCRIPTION
===
===    Retrieve a downlink data block from the rlc dl fifo. If succesful,
===    the block is copied to the designated block pointer and the function
===    adjust the out index and counter and return DL_FIFO_GET_OK. If the
===    fifo is empty then DL_FIFO_EMPTY is returned.
===
===  PARAMETERS
===
===    pl1_blk_ptr: points to PL1 structure which represents an element of
===                 the DL FIFO
===
===  DEPENDENCIES
===
===    grlc_dl_fifo must be initialised and ready.
===
===  RETURN VALUE
===
===    grlc_dl_fifo_op_result_t type
===
===  SIDE EFFECTS
===
===    grlc_dl_fifo
===
===========================================================================*/
grlc_dl_fifo_op_result_t  e_grlc_dl_fifo_get_block 
(
  gas_id_t            gas_id,
  e_pl1_radio_block_t *pl1_blk_ptr
);


/*===========================================================================
===
===  FUNCTION      E_GRLC_IS_RLC_DL_REQUIRED_CONTEXT()
===
===  DESCRIPTION
===
===    This function is called by PL1 after a radio block is put into the
===    DL FIFO. It returns a boolean value which when TRUE requires PL1 to
===    call the grlc_send_ph_data_ind() to give RLC context to process and
===    flush the DL FIFO. This function ensures RLC DL only gains context
===    when it needs it in order to reduce context switches from PL1 to RLC DL.
===
===  PARAMETERS
===
===    crc_1/2: CRC status of each payload
===    fbi_e_1/2: FBI/E bits of each payload
===    esp: ES/P field
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===   Return TRUE if any of the below conditions is TRUE:
===   1. CRC of payload 1 is valid and either the FBI is set or Extension
===      indicates there is an octet following (0)
===   2. CRC of payload 1 is valid and either the FBI is set or Extension
===      indicates there is an octet following (0)
===   3. ES/P bits indicate polling (any values other than ESP_NO_POLL)
===   4. High water mark of DL FIFO is reached
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
boolean e_grlc_is_rlc_dl_required_context
(
  gas_id_t  gas_id,
  uint8     crc_1,
  uint8     crc_2,
  uint8     fbi_e_1,
  uint8     fbi_e_2,
  uint8     esp
);

/*===========================================================================
===
===  FUNCTION      grlc_dl_free_excess_dl_blocks()
===
===  DESCRIPTION
===
===   The function read the DL FIFO and deletes the excess DL data blocks
===   as the limit DL_BLIND_DATA_BLKS is exceeded to prevent memory leak.
===
===  DEPENDENCIES
===
===    none
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void  grlc_dl_free_excess_dl_blocks( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION      SHOW_BSN_AND_FIFO_VARS()
===
===  DESCRIPTION
===
===    Show the BSN value of the downlink data block and the associated 
===    internal DL FIFO variables
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
void show_bsn_and_fifo_vars ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      SHOW_BLOCK()
===
===  DESCRIPTION
===
===    Display contents of a DL data block by packing BSN, FBI, E, LI, M, E
===    fields into a three 32-bit hex numbers
===
===  DEPENDENCIES
===    
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void show_block (gas_id_t gas_id, grlc_dl_data_block_t *blk_ptr);

#endif /* _GRLCDLFIFO_H_ */

/*** EOF: don't remove! ***/
