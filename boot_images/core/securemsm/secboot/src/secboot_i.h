/*==========================================================================

                         S E C U R I T Y    S E R V I C E S

                 S E C U R E   B O O T   A U T H E N T I C A T I O N

                       I N T E R N A L   H E A D E R  F I L E

FILE: secboot_i.h

DESCRIPTION: This is the secure boot internal header file.

 Hierarchy of dependencies in files for the secure boot process

                      secboot.h (this file, the entry point for this API)
                          |
                      secboot.c--------- secboot_i.h/c (internal, helper
                                                        functions)
                          |
               +--------------+-------+----------------------+
               |                      |                      |
  secboot_x509.h/c       secboot_rsa_math.h/c        secboot_sha1.h/c
   ( x509 parser)       (RSA operations)        (SHA1 interfaces)
         |
         |
  secboot_asn1.h/c
   ( asn1 library)

EXTERNALIZED FUNCTIONS
  None

  Copyright (c) 2013 by Qualcomm Technologies, Inc. All Rights Reserved.
==========================================================================*/

/*=========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/securemsm/secboot/src/secboot_i.h#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------ 
08/28/13   hw      Added retail unlock feature code
03/06/13   vg      Fuse info structure moved to secboot.h header file.
02/27/12   hw,mm   Add support for multiple root selection
02/10/12   sm      Reverted back to dynamic size check for context type 
05/02/06   AMW     Initial SC2X port.
08/25/04   SJD     Initial revision.  Original SW from the security team.

============================================================================*/
#ifndef SECBOOT_I_H_
#define SECBOOT_I_H_

/* Define the platform here ... */
#if defined(WIN32) || defined(_WIN32)
  #define USE_WINDOWS 1
#endif
/*===========================================================================

                              INCLUDE FILES

============================================================================*/
#define BREAKIF(cond)  { if (cond) break; }

#ifdef USE_WINDOWS
  #include <stdlib.h> // debugging for printfs
  #include <stdio.h>
#endif

#include "secboot.h"
#include "secmath.h"
/*============================================================================
                               Definitions
=============================================================================*/
//BUILD_BOOT_CHAIN get's defined through building for bootloaders
//Check if we want to use boot utilities. For now we turn this off
//as there doesn't seem a need for 32 bit aligned memory access for
//SBL's.
//#ifdef BUILD_BOOT_CHAIN
//#define USE_BOOT_UTILS
//#endif

#ifdef USE_BOOT_UTILS
#include "boot_util.h"
#define MEMSET  qmemset
#define MEMCPY  qmemcpy
#define MEMCMP  bByteCompare
#define MEMMOVE qmemcpy
#define BLOAD8  bLoad8
#define BSTOR8  bStor8
#else
#include <string.h>
#define MEMSET  memset
#define MEMCPY  memcpy
#define MEMCMP  memcmp
#define MEMMOVE memmove
#define BLOAD8(x) ((uint32)(*((uint8*)(x)))) 
// note, there is an assumption that the value
// in the uint32 is a byte (<=255). Not sure why the
// boot_util function bStor8 does not check for this and
// actually just OR's the uint32 value with byte*,
// so other bytes would also get affected if the
// uint32 value is greater than 255.
#define BSTOR8(x,y) (*((uint8*)(x))) = (uint8)(y) 
#define CEML_PBL_HASHCTX_SIZE      200 // size of buffer pointed to by pClientCtx that PBL crypto needs from caller
                                       // as PBL crypto does not allocate memory from internal static buffer but
                                       // relies on caller to pass it the memory it needs. Using a bigger value
                                       // than the actual size we need, for any future changes


#endif


#ifdef USE_WINDOWS
  #define TRUE       1
  #define FALSE      0
  typedef unsigned __int64 uint64;
  typedef unsigned __int32 uint32;
  typedef unsigned __int16 uint16;
  typedef unsigned __int8  uint8;
  typedef signed __int64 int64;
  typedef signed __int32 int32;
  typedef signed __int16 int16;
  typedef signed __int8  int8;
//  typedef int boolean;
  #define SECBOOT_PRINT printf
#else
  #define SECBOOT_PRINT
#endif

/* we stick to a small size to save memory whereever possible */
#define ATTEST_CERT_INDEX        0
#define CA_CERT_INDEX            1
#define ROOT_CERT_INDEX          2


#define SECBOOT_MAX_KEY_SIZE_IN_BITS 4096 /* Secmath has to support this size */
#define SECBOOT_MIN_KEY_SIZE_IN_BITS 2048 /* Secmath has to support this size */
#define SECBOOT_MAX_PUB_EXP_KEY_SIZE_IN_BITS 32 /* in bits, but we expect only exp=3 (2 bits) or worst 65537 (17 bits)
                                    ** the generic case would be = SECBOOT_MAX_KEY_SIZE_IN_BITS but for
                                    ** memory reasons
                                    ** we stick to a small size to save memory whereever possible */

/**
 * @brief Masks for attestation certificates debug enable OU field
 *        This is a 64 bit field, with lower (0:31) bits to indicate
 *        if debugging is to be enabled, and upper (32:63) to indicate
 *        the chip (based on serial number) that the debug cert was
 *        mean for i.e the chip that debugging is to be enabled on
 */

/**<  Mask to get the debug enable/disable bits in the debug OU field */
#define SECBOOT_DEBUG_SETTINGS_FIELD_MASK     (0xFFFFFFFF)
/**<  Shift to get at the serial number from upper 32 bits of the debug enable OU field*/
#define SECBOOT_DEBUG_SERIALNUM_FIELD_SHFT    (0x20)

/**<  Mask to get the crash dump enable/disable bits in the crash dump OU field */
#define SECBOOT_CRASH_DUMP_SETTINGS_FIELD_MASK     (0xFFFFFFFFUL)
/**<  Shift to get at the serial number from upper 32 bits of the crash dump enable OU field*/
#define SECBOOT_CRASH_DUMP_SERIALNUM_FIELD_SHFT    (0x20)

/* Error log structure to store constant data describing error location */
typedef struct secboot_error_log_type
{
  uint32                   linenum;
  uint32                   data1;
  uint32                   data2;
  uint32                   data3;
} secboot_error_log_type;


#define CONTEXT_MAGIC_NUMBER 0x50425342 //Ascii values for PBSB - PBL Secure Boot

//Make sure this structure is uint32 aligned
typedef struct secboot_context_type
{
    uint32                         magic_number;
    secboot_crypto_hash_ftbl_type  crypto_hash_ftbl;
    uint32                         version; //version of secboot release
    secboot_error_log_type         error_log;      
    secboot_fuse_info_type         fuses;
    boolean                        use_supplied_fuses; /* This value will be set to TRUE,
                                                     if fuse values are supplied from externally */
}secboot_context_type;

#endif

