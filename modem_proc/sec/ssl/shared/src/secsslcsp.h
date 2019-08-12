#ifndef SECSSLCSP_H
#define SECSSLCSP_H

#include "sec_variation.h"
#ifdef FEATURE_SEC_SSL

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                S E C U R E   S O C K E T S   L A Y E R

           S S L   V 3   C I P H E R S P E C   P R O T O C O L

GENERAL DESCRIPTION
  This file contains the SSL Cipherspec protocol.

EXTERNALIZED FUNCTIONS
  secsslcsp_process_msg   - Process a received cipherspec message
  secsslcsp_send_change_cipherspec  - Send a cipherspec message

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/secsslcsp.h_v   1.5   03 Sep 2003 11:08:22   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/ssl/shared/src/secsslcsp.h#1 $ $DateTime: 2016/12/13 07:58:24 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/02/00   om      Several fixes (first functional version)
10/23/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "secerrno.h"

#include "secssldef.h"
#include "secssli.h"

/* <EJECT> */ 
/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/



/* <EJECT> */ 
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SECSSLCSP_PROCESS_MSG

DESCRIPTION
  This function processes a received cipherspec message.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr  - Pointer to an open SSL session
  msg_ptr      - Pointer to message data

RETURN VALUE
  None
  
SIDE EFFECTS
  The data for this message is consumed from the msg_ptr.
===========================================================================*/
extern void secsslcsp_process_msg
(
  secssl_session_type*  session_ptr, 
  dsm_item_type**       msg_ptr
);


/*===========================================================================

FUNCTION SECSSLHSP_SEND_CHANGE_CIPHERSPEC

DESCRIPTION
  This function sends a cipherspec message.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session structure

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secsslcsp_send_change_cipherspec
(
  secssl_session_type* session_ptr
);


#endif /* FEATURE_SEC_SSL */

#endif
