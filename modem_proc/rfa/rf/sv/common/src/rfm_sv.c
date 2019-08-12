/*===========================================================================
   R F  D r i v e r  M u l t i m o d e  E x t e r n a l  I n t e r f a c e
                           H e a d e r  F i l e
                                  F o r
                      Simultaneous voice and Data Module

DESCRIPTION
  This file contains all declarations and definitions necessary to use
  the RF drivers for SV module.

Copyright (c) 2010 - 2011      by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/sv/common/src/rfm_sv.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
11/11/11   Saul    SV Limiting. LTE backoff support.
11/10/11   Saul    SV Limiting. L1 interface update.
05/04/11   pl     SV-LTE SAR non-RPC implementation
05/04/11   pl     remove use of rfm_mode
04/28/11   pl     Remove Compiler warnings
06/26/10   kma    Initial revision.
============================================================================*/


/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "msg.h"
#include "rfm_sv.h"
#include "rfm_internal.h"
#include "rftxplim_mm_adjust_tx_power.h"

/*===========================================================================
                         LOCAL TYPE DEFINITIONS
===========================================================================*/

/*===========================================================================
              LOCAL MACRO, VARIABLE AND CONSTANT DEFINITIONS
===========================================================================*/

/*===========================================================================
                     LOCAL FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
                      PUBLIC FUNCTION DEFINITIONS
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
boolean rfm_sv_get_sar_pwr_limits_status( )
{
  switch(rfm_get_current_mode(RFM_DEVICE_0))
  {
  case RFCOM_1XEVDO_MODE:
    return rfsv_get_hdr_sar_pwr_limits_status();
    break;

  case RFCOM_LTE_MODE:
    return rfsv_get_lte_sar_pwr_limits_status();
    break;

  default:
    MSG_FATAL("Invalid mode for remote Max power control, mode = %d", rfm_get_current_mode(RFM_DEVICE_0), NULL, NULL);
    return TRUE;
  }
}
