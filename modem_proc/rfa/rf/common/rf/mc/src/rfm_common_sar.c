/*!
   @file
   rfm_common_sar.c

   @brief
   This file contains all declarations and definitions necessary to use the RF
   SAR APIs.

   @details

*/

/*===========================================================================
Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/mc/src/rfm_common_sar.c#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/11/13   sar     Updated file for APQ, GNSS only target.
10/12/11   bmg     Added RFM critical section locking for thread safety
08/12/11    ap     Fix compiler warning  
08/11/11    ap     Implement Get SAR State API (without a mode parameter)
03/21/11   lcl     Fixed compiler warning
03/15/11   lcl     Added core layer.  Moved the code to core layer.
03/09/11   Saul    Added Tx backoff SAR support for 1x
                   rfm_common_sar_get_state now takes a mode
02/28/11   ka      Initial revision 
============================================================================*/ 

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "rfm_common_sar.h"
#include "rfcommon_core_sar.h"
#include "rfcommon_locks.h"
#include "rfm_internal.h"

/*===========================================================================

                          EXTERNAL FUNCTIONS

===========================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sets the SAR backoff state 

  @details
  This function is intended to be called by the applications processor to
  indicate the SAR backoff state. This state can be based on a combination
  of proximity sensors and active radio technologies and may cause a lower
  limit to the maximum transmit power than would otherwise occur.
 
  @pre
  rfm_enter_mode() must have been called
  

*/
void rfm_common_sar_set_state(rfm_common_sar_state_type sar_state)
{
  /* Changing driver state requires this operation be protected from
     other concurrent threads that may be in the process of updating
     the driver.
  */

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rfm_get_lock_data());

  rfcommon_core_sar_set_state(sar_state);

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rfm_get_lock_data());
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Gets the SAR backoff state 

  @details
  This function is intended to be called by the applications processor to
  read back the SAR backoff state. 
  
  @pre
  rfm_enter_mode() must have been called
  

*/
rfm_common_sar_state_type rfm_common_sar_get_state(void)
{
 return rfcommon_core_sar_get_state();
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sets the SAR override state

  @details
  This overrides the normal SAR state operation. It is intended to be called
  by the L1 for each technology, and allows for SAR backoff to be bypassed
  in certain states such as access probes or E911 calls.
 
  @pre
  rfm_enter_mode() must have been called

*/
void rfm_common_sar_set_overrride(rfm_common_sar_override_type override)
{
  /* Changing driver state requires this operation be protected from
     other concurrent threads that may be in the process of updating
     the driver.
  */

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rfm_get_lock_data());

  rfcommon_core_sar_set_overrride(override);

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rfm_get_lock_data());
}

#endif /* FEATURE_GNSS_ONLY_NO_WWAN */
