#ifndef _HDRRLP_API_H
#define _HDRRLP_API_H
/*===========================================================================

             H D R   R L P   E X T E R N A L   H E A D E R  F I L E
                   
DESCRIPTION
   This file contains external variable and function declarations for the 
   High Data Rate (HDR) Radio Link Protocol.

   This should be included by files which interface with RLP, like the
   PCP and SLP.

Copyright (c) 2009 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/hdrrlp_api.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/11   vpk     HDR SU API cleanup
01/31/11   sa      Made changes for Software decoupling.
07/07/10   ms      Data statistics support for Field Test Display.
10/28/09   vs      Removed the inclusion of customer.h
09/04/09   ss      New file created for CMI SU level changes.

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rex.h"
#include "dsm.h"
#include "hdrcom_api.h"

/*===========================================================================

                           H D R R L P

===========================================================================*/

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

FUNCTION HDRRLP_QUEUE_MSG

DESCRIPTION
  This function builds up any pending RLP signaling messages and queues them
  directly to SLP, bypassing the conventional HMP layer queueing.  Based
  on the current state of RLP, the signaling message building can generate
  one of three signaling message types:

  RESET
    The current state of RLP is DSRLPI_RESET_STATE, in which the access 
    terminal is initiating an RLP reset.  Initialization processing is 
    performed during RX processing, which directs the TX processing to issue
    a RESET message.  RLP will remain in the RESET state until a RESET ACK 
    message has been received.

  RESET ACK
    The current state of RLP is DSRLPI_RESET_STATE, in which the access 
    terminal has received a RESET message from the access network.  In 
    response, the access terminal issues a RESET ACK signaling message, and 
    exits the DSRLPI_RESET state, into the DSRLPI_ESTABLISHED state.

  NAK
    The current state of RLP is DSRLPI_ESTABLISHED_STATE, in which the access
    terminal is issuing negative acknowledgement for outstanding RLP frames.
    
  In each case, the message is built-up, and passed directly to SLP.  SLP is
  responsible for de-allocating the dsm memory items used to build up the
  signaling message.

  In addition, since this function is called once per frame build interrupt,
  the function is used to update RLP timer values.  Timing information is 
  derived from the paramater time_since_prev_req.

DEPENDENCIES
  Assumes that a connection is open.  HMP need not know the message is being
  transmitted.

RETURN VALUE
  None

SIDE EFFECTS
  RLP signaling messages are queued on an SLP Tx queue.  

===========================================================================*/
void hdrrlp_queue_msg
(
  uint16 time_since_prev_req   /* increment of 26.667 ms since last tx req */
);

/*===========================================================================

FUNCTION HDRRLP_CLEAR_STATS

DESCRIPTION
  This function clears the RLP log statistics for all registered RLPs.
  
DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrlp_clear_stats
( 
  void
);

/*===========================================================================

FUNCTION HDRRLP_PROCESS_IND

DESCRIPTION
  This function processes indications given to HDRRLP.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Name of the indication for HDRRLP to process
  ind_data_ptr - pointer to data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrrlp_process_ind
(
  hdrcom_ind_name_enum_type ind_name,
  hdrcom_ind_data_union_type * ind_data_ptr
);

/*===========================================================================

FUNCTION HDRRLP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRRLP.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Indication to process, is a global enum for all
             possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrrlp_ind_cb
(
  hdrcom_ind_name_enum_type ind_name,
  void * ind_data_ptr
);

/*===========================================================================
FUNCTION      HDRRLP_CTA_EXP_SIG_HDLR

DESCRIPTION   This is the handler function for HDRRX_RLP_CTA_EXP_SIG, which is
              called from hdrrx task context. The signal is post from timer task
			  in the cta timer call back funtion, in order to void RPC call 
			  in timer context.
            
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlp_cta_exp_sig_hdlr(void);

/*===========================================================================

                           H D R R L P N A K

===========================================================================*/

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
==========================================================================*/

/*===========================================================================

FUNCTION       HDRRLPNAK_AGE_LISTS

DESCRIPTION    Go through all the nak lists and age them.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void hdrrlpnak_age_lists(void);

/*===========================================================================

                           H D R R L P D N A K

===========================================================================*/


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
==========================================================================*/

/*===========================================================================

FUNCTION       HDRRLPDNAK_AGE_LISTS

DESCRIPTION    Go through all the nak lists and age them.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void hdrrlpdnak_age_lists(void);


/*===========================================================================

                           H D R R L P T X Q

===========================================================================*/


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
==========================================================================*/

/*=========================================================================== 
 
FUNCTION       HDRRLPTXQ_TIMER_INIT
 
DESCRIPTION    Initializes the timer for txq
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void hdrrlptxq_timer_init
(
  void
);

/*===========================================================================

FUNCTION     HDRRLPTXQ_PROCESS_SIGNAL

DESCRIPTION    Function is called if the TXQ timer expires and sets a signal
               for the HDRTX task. This funcitons goes through all the RLP's 
               tranmsitted queues and remove all the items that have their 
               nak_abort timer expired. 

 Adv: 1.This will clear up all the items in the transmitted queue which 
        are no longer needed as the AN is not suppose to request for these 
        items beyond the Nak Abort timer negotiated for that RLP.
      2.Saves the system memory by releasing the unwanted DSM items 
        in the old RLP frames.
      3.Guarantees to find an item in the transmitted queue.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrrlptxq_process_signal
( 
  rex_sigs_type signal 
);

/*=========================================================================== 
FUNCTION       HDRRLPSTATS_PROCESS_SIG 
 
DESCRIPTION    Function is called if the timer task sets a signal
               for the HDRTX task.  This function calls the corresponding
               signal handler.
                
DEPENDENCIES   NONE 
 
PARAMETERS     Rex Sig type - Signal.

RETURN VALUE   NONE 
 
SIDE EFFECTS   NONE

===========================================================================*/ 
void hdrrlpstats_process_sig
( 
  rex_sigs_type signal
); /* hdrrlpstats_process_sig() */

/*===========================================================================
FUNCTION HDRRLP_LOG_SIG_HDLR

DESCRIPTION
    This function is called to handle HDRRX_RLP_LOGGING_SIG from HDRRX

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrlp_log_sig_hdlr(void);

#endif  /* _HDRRLP_API_H */

