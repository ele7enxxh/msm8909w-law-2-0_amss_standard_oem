#ifndef SECSSLRX_H
#define SECSSLRX_H

#include "sec_variation.h"
#ifdef FEATURE_SEC_SSL

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                S E C U R E   S O C K E T S   L A Y E R

                  R E C E I V E   P R O C E S S I N G

GENERAL DESCRIPTION
  This file contains all SSL receive processing functions.

EXTERNALIZED FUNCTIONS
  secsslrx_read_v3_record  - Read a complete SSL record

INITIALIZATION AND SEQUENCING REQUIREMENTS
  secssli_init() has to be called before any other function.

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/secsslrx.h_v   1.5   30 Aug 2003 20:12:50   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/ssl/shared/src/secsslrx.h#1 $ $DateTime: 2016/12/13 07:58:24 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
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

FUNCTION SECSSLTX_READ_V3_RECORD

DESCRIPTION
  This function reads a complete SSL record. It blocks upon
  partial reveive until one record is complete.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr  - Pointer to open SSL session

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)

  Possible error conditions:
  E_WOULD_BLOCK   - No new data received yet
  E_RESET         - TCP connection was reset
  E_NOT_AVAILABLE - TCP connection was closed by peer
  E_NOT_ALLOWED   - SSL session is aborted
  E_FAILURE       - Unknown error occured

SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secsslrx_read_v3_record
(
  secssl_session_type*  session_ptr
);


/*===========================================================================

FUNCTION SECSSLRX_PARSE_HEADER

DESCRIPTION
  This function parses an SSL Record Header.

DEPENDENCIES

PARAMETERS
  session_ptr     - Pointer to open SSL session
  record_type_ptr - Pointer to record type (filled in by function)
  record_len_ptr  - Pointer to record length (filled in by function)

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)

  Possible error conditions:
  E_NO_DATA          - RX data pointer from session contains
                       insufficient data
  E_VERSION_MISMATCH - Received SSL version differs from the 
                       negotiated one

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslrx_parse_header
(
  secssl_session_type*  session_ptr,
  uint8*                record_type_ptr,
  uint16*               record_len_ptr
);


/*===========================================================================

FUNCTION SECSSLRX_DECRYPT

DESCRIPTION
  This function calls the SEC Crypto Layer to decrypt a complete
  SSL Record payload. It also verifies that the received MAC is valid.

DEPENDENCIES

PARAMETERS
  session_ptr    - Pointer to an open SLL session
  ciphertext_ptr - Pointer to the Ciphertext data
                   (consumed by function)
  plaintext_ptr  - Pointer to the Plaintext data
                   (appended to by function)
  ciphertext_len - Length of the Ciphertext data in bytes

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)

  Possible error conditions:
  E_NO_DSM_ITEMS - Out of DSM items
  E_FAILURE      - Unknown error
  See error codes from Crypto decrypt routines.

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslrx_decrypt
(
  secssl_session_type*  session_ptr,
  dsm_item_type**       ciphertext_ptr,
  dsm_item_type**       plaintext_ptr,
  uint16                ciphertext_len
);

#endif /* FEATURE_SEC_SSL */

#endif
