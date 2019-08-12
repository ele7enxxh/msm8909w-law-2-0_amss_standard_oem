/*===========================================================================
   R F  D r i v e r  M u l t i m o d e  E x t e r n a l  I n t e r f a c e
                           H e a d e r  F i l e
                                  F o r
                      Simultaneous voice and Data Module

DESCRIPTION
  This file contains all declarations and definitions necessary to use
  the RF drivers for SV module.

Copyright (c)  2011     by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/
#ifndef RFM_SV_H
#define RFM_SV_H
/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/sv/common/inc/rfm_sv.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/28/11   pl      remove unused header file
06/26/10   kma     Initial revision.
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
/*===========================================================================
                       PUBLIC MACROS/CONSTANTS
===========================================================================*/


/*===========================================================================
                       PUBLIC TYPE DEFINITIONS
===========================================================================*/

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

FUNCTION RFM_SV_GET_SAR_PWR_LIMITS_STATUS               

DESCRIPTION
  This function returns TRUE if regular pwr limit is
  used and FALSE if SAR/IM3 pwr limit is applied. This 
  API is designed specifically for targets with remote 
  RF driver.


DEPENDENCIES
  None

RETURN VALUE
  Status of power limit

SIDE EFFECTS
  None

===========================================================================*/
boolean rfm_sv_get_sar_pwr_limits_status( void );

#endif /* RFM_SV_H */
