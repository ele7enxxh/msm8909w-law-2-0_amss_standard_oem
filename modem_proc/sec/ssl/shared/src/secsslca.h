#ifndef SECSSLCA_H
#define SECSSLCA_H

#include "sec_variation.h"
#ifdef FEATURE_SEC_SSL
/*==========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

        C E R T I F I C A T E    A U T H O R I T I E S    H E A D E R

GENERAL DESCRIPTION

  This module contains the functions to interface with Embedded File 
  System (EFS) to handle the Certificate Authority (CA). The mobile 
  needs to be provisioned with a number of trusted CAs in order to 
  verify the server's certificate. The CAs are stored in EFS with
  compliance with the Distinguished Encoding Rules (DER) format. 

  During the intialization, CAs are read out, parsed and verified, 
  one by one. The file names are checked to conform with certain
  naming conventions. The file names are stored in a file name array
  so that it can speed up the searching for an issuing CA. 

EXTERNALIZED FUNCTIONS

  secsslca_init()

  secsslca_read_certificate()

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
==========================================================================*/

/*==========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/ssl/shared/src/secsslca.h#1 $
  $Date: 2016/12/13 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ---------------------------------------------------------
06/08/11   yh      Get available time for x509 cert validation
06/21/10   ml      Removed references to EFS1
11/12/04   rv      changes to correct LINT errors
07/29/03   rwh     Added state indicator for CA initialization.
02/18/03   jay     Changed a feature name
09/30/02   om      Added symlink param to secca_update_certificate().
05/18/01   yll     Several fixes.
04/24/01   yll     Initial file.

==========================================================================*/
#include "comdef.h"
#include "fs_config.h"
#include "secx509.h"
#include "secssl.h"

#define SECSSLCA_MAX_ROOT_FOLDER     5
#define SECSSLCA_MAX_NUM_CA          SECX509_NUM_CERTS
#define SECSSLCA_DIR_NAME            "cert/"
#define SECSSLCA_DIR_CHAR            '/'

typedef struct
{
boolean parse_invoked; /* State indicator of CA initialization. */
boolean save_cert;     /* Keep parsed secx509_cert_type pointer */
char    dir_char_name[SECSSL_SES_CERT_FOLDER_MAX_LEN];  /* EFS CA directory name */
uint16  fs_num_ca;    /* How many CA certs under this EFS directory */
char*   file_name[SECSSLCA_MAX_NUM_CA]; /* Certificate Authority file names from EFS */
secx509_cert_type* cert_ptr[SECSSLCA_MAX_NUM_CA];
uint8*  cert_buf_ptr[SECSSLCA_MAX_NUM_CA];
}secsslca_root_folder_type;


/*==========================================================================

                      PUBLIC FUNCTION DECLARATIONS

==========================================================================*/

/*==========================================================================

FUNCTION SECSSLCA_INIT

DESCRIPTION
  Initialize variales which are used by secsslca
RETURN VALUE
  E_SUCCESS
  
DEPENDENCIES
  None
==========================================================================*/
extern secerrno_enum_type secsslca_init( void );
/*==========================================================================

FUNCTION SECSSLCA_FREE

DESCRIPTION
  Free memory allocated by secsslca

DEPENDENCIES
  None
==========================================================================*/
extern void secsslca_free( void );

/*==========================================================================

FUNCTION SECSSLCA_READ_CERTIFICATE

DESCRIPTION
  This functions reads out a CA from the EFS. The passed in name should be
  the fullname (contain the directory). The CA is stored in the DER format.
  Upon return, if successful, a valid CA in the DER format is returned, as 
  well as the length of the CA. Otherwise, an error code will be returned.

RETURN VALUE
  E_SUCCESS,    if successful.
  E_EFS_ERROR,  if EFS returns error.
  E_FAILURE,    if failed.

DEPENDENCIES

==========================================================================*/
extern secerrno_enum_type secsslca_read_certificate
(
  char          *name,
  uint8         *cert_data,
  uint16        *cert_len
);

/*==========================================================================

FUNCTION SECSSLCA_GET_ROOT_FOLDER

DESCRIPTION
  This function parse all certificates under a EFS folder, 
  return a parsed folder pointer to caller.

RETURN VALUE
  E_SUCCESS,    if successful,
  E_INVALID_ARG,if root_folder is not a valid EFS path.
  E_FAILURE,    any other failure reasons.

DEPENDENCIES

==========================================================================*/

extern secsslca_root_folder_type* secsslca_get_root_folder
(
  const char* root_folder
);

/*==========================================================================

FUNCTION SECSSLCA_GET_FILE_NAME

DESCRIPTION
  This function allows external libraries to access the loaded CA filenames

RETURN VALUE
  char*,    if successful
  NULL,           otherwise

DEPENDENCIES
  secsslca_get_root_folder 

==========================================================================*/

extern char* secsslca_get_file_name
(
  const secsslca_root_folder_type* secsslca_root_folder,
  int index
);

/*==========================================================================

FUNCTION SECSSLCA_GET_NUM_CA

DESCRIPTION
  This function allows external libraries to get the number of loaded CA's

RETURN VALUE
  int - a number between 0 and SECSSLCA_MAX_NUM_CA inclusive

DEPENDENCIES
  secsslca_get_root_folder 

==========================================================================*/

extern int secsslca_get_num_ca
(
  const secsslca_root_folder_type* secsslca_root_folder
);

/*==========================================================================

FUNCTION SECSSLCA_GET_TIME

DESCRIPTION
  This function will get current time value in second

RETURN VALUE
  E_SUCCESS,        A good time can be use for x509 expiration check is found
  E_INVALID_ARG,    
  E_NOT_AVAILABLE   No reliable time can be found

DEPENDENCIES
  No

==========================================================================*/
secerrno_enum_type secsslca_get_time
(
  secssl_sys_time_enum_type   time_type,
  uint32                     *curr_time
);

#else /* FEATURE_SEC_SSL */

#define SECSSLCA_MAX_NUM_CA         10
#define SECSSLCA_DIR_CHAR_NAME_LEN  20
#define SECSSLCA_FILENAME_MAX_LEN   128

#endif /* FEATURE_SEC_SSL */

#endif /* SECSSLCA_H */
