#ifndef TM_LPP_CP_TRANSPORT_H
#define TM_LPP_CP_TRANSPORT_H
/*=============================================================================

                TM LPP CP Reliable Transport Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes 
  relevant for the TM LPP CP Reliable Transport

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/lpp_cp/inc/tm_lpp_cp_transport.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  03/29/16    kpn     Use of subscription ID while handling NAS DL message for LPP CP
  10/27/14    rk      Added LPP RSTD >24 measurement processing support
  2012-02-10  rh      Initial version

=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "tm_common.h"

/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/

/* error number definitions for LPP CP TRANSPORT errors, can define more detailed 
   error codes if necessary  */

#define LPP_CP_TRANSPORT_SUCCESS               (0)     /* success */

#define LPP_CP_TRANSPORT_NOT_ENABLED_ERROR     (-1)    /* LPP CP transport not enabled */
#define LPP_CP_TRANSPORT_INVALID_PARAMS_ERROR  (-2)    /* invalid parameters */
#define LPP_CP_TRANSPORT_SEND_ERROR            (-3)    /* generic send error */
#define LPP_CP_TRANSPORT_RECEIVE_ERROR         (-4)    /* generic receive error */
#define LPP_CP_TRANSPORT_SEND_MAXRETRY_ERROR   (-5)    /* error: maximum retry but still fail to send */

#define LPP_CP_TRANSPORT_MAX_PKT_SIZE          (65536) /* max LPP packet size 64K */
#define LPP_CP_TRANSPORT_MAX_OUTSTANDING_PKT   (1)     /* max one un-ACKed LPP pkt */
#define LPP_CP_TRANSPORT_MAX_RETRY_COUNT       (3)    /* will retry up to 3 times */

#define LPP_CP_TRANSPORT_NAS_ID_ANY            (0xFFFFFFFF)  /* will match any NAS ID */
#define LPP_CP_TRANSPORT_TRANS_ID_ANY          (0xFFFFFFFF)  /* will match any trans ID */


// timer callback will be called from LPP CP protocol module
#define LPP_CP_TRANSPORT_TIMEOUT_TIMER_ID      (TM_LPP_CP_TIMER_ID_RANGE+0x100)

/*=============================================================================

       Macros

=============================================================================*/


/*=============================================================================

       Typedefs

=============================================================================*/

/* LPP CP Transport's error reporting callback */
typedef void (lpp_cp_transport_cb_type_f)(
  int32  error,      /* error number defined above */
  uint32 nas_id,     /* NAS Routing ID for associated outgoing LPP packet */
  uint32 trans_id,   /* Transaction ID for associated outgoing LPP packet */
  uint8  seq_num     /* Sequence Number for associated outgoing LPP packet */
);

/* LPP CP Transport's reset type */
typedef enum 
{
  LPP_CP_TRANSPORT_RESET_ALL = 0,   /* reset all packets */
  LPP_CP_TRANSPORT_RESET_NAS_ID,    /* reset packets with this NAS routing ID */
  LPP_CP_TRANSPORT_RESET_TRANS_ID   /* reset packets with this Transaction ID */
}lpp_cp_transport_reset_e_type;


/*=============================================================================

       Function prototypes

=============================================================================*/

/*=============================================================================

FUNCTION 
  tm_lpp_cp_transport_init()

DESCRIPTION
  tm_lpp_cp_transport_init() initializes the LPP CP transport module
  (caller need to supply an error reporting callback function, mainly for
  LPP send timeout error reporting using LPP_CP_TRANSPORT_SEND_ERROR)

DEPENDENCIES
  None.

PARAMETERS
  enabled - TRUE to enable, FALSE to disable
  report_error_cb - function pointer for transport error reporting callback

RETURN VALUE
  Boolean: TRUE for success, FALSE for failure

SIDE EFFECTS
  None.

=============================================================================*/
boolean tm_lpp_cp_transport_init (
  boolean enabled, 
  lpp_cp_transport_cb_type_f *report_error_cb
);


/*=============================================================================

FUNCTION 
  lpp_transport_send()

DESCRIPTION
  lpp_transport_send() sends OTA LPP packet to NAS
  (For non-ACK-only packet, LPP CP transport will queue this OTA LPP
  packet and will try to do retransmission if no ACK is recevied)
    
DEPENDENCIES
  LPP Transport should be enabled before this call

PARAMETERS
  lpp_pkt - pointer to encoded LPP packet
  pkt_size - LPP packet size
  nas_id - NAS Routing ID
  trans_id - Transaction ID
  seq_num - Sequence number
  ack_only - TRUE if this is ACK-only packet, otherwise FALSE

RETURN VALUE
  boolean: TRUE for successul send (queued in transport), FALSE for failure

SIDE EFFECTS
  None.

=============================================================================*/
boolean tm_lpp_cp_transport_send (
  uint8 *lpp_pkt, 
  uint32 pkt_size, 
  uint32 nas_id, 
  uint32 trans_id, 
  uint8 seq_num, 
  boolean ack_only, 
  sys_modem_as_id_e_type as_id
);


/*=============================================================================

FUNCTION 
  tm_lpp_cp_transport_receive()

DESCRIPTION
  tm_lpp_cp_transport_receive() should be called to get LPP OTA packets
  (will return 0 if no LPP packet is currently available, this is not an
   error case)
  
DEPENDENCIES
  LPP Transport should be enabled before this call

PARAMETERS
  lpp_buf - pointer to memory buffer for LPP message (caller supplied)
  buf_size - buffer size for LPP memory buffer
  nas_id - LPP Transport will return NAS Routing ID 

RETURN VALUE
  int32: 0 for no packet, positive number for received LPP packet length
         negative number for error

SIDE EFFECTS
  None.

=============================================================================*/
int32 tm_lpp_cp_transport_receive (
  uint8 *lpp_buf, 
  uint32 buf_size, 
  uint32 *nas_id, 
  sys_modem_as_id_e_type *inst_id
);


/*=============================================================================

FUNCTION 
  tm_lpp_cp_transport_notify_received_ack()

DESCRIPTION
  tm_lpp_cp_transport_notify_received_ack() should be called to notify LPP
   transport about received ACK
  
DEPENDENCIES
    LPP Transport should be enabled before this call

PARAMETERS
  nas_id - NAS Routing ID
  trans_id - Transaction ID
  seq_num - Sequence Number

RETURN VALUE
  Boolean: TRUE for success(found a match for unacknowledged packet), 
           FALSE for failure

SIDE EFFECTS
  None.

=============================================================================*/
boolean tm_lpp_cp_transport_notify_received_ack (
  uint32 nas_id, 
  uint32 trans_id, 
  uint8 seq_num
);


/*=============================================================================

FUNCTION 
  tm_lpp_cp_transport_reset()

DESCRIPTION
  tm_lpp_cp_transport_reset() should be called to reset outgoing packets of
  LPP CP transport
  
DEPENDENCIES
  None.

PARAMETERS
  reset_type - reset type (all or NAS_ID or Trans_ID)
  nas_id - NAS Routing ID  (optional)
  trans_id - Transaction ID (optional)

RETURN VALUE
  Boolean: TRUE for success, FALSE for failure

SIDE EFFECTS
  None.

=============================================================================*/
boolean tm_lpp_cp_transport_reset (
  lpp_cp_transport_reset_e_type reset_type, 
  uint32 nas_id, 
  uint32 trans_id
);


/*=============================================================================

FUNCTION 
  tm_lpp_cp_transport_handle_timer_expiry()

DESCRIPTION
  tm_lpp_cp_transport_handle_timer_expiry() should be called to handle LPP CP
  transport's timer expiration
  
DEPENDENCIES
  None.

PARAMETERS
  timer_id - LPP CP's timer ID
 
RETURN VALUE
  Boolean: TRUE for success, FALSE for failure

SIDE EFFECTS
  None.

=============================================================================*/
boolean tm_lpp_cp_transport_handle_timer_expiry (uint32 timer_id);


#ifdef __cplusplus
}
#endif

#endif /* TM_LPP_CP_TRANSPORT_H */

