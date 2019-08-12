#ifndef MACCHIATO_WRAPPER_H
#define MACCHIATO_WRAPPER_H
/*===========================================================================
 Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/10/15   cz      Added a new parameter: utc seconds
11/26/14   cz      Initial Version.

===========================================================================*/
#include "qsee_macchiato.h" // buffer_t definition reference

/*************************************************************************
 *                        Struct Type Definition                         *
 *************************************************************************/
 

typedef struct {
    buffer_t   serviceData;
    buffer_t   signedServiceData;
    uint32_t  *pu32SignedServiceDataLen;
	uint32_t  *ret_error_code;
} qsee_macc_sign_data_t;
  
typedef struct {
    buffer_t   serviceNonce;
    buffer_t   deviceNonce;
    buffer_t   opaqueData;
    buffer_t   signedChallengeResponse;
    uint32_t  *pu32SignedChallengeResponseLen;
	uint32_t  *ret_error_code;
} qsee_macc_auth_dev_t;

typedef struct {
    buffer_t   provisionMsg;
    buffer_t   unwrappedKey;
    uint32_t  *pu32UnwarppedKeyLen;
    buffer_t   deviceNonce;
    uint32_t   utcSeconds;
	uint32_t  *ret_error_code;
} qsee_macc_prov_key_t;


/*************************************************************************
 *                            Function Declaration                       *
 *************************************************************************/

uint32 tzbsp_macc_sign_service_data
(
    qsee_macc_sign_data_t  sign_data
);

uint32 tzbsp_macc_authenticate_device
(
    qsee_macc_auth_dev_t   auth_dev
);

uint32 tzbsp_macc_provision_service_key
(
    qsee_macc_prov_key_t   prov_key
);

#endif /* MACCHIATO_WRAPPER_H */
