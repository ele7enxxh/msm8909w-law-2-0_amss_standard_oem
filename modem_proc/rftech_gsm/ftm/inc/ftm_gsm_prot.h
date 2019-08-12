#ifndef FTM_GSMPROT_H
#define FTM_GSMPROT_H

/*===========================================================================

          F T M  G S M   F U N C T I O N S  P R O T O T Y P E S
			                   H E A D E R  F I L E

DESCRIPTION
   This file contains GSM definitions which are also 
   contained in a file on the PC.  This keeps them in a place where they 
   can be easily modified and updated as necessary to aid in it's maintenance.

 
   Copyright (c) 2001, 2002, 2003 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  Initially, comments will be entered manually. Later, as the module is
  placed under version control, the version control tool will automatic-
  ally insert change comments into this section. [Delete this comment].

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/ftm/inc/ftm_gsm_prot.h#1 $ $DateTime: 2016/12/13 07:59:51 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/01   avi     Created.

=============================================================================*/

#include "comdef.h"
#include "ftm_gsm.h"

/*==============================================================================
                EXTERNALIZED   FUNCTION  PROTOTYPES
==============================================================================*/

/*==============================================================================
                           MACROS
==============================================================================*/
/*===========================================================================

MACRO SHADOW_VALUE

DESCRIPTION
   This macro allows to create a masked value and store it

DEPENDENCIES
   None.

RETURN VALUE
   Shadow value

SIDE EFFECTS
   None.

===========================================================================*/

#define SHADOW_VALUE(mask, val, shadow) (((shadow) & ~(mask)) | ((mask) & (val)))

/*===========================================================================

MACRO PA_POWER_LEVEL_GSM_900

DESCRIPTION
   This macro returns the enum name for the power level

DEPENDENCIES
   None.

RETURN VALUE
   Shadow value

SIDE EFFECTS
   None.

===========================================================================*/

#define PA_POWER_LEVEL_GSM_900(pcl) POWER_LEVEL_##pcl##_GSM_0900

/*===========================================================================

MACRO PA_POWER_LEVEL_GSM_1800

DESCRIPTION
   This macro returns the enum name for the power level for DCS 1800 system

DEPENDENCIES
   None.

RETURN VALUE
   Shadow value

SIDE EFFECTS
   None.

===========================================================================*/

#define PA_POWER_LEVEL_GSM_1800(pcl) POWER_LEVEL_##pcl##_GSM_1800

/*===========================================================================

MACRO PA_POWER_LEVEL_GSM_1900

DESCRIPTION
   This macro returns the enum name for the power level for PCS 1900 system

DEPENDENCIES
   None.

RETURN VALUE
   Shadow value

SIDE EFFECTS
   None.

===========================================================================*/

#define PA_POWER_LEVEL_GSM_1900(pcl) POWER_LEVEL_##pcl##_GSM_1900

#endif /* FTM_GSMPROT_H */
