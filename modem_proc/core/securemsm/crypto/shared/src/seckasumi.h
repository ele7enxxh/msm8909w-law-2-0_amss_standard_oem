#ifndef SECKASUMI_H
#define SECKASUMI_H
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                    C O M M O N   U T I L I T I E S

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2013 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/seckasumi.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/14   jk      Direction and bearer packed in IV[4]
12/12/13   nk      Optimised kasumi stream cipher
10/05/13   nk      initial version
===========================================================================*/
/*---------------------------------------------------------
 *                     Kasumi_core.h
 *---------------------------------------------------------*/
#include "comdef.h"
#include "secerrno.h"

/*----- a 64-bit structure to help with endian issues -----*/

typedef union {
  uint32 b32[2];
  uint16 b16[4];
  uint8  b8[8];
} REGISTER64;



void KasumiKeySchedule( uint8 *key );
void Kasumi( uint8 *data );

/*-------------------------------------------------------------------
 *                  F8 - Confidentiality Algorithm
 *-------------------------------------------------------------------
 *
 *  A sample implementation of f8, the 3GPP Confidentiality algorithm.
 *
 *  This has been coded for clarity, not necessarily for efficiency.
 *
 *  This will compile and run correctly on both Intel (little endian)
 *  and Sparc (big endian) machines. (Compilers used supported 32-bit ints)
 *
 *  Version 1.0   05 November  1999
 *
 *-------------------------------------------------------------------*/
/*---------------------------------------------------------
 * sec_kasumi_eea1()
 *      Given key, count, bearer, direction,  data,
 *      and bit length  encrypt the bit stream
 *---------------------------------------------------------*/
secerrno_enum_type sec_kasumi_eea1(uint8* in_msg_ptr, uint32 in_byte_len, uint8* iv_ptr, uint8* key_ptr);
void sec_kasumi_f8_eea1( uint8 *key, uint32 count, uint8 bearer, uint8 dir, uint8 *data, int32 length );

/*-------------------------------------------------------------------
 *                     F9 - Integrity Algorithm
 *-------------------------------------------------------------------
 *
 *  A sample implementation of f9, the 3GPP Integrity algorithm.
 *
 *  This has been coded for clarity, not necessarily for efficiency.
 *
 *  This will compile and run correctly on both Intel (little endian)
 *  and Sparc (big endian) machines. (Compilers used supported 32-bit ints)
 *
 *  Version 1.1       05 September  2000
 *
 *-------------------------------------------------------------------*/
/*---------------------------------------------------------
 * sec_kasumi_eia1()
 *      Given key, count, fresh, direction, data,
 *      and message length, calculate the hash value
 *---------------------------------------------------------*/
secerrno_enum_type sec_kasumi_eia1(uint32* MAC, uint8* msg_ptr, uint32 msg_len, uint8* iv_ptr, uint8* key_ptr);
uint8 *sec_kasumi_f9_eia1( uint8 *key, uint32 count, uint32 fresh, uint8 dir, uint8 *data, int32 length );

#endif /* KASUMI_CORE_H */
