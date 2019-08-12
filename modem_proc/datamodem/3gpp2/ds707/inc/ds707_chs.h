#ifndef DS707_CHS_H
#define DS707_CHS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 
                           D S 7 0 7 _ C H S

GENERAL DESCRIPTION

  This file contains functions that are used to implement the control/hold
  state processing.
  
  Control/Hold can happen during SO33 data calls.  The mobile may request
  to go into/come out of control/hold at anytime during a SO 33 call.  
  
  The base station controls when the mobile goes into/out of control hold.
  The mobile can only make REQUESTS to go into/come out of control hold.
  
  This module is tied to ds707_rrm.c, which also manages ds707_scrm.  That 
  means that SCRM and Control/Hold processing are somewhat related (for 
  example, if in control/hold, the mobile can either SCRM or send an 
  RRM (for control/hold) to come out of the control/hold state.
  
 
  By default, the mobile does not request to go into control/hold.  The user
  can configure this by setting AT$QCCHS = 0 - 255:
    0 = mobile never initiates/requests to go into control/hold
    1 - 255 = number of 20-msec frames of idleness (both tx and rx) before
              mobile asks to go into control/hold.
  
EXTERNALIZED FUNCTIONS
  DS707_CHS_TXED_DATA
    Called by RLP TX function.  Indicates if "real" data was transmitted in
    the past 20-msec.
    
  DS707_CHS_RXED_DATA
    Called by RLP RX function.  Indicates if "real" data was received in
    the past 20-msec.
    
  DS707_CHS_TOGGLE_CH
    Registered with MC at startup.  Called by MC when going into and out of 
    control hold state.  Input = TRUE means going into Control/Hold state.
  
  DS707_CHS_LEAVE_CHS
    Called by RLP Tx if there is any data or control frames which need to
    be sent.  Called every 20-msecs regardless if in/not in c/h state.
   
  DS707_CHS_BUILD_BLOB
    Called by ds707_rrm_build_blob, when MC asks data to build a BLOB.  This
    builds the actual BLOB requesting mobile to come out of control/hold
    state.
  
  DS707_CHS_RETRY_DELAY
    Called when a retry order is received for control/hold.
  
  DS707_CHS_CALL_INIT
    Called at the beginning of every SO 33 call, to clear all control/hold
    state variables.
    
  DS707_CHS_REG_FUNCS
    Called at startup, to register appropriate functions with MC.
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  At startup, must call ds707_chs_reg_funcs.  Currently called by
  ds707_rrm_reg_funcs() at startup..
  
  ds707_chs_call_init must be called at the beginning of every SO 33 call.
  Currently done via ds707_rrm_call_init.

  Implicit in this module is that rlp 3 calls ds_rrm_send_msg() every 20
  msec from both tx and rx.  This reduces complexity in this module.

 Copyright (c) 2002 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_chs.h_v   1.2   15 Nov 2002 17:55:22   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_chs.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------\
11/15/02   ak         Updated header comments.
07/12/02   ak         Include mccdma.h
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_IS2000_CHS

#include "mccdma.h"

/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/

/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_CHS_REG_FUNCS

DESCRIPTION   Causes module to register callbacks with MC.  Should only be
              called once, at startup.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_chs_reg_funcs(void);

/*===========================================================================
FUNCTION      DS707_CHS_TXED_DATA

DESCRIPTION   Called by TXC task, via RLP 3, every 20 msec.  Input param:

              data_was_txed - TRUE  -> data was transmitted this 20 msec
                              FALSE -> DTX or IDLE sent.  No relevant data.
                             
              Whenever the input is FALSE, then decrement the timer.  If
              the input is TRUE, then reset the timer.

DEPENDENCIES  Must be called every 20 msec.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_chs_txed_data
(
  boolean data_was_txed
);

/*===========================================================================
FUNCTION      DS707_CHS_RXED_DATA

DESCRIPTION   Called by RXC task, via RLP 3, every 20 msec.  Input param:

              data_was_rxed - TRUE  -> data was received this 20 msec
                              FALSE -> DTX or IDLE rx'ed.  No relevant data.
                             
              Whenever the input is FALSE, then decrement the timer.  If
              the input is TRUE, then reset the timer.
              
              If the timer expires, then send a message to MC which will
              cause MC to send a message to the BS.  This message will 
              request that we go to Control/Hold state.  As opposed to 
              leaving control/hold state, MC will not come back and ask us
              to build a BLOB.

DEPENDENCIES  Must be called every 20 msec.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_chs_rxed_data
(
  boolean data_was_rxed
);

/*===========================================================================
FUNCTION      DS707_CHS_CALL_INIT

DESCRIPTION   Called at beginning of a call to reset values.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_chs_call_init(void);

/*===========================================================================
FUNCTION      DS707_CHS_TOGGLE_CH

DESCRIPTION   Called by MC, when Control/Hold is turned on or off.  Two
              parameters:
                 TRUE - Going into Control/Hold State
                 FALSE - No longer in Control/Hold State.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_chs_toggle_ch
(
  boolean      in_chs_state
);

/*===========================================================================
FUNCTION     DS707_CHS_LEAVE_CHS

DESCRIPTION  Called by data, when it wants to send a msg to the BS to leave
             Control/Hold state.

DEPENDENCIES None.

RETURN VALUE None

SIDE EFFECTS None.
===========================================================================*/
void ds707_chs_leave_chs(void);

/*===========================================================================
FUNCTION     DS707_CHS_BUILD_BLOB

DESCRIPTION  Called by ds_rsrc_request_func, to see if an RRM should be
             built.

DEPENDENCIES None.

RETURN VALUE None

SIDE EFFECTS None.
===========================================================================*/
boolean ds707_chs_build_blob(void);

/*===========================================================================
FUNCTION     DS707_CHS_RETRY_PROCESSING

DESCRIPTION  Called to clear RRM timer.

DEPENDENCIES None.

RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void ds707_chs_retry_processing(void);

/*===========================================================================
FUNCTION     DS707_CHS_RETRY_DELAY

DESCRIPTION  Called by MC when a RRM retry delay is received.

DEPENDENCIES None.

RETURN VALUE None

SIDE EFFECTS None.
===========================================================================*/
void ds707_chs_retry_delay
(
  retry_delay_type retry_parm
);

#endif /* FEATURE_IS2000_CHS */
#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_CHS_H        */

