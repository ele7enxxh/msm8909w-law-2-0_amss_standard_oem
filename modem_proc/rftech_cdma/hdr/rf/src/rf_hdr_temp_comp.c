/*!
  @file
  rf_hdr_temp_comp.c

  @brief
  APIs to hdr temperature compensation.

  @details
  Defines APIs that are used for hdr temperature compensation.
 
  @addtogroup RF_CDMA_TEMP_COMP
  @{
 
*/

/*==============================================================================

  Copyright (c) 2011-2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

 
when       who   what, where, why
--------   ---   --------------------------------------------------------------- 
04/18/12   hdz   Changed interface of rf_hdr_temp_comp_read_handler() to
                 handle cmd from rf_apps_task
11/23/11   hdz   Initial version

==============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_hdr_temp_comp.h"
#include "rfcommon_msg.h"


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize hdr temp comp routine.

  @details
  This function will initialize 1x temperature compensation routine and define
  a dedicated timer for polling temperature sensor. The function should be 
  called before any other 1x temp comp functions.

  @param device
  Current device ID

  @param logical_dev
  Pointer to the current logical device
  
  @param temp_comp_data
  pointer to rf_cdma_hdr_temp_comp_data

  @return 
  TRUE on success and FALSE on failure
*/
boolean
rf_hdr_temp_comp_init
(
  rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rf_cdma_temp_comp_data_type* temp_comp_data
)
{
  boolean init_status = TRUE;

  init_status &= rf_cdma_temp_comp_init( device,
                                         logical_dev,
                                         temp_comp_data );
  
  return init_status;

} /*! rf_1x_temp_comp_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Start temp sensor polling. 

  @details
  Start the timer defined in rf_hdr_temp_comp_init(). This function should be
  called after rf_hdr_temp_comp_init().
  
  @param temp_comp_data
  Pointer to rf_cdma_hdr_temp_comp_data

  @param mode
  Current rf mode of operation
  
  @return 
  TRUE on success and FALSE on failure
*/
boolean
rf_hdr_temp_comp_start
(
  rf_cdma_temp_comp_data_type * temp_comp_data,
  rfm_mode_enum_type mode,
  const rfc_cdma_logical_device_params_type* logical_dev,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan
)
{

  boolean start_status = TRUE;

  start_status &= rf_cdma_temp_comp_start( temp_comp_data,
                                           RFM_1XEVDO_MODE,
                                           logical_dev,
                                           band,
                                           chan );   

  RF_MSG_1( RF_MED, "rf_hdr_temp_comp_start: temp_comp rf_cdma_temp_comp_start" 
            "succeed at device %d !!", temp_comp_data->device );
      
  return start_status;

} /*! rf_1x_temp_comp_start */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stop temp sensor polling. 

  @details
  Stop the active 1x temp comp timer.
  
  @param temp_comp_data
  Pointer to temp_comp_data.

  @return 
  TRUE on success and FALSE on failure
*/
boolean
rf_hdr_temp_comp_stop
(
  rf_cdma_temp_comp_data_type *temp_comp_data
)
{
  boolean stop_status = TRUE;

  stop_status &= rf_cdma_temp_comp_stop( temp_comp_data );

  return stop_status;

} /* rf_1x_temp_comp_stop */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Called to handle RF_1x_TEMP_COMP_READ_CMD command.

  @details
  This is the handler function to RF cmd RF_1X_TEMP_COMP_READ_CMD.  

  @param module_id
  Current module (1x,hdr,cdma or lte) of operation
  
  @param req_id
  RF command ID that this function handle

  @param req_payload_size
  The payload size of the RF command in the range of 0-72

  @param cmd_ptr
  Pointer to the rf command   
*/
void rf_hdr_temp_comp_read_handler
( 
  rf_apps_cmd_type* cmd_ptr
)
{
  
  rf_cdma_temp_comp_read_handler( cmd_ptr );
  
}  /*! end of rf_1x_temp_comp_read_handler  */
#endif /* FEATURE_CDMA1X */



