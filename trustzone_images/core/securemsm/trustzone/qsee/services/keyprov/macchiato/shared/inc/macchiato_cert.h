/**
 * Copyright 2015 QUALCOMM Technologies, Inc. 
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *
 * @file qsee_macchiato.h
 * @author  weilinz
 * @version 1.0
 * @since   2015-02-24
 *
 * @brief header file for macchiato_cert.c
 */

/*===========================================================================
   Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  
   This file contains the external interface definitions for Macchiato
   certificate processing.
  ===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

  when       who      what, where, why
  --------   ---      ------------------------------------
  02/24/15   wz       Initial release

===========================================================================*/

#ifndef MACCHIATO_CERT_H
#define MACCHIATO_CERT_H
#include <stdint.h>
#include "secrsa.h"

extern macchiato_err_t process_kpsk_cert(uint8 *CertificateData, uint32 CertificateLength, CE_RSA_KEY *rsaKey, uint32_t utcSeconds);
extern boolean verify_message_signature(buffer_t data_to_sign, buffer_t signature, CE_RSA_KEY *rsaKey);

#endif //MACCHIATO_CERT_H

