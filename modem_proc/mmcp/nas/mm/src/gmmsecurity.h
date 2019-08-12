#ifndef GMMSECURITY_H
#define GMMSECURITY_H
/*===========================================================================

              GMM AUTHENTICATION PROCEDURE's HEADER FILE

DESCRIPTION
  This file contains data and function declarations necessary for the
  GMM Authentication procedure implementation .

Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:   L:/src/asw/MSM5200/mm/vcs/gmmsecurity.h_v 1.15   29 Apr 2002 17:04:14
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/gmmsecurity.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/30/01   ks      Added support for GMM Identity procedures.
02/07/03   jca     Added PERFORCE header.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"
#include "mmauth.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

typedef struct
{
  /* CKSN */
  byte            gprs_ck_seqnumber ;

  /* Received Ciphering Algorithm Identifier */
  byte            ciphering_algorithm ;

  /* Received RAND value */
  auth_rand_type  rand ;

  /* Received AUTN value */
  auth_autn_type  autn ;

  /* Authentication and Ciphering Request reference number */
  byte anc_reference_number ;

  /* IMEISV Required indicator */
  byte imeisv_required ;

  /* Force-To-Standby indicator */
  byte force_to_standby ;
} gmm_anc_request_message_T;

typedef struct gmm_authentication_message_buffer_tag
{
  mm_msg_head_T gmm_msg_head ;
  byte message_body[  1 + /* Protocol Discriminator and Skip Indicator */
                      1 + /* GMM Authentication Response Message type */
                      1 + /* A&C reference number and Spare half octet */
                      5 + /* RES */
                      11+ /* IMEISV */
                      14  /* Extended RES */
                   ] ;
} gmm_authentication_message_buffer_type ;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

/* The IMEISV included indicator, The indicator is equal to 1 (TRUE) when
** IMEISV shall be included into AUTHENTICATION & CIPHERING RESPONSE message
** and 0 (FALSE) otherwise
*/
extern byte gmm_imeisv_required_sim[MAX_NAS_STACKS] ;

/* This variable keeps AUTHENTICATION & CIPHERING RESPONSE message reference
** number.
*/
extern byte gmm_anc_reference_number_sim[MAX_NAS_STACKS] ;

#define  gmm_imeisv_required gmm_imeisv_required_sim[mm_as_id]
#define  gmm_anc_reference_number gmm_anc_reference_number_sim[mm_as_id]

#else

/* The IMEISV included indicator, The indicator is equal to 1 (TRUE) when
** IMEISV shall be included into AUTHENTICATION & CIPHERING RESPONSE message
** and 0 (FALSE) otherwise
*/
extern byte gmm_imeisv_required ;

/* This variable keeps AUTHENTICATION & CIPHERING RESPONSE message reference
** number.
*/
extern byte gmm_anc_reference_number ;

#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/


/*===========================================================================

FUNCTION GMM_HANDLE_SECURITY

DESCRIPTION
  This function routes the messages relating to GMM security to the
  appropriate function for detailed processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gmm_handle_security
(
  /* Incoming security related message */
  mm_cmd_type *message
) ;


/*===========================================================================

FUNCTION    GMM_CANCEL_AUTHENTICATION

DESCRIPTION
  This function is call whenever the dedicated connection has been teared down
  or canceled from any reason in order to stop possibly running Authentication
  server.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Stops internal mechanisms of the Authentication server.

===========================================================================*/
extern void gmm_cancel_authentication( void );


/*===========================================================================

FUNCTION    GMM_HANDLE_IDENTITY_REQUEST

DESCRIPTION
  This function is called whenever the UE receives the Identity Request
  from the Network. IN the GMM Identification procedure the Mobile
  station shall select the mobile Identity type which was specified by
  the Network. Refer to section 10.5.1.4

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void gmm_handle_identity_request
(
  mm_cmd_type *message
);
/*===========================================================================

FUNCTION    GMM_STOP_AUTH_FAIL_TIMERS

DESCRIPTION
   This function is called whenever Ps registration is failed in GRAT in order to stop
   T3320/T3318

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void gmm_stop_auth_fail_timers( void );

#endif /* #ifndef GMMSECURITY_H */
