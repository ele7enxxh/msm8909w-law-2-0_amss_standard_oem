/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC UPLINK TEST
***
***
*** DESCRIPTION
***
*** 
***  This module contains definition of data and functions required for 
***  diagnostic purposes as well as functions to aid Host test and general debugging
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcultst.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 11/24/03    hv     Created
*** 08/03/04    hv     Added EGPRS code
***
*****************************************************************************/

#ifndef INC_GRLCULTST_H
#define INC_GRLCULTST_H

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
#include "grlcultypes.h"
#include "grlculfifo.h"
#include "queue.h"
#include "grlcdiag.h"
#include "memory.h"
#include "grlcul.h"
#include "grlcsgm.h"
#include "grlctx.h"
#include "grlculproc.h"


/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/

#ifndef GSM_GPRS_GRLU_SHOW_TX_VARS
#define GRLC_UL_SHOW_TX_VARS(gas_id)
#else
#define GRLC_UL_SHOW_TX_VARS(gas_id)  grlc_ul_show_tx_vars(gas_id)
#endif

#ifndef GSM_GPRS_GRLU_SHOW_BLOCK_SENT_TO_FIFO
#define GRLC_UL_SHOW_BLOCK_SENT(gas_id,bsn,state)
#else
#define GRLC_UL_SHOW_BLOCK_SENT(gas_id,bsn,state)  grlc_ul_show_block_sent(gas_id,bsn,state)
#endif

#ifndef GSM_GPRS_GRLU_CONFIRM_TX
#define GRLC_UL_SHOW_CONFIRM_TX(gas_id,bsn,state)
#define GRLC_UL_QUEUE_BLK_FOR_SHOW(gas_id,bsn,state)
#define GRLC_UL_SHOW_POST_TX_BLKS(gas_id)
#else
#define GRLC_UL_SHOW_CONFIRM_TX(gas_id,bsn,state)    grlc_ul_show_confirm_tx(gas_id,(uint16)bsn,state)
#define GRLC_UL_QUEUE_BLK_FOR_SHOW(gas_id,bsn,state) grlc_ul_queue_blk_for_show(gas_id,(uint16)bsn,state)
#define GRLC_UL_SHOW_POST_TX_BLKS(gas_id)            grlc_ul_show_post_tx_blks(gas_id)
#endif

#if (DSM_MAJOR_VER_NO == 2)
  #define DSM_POOL_STATS_MIN_FREE_CNT(pool_id)  dsmi_pool_mgmt_table[pool_id].stats_min_free_count
#else
  #define DSM_POOL_STATS_MIN_FREE_CNT(pool_id) (((dsm_pool_mgmt_table_type*)pool_id)->stats_min_free_count)
#endif


/*===========================================================================
===
===  FUNCTION      GRLC_UL_PACK_QUEUE_INFO
===
===  DESCRIPTION
===
===    Return LLC PDU queue count, number of PDUs held in 'ul' struct and 
===    number of octets as a packed 32 bit hex value
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===    uint32
===
===  SIDE EFFECTS
===
===========================================================================*/
uint32  grlc_ul_pack_queue_info ( gas_id_t gas_id );

#ifdef  GSM_GPRS_GRLU_SHOW_BLOCK_SENT_TO_FIFO
/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_BLOCK_SENT
===
===  DESCRIPTION
===
===     Show the block type, BSN, CV, payload length of the data block sent
===     to the UL FIFO
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_block_sent(gas_id_t gas_id, uint16 bsn, tx_ack_state_t ack_state);

#endif /* #ifdef  GSM_GPRS_GRLU_SHOW_BLOCK_SENT_TO_FIFO */

#ifdef GSM_GPRS_GRLU_SHOW_PAQ
/*===========================================================================
===
===  FUNCTION        GRLC_UL_SHOW_PAQ
===
===  DESCRIPTION
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_paq ( gas_id_t gas_id );

#endif /* GSM_GPRS_GRLU_SHOW_PAQ */

#ifdef GSM_GPRS_GRLU_CONFIRM_TX

/*===========================================================================
===
===  FUNCTION        GRLC_UL_INIT_POST_TX_SHOW_FIFO
===
===  DESCRIPTION     
===
===     Clear the post-tx-block-fifo
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===     
===========================================================================*/
void grlc_ul_init_post_tx_show_fifo ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION        GRLC_UL_QUEUE_BLK_FOR_SHOW
===
===  DESCRIPTION     
===
===     This function inserts the BSN and ack-status of a data block into the
===     post-tx-blk-fifo. This function is called from post_tx_actions(), ie.
===     from PL1's ISR such that the action of formatting and calling MSG_X()
===     are deferred from the ISR and executed in RLC's task context
===
===     Pushes BSN and ack_state into the global variable 
===     grlc_ul_post_tx_blk_fifo
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===     
===========================================================================*/
void grlc_ul_queue_blk_for_show
(
  gas_id_t        gas_id,
  uint16          bsn, 
  tx_ack_state_t  ack_state
);

/*===========================================================================
===
===  FUNCTION        GRLC_UL_SHOW_POST_TX_BLKS
===
===  DESCRIPTION     
===
===     This function shows all the data blocks queued up in the post-tx-blk-fifo
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===     
===========================================================================*/
void grlc_ul_show_post_tx_blks ( gas_id_t gas_id );

#endif  /* GSM_GPRS_GRLU_CONFIRM_TX */


/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_NEW_ALLOC
===
===  DESCRIPTION
===     Show the attributes of the new uplink assignment
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_new_alloc
(
  gas_id_t                gas_id,  
  const mr_ul_alloc_cnf_t *old_ptr,
  const mr_ul_alloc_cnf_t *ptr
);

/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_ALLOC_CNF_AWAIT_ASS
===
===  DESCRIPTION
===     Show the attributes of the uplink assignment during AWAIT_ASS state
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_alloc_cnf_await_ass (gas_id_t gas_id, const mr_ul_alloc_cnf_t *ptr);

/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_LLC_PDU
===
===  DESCRIPTION
===
===     Show the characteristics of LLC PDU received
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_llc_pdu_type ( gas_id_t gas_id, gprs_grr_ul_mes_t *ptr );

/*===========================================================================
===
===  FUNCTION      GRLC_UL_GET_LLC_PDU_INFO
===
===  DESCRIPTION
===
===     Returns the characteristics of LLC PDU received
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_get_llc_pdu_info ( gprs_grr_ul_mes_t *ptr, uint32 *lu, uint32 *l2 );

/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_TX_VARS
===
===  DESCRIPTION
===
===     Show the 6 Transmit Variables packed into three 32-bit hex numbers
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_tx_vars( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_FIFO_VARS
===
===  DESCRIPTION
===
===     Show Uplink FIFO variables
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_fifo_vars ( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION      grlc_ul_show_dsm_info
===
===  DESCRIPTION
===
===     Show DSM Memory info
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_dsm_info ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      SHOW_RBB
===
===  DESCRIPTION
===
===     Format the RBB array into uint32 such that the bitmap can be displayed
===     as two 32-bit hex numbers
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void show_rbb
(
  gas_id_t                gas_id, 
  const rm_acknack_desc_t *desc_ptr, 
  uint8                   fai
);

/*===========================================================================
===
===  FUNCTION      E_SHOW_RBB
===
===  DESCRIPTION
===
===     Format and display the EGPRS acknack desc in the most compact form
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void e_show_rbb (gas_id_t gas_id, const egprs_ack_nack_des_t *desc_ptr, uint8 fai);


/*===========================================================================
===
===  FUNCTION      grlc_ul_show_vars
===
===  DESCRIPTION
===
===     Show the main variables in RLC UL in verbose format
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_vars ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      grlc_ul_show_pdus
===
===  DESCRIPTION
===
===     List each of the uplink PDUs in the pending-ack queue, the temporary 
===     queue and the main queue. The PDUs are NOT removed from the queues
===     
===  DEPENDENCIES
===     When this function is called a soft recovery is about to be executed so
===     no state preservation is required
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_pdus ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      SHOW_GROUP_STATUS()
===
===  DESCRIPTION
===
===    Show PDU group status info
===
===  DEPENDENCIES
===
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void show_group_status (gas_id_t gas_id);

#if defined (DEBUG_GSM_GPRS_DS_UNIT_TEST) || defined (DEBUG_GSM_EGPRS_DS_UNIT_TEST) || defined (PERLUTF)
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST || DEBUG_GSM_EGPRS_DS_UNIT_TEST || PERLUTF */

extern void grlc_ul_diag_pdu_stat_add_entry(gprs_grr_ul_mes_t *ptr, const gas_id_t gas_id);
extern grlc_diag_ul_pdu_t* grlc_ul_diag_pdu_stat_remove_entry(const gas_id_t gas_id);
extern uint8 grlc_ul_diag_pdu_stat_get_count(const gas_id_t gas_id);
extern void grlc_ul_diag_block_stat_add_entry(grlc_ul_data_block_t *ptr, const gas_id_t gas_id);
extern grlc_diag_ul_block_t* grlc_ul_diag_block_stat_remove_entry(const gas_id_t gas_id);
extern uint8 grlc_ul_diag_block_stat_get_count(const gas_id_t gas_id);
extern void grlc_ul_diag_tx_stat_add_entry(grlc_ul_tx_array_t *ptr, const gas_id_t gas_id);
extern grlc_diag_ul_tx_t* grlc_ul_diag_tx_stat_remove_entry(const gas_id_t gas_id);
extern uint8 grlc_ul_diag_tx_stat_get_count(const gas_id_t gas_id);
extern void grlc_ul_diag_clear_stat(const gas_id_t gas_id);

#endif /* INC_GRLCULTST_H */

/*** EOF: don't remove! ***/
