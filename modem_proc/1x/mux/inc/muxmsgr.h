#ifndef MUXMSGR_H
#define MUXMSGR_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          M U X  M E S S A G E  R O U T E R  H E A D E R   F I L E


GENERAL DESCRIPTION
      This file contains external interfaces and declarations for the
      mux message router module.

EXPORTED FUNCTIONS:
      muxmsgr_init
      muxmsgr_register_msg
      muxmsgr_deregister_msg
      muxmsgr_register_1xdemod_msgs
      muxmsgr_deregister_1xdemod_msgs
      muxmsgr_process_rcv_msg
      muxmsgr_send_msg
      muxmsgr_stop

INITIALIZATION AND SEQUENCING REQUIREMENTS
      Must call muxmsgr_init() before any other functions in this module
      are invoked.

 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  Copyright (c) 2009 - 2015 Qualcomm Technologies, Inc.
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

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/mux/inc/muxmsgr.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/15   eye     Added initial changes for 1x/1xA DRX feature for THOR.
01/06/15   srk     Wait for TX start/stop response before sending ASDIV command
11/07/14   srk     Removed optimized Tx Trigger featurization around Tx Trigger
                   related functions.
10/08/14   srk     FEATURE_MODEM_1X_UTX related cleanup.
02/13/14   dkb     Add support for optimized Tx trigger.
10/09/13   dkb     Add additional RL changes.
09/30/13   dkb     Initial RL changes for new FW interface.
08/18/11   adw     Mainline 1x message router support.
08/05/10   vks     Move TX, RX MSGR module definitions from muxmsgr.h to
                   onex_msgr.h
05/17/10   vks     Replace MAX_NUM_TASKS with NUM_MUX_TASKS to fix lint error.
11/12/09   vks     Add MSGR/Q6 framework support.
09/09/09   vks     Initial implementation.

==========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/* Common */
#include "comdef.h"
#include "customer.h"
#include "modem_1x_defs.h"


/* FW */
#include "cdmafw_msg.h"

/* Message Router Includes*/
#include "msgr_umid.h"
#include "msgr.h"



/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* Enum for MUX TASKS */
typedef enum
{
  TX_TASK        = 0,
  RX_TASK        = 1,
  NUM_MUX_TASKS  = 2,
} muxmsgr_tasks_enum_type;



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

FUNCTION       MUXMSGR_INIT

DESCRIPTION    This function initializes the receive queue and registers the
               1x MUX Task client with the message router.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void muxmsgr_init
(
  muxmsgr_tasks_enum_type    task                /* MUX Task               */
);

/*===========================================================================

FUNCTION       MUXMSGR_REGISTER_MSG

DESCRIPTION    This function registers a message that MUX is interested in
               collecting from the message router.

DEPENDENCIES   1X MUX client setup with the MSGR must have been completed.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void muxmsgr_register_msg
(
  muxmsgr_tasks_enum_type    task,               /* MUX Task               */
  msgr_umid_type             umid                /* UMID of the message    */
);

/*===========================================================================

FUNCTION       MUXMSGR_DEREGISTER_MSG

DESCRIPTION    This function deregisters a message that MUX is no longer
               interested in receving from the message router.

DEPENDENCIES   1X MUX client setup with the MSGR must have been completed.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void muxmsgr_deregister_msg
(
  muxmsgr_tasks_enum_type    task,               /* MUX Task               */
  msgr_umid_type             umid                /* UMID of the message    */
);

/*===========================================================================

FUNCTION       MUXMSGR_REGISTER_1XDEMOD_MSGS

DESCRIPTION    This function registers the demod messages that mux is
               interested in receiving from 1X DEMOD FW (cdmafw_msg.h).

DEPENDENCIES   Must be called after 1X DEMOD APP is successfully enabled.

RETURN VALUE   None.

SIDE EFFECTS   MUX starts receiving the registered messages from 1X DEMOD FW.

===========================================================================*/
extern void muxmsgr_register_1xdemod_msgs
(
  muxmsgr_tasks_enum_type    task                /* MUX Task               */
);

/*===========================================================================

FUNCTION       MUXMSGR_DEREGISTER_1XDEMOD_MSGS

DESCRIPTION    This function de-registers the demod messages that mux is no
               longer interested in receiving from 1X DEMOD FW.

DEPENDENCIES   Preferable to call after the 1X DEMOD APP has been disabled.

RETURN VALUE   None.

SIDE EFFECTS   MUX stops receiving the registered messages from 1X DEMOD FW.

===========================================================================*/
extern void muxmsgr_deregister_1xdemod_msgs
(
  muxmsgr_tasks_enum_type    task                /* MUX Task               */
);

/*===========================================================================

FUNCTION       MUXMSGR_PROCESS_RCV_MSG

DESCRIPTION    This function processes the received message.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void muxmsgr_process_rcv_msg
(
  muxmsgr_tasks_enum_type    task                /* MUX Task               */
);

/*===========================================================================

FUNCTION       MUXMSGR_SEND_MSG

DESCRIPTION    This function initializes the header and sends the message to
               the message router.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void muxmsgr_send_msg
(
  muxmsgr_tasks_enum_type    task,               /* MUX Task               */
  msgr_hdr_struct_type*      msg_hdr_ptr,        /* Pointer to MSG Header  */
  msgr_umid_type             umid,               /* UMID of the message    */
  int32                      size                /* Size of the message    */
);

/*===========================================================================

FUNCTION       MUXMSGR_SET_START_RSP_FLAG

DESCRIPTION    This function sets the flag to indicate that TX
               is waiting for start response from FW.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void muxmsgr_set_start_rsp_flag( void );

/*===========================================================================

FUNCTION       MUXMSGR_SET_STOP_RSP_FLAG

DESCRIPTION    This function sets the flag to indicate that TX
               is waiting for stop response from FW.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void muxmsgr_set_stop_rsp_flag( void );

#ifndef FEATURE_MODEM_1X_SUPPORTS_RF
/*===========================================================================

FUNCTION       MUXMSGR_WAIT_ON_START_RSP

DESCRIPTION    This function waits until the tx start response message is
               processed. While waiting, it processes any other messages
               that might be received.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void muxmsgr_wait_on_start_rsp( void );
#endif /* FEATURE_MODEM_1X_SUPPORTS_RF */

/*===========================================================================

FUNCTION       MUXMSGR_WAIT_ON_STOP_RSP

DESCRIPTION    This function waits until the txfe stop response message is
               processed. While waiting, it processes any other messages
               that might be received.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void muxmsgr_wait_on_stop_rsp( void );

/*===========================================================================

FUNCTION       MUXMSGR_WAITING_FOR_TX_RESPONSE_FROM_FW

DESCRIPTION    This function checks if TX is stuck waiting for
               start/stop response from FW.

DEPENDENCIES   None.

RETURN VALUE   TRUE  - If waiting for FW response.
               FALSE - Otherwise

SIDE EFFECTS   None.

===========================================================================*/
extern boolean muxmsgr_waiting_for_tx_response_from_fw( void );

/*===========================================================================

FUNCTION       MUXMSGR_IS_TX_TRIGGER_RSP_RECEIVED

DESCRIPTION    This function checks to see if the TxFE/TxDAC
               trigger response has been received.

DEPENDENCIES   None.

RETURN VALUE   TRUE  - received TXFE/TXDAC rsp
               FALSE - waiting for TxFE/TxDAC rsp

SIDE EFFECTS   None.

===========================================================================*/
extern boolean muxmsgr_is_tx_trigger_rsp_received( void );

/*===========================================================================

FUNCTION       MUXMSGR_STOP

DESCRIPTION    This function de-registers the MUX client from the message
               router. We would no longer be able to send/receive messages.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void muxmsgr_stop
(
  muxmsgr_tasks_enum_type    task              /* MUX Task                 */
);

#ifdef FEATURE_MODEM_1X_DRX
/*===========================================================================

FUNCTION       MUXMSGR_WAIT_ON_DRX_RSP

DESCRIPTION    This function waits until the drx response message is
               processed. While waiting, it processes any other messages
               that might be received.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void muxmsgr_wait_on_drx_rsp( void );
#endif /* FEATURE_MODEM_1X_DRX */

#endif /* MUXMSGR_H */
