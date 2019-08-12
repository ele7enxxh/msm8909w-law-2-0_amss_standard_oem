/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       C A V E   M O D U L E

GENERAL DESCRIPTION
  This module contains routines to support the CAVE (Cellular
  Authentication, Voice Privacy and Encryption) algorithm as specified
  in "Common Cryptographic Algorithms" called out in IS-95.

  This module is common to all hardware targets.

NOTES
  1. See "Common Cryptographic Algorithms" for explanations and
     specifications of all these algorithms.

  2. Most of the arrays specified in "Common Cryptographic Algorithms"
     are indexed backwards as compared to C arrays: XXX[0] is the
     MOST significant byte of XXX.  This module follows that convention
     for arrays, but keeps whole values in conventional byte order.
     Some code here depends on Intel byte order.

NOTICE
  Information embodied in this module may be subject to the export
  jurisdiction of the US Department of State as specified in the
  International Traffic in Arms Regulations (title 22 CFR parts
  120 through 130 inclusive).  A license issued by the Department
  of State is required for the export of such technical data.

EXTERNALIZED FUNCTIONS
  CAVE_init
    Initializes CAVE processing, at powerup or NAM change.

  CAVE_validate_A_key
    Checks a manually-entered A-key against its check digits.

  CAVE_update_A_key
    Stores a new A-key and clears out SSD.

  CAVE_generate_SSD
    Generates new A and B values of Shared Secret Data.

  CAVE_update_SSD
    Stores new A and B values of SSD.

  CAVE_auth_signature
    Computes and authentication signature.

  CAVE_encrypt
    Encrypts a message buffer using the CMEA algorithm.

  CAVE_private_lcm
    Returns the private long code mask to be used for CDMA voice privacy.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  CAVE_init must be called once on powerup or NAM change, to set
  up the identity variables, before any other functions are called.

  CAVE_auth_signature must be called with save_registers set to TRUE,
  and CAVE_generate_key must be called, before any message encryption
  can be performed or the private long code mask can be returned.

  Copyright (c) 1994 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/cave.c_v   1.4   09 Sep 2002 16:22:20   azafer  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/cave.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/21/14   ssh     Workaround to correct the PLCM to overcome a compiler bug
03/05/13   srk     Replace existing watchdog implementation with the new
                   Dog Heartbeat Mechanism.
01/03/13   nee     Replacement of memcpy() to memscpy()
02/04/12   ppr     Feature Cleanup
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
11/15/10   jtm     Initial SU API split for CP.
10/20/10   azh     Fixed KW warnings.
09/15/10   ag      Added support for dynamic DOG.
08/19/10   jj      CSIM: Added changes to update CAVE VPM keys correctly.
06/24/10   ssh     Added support for EFS sync.
05/14/09   ag      CMI phase 2 changes
06/07/07   pg      Lint clean up.
06/15/06   an      Fixed Lint Errors for RUIM build
06/12/06   fh      AKA/AES: fixed an error when AKA and REL-D are turned ON
05/17/06   fh      Merged in new development of AKA/AES features
04/26/06   an      Fixed Lint Errors
01/20/06   fc      Merged the following:
           yll     Added the function to get CMEA key.
12/10/04   sb      Fixed lint issue.
05/20/04   fc      Lint clean up.
09/09/02   ts      Added support for Run Time R-UIM Enable feature
08/09/01   sb      Merged following changes from common archive:
                   06/19/01   ts      Moved the VPM size constant to this file.
                   06/13/01   ts      A change in the VPM octet offset has caused
                                      a problem with CAVE_update_keys.  Changed
                                      the VPM size to use the proper size and not
                                      the offset.
05/25/01   ts      Added feature switches for new UIM server.
03/09/01   fas     Added include for customer.h to inhibit compiler warning.
06/28/00   cah     Removed MSG_FILE undef work around for nonRUIM builds.
05/30/00   ck      Added undef MSG_FILE to remove compiler warning.
05/24/00   ck      Added the function CAVE_update_keys() that sets the CMEA
                   and VPM keys based on the keys computed by the RUIM.
                   Featurised out most of the CAVE functionality based on
                   FEATURE_RUIM.
04/06/00   va      Merged the following from MSM3100_CP.03.00.17
           ry      Added OTAPA support
08/13/99   jq      Removed the use of ROM, since it's no longer defined.
06/17/99   ck      Fixed the problem where the CMEA and VPM keys were computed
                   based on the old SSD during re-authentication.
05/10/99   kmp     Replaced T_AUTH with FEATURE_AUTH
04/08/99   ck      Removed the initialisation of SSD_A and SSD_B after the
                   update of A-Key by OTASP as it erased the previous contents
                   of valid SSD.
03/18/99   ck      Merged in support for A-Key exchange and Re-Authentication
03/18/99   ck      Merged the change from rev 1.10 SPSW
                   Keyword 'const' inserted in front of the ROM macro.
09/15/98   pms     Backed out _near pointers
09/04/98   pms     Neared the variables to save ROM space.
06/21/96   dna     Removed checksums for A-key, SSD-A and SSD-B.  We now make
                   them zero if unitialized and always consider them valid.
05/15/96   dna     CMEA key generation status now set ASAP.
04/24/96   dna     Now write checksum for zero'd SSD when changing A-Key.
08/23/95   dna     Checked in initial version.
04/19/94   ptw     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -e766*/
#include "1x_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"



/* Temporary change to rid msg_file compiler warning until msg fix is in */
#if defined( FEATURE_UIM_RUIM ) && !defined( FEATURE_UIM_RUN_TIME_ENABLE )
#undef MSG_FILE
#endif /* FEATURE_UIM_RUIM && !FEATURE_UIM_RUN_TIME_ENABLE */

#include "err.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "msg.h"
#include "m1x_diag.h"
#include "nv.h"
#include "qw.h"
#include "cave.h"
#include "ulpn.h"       /* For ulpn_type definition */
#include "auth_v.h"
#include "auth_i.h"
#include "authi.h"
#include "crc.h"
#ifdef FEATURE_UIM_RUIM
#include "memory.h"
#endif /* FEATURE_UIM_RUIM */
#ifdef FEATURE_CSIM
#include "mmgsdilib_common.h"
#endif
#include "fs_public.h"
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


#define  CAVE_AAV   (0xC7)
/* Authentication Algorithm Version */

LOCAL dword    CAVE_esn;
/* The electronic serial number of this phone. */

LOCAL qword    CAVE_A_key;
/* The current A_key in use for CAVE */

LOCAL qword    CAVE_SSD_A;
/* The current A value of Shared Secret Data in use for CAVE */

LOCAL qword    CAVE_SSD_B;
/* The current B value of Shared Secret Data in use for CAVE */

LOCAL qword    CAVE_new_SSD_A;
/* A possible new A value of Shared Secret Data */

LOCAL qword    CAVE_new_SSD_B;
/* A possible new B value of Shared Secret Data */

LOCAL byte     CAVE_NAM_index;
/* Index of current NAM being used */

LOCAL dword CAVE_lfsr;
/* The linear (actually non-linear) feedback shift register used
   in the CAVE algorithm. */

LOCAL dword    CAVE_saved_lfsr;
/* Value of lfsr saved from the authentication that is to be used
   as seed for encryption and voice privacy */

LOCAL byte CAVE_mixing_reg[16];
/* The mixing registers used in the CAVE algorithm */

LOCAL byte     CAVE_offset_1, CAVE_offset_2;
/* Offsets into the CAVE table used in the CAVE algorithm */

LOCAL byte     CAVE_saved_offset_1, CAVE_saved_offset_2;
/* Values of offset_1 and offset_2 saved from the authentication
   that is to be used as seed for encryption and voice privacy */

LOCAL dword    CAVE_saved_rand;
/* Value of rand_challenge saved from the authentication that is
   to be used as seed for encryption and voice privacy */

LOCAL dword    CAVE_saved_data;
/* Value of auth_data saved from the authentication that is to be used
   as seed for encryption and voice privacy */

LOCAL byte     CAVE_CMEA_key[8];
/* Key used for message encryption */

LOCAL byte     CAVE_VPM[CAVE_VPM_SIZE];
/* 520 bit voice privacy mask.  Actually, only the 40 LSBits of this
   are ever used.  VPM[64] is the least significant byte. */

/*-------------------------------------------------------------------------

      CAVE Table

   This table is taken from Exhibit 2-3 of "Common Cryptographic Algorithms"

   It is really two tables: the LS nibble of each entry is table0, and
                            the MS nibble of each entry is table1.

-------------------------------------------------------------------------*/

#define  LOMASK   0x0F        /* Mask all but the least significant nibble */
#define  HIMASK   0xF0        /* Mask all but the most significant nibble */

static byte const CAVE_table[256] = {
  /* 0 */ 0xd9, 0x23, 0x5f, 0xe6, 0xca, 0x68, 0x97, 0xb0,
  0x7b, 0xf2, 0x0c, 0x34, 0x11, 0xa5, 0x8d, 0x4e,
  /* 1 */ 0x0a, 0x46, 0x77, 0x8d, 0x10, 0x9f, 0x5e, 0x62,
  0xf1, 0x34, 0xec, 0xa5, 0xc9, 0xb3, 0xd8, 0x2b,
  /* 2 */ 0x59, 0x47, 0xe3, 0xd2, 0xff, 0xae, 0x64, 0xca,
  0x15, 0x8b, 0x7d, 0x38, 0x21, 0xbc, 0x96, 0x00,
  /* 3 */ 0x49, 0x56, 0x23, 0x15, 0x97, 0xe4, 0xcb, 0x6f,
  0xf2, 0x70, 0x3c, 0x88, 0xba, 0xd1, 0x0d, 0xae,
  /* 4 */ 0xe2, 0x38, 0xba, 0x44, 0x9f, 0x83, 0x5d, 0x1c,
  0xde, 0xab, 0xc7, 0x65, 0xf1, 0x76, 0x09, 0x20,
  /* 5 */ 0x86, 0xbd, 0x0a, 0xf1, 0x3c, 0xa7, 0x29, 0x93,
  0xcb, 0x45, 0x5f, 0xe8, 0x10, 0x74, 0x62, 0xde,
  /* 6 */ 0xb8, 0x77, 0x80, 0xd1, 0x12, 0x26, 0xac, 0x6d,
  0xe9, 0xcf, 0xf3, 0x54, 0x3a, 0x0b, 0x95, 0x4e,
  /* 7 */ 0xb1, 0x30, 0xa4, 0x96, 0xf8, 0x57, 0x49, 0x8e,
  0x05, 0x1f, 0x62, 0x7c, 0xc3, 0x2b, 0xda, 0xed,
  /* 8 */ 0xbb, 0x86, 0x0d, 0x7a, 0x97, 0x13, 0x6c, 0x4e,
  0x51, 0x30, 0xe5, 0xf2, 0x2f, 0xd8, 0xc4, 0xa9,
  /* 9 */ 0x91, 0x76, 0xf0, 0x17, 0x43, 0x38, 0x29, 0x84,
  0xa2, 0xdb, 0xef, 0x65, 0x5e, 0xca, 0x0d, 0xbc,
  /* a */ 0xe7, 0xfa, 0xd8, 0x81, 0x6f, 0x00, 0x14, 0x42,
  0x25, 0x7c, 0x5d, 0xc9, 0x9e, 0xb6, 0x33, 0xab,
  /* b */ 0x5a, 0x6f, 0x9b, 0xd9, 0xfe, 0x71, 0x44, 0xc5,
  0x37, 0xa2, 0x88, 0x2d, 0x00, 0xb6, 0x13, 0xec,
  /* c */ 0x4e, 0x96, 0xa8, 0x5a, 0xb5, 0xd7, 0xc3, 0x8d,
  0x3f, 0xf2, 0xec, 0x04, 0x60, 0x71, 0x1b, 0x29,
  /* d */ 0x04, 0x79, 0xe3, 0xc7, 0x1b, 0x66, 0x81, 0x4a,
  0x25, 0x9d, 0xdc, 0x5f, 0x3e, 0xb0, 0xf8, 0xa2,
  /* e */ 0x91, 0x34, 0xf6, 0x5c, 0x67, 0x89, 0x73, 0x05,
  0x22, 0xaa, 0xcb, 0xee, 0xbf, 0x18, 0xd0, 0x4d,
  /* f */ 0xf5, 0x36, 0xae, 0x01, 0x2f, 0x94, 0xc3, 0x49,
  0x8b, 0xbd, 0x58, 0x12, 0xe0, 0x77, 0x6c, 0xda
};

/*-------------------------------------------------------------------------
      Macros to access individual bytes of the LFSR

   These macros exist to make the code in this module read more like
   the pseudocode in "Common Cryptographic Algorithms".

   Note: these macros are lvalues, so you can say LFSR_A = x.

   Caution: the macros depend on Intel byte order.
-------------------------------------------------------------------------*/

#define  LFSR_A   (B_PTR(CAVE_lfsr)[3])
#define  LFSR_B   (B_PTR(CAVE_lfsr)[2])
#define  LFSR_C   (B_PTR(CAVE_lfsr)[1])
#define  LFSR_D   (B_PTR(CAVE_lfsr)[0])

#if !defined (FEATURE_UIM_RUIM) || defined (FEATURE_UIM_RUN_TIME_ENABLE)
/*===========================================================================

FUNCTION CAVE_init

DESCRIPTION
  This function is called to perform initialization for the CAVE
  operations.  This mainly involves setting some variables from their
  values in semi-permanent memory.

DEPENDENCIES
  This function should only be called once on powerup or NAM change,
  or when the A_Key is changed.

RETURN VALUE
  AUTH_INIT_OK         if initialization is successful
  AUTH_UNINITIALIZED   if initialization fails

SIDE EFFECTS
  None.

===========================================================================*/

auth_init_status_type CAVE_init
(
   byte    NAM_index
   /* NAM which CAVE variables will be taken from */
)

{
  nv_item_type nv_item;
  /* Item to hold values retrieved from NV */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* --------------------------------------------------
** Read the ESN first so that no matter what happens,
** the ESN is valid for use in CAVE_validate_A_key().
** -------------------------------------------------- */

#if (TG!=T_PC)
  (void)auth_get_nv_item(NV_ESN_CHKSUM_I, &nv_item);
  if (nv_item.esn_chksum.chksum != NV_VALID_ESN_CHKSUM) /*lint !e650 !e737*/
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid ESN");
    return (AUTH_UNINITIALIZED);
  }
#endif

  (void)auth_get_nv_item(NV_ESN_I, &nv_item);
  CAVE_esn = nv_item.esn.esn;

/* ---------------------------------------------
** The a_key_checksum is now ignored, and zero
** is assumed if the a_key isn't available.
** --------------------------------------------- */

  nv_item.a_key.nam = NAM_index;
  if (auth_get_nv_item(NV_A_KEY_I, &nv_item) == NV_DONE_S)
  {
    qw_equ(CAVE_A_key, nv_item.a_key.key);
  }
  else
  {
    M1X_MSG(DCP, LEGACY_HIGH,
            "Uninitialized value of A-key");
    qw_set(CAVE_A_key, 0L, 0L);
  }

/* -------------------------------------------
** SSD checksums are no longer used, and zero
** is assumed for SSD if it is not available.
** ------------------------------------------- */

  nv_item.ssd_a.nam = NAM_index;
  if (auth_get_nv_item(NV_SSD_A_I, &nv_item) == NV_DONE_S)
  {
    qw_equ(CAVE_SSD_A, nv_item.ssd_a.ssd);
  }
  else
  {
    M1X_MSG(DCP, LEGACY_HIGH,
            "Uninitialized value of SSD A");
    qw_set(CAVE_SSD_A, 0L, 0L);
  }

  nv_item.ssd_b.nam = NAM_index;
  if (auth_get_nv_item(NV_SSD_B_I, &nv_item) == NV_DONE_S)
  {
    qw_equ(CAVE_SSD_B, nv_item.ssd_b.ssd);
  }
  else
  {
    M1X_MSG(DCP, LEGACY_HIGH,
            "Uninitialized value of SSD B");
    qw_set(CAVE_SSD_B, 0L, 0L);
  }

  CAVE_NAM_index = NAM_index;

  M1X_MSG(DCP, LEGACY_MED,
          "CAVE initialized");
  return (AUTH_INIT_OK);

} /* CAVE_init */

/*===========================================================================

FUNCTION CAVE_LFSR_cycle

DESCRIPTION
  This function cycles the linear feedback shift register (lfsr) once.
  It implements the LFSR specified in "Common Cryptographic Algorithms."

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void CAVE_LFSR_cycle(void)
{
  boolean  new_bit;
  /* The new bit to shift in on the left */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Compute the new bit to shift in.  It is the XOR
     of four specific bits from the old state value. */
  /*lint -e514  Yes, I mean to XOR Booleans. */
  new_bit =   ((LFSR_B & 0x40) != 0)
     ^ ((LFSR_D & 0x04) != 0)
     ^ ((LFSR_D & 0x02) != 0)
     ^ ((LFSR_D & 0x01) != 0);
  /*lint -restore */
  /* This isn't very efficient.  Keep it straightforward for now. */

  CAVE_lfsr >>= 1;          /* Shift the shift register */

  /* Bring in the new bit */
  if (new_bit != 0)
  {
    CAVE_lfsr |= 0x80000000L; /*lint !e737 */
  }
} /* CAVE_LFSR_cycle */

/*===========================================================================

FUNCTION CAVE_rotate_right_registers

DESCRIPTION
  This function rotates the mixing_reg[] array, considered as a single
  128-bit value, right by one place.  The former LSbit becomes the new
  MSbit.

  Notice that mixing_reg[0] is consider the MSbyte.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void  CAVE_rotate_right_registers(void)
{
  boolean   new_bit;
  /* The new bit to shift in on the left */

  int       R_index;
  /* Index into the mixing_reg array */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Pick off the old LSbit for end-around-carry */
  new_bit = CAVE_mixing_reg[15] & 0x01;

  /* Shift right bytewise */
  for (R_index = 15; R_index >= 1; R_index--)
  {
    CAVE_mixing_reg[R_index] >>= 1;             /* Shift a byte */
    if (CAVE_mixing_reg[R_index - 1] & 0x01)      /* Propagate the carry bit */
    {
      CAVE_mixing_reg[R_index] |= 0x80;
    }
  }

  CAVE_mixing_reg[0] >>= 1;                     /* Shift the last byte */
  if (new_bit)                             /* Bring in the end-around carry */
  {
    CAVE_mixing_reg[0] |= 0x80;
  }

} /* CAVE_rotate_right_registers */

/*===========================================================================

FUNCTION CAVE_ROUNDS

DESCRIPTION
  This function performs the specified number of rounds of the CAVE
  algorithm.  It operates on the values found in the regional mixing_reg,
  and leaves its results there.

  Note that variable names and the structure of this function follow
  the pseudocode definition given in Exhibit 2-2 of "Common Cryptographic
  Algorithms".

DEPENDENCIES
  The regionals mixing_reg, lfsr, offset_1, and offset_2 must be properly
  initialized before calling this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void  CAVE_rounds
(
   int  n_rounds
   /* The number of rounds of the CAVE algorithm to be performed. */
)

{
  int    round;
  /* Counter keeping track of how many rounds have been performed */

  byte   temp_reg0;
  /* Storage for mixing_reg[0] for end-around shift into mixing_reg[15] */

  byte   low_nibble, high_nibble;
  /* Temporary variables used in the CAVE algorithm */

  byte   T[16];
  /* Temporary array used to shuffle the mixing_reg array */

  byte   T_inx;
  /* Index into the temporary array T, used for shuffling.
     Exhibit 2-2 calls this (among other variables) "temp" */

  byte   fail_count;
  /* Counter of how many times the scramble has been performed,
     with a match to mixing_reg[] each time.  After 32 failures,
     we'll just increment the shift register and go on anyway. */

  byte   R_index;
  /* Index into the mixing_reg array */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (round = n_rounds - 1; round >= 0; round--)
  {
    temp_reg0 = CAVE_mixing_reg[0];       /* Save R0 for later use */

    for (R_index = 0; R_index <= 15; R_index++)
    {
      fail_count = 0;
      for (;;)
      {
        CAVE_offset_1 += LFSR_A ^ CAVE_mixing_reg[R_index];
        low_nibble = CAVE_table[CAVE_offset_1] & LOMASK;
        if (low_nibble == (CAVE_mixing_reg[R_index] & LOMASK))
        {
          CAVE_LFSR_cycle();
          if (++fail_count == 32)
          {
            LFSR_D++;                 /* mod 256 */
            break;
          }
        }
        else
        {
          break;
        }
      } /* for (;;) */

      fail_count = 0;
      for (;;)
      {
        CAVE_offset_2 += LFSR_B ^ CAVE_mixing_reg[R_index];
        high_nibble = CAVE_table[CAVE_offset_2] & HIMASK;
        if (high_nibble == (CAVE_mixing_reg[R_index] & HIMASK))
        {
          CAVE_LFSR_cycle();
          if (++fail_count == 32)
          {
            LFSR_D++;
            break;
          }
        }
        else
        {
          break;
        }
      } /* for (;;) */

      if (R_index == 15)
      {
        CAVE_mixing_reg[15] = temp_reg0 ^ low_nibble ^ high_nibble;
      }
      else
      {
        CAVE_mixing_reg[R_index] =
           CAVE_mixing_reg[R_index + 1] ^ low_nibble ^ high_nibble;
      }

      CAVE_LFSR_cycle();

    } /* for R_index */

    CAVE_rotate_right_registers();

    /* Shuffle the mixing registers into temporary array T */
    for (R_index = 0; R_index <= 15; R_index++)
    {
      T_inx = CAVE_table[16 * round + R_index] & LOMASK;
      T[T_inx] = CAVE_mixing_reg[R_index];
    }
    /*lint -esym(771,T) : it looks like T might not be initialized
      completely, but since the low nibble of CAVE_table[k] is a
      permutation for any fixed k, this in fact fills up the T array. */

    /* Copy temporary array T back into the registers */
    for (R_index = 0; R_index <= 15; R_index++)
    {
      CAVE_mixing_reg[R_index] = T[R_index];
    }
  } /* for round */
} /* cave_rounds */

/*===========================================================================

FUNCTION CAVE_CALC_CHECKSUM

DESCRIPTION
  This function calculates the checksum value for a given A key, according
  to the procedure described in "Common Cryptographic Algorithms, Revision
  A"

DEPENDENCIES
  CAVE_init must be called before calling this function.
  Depends on Intel byte order.

RETURN VALUE
  Returns the calculated checksum value.

SIDE EFFECTS
  Changes value of CAVE_lfsr, CAVE_offset_1 and 2, CAVE_mixing_reg.

===========================================================================*/

static dword CAVE_calc_checksum
(
   qword  A_key_under_test
   /* The value of the A-key to use in checksum calculation */
)
{
  dword  answer;
  /* The value computed as the check signature of the proposed A-key */

  int    R_index;
  /* Index into the mixing_reg array */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize all CAVE variables for A-key verification, as
     specified in "Common Cryptographic Algorithms", Exhibit 2.1-1 */

  if (qw_hi(A_key_under_test) != 0)
  {
    CAVE_lfsr = qw_hi(A_key_under_test);
  }
  else                          /* Don't let the shift register be zero */
  {
    CAVE_lfsr = CAVE_esn;
  }
  /* Copy the candidate A-key into mixing_reg */
  for (R_index = 0; R_index <= 7; R_index++)
  {
    CAVE_mixing_reg[R_index] = QW_BYTE(A_key_under_test, 7 - R_index);
  }

  CAVE_mixing_reg[8] = CAVE_AAV;    /* Copy algorithm version into mixing_reg */

  /* Copy the candidate A-key into mixing_reg again */
  for (R_index = 9; R_index <= 11; R_index++)
  {
    CAVE_mixing_reg[R_index] = QW_BYTE(A_key_under_test, 11 - R_index);
  }

  /* Copy the ESN into mixing_reg */
  for (R_index = 12; R_index <= 15; R_index++)
  {
    CAVE_mixing_reg[R_index] = B_PTR(CAVE_esn)[15 - R_index];
  }

  CAVE_offset_1 = 128;         /* Initial values for CAVE_table offset */
  CAVE_offset_2 = 128;

  CAVE_rounds(8);         /* Perform 8 rounds of the CAVE algorithm */

  /* Construct the check value out of mixing_reg */
  B_PTR(answer)[3] = 0;
  B_PTR(answer)[2] = (CAVE_mixing_reg[0] ^ CAVE_mixing_reg[13]) & 0x03;
  B_PTR(answer)[1] =   CAVE_mixing_reg[1] ^ CAVE_mixing_reg[14];
  B_PTR(answer)[0] =   CAVE_mixing_reg[2] ^ CAVE_mixing_reg[15];

  return (answer);

} /* end CAVE_calc_checksum() */

/*===========================================================================

FUNCTION CAVE_validate_A_key

DESCRIPTION
  This function checks the internal check digits of a manually-entered
  authentication key.

  The manually entered key consists of two parts.  The last six digits,
  taken as a numeric value, are the check digits.  The first 0 to 20
  digits, taken as a numeric value, is the A-key.  This function runs
  the A-key (and some other stuff) through the CAVE algorithm.  The
  output should match the value in the check digits.

DEPENDENCIES
  CAVE_init must be called before calling this function.

RETURN VALUE
  Returns TRUE if the A-key is validated, FALSE if not.

SIDE EFFECTS
  None.

===========================================================================*/

boolean  CAVE_validate_A_key
(
   const byte full_A_key[AUTH_A_KEY_DIGITS] /*lint !e737 */
   /* A-key to be validated, given as ascii digits */
)

{
  qword  A_key_under_test = { 0, 0 };
  /* A_key to check, in binary form */
  dword entered_checksum;
  /* Checksum entered, in binary form */
  word   i;
  /* Index */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  qw_set(A_key_under_test, 0L, 0L);
  entered_checksum = 0L;

  /* Convert A_key to 64-bit representation */
  for (i = 0; i < CAVE_DATA_DIGITS; i++)
  {
    qw_mul(A_key_under_test, A_key_under_test, 10L);
    qw_inc(A_key_under_test, (dword)(full_A_key[i] - '0'));
  }

  /* Convert checksum */
  for (i = 0; i < CAVE_CHECKSUM_DIGITS; i++)
  {
    entered_checksum = (entered_checksum * 10) +
       (full_A_key[i + CAVE_DATA_DIGITS] - '0');
  }

  /* Check if calculated checksum matches entered value */
  if (entered_checksum == CAVE_calc_checksum(A_key_under_test))
  {
    M1X_MSG(DCP, LEGACY_MED,
            "A-key validated successfully");
    return TRUE;
  }
  else
  {
    M1X_MSG(DCP, LEGACY_HIGH,
            "A-key invalid");
    return FALSE;
  }

} /* CAVE_validate_A_key */

/*===========================================================================

FUNCTION CAVE_update_A_key

DESCRIPTION
  This function installs a new A-key in the phone.

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the A-key is updated successfully, FALSE if not.

SIDE EFFECTS
  NV is updated: the new A-key is stored, and SSD is cleared.

===========================================================================*/

boolean  CAVE_update_A_key
(
   const byte full_A_key[AUTH_A_KEY_DIGITS],
   /* A-key to be put into NV, given as ascii digits */

   byte NAM_index
   /* Which NAM is to be updated */
)
{
  qword  new_A_key = { 0, 0 };
  /* 64 bit representation of the new A_key */

  nv_item_type nv_item;
  /* Item to hold values retrieved from NV */

  word i;
  /* index */
#ifdef EFS_HAVE_SYNC
  int res;
#endif /* EFS_HAVE_SYNC */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  qw_set(new_A_key, 0L, 0L);

/* -------------------------------------------------------------
** Convert A_key to 64-bit representation.  The last digits in
** the array are a checksum, which is not used in this function.
** ------------------------------------------------------------- */
  for (i = 0; i < CAVE_DATA_DIGITS; i++)
  {
    qw_mul(new_A_key, new_A_key, 10L);
    qw_inc(new_A_key, (dword)(full_A_key[i] - '0'));
  }

  qw_equ(CAVE_A_key, new_A_key);               /* Accept the new A-Key */
  qw_set(CAVE_SSD_A, 0L, 0L);                  /* Clear out SSD */
  qw_set(CAVE_SSD_B, 0L, 0L);

  nv_item.a_key.nam = NAM_index;
  qw_equ(nv_item.a_key.key, CAVE_A_key);
  auth_put_nv_item(NV_A_KEY_I, &nv_item);

/* ---------------------------
** Store zero'd out SSD in NV.
** --------------------------- */

  nv_item.ssd_a.nam = NAM_index;
  qw_equ(nv_item.ssd_a.ssd, CAVE_SSD_A);
  auth_put_nv_item(NV_SSD_A_I, &nv_item);

  nv_item.ssd_b.nam = NAM_index;
  qw_equ(nv_item.ssd_b.ssd, CAVE_SSD_B);
  auth_put_nv_item(NV_SSD_B_I, &nv_item);

#ifdef EFS_HAVE_SYNC
/* -----------------------------
** Kick watchdog and reset timer
** ----------------------------- */
  auth_dog_report();

  M1X_MSG(DCP, LEGACY_MED,
          "Synchronizing the EFS");
  res = efs_sync("/");
  if (res != 0)
  {
    ERR_FATAL("EFS sync failed", 0, 0, 0);
  }
  M1X_MSG(DCP, LEGACY_MED,
          "EFS sync after Akey update is successful.");
#endif /* EFS_HAVE_SYNC */

/* Perform Initialization here if necessary */

  M1X_MSG(DCP, LEGACY_MED,
          "A-key updated");

  return TRUE;

} /* CAVE_update_A_key */

/*===========================================================================

FUNCTION CAVE_update_A_key_683

DESCRIPTION
  This function installs a new A-key in the phone generated using IS-683A.

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE.

SIDE EFFECTS
  NV is updated: the new A-key is stored

===========================================================================*/

boolean  CAVE_update_A_key_683
(
   qword new_A_key,
   /* A-key to be put into NV */

   byte NAM_index
   /* Which NAM is to be updated */
)
{
#ifdef EFS_HAVE_SYNC
  int res;
#endif /* EFS_HAVE_SYNC */

  nv_item_type nv_item;
  /* Item to hold values retrieved from NV */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  qw_equ(CAVE_A_key, new_A_key);               /* Accept the new A-Key */

  nv_item.a_key.nam = NAM_index;
  qw_equ(nv_item.a_key.key, CAVE_A_key);
  auth_put_nv_item(NV_A_KEY_I, &nv_item);

#ifdef EFS_HAVE_SYNC
  /* -----------------------------
  ** Kick watchdog and reset timer
  ** ----------------------------- */
  auth_dog_report();

  M1X_MSG(DCP, LEGACY_MED,
          "Synchronizing the EFS");
  res = efs_sync("/");
  if (res != 0)
  {
    ERR_FATAL("EFS sync failed", 0, 0, 0);
  }
  M1X_MSG(DCP, LEGACY_MED,
          "EFS sync after Akey update is successful.");
#endif /* EFS_HAVE_SYNC */

  M1X_MSG(DCP, LEGACY_MED,
          "IS 683-A A-key updated");

  return TRUE;

} /* CAVE_update_A_key_683 */

/*===========================================================================

FUNCTION CAVE_generate_SSD

DESCRIPTION
  This function generates new A and B values of Shared Secret Data.

DEPENDENCIES
  CAVE_init must be called before this function is called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void  CAVE_generate_SSD
(
   qword  rand_ssd
   /* The random input to the SSD generation process */
)

{
  int    R_index;
  /* Index into the mixing_reg array */

  qword  a_key = { 0, 0 };
  /* The version of A-Key to use for this computation. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Use the specified version of A-Key. */
  qw_equ(a_key, a_key_temp_valid ? a_key_temp : CAVE_A_key);

  /* Initialize all CAVE variables for SSD Generation, as
     specified in "Common Cryptographic Algorithms", Section 2.2.1 */

  M1X_MSG(DCP, LEGACY_MED,
          "SSD rand_ssd %ld %ld",
          qw_hi(rand_ssd),
          qw_lo(rand_ssd));

  CAVE_lfsr = qw_lo(rand_ssd) ^ qw_hi(a_key) ^ qw_lo(a_key);

  if (CAVE_lfsr == 0)                 /* Don't let the shift register be zero */
  {
    CAVE_lfsr = qw_lo(rand_ssd);
  }

  /* Copy the A-key into mixing_reg */
  for (R_index = 0; R_index <= 7; R_index++)
  {
    CAVE_mixing_reg[R_index] = QW_BYTE(a_key, 7 - R_index);
  }

  CAVE_mixing_reg[8] = CAVE_AAV;    /* Copy algorithm version into mixing_reg */

  /* Copy the random input into mixing_reg */
  for (R_index = 9; R_index <= 11; R_index++)
  {
    CAVE_mixing_reg[R_index] = QW_BYTE(rand_ssd, 15 - R_index);
  }

  /* Copy the ESN into mixing_reg */
  for (R_index = 12; R_index <= 15; R_index++)
  {
    CAVE_mixing_reg[R_index] = B_PTR(CAVE_esn)[15 - R_index];
  }

  CAVE_offset_1 = 128;         /* Initial values for CAVE_table offset */
  CAVE_offset_2 = 128;

  CAVE_rounds(8);         /* Perform 8 rounds of the CAVE algorithm */

  /* Copy the mixing registers into the new SSD values, accounting
     for the reverse byte order of the mixing registers */
  for (R_index = 0; R_index <= 7; R_index++)
  {
    /*lint -e545  B_PTR does work on qwords, even if lint is suspicious. */
    B_PTR(CAVE_new_SSD_A)[7 - R_index] = CAVE_mixing_reg[R_index];
    B_PTR(CAVE_new_SSD_B)[7 - R_index] = CAVE_mixing_reg[R_index + 8];
    /*lint -restore */
  }
} /* CAVE_generate_SSD */

/*===========================================================================

FUNCTION CAVE_update_SSD

DESCRIPTION
  This function installs new A and B values of SSD into the phone.

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the SSD is updated successfully, FALSE if not.

SIDE EFFECTS
  NV is updated: the new SSD is stored.

===========================================================================*/

boolean CAVE_update_SSD(void)
{
#ifdef EFS_HAVE_SYNC
  int res;
#endif /* EFS_HAVE_SYNC */

  nv_item_type nv_item;
  /* Item to hold values retrieved from NV */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  qw_equ(CAVE_SSD_A, CAVE_new_SSD_A);           /* Accept the new SSD values */
  qw_equ(CAVE_SSD_B, CAVE_new_SSD_B);

  nv_item.ssd_a.nam = CAVE_NAM_index;
  qw_equ(nv_item.ssd_a.ssd, CAVE_SSD_A);
  auth_put_nv_item(NV_SSD_A_I, &nv_item);

  nv_item.ssd_b.nam = CAVE_NAM_index;
  qw_equ(nv_item.ssd_b.ssd, CAVE_SSD_B);
  auth_put_nv_item(NV_SSD_B_I, &nv_item);

#ifdef EFS_HAVE_SYNC
  /* -----------------------------
  ** Kick watchdog and reset timer
  ** ----------------------------- */
  auth_dog_report();

  M1X_MSG(DCP, LEGACY_MED,
          "Synchronizing the EFS");
  res = efs_sync("/");
  if (res != 0)
  {
    ERR_FATAL("EFS sync failed", 0, 0, 0);
  }
  M1X_MSG(DCP, LEGACY_MED,
          "EFS sync after SSD update is successful.");
#endif /* EFS_HAVE_SYNC */

  M1X_MSG(DCP, LEGACY_MED,
          "SSD updated");

  return TRUE;

} /* CAVE_update_SSD */

/*===========================================================================

FUNCTION CAVE_auth_signature

DESCRIPTION
  This function computes an authentication signature as specified in
  "Common Cryptographic Algorithms".

DEPENDENCIES
  CAVE_init must have been called before this function is called.

RETURN VALUE
  Returns the computed AUTH_SIGNATURE.

SIDE EFFECTS
  Changes value of CAVE_lfsr, CAVE_offset_1 and 2, CAVE_mixing_reg.

===========================================================================*/

dword  CAVE_auth_signature
(
   dword     rand_challenge,
   /* The random number unique to this authentication challenge */

   dword     auth_data,
   /* 24 bits of data to be signed */

   boolean   use_new_SSD,
   /* TRUE if new_SSD_A and new_SSD_B are to be used, FALSE if the
current SSD_A and SSD_B are to be used. */

   boolean   save_registers,
   /* TRUE if this authentication is to be used as seed for message
encryption and voice privacy, FALSE if not.  If TRUE, the
saved_* variables will be updated. */


   boolean   validating_spasm
   /* TRUE if we are calculating the auth_otapa for SPASM validation,
where SSD_AUTH input parameter is set to exclusive OR of SSD_A
and A_KEY. */

)

{
  qword  ssd = { 0, 0 };
  /* The version of Shared Secret Data to use for this computation. */

  int    R_index;
  /* Index into the mixing_reg array */

  dword  auth_signature;
  /* The authentication signature to be computed and returned. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  qw_equ(ssd, use_new_SSD ? CAVE_new_SSD_A : CAVE_SSD_A);
  /* Use the specified version of SSD. */


  /* If validating spasm, then SSD_AUTH input parameter is set to the
     exclusive OR of SSD_A and A_KEY. */
  if (validating_spasm)
  {
    for (R_index = 0; R_index <= 7; R_index++)
    {
      QW_BYTE(ssd, R_index) ^= QW_BYTE(CAVE_A_key, R_index);
    }
  }


  M1X_MSG(DCP, LEGACY_MED,
          "Auth Sig rand_challenge %ld",
          rand_challenge);
  M1X_MSG(DCP, LEGACY_MED,
          "Auth Sig auth_data %ld",
          auth_data);

  /* Initialize all CAVE variables for an authentication signature, as
     specified in "Common Cryptographic Algorithms", Exhibit 2.3-1 */

  CAVE_lfsr = rand_challenge ^ qw_hi(ssd) ^ qw_lo(ssd);
  if (CAVE_lfsr == 0)                 /* Don't let the shift register be zero */
  {
    CAVE_lfsr = rand_challenge;
  }

  /* Copy the Shared Secret Data into mixing_reg */
  for (R_index = 0; R_index <= 7; R_index++)
  {
    CAVE_mixing_reg[R_index] = QW_BYTE(ssd, 7 - R_index);
  }

  CAVE_mixing_reg[8] = CAVE_AAV;    /* Copy algorithm version into mixing_reg */

  /* Copy the data to be signed into mixing_reg */
  for (R_index = 9; R_index <= 11; R_index++)
  {
    CAVE_mixing_reg[R_index] = B_PTR(auth_data)[11 - R_index];
  }

  /* Copy the ESN into mixing_reg */
  for (R_index = 12; R_index <= 15; R_index++)
  {
    CAVE_mixing_reg[R_index] = B_PTR(CAVE_esn)[15 - R_index];
  }

  CAVE_offset_1 = 128;         /* Initial values for CAVE_table offset */
  CAVE_offset_2 = 128;

  CAVE_rounds(8);         /* Perform 8 rounds of the CAVE algorithm */

  /* Construct the signature from mixing_reg */
  B_PTR(auth_signature)[3] = 0;
  B_PTR(auth_signature)[2] = (CAVE_mixing_reg[0] ^ CAVE_mixing_reg[13]) & 0x03;
  B_PTR(auth_signature)[1] =  CAVE_mixing_reg[1] ^ CAVE_mixing_reg[14];
  B_PTR(auth_signature)[0] =  CAVE_mixing_reg[2] ^ CAVE_mixing_reg[15];

  /* Save the variables if we're supposed to */
  if (save_registers)
  {
    CAVE_saved_lfsr = CAVE_lfsr;
    CAVE_saved_offset_1 = CAVE_offset_1;
    CAVE_saved_offset_2 = CAVE_offset_2;
    CAVE_saved_rand = rand_challenge;
    CAVE_saved_data = auth_data;
  }

  return (auth_signature);

} /* CAVE_auth_signature */

/*===========================================================================

FUNCTION CAVE_roll_lfsr

DESCRIPTION
  This function performs a manipulation on the LFSR that is used
  repeatedly by the CMEA key and VPM generation process.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void  CAVE_roll_lfsr(void)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LFSR_A = CAVE_mixing_reg[0];
  LFSR_B = CAVE_mixing_reg[1];
  LFSR_C = CAVE_mixing_reg[14];
  LFSR_D = CAVE_mixing_reg[15];

  if (CAVE_lfsr == 0)                /* Don't let the shift register be zero */
  {
    CAVE_lfsr = CAVE_saved_rand;
  }
} /* CAVE_roll_lfsr */

/*===========================================================================

FUNCTION CAVE_generate_key

DESCRIPTION
  This function generates the CMEA key (used for message encryption)
  and voice privacy mask for a call.

DEPENDENCIES
  CAVE_auth_signature must have been called with save_registers set
  to TRUE before this function may be called.

RETURN VALUE
  None.

SIDE EFFECTS
  Changes value of CAVE_lfsr, CAVE_offset_1 and 2, CAVE_mixing_reg.
  Also, it sets the flag used by other tasks (MC) to determine if it is
  OK to call the encryption routines.

===========================================================================*/

void  CAVE_generate_key
(
   boolean do_VPM,
   /* Indicates whether or not the full VPM calculation is to be done */
   boolean *valid_CMEA_key_flag_ptr,
   /* Pointer allows immediate update of this flag for other tasks */
   boolean use_new_SSD
   /* If authr that was computed for re-auth response was based on new ssd,
this is set to TRUE */
)
{
  int    R_index;
  /* Index into the mixing_reg array */

  int    VPM_inx;
  /* Index into the voice privacy mask */

  int    i;
  /* Iteration counter */

  qword  ssd = { 0, 0 };
  /* The version of Shared Secret Data to base the keys on. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  qw_equ(ssd, use_new_SSD ? CAVE_new_SSD_B : CAVE_SSD_B);
  /* Use the specified version of SSD_B. */

  /* Initialize all CAVE variables for CMEA key and VPM generation, as
     specified in "Common Cryptographic Algorithms", Section 2.4 */

  CAVE_lfsr = CAVE_saved_lfsr ^ qw_hi(ssd) ^ qw_lo(ssd);
  if (CAVE_lfsr == 0)                /* Don't let the shift register be zero */
  {
    CAVE_lfsr = CAVE_saved_rand;
  }

  /* Copy the Shared Secret Data into mixing_reg */
  for (R_index = 0; R_index <= 7; R_index++)
  {
    CAVE_mixing_reg[R_index] = QW_BYTE(ssd, 7 - R_index);
  }

  CAVE_mixing_reg[8] = CAVE_AAV;    /* Copy algorithm version into mixing_reg */

  /* Copy the data that was signed by the original
     Auth_Signature for this call into mixing_reg */
  for (R_index = 9; R_index <= 11; R_index++)
  {
    CAVE_mixing_reg[R_index] = B_PTR(CAVE_saved_data)[11 - R_index];
  }

  /* Copy the ESN into mixing_reg */
  for (R_index = 12; R_index <= 15; R_index++)
  {
    CAVE_mixing_reg[R_index] = B_PTR(CAVE_esn)[15 - R_index];
  }

  CAVE_offset_1 = CAVE_saved_offset_1; /* Restore state of CAVE_table offsets */
  CAVE_offset_2 = CAVE_saved_offset_2;

/* Iteration 1: first pass through CAVE */
  CAVE_rounds(8);         /* Perform 8 rounds of the CAVE algorithm */

/* Iteration 2: generation of first CMEA key parameters */
  CAVE_roll_lfsr();
  CAVE_rounds(4);         /* Perform 4 rounds of the CAVE algorithm */

  CAVE_CMEA_key[0] = CAVE_mixing_reg[4] ^ CAVE_mixing_reg[8];
  CAVE_CMEA_key[1] = CAVE_mixing_reg[5] ^ CAVE_mixing_reg[9];
  CAVE_CMEA_key[2] = CAVE_mixing_reg[6] ^ CAVE_mixing_reg[10];
  CAVE_CMEA_key[3] = CAVE_mixing_reg[7] ^ CAVE_mixing_reg[11];

/* Iteration 3: generation of next CMEA key parameters */
  CAVE_roll_lfsr();
  CAVE_rounds(4);         /* Perform 4 rounds of the CAVE algorithm */

  CAVE_CMEA_key[4] = CAVE_mixing_reg[4] ^ CAVE_mixing_reg[8];
  CAVE_CMEA_key[5] = CAVE_mixing_reg[5] ^ CAVE_mixing_reg[9];
  CAVE_CMEA_key[6] = CAVE_mixing_reg[6] ^ CAVE_mixing_reg[10];
  CAVE_CMEA_key[7] = CAVE_mixing_reg[7] ^ CAVE_mixing_reg[11];

/* At this point the CMEA key is valid */
  *valid_CMEA_key_flag_ptr = TRUE;
  M1X_MSG(DCP, LEGACY_MED,
          "CMEA key calculated");

  if (do_VPM)
  {
    /* Iteration 4 to 13: generation of VPM */
    VPM_inx = 0;
    for (i = 0; i < 10; i++)
    {
      CAVE_roll_lfsr();
      CAVE_rounds(4);       /* Perform 4 rounds of the CAVE algorithm */
      for (R_index = 2; R_index <= 7; R_index++)
      {
        CAVE_VPM[VPM_inx++] =
           CAVE_mixing_reg[R_index] ^ CAVE_mixing_reg[R_index + 6];
      }
    }

    /* Iteration 14: generation of last VPM bits */
    CAVE_roll_lfsr();
    CAVE_rounds(4);         /* Perform 4 rounds of the CAVE algorithm */
    for (R_index = 2; R_index <= 6; R_index++)
    {
      CAVE_VPM[VPM_inx++] =
         CAVE_mixing_reg[R_index] ^ CAVE_mixing_reg[R_index + 6];
    }
  } /* end if (do_VPM) */

  M1X_MSG(DCP, LEGACY_MED,
          "VPM calculated");

} /* CAVE_generate_key */
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

/*===========================================================================

FUNCTION CAVE_tbox

DESCRIPTION
  This function performs the tbox primitive used in the CMEA
  message encryption algorithm.

DEPENDENCIES
  CMEA_key is used.

RETURN VALUE
  Returns the result of the tbox primitive.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL byte CAVE_tbox
(
   byte  z
   /* Input to the tbox primitive */
)

{
  int    k_index;
  /* Index into the CAVE table */

  byte   temp;
  /* Computation variable as defined for the tbox; eventually returned. */

  int    i;
  /* Iteration counter */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  k_index = 0;
  temp = z;

  for (i = 0; i < 4; i++)
  {
    temp ^= CAVE_CMEA_key[k_index++];
    temp += CAVE_CMEA_key[k_index++];      /* mod 256 */
    temp = z + CAVE_table[temp];      /* mod 256 */
  }

  return temp;
} /* CAVE_tbox */

/*===========================================================================

FUNCTION CAVE_encrypt

DESCRIPTION
  This function encrypts a message buffer, in place, using the CMEA
  algorithm from "Common Cryptographic Algorithms".

DEPENDENCIES
  CAVE_generate_key must have been called before this function is called.

RETURN VALUE
  None.  (But the message buffer is encrypted, in place.)

SIDE EFFECTS
  None.

===========================================================================*/

void  CAVE_encrypt
(
   byte   msg_buf[],
   /* Message buffer to be encrypted */

   int    msg_len
   /* Number of bytes in the message buffer */
)

{
  int  msg_inx;
  /* Index into the message buffer */

  byte   k, z;
  /* Temporary computation variables defined by the CMEA algorithm */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* First Manipulation */
  z = 0;
  for (msg_inx = 0; msg_inx < msg_len; msg_inx++)
  {
    msg_buf[msg_inx] += CAVE_tbox(z ^ (byte)msg_inx);  /* mod 256 */
    z += msg_buf[msg_inx];                        /* mod 256 */
  }

/* Second Manipulation */
  for (msg_inx = 0; msg_inx < msg_len / 2; msg_inx++)
  {
    msg_buf[msg_inx] ^= (msg_buf[msg_len - 1 - msg_inx] | 0x01); /*lint !e834 */
  }

/* Third Manipulation */
  z = 0;
  for (msg_inx = 0; msg_inx < msg_len; msg_inx++)
  {
    k = CAVE_tbox(z ^ (byte)msg_inx);
    z += msg_buf[msg_inx];      /* mod 256 */
    msg_buf[msg_inx] -= k;      /* mod 256, no borrow */
  }

} /* CAVE_encrypt */

/*===========================================================================

FUNCTION CAVE_private_lcm

DESCRIPTION
  This function returns the private long code mask for CDMA voice privacy.
  The private LCM is defined to be '01' concatenated with the 40 LSBs of VPM.

DEPENDENCIES
  CAVE_generate_key must have been called before this function is called.
  Depends on Intel byte order.

RETURN VALUE
  Returns the private long code mask.

SIDE EFFECTS
  None.

===========================================================================*/

void CAVE_private_lcm(ulpn_type *lcm)
{

  byte      inx;
  /* Index into the LCM */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (inx = 0; inx <= 3; inx++)
  {
    B_PTR(lcm->low)[inx] = CAVE_VPM[64 - inx];
  }

  lcm->high = 0x0100 | CAVE_VPM[60];

  M1X_MSG(DCP, LEGACY_HIGH,
          "Private LCM: High %0x, Low %0x",
          lcm->high,
          lcm->low);

} /* CAVE_private_lcm */

#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION CAVE_update_keys

DESCRIPTION
  This function updates the private long code mask for CDMA voice privacy.
  Also updates the CMEA key that is used by CAVE.

  The Argument *key* will be present only while CSIM/UICC is present
  and it carries the 8 byte Key Data.

  The Argument *data* carries the 65 byte VPM mask only for CSIM cases,
  For RUIM cases, it carries both Key and VPM mask.

DEPENDENCIES
  Depends on Intel byte order.

RETURN VALUE
  None

SIDE EFFECTS
  Update the CMEA and VPM.

===========================================================================*/

void CAVE_update_keys
(
#ifdef FEATURE_CSIM
   byte *key,
#endif
   byte *data
)
{
  size_t copy_size;
  /* Return value of memscpy function */
  MCC_CHK_NULL_PTR(data);
#ifdef FEATURE_CSIM
  MCC_CHK_NULL_PTR(key);

  /* copy the Key data returned by MMGSDI into CMEA_key */
  copy_size = memscpy(CAVE_CMEA_key, sizeof(CAVE_CMEA_key), key, MMGSDI_KEY_SIZE);
  if (copy_size != MMGSDI_KEY_SIZE)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
      copy_size,
      MMGSDI_KEY_SIZE,
      sizeof(CAVE_CMEA_key));
  }


  /* copy the 65 bytes of VPM mask returned by MMGSDI to CAVE_VPM */
  copy_size = memscpy(CAVE_VPM, CAVE_VPM_SIZE * sizeof(byte), data, CAVE_VPM_SIZE);
  if (copy_size != CAVE_VPM_SIZE)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
      copy_size,
      CAVE_VPM_SIZE,
      CAVE_VPM_SIZE * sizeof(byte));
  }


#else
  /* copy the first 8 bytes of key from RUIM into CMEA_key */
  copy_size = memscpy(CAVE_CMEA_key, sizeof(CAVE_CMEA_key), data, 8);
  if (copy_size != 8)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Truncated memscpy, bytes_copied:%d, dest_size:%d src_size:8",
      copy_size,
      sizeof(CAVE_CMEA_key));
  }


  /* copy the next 65 bytes of mask from RUIM into VPM */
  copy_size = memscpy(CAVE_VPM, CAVE_VPM_SIZE * sizeof(byte), data + 8, CAVE_VPM_SIZE);
  if (copy_size != CAVE_VPM_SIZE)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
      copy_size,
      CAVE_VPM_SIZE,
      CAVE_VPM_SIZE * sizeof(byte));
  }


#endif /* FEATURE_CSIM */
} /*lint !e818 */
#endif /* FEATURE_UIM_RUIM */

#if defined(FEATURE_IS2000_REL_A_AES)
/*===========================================================================

FUNCTION CAVE_GENERATE_CK_IK_USING_CMEA

DESCRIPTION
  This function genertes CK and IK using CMEA key. it calls MCCKEY MODULE callback
  function to generate appropriate CK(s) (and IKs when AKA is defined)

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void CAVE_generate_ck_ik_using_cmea
(
   uint8 key_id,
   boolean(*save_cmea_key)(
                           uint8 key_id,
                           const uint8 *cmea_key

                           )
)
{
  (void)(*save_cmea_key)(
                          key_id,
                          (uint8 *)(&CAVE_CMEA_key[0])
                        );
} /* CAVE_generate_ck_ik_using_cmea */


/*===========================================================================

FUNCTION CAVE_copy_cmea

DESCRIPTION
  This function copies the CMEA key generated using CAVE.

DEPENDENCIES
  Depends on Intel byte order.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void CAVE_copy_cmea
(
   byte  *data
)
{
  size_t copy_size;
  /* Return value of memscpy function */
  if (data != NULL)
  {
    copy_size = memscpy(data, 8 * sizeof(uint8), CAVE_CMEA_key, 8);
    if (copy_size != 8)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Truncated memscpy, bytes_copied:%d, dest_size:%d, src_size:8",
        copy_size,
        8 * sizeof(uint8));
    }

  }
} /* CAVE_copy_cmea */

#endif /* FEATURE_IS2000_REL_A_AES */

