#ifndef RF_RFFE_COMMON_H
#define RF_RFFE_COMMON_H
/*!
   @file
   rf_rffe_common.h

   @brief
   This file contains definition & prototypes for rffe protocol

*/
/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/08/15   dbc     Added extended product id programming function
06/16/14   vv      Update rffe functions parameters to physical device config structure
05/19/14   sn      added rffe chip id scan
10/24/13   cd      Fixed compiler warnings
09/30/13   cd      Added API to return MFG ID address as dummy-write address
01/17/13   sr      changes to assign USID and then create device objects
01/16/13   sr      changes to return usid assignment status in rffe_scan()
01/02/13   sn      added rffe_scan_ext to support extended mode
08/14/12   spa     Update rffescan to use device config information
08/10/12   tks     Added support for preparing script and write to hw
07/28/12   tks     Added support for rffe_prepare_buffer  
07/27/12   tks     Added rffe_write_to_hw and rffe_read_to_hw  
07/25/12   tks     Added rffe_scan for device factory support  
07/23/12   tks     Initial Revision

==============================================================================*/

#ifdef __cplusplus
extern "C" 
{
#endif
#include "rfcom.h"
#include "rf_hal_rffe.h"
#include "rf_hal_bus_types.h"
#include "rfdevice_cmn_intf.h" 
#include "rfc_common.h"

#ifdef __cplusplus
}
#endif

#include "rf_buffer_intf.h" 
#include "rfcommon_msg.h" 
#define RFFE_MAX_SCRIPT_SIZE 16 

#ifdef __cplusplus
extern "C" 
{
#endif
typedef enum
{
  RF_RFFE_READ_SINGLE  = 0,
  RF_RFFE_READ_NON_EXT = 1,
  RF_RFFE_READ_EXT     = 2
}rf_rffe_read_type;

typedef enum
{
  RF_RFFE_WRITE_SINGLE  = 0,
  RF_RFFE_WRITE_NON_EXT = 1,
  RF_RFFE_WRITE_EXT     = 2
}rf_rffe_write_type;

/*! @brief 
    RFFE transaction data type.   
*/
typedef struct
{
  rf_buffer_transaction_type rd_wr; /* 0 - WRITE 1 - READ */
  boolean rd_delay;                 /* read delay         */
  boolean half_rate;                /* half rate mode     */
  uint8 num_bytes;                  /* number of bytes    */
  uint16 addr;                      /* register address   */
  uint8 data;                       /* register data      */
  int16 start_delta;                /* start time in ??   */
} rf_rffe_data_type;              

/*----------------------------------------------------------------------------*/
boolean rffe_scan (rfc_phy_device_info_type* cfg); 

/*----------------------------------------------------------------------------*/

boolean rffe_program_assigned_usid (rfc_phy_device_info_type* cfg );

/*----------------------------------------------------------------------------*/
boolean rffe_scan_ext (rfc_phy_device_info_type* cfg); 

/*----------------------------------------------------------------------------*/

boolean rffe_program_assigned_usid_ext (rfc_phy_device_info_type* cfg );


/*----------------------------------------------------------------------------*/

boolean rffe_program_assigned_usid_ext_product_id (rfc_phy_device_info_type* cfg );

/*----------------------------------------------------------------------------*/
boolean rffe_set_gsid(void); 

/*----------------------------------------------------------------------------*/
boolean rffe_write_to_hw
(
  uint8 channel,
  uint8 slave_id,
  uint16 reg,
  uint8 data,
  rf_hal_bus_resource_script_settings_type* settings
); 

/*----------------------------------------------------------------------------*/
boolean rffe_write_to_hw_ext
(
  uint8 channel,
  uint8 slave_id,
  uint16 reg,
  uint8 data,
  rf_hal_bus_resource_script_settings_type* settings
); 

/*----------------------------------------------------------------------------*/
boolean rffe_read_from_hw
(
  uint8 channel,
  uint8 slave_id,
  uint16 reg,
  uint8 *data,
  rf_hal_bus_resource_script_settings_type* settings
); 

/*----------------------------------------------------------------------------*/
boolean rffe_prepare_buffer
(
  rf_buffer_intf *buffer,
  rf_rffe_data_type *device_script,
  rf_device_execution_type exec_type,
  uint8 channel,
  uint8 slave_id,
  uint8 script_size
);

/*----------------------------------------------------------------------------*/
uint8 rffe_scan_chip_id_ext (rfc_phy_device_info_type* cfg, uint8 addr );


/*----------------------------------------------------------------------------*/
boolean rffe_get_dummy_address
(
  uint8 *dummy_addr
);

#ifdef __cplusplus
}
#endif
#endif /*RF_RFFE_COMMON_H*/

