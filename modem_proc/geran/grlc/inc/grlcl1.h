/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC EXPORTED FUNCTIONS TO PORTABLE LAYER ONE
***
***
*** DESCRIPTION
***
***  This module provides access function(s) for PL1 to send primitives
***  to RLC UL and RLC DL tasks plus an RLC-timer service function for PL1 
***  to call when its block-period ISR occurs.
***
***  void grlc_send_ph_data_ind (void)
***  void grlc_send_ready_to_send_ind (void)
***  void grlc_service_block_timers (void)
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/inc/grlcl1.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03/21/01    hv     Created
*** mm/dd/yy   xyz
***
*****************************************************************************/

#ifndef INC_GRLCL1_H
#define INC_GRLCL1_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "dsm.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "gprs_pdu.h"
#include "gl1rlccmn.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

/* Turn this on to show the header and the first 12 bytes of all the the DL 
** payloads in the DL FIFO when an invalid LI/E combination is detected. Note  
** that the dump happens before padding removal and FBI & E bit shifting.
*/
#undef  ERLD_DUMP_12BYTES_PLD

/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/

/* This structure is used to return the dl window information to PL1 for IR 
** purposes via the function e_grlc_dl_get_window_info( )
*/
typedef struct
{
  /* Beginning of DL window */ 
  uint16  start_bsn;
  
  /* End of DL window. This is (start_bsn + dl_window_size) % 2048 */  
  uint16  end_bsn;
  
}e_grlc_dl_window_info_t;

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
===  FUNCTION      GRLC_IS_XFER_ACTIVE()
===
===  DESCRIPTION
===
===    Returns TRUE if RLC is in acknowledged or unacknowledged transfer state
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
extern boolean grlc_is_xfer_active ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      GRLC_UL_DATA_DETECT()
===
===  DESCRIPTION
===
===    This function is called by PL1 to aid detecting the "rlc no data"
===    condition. PL1 is to pass TRUE on a good access and FALSE on a bad one.
===
===  PARAMETERS
===    b: boolean value indicating whether a data block was succesfully obtained
===       or not
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void grlc_ul_data_detect (gas_id_t gas_id, boolean b);

/*===========================================================================
===
===  FUNCTION      GRLC_SEND_PH_DATA_IND()
===
===  DESCRIPTION
===
===    Access function provided by RLC to allow PL1 to send the PH_DATA_IND
===    primitive to RLC DL task
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
extern void grlc_send_ph_data_ind ( gas_id_t gas_id );

extern void grlc_send_block_end_ind(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      GRLC_SEND_PH_READY_TO_SEND_IND()
===
===  DESCRIPTION
===
===    Access function provided by RLC to allow PL1 to send the PH_DATA_IND
===    primitive to RLC UL task
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
extern void grlc_send_ph_ready_to_send_ind ( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION      GRLC_SERVICE_BLOCK_TIMERS()
===
===  DESCRIPTION
===
===    This function invokes the RLC UL data block timer service which is
===    active during RLC acknowledged transfer mode. PL1 is expected to
===    call this function every block period (18.464ms)
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
===
===  SIDE EFFECTS
===
===    Alters TX_ARR
===  
===========================================================================*/
extern void grlc_service_block_timers ( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION      GRLC_DL_START_T3192()
===
===  DESCRIPTION
===
===    This function is provided by RLC DL to PL1 so PL1 can start this timer 
===    when it sends the Packet Downlink Ack/nack with the FAI bit set.
===
===    This function checks the "b_allow_t3192_to_start" flag in RLC DL space
===    first. If it is set then T3192 is started and the flag is reset afterwards 
===    so PL1 can not accidentally start T3192 again until it set by RLC DL.
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
extern void grlc_dl_start_t3192 ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      GRLC_DL_STOP_T3192()
===
===  DESCRIPTION
===
===    Stop the T3192 timer if it is active.
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
extern void grlc_dl_stop_t3192 ( gas_id_t gas_id );

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
pl1_rlc_msg_t *grlc_get_cmd_buf_pl1 ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      GRLC_PUT_CMD_PL1()
===
===  DESCRIPTION
===
===   Send the given message from MAC to RLC UL or RLC DL tasks
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
void grlc_put_cmd_pl1 ( gas_id_t gas_id, pl1_rlc_msg_t *msg_ptr );

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

void gprs_rlc_l1isr_init_buffers( gas_id_t gas_id );
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
  
void gprs_rlc_l1isr_free_buffer( gas_id_t gas_id, pl1_rlc_msg_t *sig_ptr );

/*===========================================================================
===
===  FUNCTION      E_GRLC_DL_IS_PADDING_REMOVAL_REQUIRED()
===
===  DESCRIPTION
===
===    This is an access function provided for PL1 to call to determine if
===    padding octets removal is required given the pointer to the RLC header
===    and the MCS associated with it.
===
===    If this function returns TRUE then PL1 strips off the first 6 padding
===    octets from a payload.
===
===  PARAMETERS
===
===    uint8 *hdr_ptr: pointer to the header of the EGPRS RLC Downlink Data Block
===    uint8 mcs:     the modulation and coding scheme of the data block
===
===  DEPENDENCIES
===
===    none
===  
===  RETURN VALUE
===
===    boolean
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/
extern boolean e_grlc_dl_is_padding_removal_required 
(
  const uint8 *hdr_ptr, 
  uint8       mcs
);


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
boolean e_grlc_dl_get_window_info( gas_id_t gas_id, e_grlc_dl_window_info_t *info_ptr );

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
void grlc_dl_set_oper_win_size(gas_id_t gas_id, uint16 oper_win_sz);

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
===   The flag ERLD_DUMP_12BYTES_PLD needs to be defined in grlcdl.c
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
void e_grlc_dl_copy_pld_for_dump( uint16* hdr_ptr, uint8* pld1_ptr, uint8* pld2_ptr  );

#endif /* ERLD_DUMP_12BYTES_PLD  */


#endif /* INC_GRLCL1_H */

/*** EOF: don't remove! ***/
