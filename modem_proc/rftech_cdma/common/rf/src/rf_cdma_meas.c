/*!
  @file
  rf_cdma_meas.c
 
  @brief
  RF Driver's Common CDMA IRAT (meas) interface file

  @details
  This file defines the Common CDMA Main Control interfaces
 
  @addtogroup RF_CDMA_COMMON_MEAS
  @{
*/

/*==============================================================================

  Copyright (c) 2010 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_meas.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/27/15   rs      Added NV control support for FR25072
11/23/15   rs      Add band as argument to wtr power down API
11/23/15   rs      Power down WTR in case of abort
07/08/13   spa     Add featurization for CDMA
07/05/13   spa     Added rf_cdma_meas_cleanup_ccs_events 
04/08/13   pl      Add null function pointer to support pre_config
11/07/12   pl      Added cleanup function pointer support in API structure
10/05/12   sty     Initial version

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#include "rfcommon_msg.h"
#include "rfmeas_mc.h"
#ifdef FEATURE_CDMA1X
#include "rf_cdma_data.h"
#include "rf_cdma_mc.h"

/*============================================================================*/
/*!
  @name APIs common to 1x and HDR for IRAT

  @brief
*/
/*! @{ */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Cleanup CCS events upon exit for 1x and HDR
 
  @details
  This function will be called to cleanup CCS events when exitting measurement
  This will also put the CCS event back into IDLE state, so that the event
  does not get stuck in active state (in case it was not executed by FW in 
  previous measurement)
 
  @param device
  device for which CCS events need to be cleaned up
 
  @return
  Status of function execution (TRUE/FALSE)
*/
boolean
rf_cdma_meas_cleanup_ccs_events 
( 
  rfm_device_enum_type device
)
{
  boolean success = TRUE ;

  /* Call API to cleanup CCS event and set the flag associated with that event 
     to IDLE, this is done to ensure that the event state is valid, even if 
     L1 chooses not to send pilot meas config to FW (for example if it is
     too close to the gap), so that in the next measurement the scripts are
     created properly */
  success &= rf_cdma_mc_cleanup_ccs_event( device, 
                                           RF_CDMA_EVENT_IRAT_TUNE_IN );
  if (  success == FALSE  )
  {
    RF_MSG ( RF_ERROR , "rf_cdma_meas_cleanup_ccs_events: "  
             "Couldnt clean up the event "
             "built for RF_CDMA_EVENT_IRAT_TUNE_IN " ) ;
  }
  else
  {
    RF_MSG ( RF_MED , "rf_cdma_meas_cleanup_ccs_events: "  
             "RF_CDMA_EVENT_IRAT_TUNE_IN clean up success!" ) ;
  }

  /* Cleanup Tune-out event. Even though in some scenarios
  this event will not be needed, it is only used if IRAT for LTE and 1x is on 
  seperate devices. Its safe to do unconditionally because this API will not
  mark it as failure, it will just throw an F3 mssg, which is ok*/ 
  success &= rf_cdma_mc_cleanup_ccs_event( device, 
                                           RF_CDMA_EVENT_IRAT_TUNE_OUT );
  if (  success == FALSE  )
  {
    RF_MSG ( RF_ERROR , "rf_cdma_meas_cleanup_ccs_events: "  
             "Couldnt clean up the event "
             "built for RF_CDMA_EVENT_IRAT_TUNE_OUT " ) ;
  }
  else
  {
    RF_MSG ( RF_MED , "rf_cdma_meas_cleanup_ccs_events: "  
             "RF_CDMA_EVENT_IRAT_TUNE_OUT clean up success!" ) ;
  }

  /* Cleanup Rx AGC LNA SM events. Its safe to do unconditionally because 
  this API will not mark it as failure, it will just throw an F3 mssg, 
  which is ok */
  success &= rf_cdma_mc_cleanup_rx_agc_lna_ccs_events( device );
  if ( success == FALSE )
  {
    RF_MSG ( RF_ERROR , "rf_cdma_meas_cleanup_ccs_events: "  
             "Couldnt clean up Rx AGC LNA SM events " ) ;
  }
  else
  {
    RF_MSG ( RF_MED , "rf_cdma_meas_cleanup_ccs_events: "  
             "Rx AGC LNA SM events clean up success!" ) ;
  }

  return success ;

}/* rf_cdma_meas_cleanup_ccs_events*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function will populate all the timing information related to IRAT
  measurements. This timing information will be used by L1 scheduling the 
  measurements during IRAT gap.
 
  @param rfm_meas_irat_info_type: data structure for all info to be returned
 
  @pre
  rfm_init() must have already been called. We should be in a certain 
  technology.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function
  did work correctly or not.
*/
rfm_meas_result_type 
rf_cdma_mc_irat_info_get
( 
  rfm_meas_irat_info_type *irat_info_param
)
{
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;

  RF_MSG ( RF_ERROR, "rf_cdma_mc_irat_info_get: NOT implemented" );

  return(result);

} /* rf_cdma_mc_irat_info_get */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is called first when the measurement is requested. Each
  technology basically prepares for the measurement.
 
  @param rfm_meas_enter_param : This is a pointer that basically has a header
  that has the target technology and the source technology. Based on this 
  header, the rest is casted as a tech specific pointer.
 
  @pre
  rfm_init() must have already been called.
  We should be in a certain technology.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function
  did work correctly or not.
*/
rfm_meas_result_type 
rf_cdma_mc_meas_enter
( 
  rfm_meas_enter_param_type *meas_enter_param
)
{
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;

  RF_MSG ( RF_ERROR, "rf_cdma_mc_meas_enter: NOT implemented" );

  return(result);

} /* rf_cdma_mc_meas_enter */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is called to prepare the clean-up and start-up scripts.
 
  @param rfm_meas_setup_param : This is a pointer that basically has a header
  that has the target technology and the source technology. Based on this 
  header, the rest is casted as a tech specific pointer.
 
  @pre
  rfm_meas_common_enter function must have been called.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function
  did work correctly or not.
*/
rfm_meas_result_type 
rf_cdma_mc_meas_build_scripts
( 
  rfm_meas_setup_param_type *meas_scripts_param
)
{
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;

  RF_MSG ( RF_ERROR, "rf_cdma_mc_meas_build_scripts: NOT implemented" );

  return(result);

} /* rf_cdma_mc_meas_build_scripts */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is called to exit the measurement mode. It is used to put the
  RF in the right state and do any clean-ups required.
 
  @param rfm_meas_setup_param : This is a pointer that basically has a header
  that has the target technology and the source technology. Based on this 
  header, the rest is casted as a tech specific pointer.
 
  @pre
  rfm_meas_common_script_enter function must have been called.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function
  did work correctly or not.
*/
rfm_meas_result_type 
rf_cdma_mc_meas_exit
( 
  rfm_meas_exit_param_type *meas_exit_param
)
{
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;

  RF_MSG ( RF_ERROR,"rf_cdma_mc_meas_exit: NOT implemented" );

  return(result);
} /* rf_cdma_mc_meas_exit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  List of function pointers needed for IRAT
 
*/
static rfmeas_mc_func_tbl_type rf_cdma_mc_meas_apis =             
{
  rf_cdma_mc_irat_info_get, /* rfmeas_mc_irat_info_get_fp*/
  rf_cdma_mc_meas_enter, /* rfmeas_mc_enter_fp */
  NULL,                  /* rfmeas_mc_pre_config_fp */
  NULL,                  /* rfmeas_build_reset_scripts_fp*/
  rf_cdma_mc_meas_build_scripts, /* rfmeas_build_scripts_fp */
  rf_cdma_mc_meas_exit, /* rfmeas_mc_exit_fp */
};

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function registers the IRAT APIs for CDMA/1X with meas module.
 
  @retval TRUE on success and FALSE on failure
*/
boolean 
rf_cdma_mc_meas_init
(
  void 
)
{

  boolean res;

  res = rfmeas_mc_register( (rfcom_mode_enum_type) RFM_1X_MODE, 
                            &rf_cdma_mc_meas_apis );

  return res;

} /* rf_cdma_mc_meas_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is called when the irat gap is aborted, it disables the wtr
  on the band on which irat was aborted
 
  @param rfm_device_enum_type device : The device on which irat was aborted
 
  @param band: The band on which irat gap was aborted 

  @retval
  TRUE: wtr power down success
  FALSE: wtr power down failure 
*/
boolean
rf_cdma_meas_power_down_wtr_for_abort
( 
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band
)
{
  boolean ret_val = TRUE;

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  /* Get Device Status Read Pointer*/
  dev_status_r = rf_cdma_get_device_status( device );

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  
  /* Perform NULL Pointer check */
  if ( ( dev_status_r == NULL ) || ( logical_dev == NULL ) )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_meas_power_down_wtr_for_abort: NULL data for "
                        "device %d", device );
    ret_val = FALSE;
  }
  else
  {
    RF_MSG_2( RF_LOW, "rf_cdma_meas_power_down_wtr_for_abort:calling wtr " 
              " sleep for device %d and band: %d", device,band );

    /* call device API to power down the wtr rx device */
    ret_val = 
    rfdevice_cdma_rx_pwr_sleep(logical_dev->rx_device[band]); 
  }

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  return ret_val;
} /* rf_cdma_meas_power_down_wtr_for_abort */

#endif /* FEATURE_CDMA1X */
/*! @} */

/*! @} */
