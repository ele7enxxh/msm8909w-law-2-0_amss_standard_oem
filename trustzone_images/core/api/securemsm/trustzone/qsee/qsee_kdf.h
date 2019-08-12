#ifndef _QSEE_KDF_H
#define _QSEE_KDF_H

/**
@file qsee_kdf.h 
@brief This is an implementation of the key derivation algorithm.
The counter-based algorithm from NIST SP 800-108 using CMAC where the block cipher is AES-128
All sensitive data is zeroized before return.
*/



/*=============================================================================
              Copyright (c) 2000-2012 Qualcomm Technologies Incorporated.
                      All Rights Reserved.
              Qualcomm Confidential and Proprietary

=============================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $PVCVPath: $
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/trustzone/qsee/qsee_kdf.h#1 $ 
  $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/11   yh      initial version

===========================================================================*/
#include "stddef.h"
#include "comdef.h"



/**
* @brief kdf key derivation algorithm.
* 
* @details The software is a 3-level stack.
* The lowest level is AES. 
* The second level is the CMAC algorithm from NIST SP 800-38B. 
* The third level is the counter-based algorithm from NIST SP 800-108, named KDF in the implementation.
*
* The inputs are a key derivation key (key, key_len ), a label
* (label, label_len), and a context (context, context_len). 
* The output will be output_len bytes long stored at output.
* All sensitive data is zeroized before return..
*
* When key is set to NULL, key_len will be ignored. 
* Qsee will set a 32 bytes key; which is not accessible from outside Trustzone; as the input key.
*
*
* @param[in] key           key derivation key pointer
* @param[in] key_len       key derivation key pointer length in byte
* @param[in] label         key derivation label pointer
* @param[in] label_len     key derivation label pointer length in byte 
* @param[in] context       key derivation context pointer
* @param[in] context_len   key derivation context pointer length in byte
* @param[in/out] output    derived key pointer
* @param[in] output_len    derived key pointer length in byte
*
* @return
* The return value follows OpenSSL: 0 for success, -1 for errors like
* null pointers where not allowed, -2 for invalid values or overflow.
*/
int qsee_kdf(const void *key, size_t key_len, const void *label, size_t label_len,
            const void *context, size_t context_len, void* output, size_t output_len);


#endif /* _QSEE_KDF_H */
