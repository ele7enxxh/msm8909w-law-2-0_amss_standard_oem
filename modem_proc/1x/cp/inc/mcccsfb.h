#ifndef MCCCSFB_H
#define MCCCSFB_H

/*===========================================================================

               E X T E R N A L   C S F B
                D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the CSFB operation.

  Copyright (c) 2010 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/19/12   ssh     Added SMS over CSFB support.
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
08/26/11   jj      CSFB: Support for GCSNA negotiation.
05/16/11   jj      CSFB: Added support to send GCSNA ack for incoming 1x
                   CRKT SRV message.
05/16/11   jj      CSFB: Added support to drop rxed msgs incase previous
                   access is in progress.
02/24/11   jj      Fixed changes to avoid multiple registrations.
01/27/11   jj      CSFB: Added changes to support power-up reg because of
                   band-class change when there is toggle between 1x -> LTE
                   modes.
01/03/11   jj      CSFB: cleaned up an unused function.
11/15/10   ssh     CSFB: added MO call support.
11/15/10   jj      CSFB: Fixed compiler warnings.
11/09/10   jj      CSFB  state redesign changes.
10/21/10   jj      Added Redirection support.
10/10/10   ssh     Declared mcc_csfb_is_psist_test_pass.
09/24/10   ssh     CSFB: Added retry and retransmission support.
09/23/10   jj      Added support for Reg context update.
08/18/10   jj      Added state level implementation.
08/02/10   jj      CSFB:Added support for CSFB state machine.
07/29/10   jj      CSFB:Created file to have external functions for LTE-1X CSFB
                   operation.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "modem_1x_defs.h"
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
#include "comdef.h"
#include "queue.h"
#include "customer.h"
#include "mcccsfbsup.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mc_v.h"
#include "mc_i.h"
#include "mci.h"
#include "mci_i.h"
/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Reason codes for entering MCC_CSFB_ACCESS state */
typedef enum {
  MCC_CSFB_ACCESS_REG,               /* Access is because of REG */
  MCC_CSFB_ACCESS_ORIG,              /* Access is because of ORIG */
  MCC_CSFB_ACCESS_PRM,               /* Access is because of PRM */
  MCC_CSFB_ACCESS_ORD_RES,               /* Access is because of ORD RES */
  MCC_CSFB_ACCESS_MSG,               /* Access is because of MSG */
  MCC_CSFB_ACCESS_GCSNA_MSG,         /* Access is because of GCSNA MSG */
  MCC_CSFB_ACCESS_NONE
} mcc_csfb_access_rsn_type;

extern mcc_csfb_access_rsn_type mcc_csfb_access_reason;

extern dword mcc_csfb_psist;

/* variable to track if ack is received for a transmitted message */
extern boolean mcc_gcsna_reg_ack_found;

/* Variable to track if there was a pending access in preogress */
extern boolean mcc_csfb_is_dl_gcsna_ack_pending;

/*===========================================================================
                        FUNCTIONS
===========================================================================*/
/*========================================================================
FUNCTION CDMA_CSFB_STATE

DESCRIPTION
  This procedure performs CSFB sub State machine operations.
  The two main sub-states while being in  CSFB mode are,
  CSFB_IDLE and CSFB_ACCESS states.

  For any transmission over Tunnel, CSFB ACCESS state will be
  entered, otherwise CP will remain in CSFB_IDLE state.

DEPENDENCIES
  None

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.
===========================================================================*/

extern word cdma_csfb_state( void );
/*===========================================================================

FUNCTION MCC_CSFB_STATE_INIT

DESCRIPTION
  This procedure performs init operations before entering into CSFB sub-states.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_csfb_state_init(void);

/*===========================================================================

FUNCTION CDMA_CSFB_IDLE

DESCRIPTION
  This procedure performs CSFB IDLE State operations and
  triggers to perform CSFB Tunnel Access Procedures upon
  hitting access conditions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void cdma_csfb_idle_state(void);

/*===========================================================================

FUNCTION CDMA_CSFB_ACCESS

DESCRIPTION
  This procedure performs CSFB ACCESS State operations and
  tunnels GCSNA messages to 1X n/w through LTE n/w.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the next state within the CSFB state machine to be processed.

SIDE EFFECTS
  None.
===========================================================================*/

void cdma_csfb_access_state( void );


/*===========================================================================

FUNCTION MCCSA_SEND_REGISTRATION_MSG

DESCRIPTION
  This function build a Registration Message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the access attempt was aborted, else FALSE.

SIDE EFFECTS
  None

===========================================================================*/
word mccsa_send_registration_msg
(
  word curr_state
    /* Current state being processed */
);

/*===========================================================================
FUNCTION MCC_CSFB_WAIT_FOR_ACCESS_RESPONSE
DESCRIPTION
DEPENDENCIES
  None.
RETURN VALUE
  Returns the next access state to be entered.
SIDE EFFECTS
  None.
FOOT NOTE:
next_access_rsn :
will determine what needs to be done after something happens in wait for access.
is_continue is a flag used to brk the loop for the cases of transmissions.
===========================================================================*/
void mcc_csfb_wait_for_access_response(void);

/*===========================================================================

FUNCTION MCCREG_CSFB_REG_INIT

DESCRIPTION
  This procedure performs init operations before entering into CSFB sub-states.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void mccreg_csfb_reg_init(void);

/*===========================================================================

FUNCTION MCC_CSFB_TXN_TIMER_EXPIRY

DESCRIPTION
  This function processes the timer expiry for any GCSNA msg transmission.
  It handles both retry backoff and retransmission timer expiry.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_csfb_txn_timer_expiry(dword sigs);

/*===========================================================================

FUNCTION MCC_CSFB_IS_PSIST_TEST_PASS

DESCRIPTION
  E-UTRAN provides PSIST values in the SIB8 [TS36.331]. When the PSIST values
  are set, the cdma2000 upper layer in the UE shall perform a persistence test
  only one time using a random value locally generated by the UE when the UE
  makes a voice call origination, SMS origination or registration.
  The persistence test procedure shall be based on C.S0003.

  The cdma2000 upper layers in the UE shall generate a random number RP,
  0 < RP < 1, using the technique described in  the "Pseudorandom Number
  Generator" C.S0005. The persistence test passes when RP is less than the value
  of P (P is PSIST which is obtained from SIB8).

  If the persistence test passes, the cdma2000 upper layer may trigger a voice call
  origination, SMS origination or registration. If the persistence test fails when
  the UE makes a voice call origination or SMS origination, the UE follows
  requirements configured by the operator.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_csfb_is_psist_test_pass(void);

/*===========================================================================

FUNCTION MCC_CSFB_STATE_EXIT

DESCRIPTION
  This procedure performs init operations before entering into CSFB sub-states.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_csfb_state_exit( void );

/*===========================================================================

FUNCTION MCC_CSFB_SEND_TUNNELED_ORM

DESCRIPTION
  This function build a Origination Message and send it to LTE for transmission.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_send_tunneled_orm (void);

/*===========================================================================

FUNCTION MCC_CSFB_SEND_ORD_RES

DESCRIPTION
  This function builds a Message for Orders/ any UL message and send it to LTE for
  transmission.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_send_ord_res(void);

/*===========================================================================
FUNCTION MCC_CSFB_SEND_GCSNA_ACK

DESCRIPTION
  This function builds a GCSNA ack with ack sequence same as msg seq of the
  msg which needs to be acked.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcc_csfb_send_gcsna_ack(byte gcsna_ack_seq);

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
#endif /* MCCCSFB_H */
