/*!
  @file
  rfc_card_meas.c

  @brief
  This file contains functions to support the RF Card (RFC) module which
  manages the RF HW configuration.


*/

/*===========================================================================

Copyright (c) 2008 - 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/meas/rf/rfc/src/rfc_card_meas.c#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
11/13/12   aa      GRFC update for dime, compiler warning clean up
10/30/12   sr      Warnings fix
10/01/12   sb      Removed GRFC engine references in Dime code 
07/29/12   jfc     Fix merge. 7/09 change was reverted by accident.
07/09/12   spa     Removed cdma instance since CDMA does not call this API 
07/06/12   spa     Removed is_irat from rfc_cdma_generate_rx_wakeup_grfc_script 
05/14/12   zw      Updates for CDMA API
04/19/12   jfc     Remove unused RFD includes
11/09/11   zg      Updates for TDSCDMA iRAT. 
07/29/11   Saul    IRAT update. Skip setting GPIOs during IRAT.
07/23/11   Saul    Converted rx wake up APIs to use GRFC generate script.
07/19/11   whc     Populate W and G functions, cleanup old/unused APIs
07/15/11   whc     IRAT GRFC script support for autogen RFC  
07/13/11   vb      Removed usage of rfc_wcdma_get_meas_data() 
07/12/11   vb      rfc cleanup and rfc interface changes to suit the new auto-gen 
                   of rf card files
07/11/11   whc     Add API stub for rfc_meas_gen_grfc_script
07/11/11   cd      Removed support for legacy RFC functionality for CDMA
06/10/11   tnt     Add support for new GRFC info passing for IRAT
06/07/11   aro     Mainlined FEATURE_RF_SVDO_API
05/05/11   tnt     GRFC code clean-up
03/24/11   tnt     L<->W IRAT integration
03/08/11   tnt     IRAT integration for LTE
02/28/11   tnt     Merge to MDM9K PLF110
12/13/10   aro     Added Fixes to remove Linker Error on SVDO Build
12/13/10   aro     Added New API for band conversion
12/09/10   aro     Fixes for Zero-Compile Error SVDO Enabled Build
11/22/10   jyu     Provided work-around for GRFC script generation bug for G2L 
09/22/10   tnt     Remove hardcoded GRFC setting, setting are readding from RFC files
08/18/10   pl      Update GRFC script with timing information
08/18/10   pl      Support STOP script generation
08/10/10   pl      Revert to #2, remove non-shipped header file
08/10/10   pl      Revert to #1, remove non-shipped header file
07/29/10   tnt     Removing hardcode GRFC and read them from RFC files
07/22/10   pl      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfa_variation.h"
#include "rfc_card.h"
//#include "rfc_grfc_defs.h"
#include "rfc_card_meas.h"
#include "rfcommon_msm.h"
#include "rfc_common.h"

#ifdef FEATURE_WCDMA
#include "rfc_card_wcdma.h"
#include "rfc_meas_wcdma.h"
#endif
#ifdef FEATURE_CDMA1X
#include "rfc_cdma.h"
#include "rf_cdma_utils.h"
#endif
#ifdef FEATURE_GSM
#include "rfc_card_gsm.h"
#endif
#ifdef FEATURE_LTE
#include "rfc_card_lte.h"
#include "rfc_meas_lte.h"
#endif
#ifdef FEATURE_TDSCDMA
#include "rfc_card_tdscdma.h"
#endif

/*===========================================================================
                           Data Definitions
===========================================================================*/


/*===========================================================================
                           Functions
===========================================================================*/

/*!
  @brief
  This function creates a pair of values which are used to program the GRFC
  engines in IRAT cases.
  The value will follow the defintion in fw_rf_irat_intf.h

  @details

  @param mode      technology for which the grfc will be generated for
  @param band      band for which the grfc will be generated for
  @param device_info  structure containing rfm device info for pri and sec rx device
  @param grfc_ptr  ptr to structure holding the grfc info

*/
void rfc_meas_generate_grfc_script
( 
  rfm_mode_enum_type mode,
  uint32             band,
  rfc_grfc_device_type  *device_info,
  rf_buffer_intf *grfc_buf_ptr,
  boolean status
)
{

  if(grfc_buf_ptr == NULL || device_info == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_FATAL, "null pointer in rfc_meas_generate_grfc_script");
    return;
  }


  switch(mode)
  {
#ifdef FEATURE_WCDMA
    case RFCOM_WCDMA_MODE:

      rfc_meas_wcdma_generate_grfc_script(band, device_info, grfc_buf_ptr, status);
      break;
#endif

#ifdef FEATURE_LTE
    case RFCOM_LTE_MODE:
      
      // To be updated by LTE with Dime interface in the case needed
      //rfc_meas_lte_generate_grfc_script(band, device_info, grfc_buf_ptr);
      break;

#endif

#if defined(FEATURE_GSM)
    case RFCOM_GSM_MODE:

      break;
#endif

#if defined(FEATURE_TDSCDMA)
    case RFCOM_TDSCDMA_MODE:
       
      // To be updated by tdscdma with Dime interface in the case needed   
      //rfc_meas_tdscdma_generate_grfc_script(band, device_info, grfc_buf_ptr, TRUE);
      break;
#endif

    default:
      MSG(MSG_SSID_RF, MSG_LEGACY_FATAL, "Invalid source tech in rfc_meas_gen_grfc_info");
      break;
  }

  return;
}


