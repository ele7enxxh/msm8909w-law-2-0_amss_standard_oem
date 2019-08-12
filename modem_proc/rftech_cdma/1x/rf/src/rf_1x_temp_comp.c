/*!
  @file
  rf_1x_temp_comp.c

  @brief
  APIs to 1x temperature compensation.

  @details
  Defines APIs that are used for 1x temperature compensation.
 
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
04/17/12   hdz   Changed interface of rf_1x_temp_comp_read_handler()
12/02/11   hdz   Moved common functions to rf_cdma_temp_comp.c
11/28/11   hdz   Fixed the sanity check passed if NV_THERM_BINS_I are identical 
11/22/11   hdz   Added sanity check for temp comp related NV items. If check 
                 fails, do not enable temp comp timer.
11/21/11   aro   Mainlined debug variable to use new RF task dispatch
11/21/11   hdz   Fixed temp comp not restore HKADC back to HDET due to MDSP read 
                 fails 
11/17/11   shb   Added therm_supported flag in therm params from device to 
                 selectively enable/disable temp comp based on chipset info
11/12/11   hdz   Disable therm timer in calibration mode
11/11/11   hdz   Enabled 3ms timer
11/11/11   hdz   Fix bug that HKADC count doen't reset after one valid read
11/10/11   hdz   Restore HKADC setting back to HDET after therm read
11/09/11   cd    Added debug functionality to disable temp comp
11/07/11   hdz   Added rf_1x_configure_tx_AGC in rf_1x_temp_comp_read_handler()
11/02/11   hdz   Change arg list of rf_1x_temp_comp_start()
10/28/11   aro   Added Temp Comp Command to common dispatcher
10/27/11   hdz   Added supports for co-existence between temp comp and HDET 
10/24/11   hdz   Removed testing code for SW read 
10/20/11   hdz   Added Flag FW_THERM_READ 
10/20/11   hdz   Changed rf_1x_mdsp_configure_therm_data() to 
                 rf_1x_mdsp_configure_therm_data()
10/18/11   hdz   Fix Klocwork issue  
10/03/11   hdz   Fix according to review comments
09/15/11   hdz   Initial version

==============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_1x_temp_comp.h"
#include "rfcommon_msg.h"


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize 1x temp comp routine.

  @details
  This function will initialize 1x temperature compensation routine and define
  a dedicated timer for polling temperature sensor. The function should be 
  called before any other 1x temp comp functions.

  @param device
  Current device ID

  @param logical_dev
  Pointer to the current logical device
  
  @param temp_comp_data
  pointer to rf_cdma_1x_temp_comp_data

  @return 
  TRUE on success and FALSE on failure
*/
boolean
rf_1x_temp_comp_init
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
  Start the timer defined in rf_1x_temp_comp_init(). This function should be
  called after rf_1x_temp_comp_init().
  
  @param temp_comp_data
  Pointer to rf_cdma_1x_temp_comp_data

  @param mode
  Current rf mode of operation

  @param logical_dev
  Pointer to the logical device parameter information for the device that
  is starting temperature compensation.

  @param band
  The band that this device is operating in.

  @param chan
  The channel that this device is operating in.
  
  @return 
  TRUE on success and FALSE on failure
*/
boolean
rf_1x_temp_comp_start
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
                                           RFM_1X_MODE,
                                           logical_dev,
                                           band,
                                           chan );   

  RF_MSG_1( RF_MED, "rf_1x_temp_comp_start: temp_comp rf_cdma_temp_comp_start" 
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
rf_1x_temp_comp_stop
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

  @param cmd_ptr
  Pointer to the rf command   
*/
void rf_1x_temp_comp_read_handler
( 
  rf_apps_cmd_type* cmd_ptr
)
{
  
  rf_cdma_temp_comp_read_handler( cmd_ptr );
  
}  /*! end of rf_1x_temp_comp_read_handler  */

#endif /* FEATURE_CDMA1X */
