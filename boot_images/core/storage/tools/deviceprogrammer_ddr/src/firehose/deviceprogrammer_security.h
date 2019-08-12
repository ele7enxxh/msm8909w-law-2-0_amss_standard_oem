/*==================================================================
 *
 * FILE:        deviceprogrammer_security.h
 *
 * DESCRIPTION:
 *   
 *
 *        Copyright © 2013-2015 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *               QUALCOMM Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/deviceprogrammer_ddr/src/firehose/deviceprogrammer_security.h#1 $ 
 *   $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * YYYY-MM-DD   who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 2015-05-11   sb      Use crypto sha for ddr version only
 * 2015-04-01   sb      Use crypto bam for hash calculation
 * 2013-06-03   ah      Added legal header
 * 2013-05-31   ab      Initial checkin
 */

#ifndef DEVICEPROGRAMMER_SECURITY_H
#define DEVICEPROGRAMMER_SECURITY_H

#include "deviceprogrammer_utils.h"

#define CONTEXT_LEFTOVER_FIELD_SIZE 64

/* This is the state of a running hash, known by all platforms */
struct __sechsh_ctx_s
{
	uint32  counter[2];
	uint32  iv[16];  // is 64 byte for SHA2-512
	uint8   leftover[CONTEXT_LEFTOVER_FIELD_SIZE];
	uint32  leftover_size;
};

/* Forward declarations of the internal working functions */
void sechsharm_sha256_init(   struct __sechsh_ctx_s* );
void sechsharm_sha256_update( struct __sechsh_ctx_s*, uint8*, uint32*, uint8*, uint32 );
void sechsharm_sha256_final(  struct __sechsh_ctx_s*, uint8*, uint32*, uint8* );

#ifdef FEATURE_BOOT_EXTERN_DEVICEPROGRAMMER_DDR_COMPLETED
void PerformSHA256HW(
    uint8* inputPtr, 
	uint32 inputLen, 
	uint8* outputPtr);
#else
void PerformSHA256(
    uint8* inputPtr,
    uint32 inputLen,
    uint8* outputPtr);
#endif

boolean authenticateSignedHash(byte *buffer,
                               SIZE_T length,
                               byte** hash_position_address,
                               SIZE_T *signed_payload_size);

boolean isAuthenticationEnabled(void);

#endif
