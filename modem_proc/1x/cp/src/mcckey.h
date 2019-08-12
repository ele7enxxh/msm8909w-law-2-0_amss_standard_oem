#ifndef MCCKEY_H
#define MCCKEY_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    C D M A   M A I N   C O N T R O L

                       AKA/AES KEY MODULE

GENERAL DESCRIPTION

  This is the module for key management for AKA/AES

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2005 - 2013 Qualcomm Technologies, Inc. 
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


/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccbcsms.h_v   1.7   27 Jun 2002 19:49:36   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mcckey.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/04/12   ppr     Feature Cleanup
10/01/06   fh      Removed FEATURE_IS2000_REL_A_AES_ECK
07/26/06   fh      Added a new feature FEATURE_IS2000_REL_A_AES_ECK to keep
                   the key schedule (expanded CK) in buffer for future use
                   until the key is invalidated or regenerated.
07/24/06   fh      Cleaned up unused functions after code review
05/24/06   fh      AKA/AES: misclaneous fixes within AKA/AES features
05/17/06   fh      Merged in new development of AKA/AES features
05/26/06   fh      create this module

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -e766*/
#include "1x_variation.h"
#include "comdef.h"


#if defined(FEATURE_IS2000_REL_A_AES)

#ifdef FEATURE_IS2000_REL_A_AES
#include "mccaesalg.h"
#endif /* FEATURE_IS2000_REL_A_AES */


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#define MCCKEY_KEY_LENGTH          16  /* 16 bytes */
#define MCCKEY_AES_KEY_ID          0     /* when AKA is not enabled, key_id is always 0 */

/*===========================================================================

FUNCTION MCCKEY_SAVE_CK

DESCRIPTION
  This function saves the given CK with the current key_id when AKA is defined,
  or save to the location MCCAES_KEY_ID when AKA is not defined.

  If current key_id is not valid, do not save anything.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successfully saved, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mcckey_save_ck
(
  const uint8 *ck        /* the CK to be saved. */
);


/*===========================================================================

FUNCTION MCCKEY_CK_AVAILABLE_WITH_ID

DESCRIPTION
    This function checks if CK is available (valid) with the given key_id


DEPENDENCIES
  None

RETURN VALUE
  TRUE if available, FALSE if not

SIDE EFFECTS
  None

===========================================================================*/
boolean mcckey_ck_available_with_id
(
  uint8 key_id
);

/*===========================================================================

FUNCTION MCCKEY_CK_AVAILABLE

DESCRIPTION
    This function checks if CK (only one set because AKA is not defined) is
    available (valid)


DEPENDENCIES
  None

RETURN VALUE
  TRUE if available, FALSE if not

SIDE EFFECTS
  None

===========================================================================*/
boolean mcckey_ck_available
(
  void
);



/*===========================================================================

FUNCTION MCCKEY_INIT

DESCRIPTION
  This function initialize the static variables


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcckey_init
(
  void

);


/*===========================================================================

FUNCTION MCCKEY_SAVE_CMEA_KEY

DESCRIPTION
  If AKA is defined, this function expands the given CMEA key to create a pair
  of (IK, CK) and save it with the given key_id.

  If AKA is not defined, this function expands the given CMEA key to create
  a CK and save it for AES use only. In this case, key_id will be always
  MCCKEY_AES_KEY_ID (0)

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successfully saved, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mcckey_save_cmea_key
(
  uint8 key_id,     /* the key_id to be associated with. If AKA is not enabled,
                     * this key_id should be 0 */
  const uint8 *cmea_key   /* the CMEA Key to be used. */
);


/*===========================================================================

FUNCTION MCCKEY_GET_CK_WITH_ID

DESCRIPTION
  This function returns the CK with the given key_id

DEPENDENCIES
  None

RETURN VALUE
  TRUE if a valid key is set, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mcckey_get_ck_with_id
(
  uint8 key_id,    /* the key_id */
  uint8 *ck        /* the buffer for the CK */
);


#ifdef FEATURE_IS2000_REL_A_AES
/*===========================================================================

FUNCTION MCCKEY_GET_ECK_WITH_ID

DESCRIPTION
  This function returns the ECK (key schedule) calculated from the CK with
  the given key_id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcckey_get_eck_with_id
(
  boolean (*get_ck_with_id)(uint8 key_id, uint8 *ck),
  uint8 key_id,    /* the key_id */
  uint8 eck[MAXROUNDS+1][4][MAXBC]        /* the buffer for the ECK */
);

#endif /* FEATURE_IS2000_REL_A_AES */


/*===========================================================================

FUNCTION MCCKEY_GET_CK

DESCRIPTION
  This function returns the CK with the current key_id when AKA is defined,
  or the first set of CK when AES only

DEPENDENCIES
  None

RETURN VALUE
  TRUE if a valid key is set, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mcckey_get_ck
(
  uint8 *ck        /* the buffer for the CK */
);


#endif /* FEATURE_IS2000_REL_A_AES */
#endif /* MCCKEY_H */
