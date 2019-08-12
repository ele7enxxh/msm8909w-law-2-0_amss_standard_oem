/*****************************************************************************
***
***  GPRS MAC Radio Block handler
***
***
*** DESCRIPTION
***
***  For uplink control blocks MAC performs no segmentation. The sole function for
***  MAC in the uplink direction is to append the MAC header to the logical channel
***  message being sent to the network.
***
***  In the DL direction the control block handles has to administrate T3200,
***  Re-assembly segmented DL control messages, Removal and decoding of the bit
***  fields of the MAC headers and Organize up to 8 partially received DL control
***  messages (depending on the mode MAC is in).
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/inc/gmacrb.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 10-03-30   tjw     Explicitly include dependent headers to make SU API files
***                    stand alone
*** 07/05/01   tlx     Created
***
*****************************************************************************/

#ifndef INC_GMACRB_H
#define INC_GMACRB_H

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


#include "gmacmacsig.h"
#include "gmacsigl1.h"
#include "gmacsigl1_g.h"

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

/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      rb_block_handler
===
===  DESCRIPTION
===  This function  determines the DL logical channel msg that has been received
===  in PH_DATA_IND.
===
===  Determines the  "ctrl ack value " that's to be used in PCA messages (11.2.2)
===
===  If a valid RRBP is associated with the DL control msg send PCA to network.
===
===  For segmented msg's this is done via retrieve_dl_logical_channel_msg()
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
boolean rb_block_handler(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION     T3200_EXPIRY_HANDLER()
===
===  DESCRIPTION
===  T3200 is the timer thats used to control the segmented
===  downlink MAC control messages.
===
===  On Expiry the segmented message associted with the timer
===  is removed from the  list of segmented dl messages.
===
===  see 4.60, chapeter 9.1.11b  (1997)
===  see 4.60, chapeter 9.1.12b  (1999)
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
void t3200_expiry_handler(mac_mac_sig_id_t expired_timer, gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      rrbp_control_block_handler_2
===
===  DESCRIPTION
===  This function determines if a PCA is to be sent and the control ack value
===  to append to a PCA that's sent using the RRBP functionality.
===
===  Formats the PCA
===
===  Sends PCA in no erros occur.
===
===  For segmented dl msg:- the dl msg RLC?MAC control  header is used to check
===  if the MS is address correctly.
===
===  For un-segmented OR completely  re-assembled segmented Dl msg, the calling
===  function must only call the function if the dl msg address the MS correctly.
===
===  special cases.
===    PDA :- before PCA can be sent MAC has to check if PDA has TA info
===    PUAN : - before PCA can be sent MAC has to check if " FAI and EXT_TBF" is
===             set.
===
===
===  DEPENDENCIES
===
===    last_segmented_rrbp_info[].
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void rrbp_control_block_handler_2(rrbp_msg_t pca_direction, gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_rrbp_absolute_fn()
===
===  DESCRIPTION
===
===  calculates the absolute frame number relative to the RRBP value
===  received in the rlc/mac dl control msg header.
===
===  ref 4.60 chapter 10.4.5
===
===  note for rrbp values 01 and 10 ,there are two possible values
===       for the absolute fn. the two possible values differ by 1 fn
===       this is to accommodate the case where absolute fn coincides
===       with an idle frame.
===       Currently the lower value is calculated and it is up to l1 to
===       determine if the absolute fn considers with the an idle frame,
===       if so it adds the extra fn.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  absloute frame number
===
===  SIDE EFFECTS
===
===
===========================================================================*/
uint32 gmac_rrbp_absolute_fn(uint32 dl_frame_number, uint8 rrbp_value, gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_rrbp_send_pca()
===
===  DESCRIPTION
===  Compile and send PCA message to L1 using MAC_L1_PACCH_RRBP
===
===  If rrpb_present = TRUE then rrbp_value contains RRBP
===                         and fn contains dl frame number
===  If rrpb_present = FALSE then rrbp_value is not present
===                          and fn contains absolute fn
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
void gmac_rrbp_send_pca(uint32 fn, uint8 ts, boolean rrbp_present,uint8 rrbp_value,rrbp_msg_t direction,
                        gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_init_rb_data()
===
===  DESCRIPTION
===  Initialises rb data.  Used for Lower layer resets.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_rb_data( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION     T3200_RESET_HANDLER()
===
===  DESCRIPTION
===  Used when lower layer failure causes reset.
===  Clears whole buffer, freeing memory and clearing timers.
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
===
===========================================================================*/
void t3200_reset_handler( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION    gmac_init_rb_data_non_zero()
===
===  DESCRIPTION
===  Initialises non zero (including all enums) rb data.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_rb_data_non_zero( gas_id_t gas_id );

#endif /* INC_GMACRB_H */
/*** EOF: don't remove! ***/
