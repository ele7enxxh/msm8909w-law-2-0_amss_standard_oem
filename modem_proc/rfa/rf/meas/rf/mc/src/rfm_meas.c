/*!
   @file
   rfm_meas.c

   @brief
   This file contains all declarations and definitions necessary to use the RF
   Inter Freq Meas driver.

   @details
   
*/

/*===========================================================================
Copyright (c) 2008 - 2012 by Qualcomm Technologies, Incorporated.  All Rights

                           Reserved. EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/meas/rf/mc/src/rfm_meas.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/04/12   sr      IRAT code cleanup for dime. 
09/20/12   pl      remove obsolete interfaces
07/02/12   zg      Added enter_mode support for TDD LTE<->TDSCDMA. 
06/26/12   sbm     KW fixes. 
06/25/12   sbm     L-W abort support.
06/14/12   zg      KW fixes. 
06/13/12   jyu     Added support for TDD LTE<->TDSCDMA to avoid crash on L1
04/27/12   zg      Fixed shadowing location in RF Abort and compiling warnings.
04/12/12   zg      Fixed the problem of missing BREAKs in RF Abort switch cases. 
04/12/12   jyu     Added RF abort for T2G and G2T only                    
03/21/12   Saul    IRAT. 1X/HDR to set second device to sleep in L2DO/L21X.
03/20/12  Saul/aro IRAT. 1x2L 1x Stop Rx sequence change to delay premature 
                   RF sleep/vreg shutdown which affects LTE measurements.
03/02/12   zg/jyu  Updated call flow for TDSCDMA<->GSM 
03/02/12   zg/jyu  Added support for TDD LTE<->TDSCDMA 
02/09/12   adk     Updated 1x->LTE measurement sequence
02/03/12   av      IRAT cleanup phase 1
01/17/12   av      Fix call flow for XtoG
01/03/12   sty     fixed calls to RFM_DEBUG_EN_Q
12/21/11   aro     Added a new function to perform HDR cleanup which is
                   called before enter LTE script enter in DO2L measurement.
12/16/11   Saul    IRAT. D2L HDR Stop Rx sequence change to delay premature 
                   RF sleep/vreg shutdown which affects LTE measurements.
12/06/11   zg      Moved rf_tdscdma_mc_ttog_enter_qdsp6() from build_script to
                   iRAT enter API.
11/23/11   zg      Updates for T2G iRAT script building.
11/11/11   sr      removed unused IRAT code.
11/09/11   zg      Updates on building script for G2T iRAT. 
10/21/11   zg      Added TDSCDMA iRAT support.  
09/19/11   dw      Add get LtoW PLL timing support 
09/12/11   Saul    IRAT update. Skip building HDR script for DO->LTE case.
08/02/11   ckl     Move SSBI and PLL related definitions to rfmeas_types.h
07/29/11   av       Populate startup and cleanup time for GtoW
07/15/11   Saul    IRAT updates.
07/08/11   Saul    Preliminary IRAT changes.
07/08/11   Saul    Preliminary IRAT changes.
06/07/11   aro     Mainlined FEATURE_RF_SVDO_API
05/27/11   dw      Add more IRAT info timing parameters support
04/05/11   sar     Included rfm_helper_incs.h for debug code. 
02/24/11   tnt     Merge to MDM9k PLF110
02/01/11   jyu     Added initial support for L21x connected mode IRAT 
12/13/10   aro     Added Fixes to remove Linker Error on SVDO Build
12/09/10   aro     Fixes for Zero-Compile Error SVDO Enabled Build
12/07/10   aro     Added Initial SVDO Support
12/01/10   jyu/av  created local copy for the meas_param passed from L1 to convert 
                    GSM band to the RFCOM type. 
11/29/10   jyu     Added debug message for inter-RAT 
11/22/10   jyu     Provided work-around for GRFC script generation bug for G2L
11/11/10   jyu     Added LTE exit mode before re-enter DO in DO2L idle IRAT 
10/09/10   lcl     Remove compiler warning
10/29/10   jyu     Update rfm_meas_common_enter() to support L2G Idle measurement
10/05/10   jyu     Fix linker error and compilation warnings 
09/30/10   av      Using common LTE cold start apis for GtoL and LtoG.
09/29/10   av/jyu  Created new API rfm_meas_common_get_irat_info_param 
09/24/10   av      GtoL support 
09/14/10   av      LtoG support 
09/10/10   pl      Adding temporary LTE exit mode API for IRAT
09/09/10   kguo   Call wcdma tune back script for w2L at the right place
09/08/10   pl     Update get_time constant with hardcoded values for now
08/31/10   pl     Remove hdr build_script in idle mode measurement interfaces
08/20/10   pl     Added Script-based IRAT interfaces
08/08/10   pl     Added cleanup script support for L and DO
08/06/10   kguo   Added cleanup script for tune back to W 
07/27/10   av     Fix compiler warnings
07/08/10   pl     Clean up LTE when tuning back to W after L measurement is done
06/21/10   pl     Adding LTE RF device cleanup before entering other tech mode
05/25/10   pl     IRAT script generation for MDM9k 
05/24/10   kguo   Added L<->W build script API implementation
05/20/10   tws    IRAT for MDM9K.
05/17/10   ans    Updates for LTE to DO interRAT API interface.
05/05/10   kguo   Fixed wrong pointer cast for L<->W
04/19/10   kguo   Fixed compiler error
04/16/10   kguo   Fixed link error
04/15/10   kguo   Added support for L->W
03/19/10   can    Initial common measurement APIs and support.
03/02/10   lcl    Use unify rfm meas and changed them to return void instead.
03/01/10   tws    Add new measurement API functions.
11/02/09   av     Move RXFe intialization to rfm_setup_intersystem instead of enter_mode
10/14/08   sr     Initial version to separate meas specific external interface.

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "rfa_variation.h"
#include "rfcom.h"
#include "rfcommon_msg.h"
#include "rfm_types.h"
#include "rfmeas_mc.h"
#include "rfmeas_types.h"  /* Unify rfm_meas API definitions */
#include "rfm_helper_incs.h"



/*----------------------------------------------------------------------------*/
/*!
  @brief
    rfm_meas_common_get_irat_info_param function will basically allow RFSW to pass to
    L1 how much the start-up and clean-up scripts take, and IRAT specific info if necessary
    for individual IRAT combination.
 
  @param rfm_meas_irat_info_type: data structure for all info to be returned
  @param rfm_cb_handler_type: callback 
 
  @pre
  rfm_init() must have already been called.
  We should be in a certain technology.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/

rfm_meas_result_type rfm_meas_common_get_irat_info_param(rfm_meas_irat_info_type *rfm_meas_irat_info_param, 
                                                        rfm_cb_handler_type cb_handler )
{
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;

  result = rfmeas_mc_irat_info_get(rfm_meas_irat_info_param, cb_handler);
  return(result);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
    rfm_meas_get_time_constant function will basically allow RFSW to pass to
    L1 how much the start-up and clean-up scripts take.
 
  @param rfcom_mode_enum_type source: source technology
  @param rfcom_mode_enum_type target: target technology
 
  @pre
  rfm_init() must have already been called.
  We should be in a certain technology.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_time_const_type rfm_meas_common_get_time_constant ( rfcom_mode_enum_type source,  
                                                             rfcom_mode_enum_type target)
{
  rfm_meas_time_const_type time_const;

  time_const.start_up_time = 500;
  time_const.clean_up_time = 500;

  MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfm_meas_common_get_time_constant() is called, source_tech= %d, target_tech= %d", source, target);

  return(time_const);

}


/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called first when the measurement is requested. Each technology
    basically prepares for the measurement.
 
  @param rfm_meas_enter_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
  @param cb_handler : This is the call back function.
 
  @pre
  rfm_init() must have already been called.
  We should be in a certain technology.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rfm_meas_common_script_enter( rfm_meas_enter_param_type *rfm_meas_enter_param, 
                                                   rfm_cb_handler_type cb_handler )
{
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;

  result = rfmeas_mc_enter(rfm_meas_enter_param, cb_handler);

  if(result == RFM_MEAS_COMMON_FAILURE)
  {
    RF_MSG( RF_ERROR, "rfm_meas_common_script_enter() returning failure status." );
  }

  return(result);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called to prepare the clean-up and start-up scripts.
 
  @param rfm_meas_setup_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
  @param cb_handler : This is the call back function.
 
  @pre
  rfm_meas_common_enter function must have been called.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rfm_meas_common_script_build_scripts( rfm_meas_setup_param_type *rfm_meas_setup_param, 
                                                           rfm_cb_handler_type cb_handler )
{
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;

  result = rfmeas_build_scripts(rfm_meas_setup_param, cb_handler);

  if(result == RFM_MEAS_COMMON_FAILURE)
  {
    RF_MSG( RF_ERROR, "rfm_meas_common_script_build_scripts() returning failure status." );
  }

  return(result);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called to exit the measurement mode. It is used to put the RF
    in the right state and do any clean-ups required.
 
  @param rfm_meas_setup_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
  @param cb_handler : This is the call back function.
 
  @pre
  rfm_meas_common_script_enter function must have been called.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rfm_meas_common_script_exit( rfm_meas_exit_param_type *rfm_meas_exit_param, 
                                                  rfm_cb_handler_type cb_handler )
{
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;

  result = rfmeas_mc_exit(rfm_meas_exit_param, cb_handler);

  if(result == RFM_MEAS_COMMON_FAILURE)
  {
    RF_MSG( RF_ERROR, "rfm_meas_common_script_exit() returning failure status." );
  }

  return(result);
}

