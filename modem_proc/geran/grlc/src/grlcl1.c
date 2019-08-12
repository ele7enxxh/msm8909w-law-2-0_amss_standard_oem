/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC ACCESS FUNCTIONS FOR INTERFACING TO PORTABLE LAYER ONE
***
***
*** DESCRIPTION
***
***  This module provides the access function implementations to support the
***  RLC to PHY interface.
***
*** EXTERNALIZED FUNCTIONS
***
***  void grlc_send_ph_data_ind (void)
***  void grlc_send_ph_ready_to_send_ind (void)
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  none
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcl1.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03/21/01    hv     Created
*** 03/23/01    hv     RLC to PHY acces function(s).
*** 04/07/13    hv     CR461047 - Make safe usage of memcpy() and memmove() in GRLC
*** 20/08/13    hv     CR531564
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include <stringl.h>

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

#include "comdef.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "grlcl1.h"
#include "gprs_mem.h"
#include "grlcdli.h"
#include "grlcutil.h"
#include "geran_msgs.h"
#include "grlctsk.h"
#include "grlcdl.h"
#include "geran_tasks.h"
#include "geran_multi_sim.h"

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/




/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

/* Messaging resource and free list for L1 ISR to MAC messaging. */

#define GRLC_L1ISR_MAX_BUFS      12
#define GRLC_L1ISR_LOWATER_BUFS  ((GRLC_L1ISR_MAX_BUFS * 2) / 10) /* 20% say */

static pl1_rlc_msg_t  grlc_l1isr_mes_buf[NUM_GERAN_DATA_SPACES][ GRLC_L1ISR_MAX_BUFS ];
static q_type         grlc_l1isr_free_list[NUM_GERAN_DATA_SPACES];

/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/

static pl1_rlc_msg_t *gprs_rlc_l1isr_get_buffer( gas_id_t gas_id );

/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/




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




/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


/*===========================================================================
===
===  FUNCTION      gprs_rlc_l1isr_init_buffers()
===
===  DESCRIPTION
===  Initialises the free list of message buffers for use by L1 ISR context
===  when sending messages to RLC.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/

void gprs_rlc_l1isr_init_buffers( gas_id_t gas_id )
{
  uint32 ctr;

  (void)q_init( &grlc_l1isr_free_list[gas_id] );

  for ( ctr = 0; ctr < GRLC_L1ISR_MAX_BUFS; ctr++ )
  {
    (void)q_link( &grlc_l1isr_mes_buf[gas_id][ctr], &(grlc_l1isr_mes_buf[gas_id][ctr].link) );
    q_put( &grlc_l1isr_free_list[gas_id], &(grlc_l1isr_mes_buf[gas_id][ctr].link) );
  }
}

/*===========================================================================
===
===  FUNCTION      GRLC_GET_CMD_BUF_PL1()
===
===  DESCRIPTION
===
===   Return a pointer to a buffer for PL1 to build a message destined
===   for RLC (DL)
===
===
===  DEPENDENCIES
===
===   none
===
===  RETURN VALUE
===
===   pointer to a PL1-to-RLC message buffer
===
===  SIDE EFFECTS
===
===   none
===
===========================================================================*/
pl1_rlc_msg_t *grlc_get_cmd_buf_pl1 ( gas_id_t gas_id )
{
#if defined (DEBUG_GSM_GPRS_DS_UNIT_TEST) || defined (PERLUTF)
  #error code not present
#else /* (DEBUG_GSM_GPRS_DS_UNIT_TEST) || defined (PERLUTF) */
  /* Pre-loaded with NULL
  */
  pl1_rlc_msg_t     *pl1_rlc_msg_ptr = NULL;

  pl1_rlc_msg_ptr = gprs_rlc_l1isr_get_buffer(gas_id);

  if(pl1_rlc_msg_ptr == NULL)
  {
    ERR_GERAN_FATAL_0_G(" ERROR, Can't send msg to RLC ");
  }
  return(pl1_rlc_msg_ptr);
#endif
}

/*===========================================================================
===
===  FUNCTION      GRLC_PUT_CMD_PL1()
===
===  DESCRIPTION
===
===   Send the given message from PL1 to RLC UL or RLC DL tasks
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
void grlc_put_cmd_pl1 (gas_id_t gas_id, pl1_rlc_msg_t *msg_ptr)
{
  if (msg_ptr == NULL)
  {
    ERR_GERAN_FATAL_0_G("ERLC PL1 sends null-ptr-msg to RLC");
  }
  else
  {
    /* init q item by pointing to data area
    */
    (void)q_link(msg_ptr, &msg_ptr->link);

    /* Parse the signal according to UL or DL operation
    */
    if (msg_ptr->sig == PR_DL_EPDAN_REQ)
    {
      GRLC_DL_MSG_LOCK(gas_id);

      q_put(&DPQ(gas_id), &(msg_ptr->link));
      RLC_DL_SET_PL1_EPDAN_REQ_SIG(gas_id);

      GRLC_DL_MSG_UNLOCK(gas_id);
    }
    else
    {
      /* Unexpected PL1 signal
      */
      MSG_GRLC_ERROR_1_G(GRLC_F3_MSG_ENUM_103, "Invalid PL1 sig %d",msg_ptr->sig);
    }
  }
}


/*===========================================================================
===
===  FUNCTION      E_GRLC_DL_GET_WINDOW_INFO()
===
===  DESCRIPTION
===
===   Called by PL1 to obtain the beginning and the end
===   of the EGPRS DL window to be use for IR purposes.
===   The function parameter, pointer to erlc_dl_window_info_t, will be used
===   to return the DL window information.
===
===  DEPENDENCIES
===
===    none
===
===  RETURN VALUE
===
===    BOOLEAN - TRUE if RLC in ACK mode and either in transfer state
===    or pending release state
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
boolean e_grlc_dl_get_window_info( gas_id_t gas_id, e_grlc_dl_window_info_t *info_ptr )
{
  boolean   is_win_info_valid = FALSE;

  if( (dl[gas_id].state == DS_RX_ACK) || (dl[gas_id].state == DS_RX_ACK_PENDING_RELEASE) )
  {
    /* Get the beginning of the DL window */
    info_ptr->start_bsn = dl[gas_id].vq;

    /* Get the end of the DL window. This is ( V(Q) + dl_window_size ) % 2048 */
    info_ptr->end_bsn   = MOD_ESNS(dl[gas_id].vq + dl[gas_id].ws);
    is_win_info_valid   = TRUE;
  }
  return( is_win_info_valid );
}

#ifdef FEATURE_GSM_EGPRS_IR_WS_REDUCTION
/*===========================================================================
===
===  FUNCTION      grlc_dl_set_oper_win_size()
===
===  DESCRIPTION
===
===   Called by PL1 to set the EDGE IR operational window size
===   It used to decide when to set ms_out_of_mem based on
===   opertaional win size. This is only used in EDGE RLC ACK
===   mode and set when IR is enabled for DL TBF
===
===  DEPENDENCIES
===
===    none
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void grlc_dl_set_oper_win_size( gas_id_t gas_id, uint16 oper_win_sz)
{
  dl[gas_id].ir_oper_win_size = oper_win_sz;
  MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_104, "ERLD EDGE IR operational window size %d",dl[gas_id].ir_oper_win_size);
}
#endif /* FEATURE_GSM_EGPRS_IR_WS_REDUCTION */

#ifdef ERLD_DUMP_12BYTES_PLD
/*===========================================================================
===
===  FUNCTION      E_GRLC_DL_COPY_PLD_FOR_DUMP()
===
===  DESCRIPTION
===
===   A copy of the first 12 octets of the DL payload is obtained before padding
===   removal or shifting of the FBI & E bits. The copy of the payload will be dumped
===   during DL processing in RLC when an LI/E inconsistency is detected.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void e_grlc_dl_copy_pld_for_dump( gas_id_t gas_id, uint16* hdr_ptr, uint8* pld1_ptr, uint8* pld2_ptr  )
{
  static uint8 idx = 0; /* static_dual_spaced_ignore - debug fn */

  /* Copy the header */
  (void) memscpy( (void*) dl[gas_id].dump_buff[idx].hdr, sizeof(dl[gas_id].dump_buff[idx].hdr), (void*) hdr_ptr, 6 ); /* 3 of uint16  */

  /* Copy the first DUMP_PLD_LEN octets of the DL blocks   */
  (void) memscpy( dl[gas_id].dump_buff[idx].pld1, sizeof(dl[gas_id].dump_buff[idx].pld1), pld1_ptr, DUMP_PLD_LEN );
  (void) memscpy( dl[gas_id].dump_buff[idx].pld2, sizeof(dl[gas_id].dump_buff[idx].pld2), pld2_ptr, DUMP_PLD_LEN );

  idx = (idx + 1) % (GRLC_DL_PL1_FIFO_SIZE + 1);
}
#endif /* ERLD_DUMP_12BYTES_PLD  */


/*===========================================================================
===
===  FUNCTION      GRLC_SEND_PH_DATA_IND()
===
===  DESCRIPTION
===
===    Access function provided by RLC to allow PL1 to send the PH_DATA_IND
===    primitive to RLC DL task.
===
===    This is the only primitive sent to RLC DL task from PL1. A dedicated
===    signal mask is used for this purpose. The primitive contains no other
===    data hence a message is not required, only a signal needs to be set.
===
===  PARAMETERS
===
===    prim:   PH_DATA_IND
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
void grlc_send_ph_data_ind ( gas_id_t gas_id )
{
  if (!RLC_DL_IS_PL1_SIG_SET(gas_id))
  {
    RLC_DL_SET_PL1_SIG(gas_id);
  }
}

/*===========================================================================
===
===  FUNCTION      GRLC_SEND_BLOCK_END_IND()
===
===  DESCRIPTION
===
===    Access function provided by RLC to allow PL1 to indicate the end of 
===    block period.
===
===    This indication will be used to create log packet.
===
===  PARAMETERS
===
===    gas_id:   Current GAS ID
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
void grlc_send_block_end_ind(gas_id_t gas_id)
{
  if (!RLC_DL_IS_LOG_SIG_SET(gas_id))
  {
    RLC_DL_SET_LOG_SIG(gas_id);
  }

  if (!RLC_UL_IS_LOG_SIG_SET(gas_id))
  {
    RLC_UL_SET_LOG_SIG(gas_id);
  }
}


#ifndef PERLUTF
/*===========================================================================
===
===  FUNCTION      GRLC_SEND_PH_READY_TO_SEND_IND()
===
===  DESCRIPTION
===
===    Access function provided by RLC to allow PL1 to send the PH_DATA_IND
===    primitive to RLC UL task
===
===    This is the only primitive sent to RLC DU task from PL1. A dedicated
===    signal mask is used for this purpose. The primitive contains no other
===    data hence a message is not required, only a signal needs to be set.
===
===  PARAMETERS
===
===    prim:   PH_READY_TO_SEND_IND
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
void grlc_send_ph_ready_to_send_ind ( gas_id_t gas_id )
{
  if (!RLC_UL_IS_PL1_SIG_SET(gas_id))
  {
    RLC_UL_SET_PL1_SIG(gas_id);
  }
}
#endif
/*===========================================================================
===
===  FUNCTION      gprs_rlc_l1isr_get_buffer()
===
===  DESCRIPTION
===  Pull the next available message buffer off the free list.
===
===  DEPENDENCIES
===  None
===
===  RETURN VALUE
===  l1_mac_sig_t - type buffer pointer.
===  If no buffers are available then NULL is returned.
===
===  SIDE EFFECTS
===
===
===========================================================================*/

static pl1_rlc_msg_t *gprs_rlc_l1isr_get_buffer( gas_id_t gas_id )
{
  pl1_rlc_msg_t *sig_ptr = q_get( &grlc_l1isr_free_list[gas_id] );

  if ( q_cnt( &grlc_l1isr_free_list[gas_id] ) <= GRLC_L1ISR_LOWATER_BUFS )
  {
    MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_105, "GRLC: Get Static Mes Buf: Lo Water!");
  }

  return ( sig_ptr );
}

/*===========================================================================
===
===  FUNCTION      gprs_rlc_l1isr_free_buffer()
===
===  DESCRIPTION
===  Returns a free L1 ISR to RLC static messaging buffer to the free list.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/

void gprs_rlc_l1isr_free_buffer( gas_id_t gas_id, pl1_rlc_msg_t *sig_ptr )
{
  if ( sig_ptr != NULL )
  {
    (void)q_link( sig_ptr, &sig_ptr -> link );
    q_put( &grlc_l1isr_free_list[gas_id], &sig_ptr -> link );
  }
}


/*** EOF: don't remove! ***/

