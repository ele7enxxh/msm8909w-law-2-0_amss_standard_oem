#ifndef MCCPD_H
#define MCCPD_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          M A I N  C O N T R O L

        P O S I T I O N   D E T E R M I N A T I O N   S E R V I C E S

GENERAL DESCRIPTION

  This file contains the support for Position Determination services.

EXTERNALIZED FUNCTIONS
  mcc_pd_init - Register the position determination service options with
                service negotiation manager (SNM).


INITIALIZATION AND SEQUENCING REQUIREMENTS
  mcc_pd_init needs to be called during the powerup.

  Copyright (c) 2000 - 2013 Qualcomm Technologies, Inc. 
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccpd.h_v   1.0.2.0   30 Nov 2001 17:12:38   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccpd.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/11   ag      Fixed compiler and lint warnings.
06/18/04   ank     Mainlined FEATURE_GPSONE_DBM.
06/28/00   yll     Initial revision.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"

#ifdef FEATURE_GPSONE

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION MCC_PD_INIT

DESCRIPTION
  This function initializes the service option descriptors for
  Position Determination Service Options. These descriptors are
  used throughout an Position Determination call for service
  configuration and service negotiation.

DEPENDENCIES
  FEATURE_GPSONE_DBM must be turned on.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mcc_pd_init
(
  void
);

/*===========================================================================

FUNCTION MCC_PD_SET_BS_INFO

DESCRIPTION
  This function initializes PD BS info data structure.
  use.
DEPENDENCIES

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
extern void mcc_pd_set_bs_info
(
  word pilot_pn,
  word sid,
  word nid,
  word base_id,
  uint32 base_lat,
  uint32 base_long
);

#endif /* FEATURE_GPSONE */

#endif /* MCCPD_H */
