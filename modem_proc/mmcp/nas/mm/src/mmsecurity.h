#ifndef MMSECURITY_H
#define MMSECURITY_H
/*===========================================================================

              GMM AUTHENTICATION PROCEDURE's HEADER FILE

DESCRIPTION
  This file contains data and function declarations necessary for the
  GMM Authentication procedure implementation .

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/
/*==========================================================================  
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmsecurity.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $ 
 
when       who     what, where, why
--------   ---     --------------------------------------------------------- 
 
=============================================================================*/
/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "mmtask.h"
#include "mmtask_v.h"
#include "mm_mm_v.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define IEI_AUTH_RSP_EXT 0x21

#define FIELD_NOT_IN_USE 0xFF

typedef struct mm_authentication_message_buffer_tag
{
  mm_msg_head_T gmm_msg_head ;
  byte message_body[1 + /* Protocol Discriminator and Skip Indicator */
                    1 + /* MM Authentication Response Message type */
                    4 + /* RES */
                    14  /* Extended RES */] ;
} mm_authentication_message_buffer_type ;


/*===========================================================================

FUNCTION MM_HANDLE_SECURITY

DESCRIPTION
  This function routes the messages relating to security to the
  appropriate function for detailed processing

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_handle_security
(
  mm_cmd_type *msg_p
) ;


/*===========================================================================

FUNCTION MM_CANCEL_AUTHENTICATION

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_cancel_authentication
(
  void
) ;


/*===========================================================================

FUNCTION MM_PERFORM_REJECT_ACTIONS

DESCRIPTION
  This function responds to the authentication reject indication from
  the network. The function is activated upon reception of the Authentication 
  Reject message by the GMM module or by the Auth server. 
  The rection to this is similar to that when a reject
  is received with cause 6, illegal ME.
  If an MM connection exists, then the CNM is requested to abort all
  connections,after which it will send MMCNM_REL_REQ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mm_perform_reject_actions
(
  rrc_cn_domain_identity_e_type domain_id
);

#endif /* #ifndef MMSECURITY_H */
