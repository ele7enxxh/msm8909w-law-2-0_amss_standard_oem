/*!
   @file
   rfc_cdma_transceiver_hdet_adapter.cpp

   @brief
   Definition of the CDMA Transceiver device to HDET interface adapter.
*/

/*==============================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/common/src/rfc_cdma_transceiver_hdet_adapter.cpp#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/22/14   sbo     Removed critical section as same is moved to rfdevice cdma intf
08/26/13   shb     Access transceiver HDET APIs thru function pointer table 
                   directly instead of rfdev_intf wrapper functions
03/11/13   sar     Updated file for APQ, GNSS only target.
12/06/12   bmg     Added get_fw_trigger_script().
12/03/12   bmg     Initial version.

==============================================================================*/
#include "comdef.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#ifdef FEATURE_CDMA1X
#include "rfc_cdma_transceiver_hdet_adapter.h"
#include "rfcommon_msg.h"
#include "rfdevice_cdma_interface.h"

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  CDMA Transciever device to HDET interface adapter constructor.
 
  @details
  Must be initialized with the transceiver pointer that this object is
  adapting.
 
  @param cdma_tx_ptr
  Pointer to the CDMA transceiver device that the adapter will use.
*/
rfc_cdma_transceiver_hdet_adapter::rfc_cdma_transceiver_hdet_adapter
(
  rfdevice_cdma_tx_type* cdma_tx_ptr
) : cdma_tx_ptr(cdma_tx_ptr)
{
}

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  CDMA Transciever device to HDET interface adapter destructor.
*/
rfc_cdma_transceiver_hdet_adapter::~rfc_cdma_transceiver_hdet_adapter()
{
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Adapts the HDET disable_hdet() interface to the CDMA transceiver
  rfdevice_cdma_disable_hdet() interface.
 
  @details
  The CDMA transceiver disable HDET interface does not support scripting,
  so this function will exit with a failure if attempting to call it with
  a script parameter.
 
  @param buff_obj_ptr
  Unused.
 
  @param dev_action
  Must be RFDEVICE_EXECUTE_IMMEDIATE for the API to succeed.
 
  @return boolean
  Indicates if the API was successful.
*/
boolean
rfc_cdma_transceiver_hdet_adapter::disable_hdet
(
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action
)
{
  if ( dev_action != RFDEVICE_EXECUTE_IMMEDIATE )
  {
    RF_MSG_1(RF_ERROR,
             "rfc_cdma_transceiver_hdet_adapter::disable_hdet: "
             "Immediate execution is the only option supported. (%d)",
             dev_action);
    return FALSE;
  }

  boolean status; /* Return value */

  if ( (cdma_tx_ptr != NULL) && (cdma_tx_ptr->func_tbl != NULL) )
  {
    
    /* Call Device Driver to disable HDET */
    status = cdma_tx_ptr->func_tbl->disable_hdet( cdma_tx_ptr );


  }
  else
  {
    RF_MSG( RF_ERROR, "rfc_cdma_transceiver_hdet_adapter::disable_hdet: "
                      "Null Tx device obj" );
    status = FALSE;
  }

  return status;

} /* rfc_cdma_transceiver_hdet_adapter::disable_hdet() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Adapts the HDET enable_hdet() interface to the CDMA transceiver
  rfdevice_cdma_enable_hdet() interface.
 
  @details
  The CDMA transceiver enable HDET interface does not support scripting,
  so this function will exit with a failure if attempting to call it with
  a script parameter.
 
  @param buff_obj_ptr
  Unused.
 
  @param dev_action
  Must be RFDEVICE_EXECUTE_IMMEDIATE for the API to succeed.
 
  @return boolean
  Indicates if the API was successful.
*/
boolean
rfc_cdma_transceiver_hdet_adapter::enable_hdet
(
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action
)
{
  if ( dev_action != RFDEVICE_EXECUTE_IMMEDIATE )
  {
    RF_MSG_1(RF_ERROR,
             "rfc_cdma_transceiver_hdet_adapter::enable_hdet: "
             "Immediate execution is the only option supported. (%d)",
             dev_action);
    return FALSE;
  }

  boolean status; /* Return value */

  if ( (cdma_tx_ptr != NULL) && (cdma_tx_ptr->func_tbl != NULL) )
  {
  
    /* Call Device Driver to enable HDET */
    status = cdma_tx_ptr->func_tbl->enable_hdet( cdma_tx_ptr );


  }
  else
  {
    RF_MSG( RF_ERROR, "rfc_cdma_transceiver_hdet_adapter::enable_hdet: "
                      "Null Tx device obj" );
    status = FALSE;
  }

  return status;

} /* rfc_cdma_transceiver_hdet_adapter::enable_hdet() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Adapts the HDET get_hdet_param() interface to the CDMA transceiver
  rfdevice_cdma_tx_get_hdet_param() interface.
 
  @param hdet_param
  Container where HDET parameters will be populated by the device driver 
 
  @return boolean
  Indicates if the API was successful.
*/
boolean
rfc_cdma_transceiver_hdet_adapter::get_hdet_param
(
  rfdevice_cdma_hdet_param_type* hdet_param
)
{
  boolean status; /* Return value */

  if ( (cdma_tx_ptr != NULL) && (cdma_tx_ptr->func_tbl != NULL) )
  {
  
    /* Call Device Driver to get HDET param */
    cdma_tx_ptr->func_tbl->get_hdet_param( cdma_tx_ptr, hdet_param );



    status = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR, "rfc_cdma_transceiver_hdet_adapter::get_hdet_param: "
                      "Null Tx device obj" );
    status = FALSE;
  }

  return status;
} /* rfc_cdma_transceiver_hdet_adapter::get_hdet_param() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Adapts the HDET get_hdet_scripts() interface to the CDMA transceiver
  rfdevice_cdma_tx_get_hdet_scripts() interface.
 
  @param hdet_scripts
  Pointer to structure where HDET scripts will be populated
 
  @return boolean
  Indicates if the API was successful.
*/
boolean
rfc_cdma_transceiver_hdet_adapter::get_hdet_scripts
(
  rfdevice_cdma_hdet_scripts_type* hdet_scripts
)
{
  boolean status; /* Return value */

  if ( (cdma_tx_ptr != NULL) && (cdma_tx_ptr->func_tbl != NULL) )
  {
  
    /* Call Device Driver to get HDET scripts */
    cdma_tx_ptr->func_tbl->get_hdet_scripts( cdma_tx_ptr, hdet_scripts );



    status = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR, "rfc_cdma_transceiver_hdet_adapter::get_hdet_scripts: "
                      "Null Tx device obj" );
    status = FALSE;
  }

  return status;

} /* rfc_cdma_transceiver_hdet_adapter::get_hdet_scripts() */

boolean
rfc_cdma_transceiver_hdet_adapter::get_fw_trigger_script
(
  rf_buffer_intf* buff_obj_ptr
)
{
  rfdevice_cdma_hdet_param_type timing_params;
  rfdevice_cdma_hdet_scripts_type ssbi_script_data;
  boolean buffer_status;

  if ( get_hdet_param(&timing_params) == FALSE ||
       get_hdet_scripts(&ssbi_script_data) == FALSE )
  {
    RF_MSG(RF_ERROR,
           "rfc_cdma_transceiver_hdet_adapter::get_fw_trigger_script: "
           "Error getting Tx HDET timing or SSBI information");
    return FALSE;
  }

  int ssbi_script_iterator;

  /* PDET enable portion of the script */
  /* Assumed enable time before the Tx AGC event to allow the PDET circuit
     to warm up.  In microseconds. */
  const int PDET_ENABLE_ADVANCE_US = -52;
  for (ssbi_script_iterator = 0;
       ssbi_script_iterator < ssbi_script_data.pdet_on_script_size;
       ssbi_script_iterator++)
  {
    if ( ssbi_script_iterator == 0 )
    {
      /* First transaction needs to be set at absolute time.
         (Before the Tx AGC event) */
      buffer_status =
        buff_obj_ptr->append_ssbi(ssbi_script_data.pdet_on_script[ssbi_script_iterator].bus,
                                  ssbi_script_data.pdet_on_script[ssbi_script_iterator].address,
                                  ssbi_script_data.pdet_on_script[ssbi_script_iterator].data,
                                  RF_BUFFER_WRITE,
                                  PDET_ENABLE_ADVANCE_US,
                                  TRUE);
    }
    else
    {
      /* Remaining transactions are relative to the first. */
      buffer_status =
        buff_obj_ptr->append_ssbi(ssbi_script_data.pdet_on_script[ssbi_script_iterator].bus,
                                  ssbi_script_data.pdet_on_script[ssbi_script_iterator].address,
                                  ssbi_script_data.pdet_on_script[ssbi_script_iterator].data,
                                  RF_BUFFER_WRITE,
                                  0);
    }

    if ( buffer_status == FALSE )
    {
      RF_MSG(RF_ERROR,
             "rfc_cdma_transceiver_hdet_adapter::get_fw_trigger_script: "
             "Error creating PDET enable script");
      return FALSE;
    }
  } /* For each PDET enable SSBI */

  /* Trigger portion of the script */
  /* Expected worst-case time for the last Tx AGC adjustment to occur.
     In microseconds. */
  const int WORST_CASE_TXAGC_DELAY = 10;
  for (ssbi_script_iterator = 0;
       ssbi_script_iterator < ssbi_script_data.hkadc_trig_script_size;
       ssbi_script_iterator++)
  {
    if ( ssbi_script_iterator == 0 )
    {
      /* First transaction needs to be set at absolute time.
         (After the Tx AGC event) */
      buffer_status =
        buff_obj_ptr->append_ssbi(ssbi_script_data.hkadc_trig_script[ssbi_script_iterator].bus,
                                  ssbi_script_data.hkadc_trig_script[ssbi_script_iterator].address,
                                  ssbi_script_data.hkadc_trig_script[ssbi_script_iterator].data,
                                  RF_BUFFER_WRITE,
                                  (int16)(timing_params.pdet_lpf_settling_t + WORST_CASE_TXAGC_DELAY),
                                  TRUE);
    }
    else
    {
      /* Remaining transactions are relative to the first. */
      buffer_status =
        buff_obj_ptr->append_ssbi(ssbi_script_data.hkadc_trig_script[ssbi_script_iterator].bus,
                                  ssbi_script_data.hkadc_trig_script[ssbi_script_iterator].address,
                                  ssbi_script_data.hkadc_trig_script[ssbi_script_iterator].data,
                                  RF_BUFFER_WRITE,
                                  0);
    }

    if ( buffer_status == FALSE )
    {
      RF_MSG(RF_ERROR,
             "rfc_cdma_transceiver_hdet_adapter::get_fw_trigger_script: "
             "Error creating HKADC trigger script");
      return FALSE;
    }
  } /* For each HKADC trigger SSBI */

  /* PDET disable portion of the script */
  for (ssbi_script_iterator = 0;
       ssbi_script_iterator < ssbi_script_data.pdet_off_script_size;
       ssbi_script_iterator++)
  {
    if ( ssbi_script_iterator == 0 )
    {
      /* First transaction needs to be delayed by the HKADC converstion time,
         relative to the last transaction in the trigger script. */
      buffer_status =
        buff_obj_ptr->append_ssbi(ssbi_script_data.pdet_off_script[ssbi_script_iterator].bus,
                                  ssbi_script_data.pdet_off_script[ssbi_script_iterator].address,
                                  ssbi_script_data.pdet_off_script[ssbi_script_iterator].data,
                                  RF_BUFFER_WRITE,
                                  timing_params.hkadc_conversion_t,
                                  FALSE);
    }
    else
    {
      /* Remaining transactions are relative to the first. */
      buffer_status =
        buff_obj_ptr->append_ssbi(ssbi_script_data.pdet_off_script[ssbi_script_iterator].bus,
                                  ssbi_script_data.pdet_off_script[ssbi_script_iterator].address,
                                  ssbi_script_data.pdet_off_script[ssbi_script_iterator].data,
                                  RF_BUFFER_WRITE,
                                  0);
    }

    if ( buffer_status == FALSE )
    {
      RF_MSG(RF_ERROR,
             "rfc_cdma_transceiver_hdet_adapter::get_fw_trigger_script: "
             "Error creating PDET disable script");
      return FALSE;
    }
  } /* For each PDET disable SSBI */

  return TRUE;
} /* rfc_cdma_transceiver_hdet_adapter::get_fw_trigger_script() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stub function for HDET init in the CDMA HDET interface which has no
  corresponding transceiver device function.
 
  @details
  Transceiver device HDET initialization occurs automatically as part of
  Tx enable.  The HDET adapter assumes that MC will have already enabled Tx
  in the transceiver before operating HDET.
 
  @param buff_obj_ptr
  Unused.
 
  @param dev_action
  Unused.
 
  @param tx_freq
  Unused.
 
  @return
  Always returns TRUE.
*/
boolean
rfc_cdma_transceiver_hdet_adapter::hdet_init
(
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action,
  uint32 tx_freq
)
{
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Adapts the HDET read_hdet() interface to the CDMA transceiver
  rfdevice_cdma_read_hdet() interface.
 
  @param mdsp_triggered_read
  Flag indicating if MDSP has already triggered HDET read or not. If MDSP
  has triggered conversion, device driver will only read HDET results else
  it will trigger conversion within software.

  @param *hdet_output
  Container to store HDET reading and quality indicator 
 
  @return boolean
  Indicates if the API was successful.
*/
boolean
rfc_cdma_transceiver_hdet_adapter::read_hdet
(
  boolean mdsp_triggered_read,
  rfdevice_cdma_hdet_read_type* hdet_output
)
{
  boolean status; /* Return value */

  if ( (cdma_tx_ptr != NULL) && (cdma_tx_ptr->func_tbl != NULL) )
  {
  
    /* Call Device Driver to read HDET */
    cdma_tx_ptr->func_tbl->read_hdet( cdma_tx_ptr,
                                      mdsp_triggered_read,
                                      hdet_output );


    status = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR, "rfc_cdma_transceiver_hdet_adapter::read_hdet: "
                      "Null Tx device obj" );
    status = FALSE;
  }
  
  return status;
} /* rfc_cdma_transceiver_hdet_adapter::read_hdet() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Adapts the HDET setup_hdet() interface to the CDMA transceiver
  rfdevice_cdma_tx_setup_hdet() interface.
 
  @details
  The CDMA transceiver setup HDET interface does not support scripting,
  so this function will exit with a failure if attempting to call it with
  a script parameter.
 
  @param buff_obj_ptr
  Unused.
 
  @param dev_action
  Must be RFDEVICE_EXECUTE_IMMEDIATE for the API to succeed.
 
  @param mode
  Mode - 1x or HDR for which HDET on TX device must be setup.
 
  @param power_mode
  Specify whether HDET must be set up for high power mode or low power mode.
 
  @return boolean
  Indicates if the API was successful.
*/
boolean
rfc_cdma_transceiver_hdet_adapter::setup_hdet
(
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action,
  rfdevice_cdma_hdet_power_mode_type power_mode
)
{
  if ( dev_action != RFDEVICE_EXECUTE_IMMEDIATE )
  {
    RF_MSG_1(RF_ERROR,
             "rfc_cdma_transceiver_hdet_adapter::setup_hdet: "
             "Immediate execution is the only option supported. (%d)",
             dev_action);
    return FALSE;
  }

  boolean status;

  if ( (cdma_tx_ptr != NULL) && (cdma_tx_ptr->func_tbl != NULL) )
  {
  
    /* Call Device Driver to setup HDET */
    status = cdma_tx_ptr->func_tbl->setup_hdet( cdma_tx_ptr,
                                                RFM_1X_MODE,
                                                power_mode );

  }
  else
  {
    RF_MSG( RF_ERROR, "rfc_cdma_transceiver_hdet_adapter::setup_hdet: "
                      "Null Tx device obj" );
    status = FALSE;
  }
  
  return status;
 
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Adapts the HDET multi_read_hdet() interface to the CDMA transceiver
  rfdevice_cdma_multi_read_hdet() interface.
 
  @param num_of_reads
  Number of HDET samples to be taken for averaging.
 
  @return
  Average HDET read after sampling.
*/
uint16 rfc_cdma_transceiver_hdet_adapter::multi_read_hdet(uint16 num_of_reads)
{
  uint16 hdet_read = 0xFFFF;

  if ( (cdma_tx_ptr != NULL) && (cdma_tx_ptr->func_tbl != NULL) )
  {
  
    /* Call Device Driver to read HDET */
    hdet_read = cdma_tx_ptr->func_tbl->multi_read_hdet( cdma_tx_ptr,
                                                        num_of_reads );

  }
  else
  {
    RF_MSG( RF_ERROR, "rfc_cdma_transceiver_hdet_adapter::multi_read_hdet: "
                      "Null Tx device obj" );
  }
  
  return hdet_read;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Required implementation of the common init() function required by all
  classes derived from rfdevice_class.
 
  @details
  No operations are needed to initialize this adapter class.
*/
void rfc_cdma_transceiver_hdet_adapter::init()
{
}
#endif /* FEATURE_CDMA1X */
#endif /* FEATURE_GNSS_ONLY_NO_WWAN */
