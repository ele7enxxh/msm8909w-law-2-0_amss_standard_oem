#ifndef ZUC_H
#define ZUC_H

/**
 * Copyright 2012 Qualcomm Technologies Inc. 
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *
 * @author	Billy Brumley <bbb AT qualcomm DOT com>
 * @author	Roberto Avanzi <ravanzi AT qualcomm DOT com>
 * @version     1.7
 * @since       2012-02-01
 */

/*===========================================================================
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/seczuc.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $
===========================================================================*/

#include <string.h>

#if defined(__HEXAGON_ARCH__) && !defined(__QDSP6_ARCH__)
#define __QDSP6_ARCH__ __HEXAGON_ARCH__
#endif
//temporary turnoff for 8974
#undef __QDSP6_ARCH__

#ifdef __QDSP6_ARCH__

#include <q6protos.h>
#include <q6types.h>
#include "comdef.h"

#else

#include <stdint.h>

//typedef uint8_t uint8;
//typedef uint16_t uint16;
//typedef uint32_t uint32;
//typedef uint64_t uint64;

#endif

/* ZUC keystream generator context structure. Represents one instance of the primitive. */
typedef struct 
{
	/* sliding window LFSR; twice the number of cells */
	uint32 L[32];
	/* FSM internal state */
	uint32 R1;
	uint32 R2;
	/* pointer to current LFSR state within L */
	uint32 *S;
} sec_zuc_ctx;

secerrno_enum_type sec_zuc_keystream(uint32 *k, uint32 len, sec_zuc_ctx *ctx);
secerrno_enum_type sec_zuc_init(const unsigned char *k, const unsigned char *iv, sec_zuc_ctx *ctx);
secerrno_enum_type sec_zuc_eea3(unsigned char *m, uint32 len, const unsigned char *iv, const unsigned char *k);
secerrno_enum_type sec_zuc_eia3(uint32 *mac, const unsigned char *m, uint32 len, const unsigned char *iv, const unsigned char *k);

/* ZUC_SBOX_SIZE can be 8, 16, or 32 */
#define ZUC_SBOX_SIZE 8

#endif /* ZUC_H */
