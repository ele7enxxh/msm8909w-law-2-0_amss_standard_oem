#ifndef OTASPX_H
#define OTASPX_H
/*===========================================================================

     O T A S P   T R A N S L A T I O N   D A T A   S T R U C T U R E S

DESCRIPTION
  This file contains data structures and functions for OTASP translation
  services.  Translating OTASP messages to and from external and internal
  formats.

  Copyright (c) 1996 - 2013 Qualcomm Technologies, Inc. 
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

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/otaspx.h_v   1.0.2.0   30 Nov 2001 17:57:00   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/otaspx.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/04/12   ppr     Feature Cleanup
12/15/10   ag      SU level CMI clean up.
05/14/09   ag      CMI phase 2 changes
06/25/96   dna     Checked in initial version.
04/03/96   day     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "otaspi_i.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*     TYPES FOR OTASP PARAMETER VALUES                                    */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* OTASP Translation Status Codes                       */
/* Used to indicate status of OTASP message translation */
typedef enum
{
  OTASPX_DONE_S,           /* Successful translation completed           */
  OTASPX_INV_MSG_S,        /* Invalid OTASP message type detected        */
  OTASPX_INV_LEN_S         /* Invalid OTASP Data Message length detected */
} otaspx_status_type;

/* <EJECT> */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION OTASPX_EXT_TO_INT

DESCRIPTION
  Procedure which translates OTASP messages from external to internal
  format.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern word otaspx_ext_to_int
(
  word otasp_len,
    /* Length of OTASP message in bytes */
  byte *rx_otasp_ptr,
    /* Pointer to received message to be translated */
  otaspi_ftc_msg_type *int_otasp_ptr
    /* Pointer to place where translated message should be placed */
);

/* <EJECT> */
/*===========================================================================

FUNCTION OTASPX_INT_TO_EXT

DESCRIPTION
  Procedure which translates OTASP messages from internal to external
  format.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern word otaspx_int_to_ext
(
  word *len_ptr,
    /* Pointer to where message length, in bits should be placed */
  otaspi_rtc_msg_type *int_msg_ptr,
    /* Pointer to internal message to be translated */
  byte *tx_otasp_ptr
    /* Pointer to place where translated message should be placed */
);

#endif /* OTASPX_H */


