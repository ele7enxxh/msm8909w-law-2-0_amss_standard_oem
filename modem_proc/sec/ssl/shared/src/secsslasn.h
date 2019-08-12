#ifndef SECSSLASN_H
#define SECSSLASN_H

#include "sec_variation.h"
#ifdef FEATURE_SEC_SSL

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

            A B S T R A C T   S Y N T A X   N O T A T I O N

              S S L   S P E C I F I C   F U N C T I O N S

GENERAL DESCRIPTION
  The Abstract Syntax Notation allows country and machine independent
  exchange of information.

EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/secsslasn.h_v   1.4   03 Sep 2003 11:08:22   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/ssl/shared/src/secsslasn.h#1 $ $DateTime: 2016/12/13 07:58:24 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/23/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "secerrno.h"

/* <EJECT> */ 
/*===========================================================================

              DEFINITIONS AND CONSTANTS FROM ASN.1

===========================================================================*/


/* <EJECT> */ 
/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/



/* <EJECT> */ 
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SECSSLASN_PARSE_CERT_LIST

DESCRIPTION
  This function parses a list of certificates sent by the SSL server.

DEPENDENCIES
  This function uses the secasn and ossl_asn functionality.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslasn_parse_cert_list
(
  secssl_session_type*  session_ptr,
  dsm_item_type**       msg_ptr,
  uint16                msg_len
);

#endif /* FEATURE_SEC_SSL */

#endif
