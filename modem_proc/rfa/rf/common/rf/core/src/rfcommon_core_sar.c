/*!
   @file
   rfcommon_core_sar.c

   @brief
   This file contains all declarations and definitions necessary to use the RF
   SAR APIs.

   @details

*/

/*===========================================================================
Copyright (c) 2011 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/src/rfcommon_core_sar.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/11/13   krg     Fixed compiler warning.
03/11/13   sar     Updated file for APQ, GNSS only target.
01/29/13   pl      Fix KW warnings
09/20/12   pl      use direct function call for LTE instead of RFLTE_MC() macro
10/27/11    sr     made changes to trigger SAR state change for GSM.
10/25/11   ac      SAR fix for WCDMA
10/12/11   bmg     Added SV path support for SAR updates.
08/12/11   ap      Fix compiler warning 
08/11/11   ap      Modify Get SAR State API to remove mode parameter 
04/07/11   pl      Fix compiler warning
04/05/11   tnt     Fix compiler error
03/21/11   lcl     Fixed compiler warning
03/15/11   lcl     Moved code from rfm_common_sar.c here to create core layer
============================================================================*/ 

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "rfa_variation.h"
#include "rfm.h"
#include "rfcom.h"
#include "rfm_common_sar.h"
#include "rfm_types.h"
#include "rfm_mode_types.h"
#include "rfcommon_core_sar.h"
#include "rfm_internal.h"
#include "rfcommon_msg.h"

#ifdef FEATURE_WCDMA
#include "rfwcdma_mc.h"
#endif

#ifdef FEATURE_LTE
#include "rflte_mc.h"
#endif

#ifdef FEATURE_CDMA1X
#include "rf_1x_mc.h"
#endif

#ifdef FEATURE_HDR
#include "rf_hdr_mc.h"
#endif

#ifdef FEATURE_GSM
#include "rfgsm_mc.h"
#endif

/*===========================================================================

                           TYPEDEFS

===========================================================================*/
typedef struct 
{
  rfm_common_sar_state_type state;
  /* Each technology maintains their own override state */
  rfm_common_sar_state_type state_override[RFCOM_NUM_MODES];
  boolean                   override_is_enabled;
} rfcommon_core_sar_type;

/*===========================================================================

                           LOCAL VARIABLES

===========================================================================*/
static rfcommon_core_sar_type rfcommon_core_sar = 
{
  RF_SAR_STATE_DEFAULT,
  { RF_SAR_STATE_DEFAULT, /* RFCOM_PARKED_MODE */
    RF_SAR_STATE_DEFAULT, /* RFCOM_WCDMA_MODE */
    RF_SAR_STATE_DEFAULT, /* RFCOM_GSM_MODE */
    RF_SAR_STATE_DEFAULT, /* RFCOM_1X_MODE */
    RF_SAR_STATE_DEFAULT, /* RFCOM_GPS_MODE */
    RF_SAR_STATE_DEFAULT, /* RFCOM_1XEVDO_MODE */
    RF_SAR_STATE_DEFAULT, /* RFM_SVDO_MODE */ 
    RF_SAR_STATE_DEFAULT, /* RFCOM_GNSS_MODE */
    RF_SAR_STATE_DEFAULT, /* RFCOM_LTE_MODE */
    RF_SAR_STATE_DEFAULT, /* RFCOM_LTE_FTM_MODE */
    RF_SAR_STATE_DEFAULT, /*  RFM_TDSCDMA_MODE */
  },
  FALSE,
};

/*===========================================================================

                           INTERNAL FUNCTIONS

===========================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Sends a command to the active technology to trigger an update of the
  SAR state.
*/
void rfcommon_core_sar_trigger_update_device(rfm_device_enum_type device)
{
  rfm_mode_enum_type curr_mode; /* Var to store current mode */

  
  curr_mode = rfm_get_current_mode(device);
  switch (curr_mode)
  {
  #ifdef FEATURE_WCDMA
    case RFCOM_WCDMA_MODE:
    RFWCDMA_MC_V(cmd_dispatch)(RFM_DEVICE_0, RFWCDMA_MC_TRIGGER_SAR_UPDATE, NULL);
    break;
  #endif

  #if defined(FEATURE_GSM) && !defined(FEATURE_WPLT)
    case RFCOM_GSM_MODE:
      rfgsm_mc_update_sar_backoff(device);
      break;
  #endif

  #ifdef FEATURE_LTE
    case RFCOM_LTE_MODE:
      rflte_mc_tx_update_sar_backoff(device);
      break;
  #endif

  #ifdef FEATURE_CDMA1X
    case RFCOM_1X_MODE:
      rf_1x_mc_tx_update_sar_backoff();
      break;
  #endif /* FEATURE_CDMA1X */

  #ifdef FEATURE_HDR
    case RFCOM_1XEVDO_MODE:
      rf_hdr_mc_tx_update_sar_backoff();
      break;
  #endif /* FEATURE_HDR */

    default:
      break;
  }

  return;
} /* rfcommon_core_sar_trigger_update_device() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Inform all transmit-capable chains of a SAR state change.
*/
void rfcommon_core_sar_trigger_update(void)
{
  /* Device 0 is the "primary" transmit chain. */
  rfcommon_core_sar_trigger_update_device(RFM_DEVICE_0);

  /* Device 2 is the SV path transmit chain, if present. */
  rfcommon_core_sar_trigger_update_device(RFM_DEVICE_2);
} /* rfcommon_core_sar_trigger_update() */

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
void rfcommon_core_sar_set_state(rfm_common_sar_state_type sar_state)
{
  if (sar_state < RF_SAR_STATE_MAX)
  {
    rfcommon_core_sar.state = sar_state;
  }
  else
  {
    rfcommon_core_sar.state = RF_SAR_STATE_DEFAULT;
  }

  if (FALSE == rfcommon_core_sar.override_is_enabled)
  {
    rfcommon_core_sar_trigger_update();
  }

  return;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sets the SAR override state for a specific device.
*/
void
rfcommon_core_sar_set_overrride_device
(
  rfm_device_enum_type device,
  rfm_common_sar_override_type override
)
{
  rfm_mode_enum_type curr_mode; /* Var to store current mode */

  curr_mode = rfm_get_current_mode(device);

  switch (override)
  {
  /* OEMs can change this logic as desired, based on the states where one
   * wishes to use override mode and what is the desired override behavior */
  case RF_SAR_DEFAULT:
    rfcommon_core_sar.override_is_enabled = FALSE;
    break;

  case RF_SAR_EMERGENCY_CALL:
    switch (curr_mode)
    {
      /* This code can be changed to make the behavior technology-specific */
    default:     
      /* rfcommon_core_sar.state_override[rfm_mode[RFCOM_TRANSCEIVER_0]] = RF_SAR_STATE_DEFAULT; */
      /* NOTE TO OEM: 
         Set rfcommon_core_sar.override_is_enabled = TRUE for overriding SAR 
         during emergency calls. When set to TRUE, also un-comment out the line above.
       */      
      rfcommon_core_sar.override_is_enabled = FALSE;
      break;
    }
    break;

  case RF_SAR_ACCESS:
    switch (curr_mode)
    {
    case RFM_PARKED_MODE:
    case RFM_IMT_MODE:
    case RFM_EGSM_MODE:
    case RFM_1X_MODE:
    case RFM_GPS_MODE:
    case RFM_1XEVDO_MODE:
    case RFM_SVDO_MODE:
    case RFM_GNSS_MODE:
    case RFM_LTE_MODE:
    case RFM_LTE_FTM_MODE:
    case RFM_TDSCDMA_MODE:
      /* This code can be changed to make the behavior technology-specific */
      rfcommon_core_sar.state_override[curr_mode] = RF_SAR_STATE_DEFAULT;
      rfcommon_core_sar.override_is_enabled = TRUE;
    default:
      rfcommon_core_sar.override_is_enabled = FALSE;
      break;
    }
    break;

  default:
    break;
  }
  rfcommon_core_sar_trigger_update();

  return;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sets the SAR override state

  @details
  This overrides the normal SAR state operation. It is intended to be called
  by the L1 for each technology, and allows for SAR backoff to be bypassed
  in certain states such as access probes or E911 calls.
*/
void
rfcommon_core_sar_set_overrride
(
  rfm_common_sar_override_type override
)
{
  /* Device 0 is the "primary" transmit chain. */
  rfcommon_core_sar_set_overrride_device(RFM_DEVICE_0, override);

  /* Device 2 is the SV path transmit chain, if present. */
  rfcommon_core_sar_set_overrride_device(RFM_DEVICE_2, override);
} /* rfcommon_core_sar_set_overrride() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the current SAR state.

  @details
  This function is intended to be called by the RF driver or any other module
  that needs to get the current SAR state.

*/
rfm_common_sar_state_type rfcommon_core_sar_get_state(void)
{
  rfm_common_sar_state_type return_state;
  if (rfcommon_core_sar.override_is_enabled)
  {
    /* Remove Implementation for Override Mode in get_state API **
    ** Current implementations will not use Override            **
    ** Get_State API will always return SAR_STATE               */
    
    //return_state = rfcommon_core_sar.state_override[mode];
    return_state = rfcommon_core_sar.state;
  }
  else
  {
    return_state = rfcommon_core_sar.state;
  }

  if ( return_state > RF_SAR_STATE_MAX )
  {
    RF_MSG_1( RF_ERROR,
              "rfcommon_core_sar_get_state: SAR state %d is out of range.  Returning inactive SAR state.",
              return_state );
    return_state = RF_SAR_STATE_DEFAULT;
  }

  return return_state;
}
#endif /* FEATURE_GNSS_ONLY_NO_WWAN */
