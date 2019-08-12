/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    M A I N   C O N T R O L   T A S K

                                 C D M A

              N E T W O R K   P E R S O N A L I Z A T I O N

GENERAL DESCRIPTION

  This file contains the lowest level functions to implement network
  personalization, for locking a subscription to a network.

EXTERNALIZED FUNCTIONS

  mccnp_imsi_valid
    Check an IMSI to confirm it is suitable to use

  mccnp_np_table_init -- requires FEATURE_OPEN_NETWORK_PERSONALIZATION
    Initialize the Network Personalization Table

INITIALIZATION AND SEQUENCING REQUIREMENTS

  When the external API is defined for initializing the network
  personalization table, it needs to be called before the validation.

FEATURES

  FEATURE_NETWORK_PERSONALIZATION
    Functions in this module

  FEATURE_OPEN_NETWORK_PERSONALIZATION
    External API to initialize the network personalization table

  Copyright (c) 2004 - 2014 Qualcomm Technologies, Inc. 
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

$PVCSPath$
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccnp.c#1 $
$DateTime: 2016/12/13 08:00:08 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/06/14   gga     1X_CP F3 redistribution
02/15/12   srk     Feature Cleanup: Removed FEATURE_UI_IMSI_ALL_0_OK
11/15/10   jtm     Initial SU API split for CP.
05/14/09   ag      CMI phase 2 changes
05/30/06   fc      Fixed lint errors.
12/10/04   sb      Fixed lint error.
11/19/04   va      Fixed compiler Warning
10/05/04   jah     initial version of FEATURE_NETWORK_PERSONALIZATION

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -e766*/
#include "1x_variation.h"
#include "customer.h"
#include "mccdma_v.h"     /* IMSI constants and mcc_default_imsi_s1() */
#include "mccdma_i.h"

#ifdef FEATURE_NETWORK_PERSONALIZATION
#include "mccnp.h"      /* typedef for cdma_imsi_check_type         */
#include "comdef.h"     /* common types used in this file           */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#if ! defined( FEATURE_OPEN_NETWORK_PERSONALIZATION )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Table of IMSI validation criteria                                       */
/*                                                                         */
/* IMSI fields as per IS-95A 6.3.1                                         */
/*                                                                         */
/* MCC = D1 * 100 + D2 * 10 + D3 - 111, where '0' digits value is 10       */
/*     '520' is 5 * 100 + 2 * 10 + 10 - 111, coded as decimal = 419        */
/*                                                                         */
/* MNC = D12 * 10 + D11 - 11, where '0' digits value is 10                 */
/*     '00' is 10 * 10 + 10 - 11, coded as binary=0x63 or 99 decimal       */
/*                                                                         */
/* S1 is 3 digits encoded like MCC, as 10 bits                             */
/*       1 digit  encoded as 4-bit BCD                                     */
/*       3 digits encoded like MCC, as 10 bits                             */
/*     '345689' = 0x3A9AA6                                                 */
/*                                                                         */
/* S2 is 3 digits encoded like MCC                                         */
/*     '321' is 3 * 100 + 2 * 10 + 1 - 111, coded as decimal = 210         */
/*                                                                         */

static const cdma_imsi_check_type mccnp_np_table[] = {
  {
    419,        /* (word)  MCC    */
    99,         /* (byte)  MNC    */
    0x00000000, /* (dword) S1 Min */
    0xFFFFFFFF, /* (dword) S1 Max */
    0x0000,     /* (word)  S2 Min */
    0xFFFF      /* (word)  S2 Max */
  }
};

const int mccnp_np_num_entries = (sizeof(mccnp_np_table) / sizeof(mccnp_np_table[1]));

#else

static cdma_imsi_check_type *mccnp_np_table=0;

static int mccnp_np_num_entries = 0;

/*===========================================================================

FUNCTION MCCNP_NP_TABLE_INIT

DESCRIPTION
  Initialize the Network Personalization Table

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mccnp_np_table_init
(
  cdma_imsi_check_type *np_table,
  int num_entries
)
{
  mccnp_np_table = np_table;
  mccnp_np_num_entries = num_entries;
}

#endif /* #else ! defined( FEATURE_OPEN_NETWORK_PERSONALIZATION ) */

/*===========================================================================

FUNCTION MCCNP_IMSI_VALID

DESCRIPTION
  Validate an IMSI using the network personalization table.

  If the network personalization table is not initialized,
  the IMSI will be declared invalid.

DEPENDENCIES
  None

RETURN VALUE
  True    IMSI is valid

  False   IMSI is not valid

SIDE EFFECTS
  None

===========================================================================*/

boolean mccnp_imsi_valid
(
  word  imsi_mcc,      /* Mobile Country Code (MCC) */
  byte  imsi_11_12,    /* Mobile Network Code (MNC) */
  dword imsi_s1,       /* IMSI_S1                   */
  word  imsi_s2        /* IMSI_S2                   */
)
{
  int i;
  boolean imsi_ok = FALSE;

  M1X_MSG( DCP, LEGACY_MED,
    "mcc %d, mnc %d",
    imsi_mcc,
    imsi_11_12);
  M1X_MSG( DCP, LEGACY_MED,
    "imsi_s1 %1x, imsi_s2 %d",
    imsi_s1,
    imsi_s2);
  M1X_MSG( DCP, LEGACY_MED,
    "imsi_s1 def %1x ",
    mcc_default_imsi_s1() );

  /* ---------------------------------------------------------------
  ** Check the IMSI against a table of values/ranges.
  ** If it matches an entry, it is a valid subscription.
  ** --------------------------------------------------------------- */

  #ifdef FEATURE_OPEN_NETWORK_PERSONALIZATION
  /* If there is no table, set the size to 0
  */
  if (mccnp_np_table == (void *) 0L)
  {
    mccnp_np_num_entries = 0;
  }
  #endif /* FEATURE_OPEN_NETWORK_PERSONALIZATION */

  /* If IMSI is not programmed, it is "OK", else check it.
  */
  if( imsi_mcc   == IMSI_MCC_ZERO         &&
      imsi_11_12 == IMSI_11_12_ZERO       &&
      imsi_s2    == IMSI_S2_ZERO          &&
      (imsi_s1   == mcc_default_imsi_s1()
      )
    )
  {
    imsi_ok = TRUE;
  }
  else
  {
    /* Initialize imsi_ok to Fail / "no match"
    */
    imsi_ok = FALSE;

    /* Loop over the table entries
    **
    ** Check MCC, MNC, S1 range, and S2 range
    **
    ** If there is a match, declare it valid, and stop checking
    */
    for (i = 0; i < mccnp_np_num_entries; i++)
    {
      if (imsi_mcc == mccnp_np_table[i].mcc        &&

          imsi_11_12 == mccnp_np_table[i].imsi_11_12 &&

          imsi_s2 >= mccnp_np_table[i].imsi_s2_min &&
          imsi_s2 <= mccnp_np_table[i].imsi_s2_max &&

          imsi_s1 >= mccnp_np_table[i].imsi_s1_min &&
          imsi_s1 <= mccnp_np_table[i].imsi_s1_max )
      {
        imsi_ok = TRUE;
        break;
      }
    }
  }

  return (imsi_ok);

} /* mccnp_imsi_valid */

#endif /* FEATURE_NETWORK_PERSONALIZATION */

