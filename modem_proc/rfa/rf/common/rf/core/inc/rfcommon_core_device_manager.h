#ifndef RFCOMMON_CORE_DEVICE_MANAGER_H
#define RFCOMMON_CORE_DEVICE_MANAGER_H
/*!
   @file
   rfcommon_core_device_manager.h

   @brief
   This file contains enum, typedefs, API function prototypes for
   RF Common Core Device Manager for the RF Device[s].
 
   @addtogroup RF_COMMON_DEVICE_MANAGER
   @{
 
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
Copyright (c) 2012-13 by Qualcomm Technologies, Incorporated.  All Rights Reserved.                          
                              EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_core_device_manager.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
05/22/14   bm      Added interface for light_sleep() API
10/31/13   shb     Switch to C++ TRX common interface
05/06/13   pv      Triton Support for multiple transceivers for GPS.
01/06/13   dyc     Temp restore msg headers.
01/15/13   pv      Support for Dime.
06/08/12   cd      Fix for incorrect first HKADC reading in thermistor cal in CDMA mode (SVOC)
02/24/12   pv      Documentation Update. 
02/22/12   shb     Added RFCOMMON_CORE_DEVICE_MANAGER_H to avoid recursive 
                   inclusion of header - needed for offtarget compilation
01/16/12   pv      RF Common Device Manager Implementation. 
01/16/12   pv      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfm_device_types.h"
#include "rfcommon_locks.h"
#include "rfc_common.h"
#include "rfc_class.h"
#include "rfdevice_cmn_intf.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enum for of Device Status
*/

typedef enum
{
  /*! var that specifies Device is OFF */
  RFCMN_CORE_DEV_OFF = 0,
  /*! var that specifies Device is ON */
  RFCMN_CORE_DEV_ON = 1

} rfcmn_core_dev_status_type;

/*!
  @brief
  Enum for of Vote status type on a specific Device
*/

typedef enum
{
  /*! var that specifies Vote to turn OFF Device */
  RFCMN_CORE_DEV_VOTE_OFF = 0,
  /*! var that specifies Vote to turn ON Device */
  RFCMN_CORE_DEV_VOTE_ON = 1

} rfcmn_core_dev_vote_status_type;

/*!
  @brief
  Enum for of Vote type on a specific Device
*/

typedef enum
{
  /*! var that specifies No vote */
  RFCMN_CORE_DEV_NO_VOTE = 0,

  /*! var that specifies Normal vote to be used by all techs except GPS */
  RFCMN_CORE_DEV_NORMAL_VOTE, 

  /*! var that specifies Critical vote to be used by GPS tech only */
  RFCMN_CORE_DEV_CRITICAL_VOTE = RFCMN_CORE_DEV_NORMAL_VOTE,

  /*! var that specifies Non-Critical vote to be used by GPS tech only */
  RFCMN_CORE_DEV_NON_CRITICAL_VOTE,

} rfcmn_core_dev_vote_type;

/*!
  @brief
  Enum to ID a specific RF Device
*/

typedef enum
{
  /*! var that specifies Physical Dev ID */
  RFCMN_CORE_PHY_DEVICE_0 = 0,
  /*! var that specifies Physical Dev ID */
  RFCMN_CORE_PHY_DEVICE_1,
  /*! var that specifies Physical Dev ID */
  RFCMN_CORE_PHY_DEVICE_2,
  /*! Max var that specifies Physical Dev ID */
  RFCMN_CORE_MAX_PHY_DEVICES
}rfcmn_core_dev_id_type;

/*!
  @brief
  Structure for each unique Device Manager
*/

typedef struct 
{
  /*! var specifies physical Device pointer */
  rfdevice_rxtx_common_class *dev;

  /*! var specifies RF Device ID */
  rfcmn_core_dev_id_type dev_id;

  /*! var specifies current status of the RF Device */
  rfcmn_core_dev_status_type dev_status;

} rfcmn_core_dev_mgr_type;

/*!
  @brief
  Global Structure for managing all the Device Manager[s].
*/

typedef struct 
{
  /*! flag specifies debug msgs to be ON/OFF */
  boolean debug_flag;

  /*! flag specifies RF Dev Mgr Init completed or not */
  boolean init_done;
  
    /*! flag specifies GNSS based RF Dev detection completed or not */
  boolean gps_devices_detection_done;	

  /*! flag specifies a WWAN critical vote on RF Device */
  boolean is_wwan_vote_on;

  /*! flag specifies a GPS vote on RF Device */
  boolean is_gps_vote_on;
  
  /*! var specifies number of phyiscal RF devices */
  uint8 phy_dev_count;

  /*! array specifies voting type on specific RFM logical device */
  rfcmn_core_dev_vote_type voting_status[RFM_MAX_DEVICES];

  /*! This Param specifies RFM GPS device manager */
  rfcmn_core_dev_mgr_type *gps_dev_mgr[RFCMN_CORE_MAX_PHY_DEVICES];

  /*! var specifies RFM Device for GPS path */
  rfm_device_enum_type rfm_dev_for_gps;
  
  /*! array specifies the actual dev managers based on RFC  */
  rfcmn_core_dev_mgr_type dev_mgr[RFCMN_CORE_MAX_PHY_DEVICES];

  /*! Critical section lock for Device Manager */
  rf_lock_data_type lock;

}rfcmn_core_mgr_type;

/* Interface API to turn ON/OFF the Vote on a logical device*/
boolean rfcommon_core_dev_power_vote
(
  rfm_device_enum_type rfm_dev_val, 
  rfcmn_core_dev_status_type dev_status , 
  rfcmn_core_dev_vote_type dev_voting_type
); 

/* Interface API to put RF device to light sleep */
boolean rfcommon_core_dev_light_sleep(void);


/* Initialization API for RF Common Core Device Manager */
boolean rfcommon_core_dev_mgr_init(void);

boolean rfcommon_get_dev_power_voting_status
(
  rfm_device_enum_type rfm_dev_val
);

/*! @} */

#endif
