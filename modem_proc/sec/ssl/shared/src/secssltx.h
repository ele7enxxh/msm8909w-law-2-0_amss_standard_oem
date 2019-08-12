#ifndef SECSSLTX_H
#define SECSSLTX_H

#include "sec_variation.h"
#ifdef FEATURE_SEC_SSL

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                S E C U R E   S O C K E T S   L A Y E R

                 T R A N S M I T   P R O C E S S I N G

GENERAL DESCRIPTION
  This file contains all SSL transmit processing functions.

EXTERNALIZED FUNCTIONS
  secssltx_write        - Write application data to the SSL session
  secssltx_write_record - Write a signaling record to the session
  secssltx_nagle_timer_cb           - Nagle Mode timer callback
  secssltx_encrypt_pending_async_cb - Calback for async. execution

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/secssltx.h_v   1.6   30 Aug 2003 20:13:28   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/ssl/shared/src/secssltx.h#1 $ $DateTime: 2016/12/13 07:58:24 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/20/10   yh      Old clock API replacement
05/24/01   om      Added SSL session resumption and multiple session.
03/02/00   om      Several fixes (first functional version)
12/12/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#include "secssldef.h"
#include "secssl.h"
#include "secssli.h"

/* <EJECT> */ 
/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SECSSLTX_WRITE

DESCRIPTION
  This function is used by the application to write arbitrary amounts
  of data to be sent over the secure socket.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr  - Pointer to an open SSL session

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)

  Possible error conditions:
  See error codes for secssltx_encrypt_pending()
  
SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secssltx_write
(
  secssl_session_type*  session_ptr
);


/*===========================================================================

FUNCTION SECSSLTX_WRITE_RECORD

DESCRIPTION
  This function is used by the SSL signaling protocols to write a
  complete record for this protocol. If messages for the given
  protocol are already pending this function appends the nwe message
  before transforming it into an SSL record.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr  - Pointer to an open SSL session
  prot_id      - SSL Protocol Type for this record
  msg_ptr      - Pointer to signaling data 

RETURN VALUE
  Return value from secssltx_encrypt_pending()
  
SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secssltx_write_record
(
  secssl_session_type*          session_ptr,
  ssl_v3_protocol_id_enum_type  prot_id,
  dsm_item_type*                msg_ptr
);


/*===========================================================================

FUNCTION SECSSLTX_NAGLE_TIMER_CB

DESCRIPTION
  This function serves as a callback for the Nagle Mode Timer.

DEPENDENCIES
  None

PARAMETERS
  (see timer.h)

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void secssltx_nagle_timer_cb
(
  int32                           time_ms,
  timer_cb_data_type              data
);


/*===========================================================================

FUNCTION SECSSLTX_ENCRYPT_PENDING_ASYNC_CB

DESCRIPTION
  This function serves as a callback for asynchronous execution
  in the Crypto Layer.

DEPENDENCIES
  None

PARAMETERS
  _errno   - Outcome of Crypto layer operation
  usr_data - Used to store the session pointer

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void secssltx_encrypt_pending_async_cb
(
  secerrno_enum_type  _errno,
  void*               usr_data
);


/*===========================================================================

FUNCTION SECSSLTX_ENCRYPT_PENDING

DESCRIPTION
  This function takes a session's pending plaintext data and
  encrypts it. Used to generate the encrypted payload of an 
  SSL record.

DEPENDENCIES
  None    

PARAMETERS
  session_ptr  - Pointer to an open SSL session

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)

  Possible error conditions:
  E_INVALID_ARG   - Function was called w/o pending data
  E_NOT_ALLOWED   - Session aborted or CAPI not initialized
  E_ABORTED       - Execution timer expired for CAPI
  E_IN_PROGRESS   - A record encryption is already in progress
  E_RESET         - TCP connection was reset
  E_NOT_AVAILABLE - TCP connection was closed by peer
  E_NOT_SUPPORTED - Functionality not supported by CAPI
  E_FAILURE       - Unknown error

SIDE EFFECTS
  The CAPI semaphore for this session is blocked during execution
  of this function.
===========================================================================*/
secerrno_enum_type secssltx_encrypt_pending
(
  secssl_session_type* session_ptr
);


/*===========================================================================

FUNCTION SECSSLTX_CREATE_HEADER

DESCRIPTION
  This function creates an SSL record header and prepends it
  to the record payload pointer passed in.
  The record type is fetched from the first dsm item in the
  payload chain (item_ptr->kind).

DEPENDENCIES
  None

PARAMETERS
  session_ptr  - Pointer to an open SSL session
  rec_ptr      - Pointer to record payload
  rec_len      - Length of record payload in bytes  

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssltx_create_header
(
  secssl_session_type*  session_ptr,
  dsm_item_type**       rec_ptr,
  uint16                rec_len
);


/*===========================================================================

FUNCTION SECSSLTX_SEND_PENDING

DESCRIPTION
  This function sends a session's pending ciphertext data.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)

  Possible error conditions:
  E_NOT_AVAILABLE - Peer closed connection
  E_FAILURE       - Write() to dssocket failed

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssltx_send_pending
(
  secssl_session_type* session_ptr
);

#endif /* FEATURE_SEC_SSL */

#endif
