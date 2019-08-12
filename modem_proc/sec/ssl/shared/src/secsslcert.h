#ifndef SECSSLCERT_H
#define SECSSLCERT_H

/*==========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

             C L I E N T   C E R T I F I C A T E   H E A D E R

GENERAL DESCRIPTION

  This module contains the functions to interface with Embedded File 
  System (EFS) to handle the client certificate. The mobile 
  needs to be provisioned with a client certificate for  mututal 
  authentication.  The certificates are stored in EFS with
  compliance with the Distinguished Encoding Rules (DER) format. 

  During the intialization, certificate is read out, parsed and verified, 
  The file names are checked to conform with certain naming conventions. 

EXTERNALIZED FUNCTIONS

  secsslcert_init()
 
  secsslcert_read_certificate()

INITIALIZATION AND SEQUENCING REQUIREMENTS

  secsslcert_init() must be called before using the other functions.

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
==========================================================================*/

/*==========================================================================

                      EDIT HISTORY FOR FILE
 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/ssl/shared/src/secsslcert.h#1 $
 $DateTime: 2016/12/13 07:58:24 $
 $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ---------------------------------------------------------
06/21/10   ml      Changed local file size definitons to EFS2
04/10/10   yh      Support to read in client cert\key from session parameter
08/21/07   df      Added support for client cert chain
11/30/06   df/avm  CR100389 read certificate clears data before it is used
04/05/05   sv      Created the module to support EAP-TLS

==========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "secssli.h"  /* secssl_session_type */
#include "secssl.h"   /* secssl_ses_connection_end_enum_type */


/*--------------------------------------------------------------------------
                Modular Constants and definitions
--------------------------------------------------------------------------*/
#define SECSSLCERT_KEY_FIELD_NUM  4
#define SECSSLCERT_MAX_CONF_SIZE  0X7FFF

#define SECSSLCERT_KEY_START          "<KEY>"
#define SECSSLCERT_KEY_END            "</KEY>"
#define SECSSLCERT_CERT_START         "<CERT>"
#define SECSSLCERT_CERT_END           "</CERT>"
#define SECSSLCERT_ROOT_START         "<ROOT>"
#define SECSSLCERT_ROOT_END           "</ROOT>"

#define SECSSLCERT_CLIENT_DIR_NAME     "client-cert/"
#define SECSSLCERT_SERVER_DIR_NAME     "server-cert/"
#define SECSSLCERT_CONF_NAME           "cert.conf"

#define SECSSLCERT_EXT_NAME            ".der"

#define SECSSLCERT_START_TAG           '<'
#define SECSSLCERT_END_TAG             '>'

#define SECSSLCERT_DIR_MAX_LEN         50

/* the following macro are concatenated strings */
#define SECSSLCERT_CLIENT_CONF        SECSSLCERT_CLIENT_DIR_NAME SECSSLCERT_CONF_NAME
#define SECSSLCERT_SERVER_CONF        SECSSLCERT_SERVER_DIR_NAME SECSSLCERT_CONF_NAME

#define SECSSLCERT_MAX_NUM_CERTS 5  // max client chain len

/*==========================================================================

                      PUBLIC FUNCTION DECLARATIONS

==========================================================================*/

/*==========================================================================

FUNCTION SECSSLCERT_INIT

DESCRIPTION
  The function initialize the Authority Certificate. It reads out the
  CAs from the EFS under the specified cert directory, parse it, and 
  verify it. It then checks on the CA name to conform to a certain
  name convention, to speed up the searching for an issuing CA.

RETURN VALUE
  E_SUCCESS, if processing is successful.
  E_EFS_ERROR, if EFS returns an error.
  E_NO_DATA, if there is no CA provisioned in the EFS.
  E_FAILURE, other error reasons.
  
DEPENDENCIES
  None
==========================================================================*/
//extern secerrno_enum_type secsslcert_init( void );

/*==========================================================================

FUNCTION SECSSLCERT_READ_CERTIFICATE

DESCRIPTION
  This function reads out the certificate from the EFS under the specified 
  cert directory, parse it, and verify it. It put the cert or cert chain 
  data in session_ptr. 

RETURN VALUE
  E_SUCCESS, if processing is successful.
  E_EFS_ERROR, if EFS returns an error.
  E_NO_DATA, if there is no CA provisioned in the EFS.
  E_FAILURE, other error reasons.
  
DEPENDENCIES
  None
==========================================================================*/
secerrno_enum_type secsslcert_read_certificate
(
  secssl_session_type                    *session_ptr,
  secssl_ses_connection_end_enum_type    connection_end
);

/*==========================================================================

FUNCTION SECSSLCERT_READ_PRIVATE_KEY

DESCRIPTION
  The function initialize the client Certificate. It reads out the
  certificate from the EFS under the specified cert directory, parse it, and 
  verify it. 

RETURN VALUE
  E_SUCCESS, if processing is successful.
  E_EFS_ERROR, if EFS returns an error.
  E_NO_DATA, if there is no CA provisioned in the EFS.
  E_FAILURE, other error reasons.
  
DEPENDENCIES
  None
==========================================================================*/
secerrno_enum_type secsslcert_read_private_key
( 
   secssl_session_type                    *session_ptr,
   secssl_ses_connection_end_enum_type    connection_end
);

#endif /* SECSSLCERT_H */
