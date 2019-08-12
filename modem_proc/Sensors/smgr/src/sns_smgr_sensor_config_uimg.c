/*=============================================================================
  @file sns_smgr_sensor_config.c

  This file initializes the constant table using sensor configuration

*******************************************************************************
* Copyright (c) 2014 - 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_sensor_config_uimg.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-06-29  agk  Power rail changes
  2015-01-22  BD     Changed the way of putting functions in uImage to use sections
  2014-06-11  vy   Renamed (original sns_smgr_sensor_config.c) for uImage
  2014-04-23  pn   Initial version

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "sns_smgr_main.h"
#include "sns_smgr_sensor_config.h"
#include "sns_dd.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/

  /* Constants for each sensor. Initialized from definitions in
     sns_smgr_sensor_config.h */
#ifdef ADSP_STANDALONE
SNS_SMGR_UIMAGE_DATA smgr_sensor_cfg_s smgr_sensor_cfg[ SNS_SMGR_NUM_SENSORS_DEFINED ] =
  {
   {
    SNS_SMGR_SENSOR_0_DD_FN_LIST_PTR,
    {0},
    (uint16_t)((SNS_SMGR_SENSOR_0_OFF_TO_IDLE/SNS_SMGR_USEC_PER_TICK)+1),
    (uint16_t)((SNS_SMGR_SENSOR_0_IDLE_TO_READY/SNS_SMGR_USEC_PER_TICK)+1),
    SNS_SMGR_SENSOR_0_ENUM_CODE,
    SNS_SMGR_SENSOR_0_BUS_ADDRESS,
    SNS_SMGR_SENSOR_0_RANGE_TYPE,
    SNS_SMGR_SENSOR_0_SENSITIVITY_DEFAULT,
    SNS_SMGR_SENSOR_0_REG_ITEM_TYPE,
    SNS_SMGR_SENSOR_0_REG_ITEM_ID,
    SNS_SMGR_SENSOR_0_CAL_PRI_TYPE,
    SNS_SMGR_SENSOR_0_CAL_PRI_ID,
    SNS_SMGR_SENSOR_0_DEVICE_ID,
    SNS_SMGR_SENSOR_0_FLAGS,
    {SNS_SMGR_SENSOR_0_DATA_TYPE_1, SNS_SMGR_SENSOR_0_DATA_TYPE_2},
    SNS_SMGR_SENSOR_0_GPIO_FIRST,
    SNS_SMGR_SENSOR_0_GPIO_SECOND,
    SNS_SMGR_SENSOR_0_BUS_INSTANCE,
    SNS_SMGR_SENSOR_0_VDD,
    SNS_SMGR_SENSOR_0_VDDIO
   },

   {
    SNS_SMGR_SENSOR_1_DD_FN_LIST_PTR,
    {0},
    (uint16_t)((SNS_SMGR_SENSOR_1_OFF_TO_IDLE/SNS_SMGR_USEC_PER_TICK)+1),
    (uint16_t)((SNS_SMGR_SENSOR_1_IDLE_TO_READY/SNS_SMGR_USEC_PER_TICK)+1),
    SNS_SMGR_SENSOR_1_ENUM_CODE,
    SNS_SMGR_SENSOR_1_BUS_ADDRESS,
    SNS_SMGR_SENSOR_1_RANGE_TYPE,
    SNS_SMGR_SENSOR_1_SENSITIVITY_DEFAULT,
    SNS_SMGR_SENSOR_1_REG_ITEM_TYPE,
    SNS_SMGR_SENSOR_1_REG_ITEM_ID,
    SNS_SMGR_SENSOR_1_CAL_PRI_TYPE,
    SNS_SMGR_SENSOR_1_CAL_PRI_ID,
    SNS_SMGR_SENSOR_1_DEVICE_ID,
    SNS_SMGR_SENSOR_1_FLAGS,
    {SNS_SMGR_SENSOR_1_DATA_TYPE_1, SNS_SMGR_SENSOR_1_DATA_TYPE_2},
    SNS_SMGR_SENSOR_1_GPIO_FIRST,
    SNS_SMGR_SENSOR_1_GPIO_SECOND,
    SNS_SMGR_SENSOR_1_BUS_INSTANCE,
    SNS_SMGR_SENSOR_1_VDD,
    SNS_SMGR_SENSOR_1_VDDIO
   },

   {
    SNS_SMGR_SENSOR_2_DD_FN_LIST_PTR,
    {0},
    (uint16_t)((SNS_SMGR_SENSOR_2_OFF_TO_IDLE/SNS_SMGR_USEC_PER_TICK)+1),
    (uint16_t)((SNS_SMGR_SENSOR_2_IDLE_TO_READY/SNS_SMGR_USEC_PER_TICK)+1),
    SNS_SMGR_SENSOR_2_ENUM_CODE,
    SNS_SMGR_SENSOR_2_BUS_ADDRESS,
    SNS_SMGR_SENSOR_2_RANGE_TYPE,
    SNS_SMGR_SENSOR_2_SENSITIVITY_DEFAULT,
    SNS_SMGR_SENSOR_2_REG_ITEM_TYPE,
    SNS_SMGR_SENSOR_2_REG_ITEM_ID,
    SNS_SMGR_SENSOR_2_CAL_PRI_TYPE,
    SNS_SMGR_SENSOR_2_CAL_PRI_ID,
    SNS_SMGR_SENSOR_2_DEVICE_ID,
    SNS_SMGR_SENSOR_2_FLAGS,
    {SNS_SMGR_SENSOR_2_DATA_TYPE_1, SNS_SMGR_SENSOR_2_DATA_TYPE_2},
    SNS_SMGR_SENSOR_2_GPIO_FIRST,
    SNS_SMGR_SENSOR_2_GPIO_SECOND,
    SNS_SMGR_SENSOR_2_BUS_INSTANCE,
    SNS_SMGR_SENSOR_2_VDD,
    SNS_SMGR_SENSOR_2_VDDIO
   },

   {
    SNS_SMGR_SENSOR_3_DD_FN_LIST_PTR,
    {0},
    (uint16_t)((SNS_SMGR_SENSOR_3_OFF_TO_IDLE/SNS_SMGR_USEC_PER_TICK)+1),
    (uint16_t)((SNS_SMGR_SENSOR_3_IDLE_TO_READY/SNS_SMGR_USEC_PER_TICK)+1),
    SNS_SMGR_SENSOR_3_ENUM_CODE,
    SNS_SMGR_SENSOR_3_BUS_ADDRESS,
    SNS_SMGR_SENSOR_3_RANGE_TYPE,
    SNS_SMGR_SENSOR_3_SENSITIVITY_DEFAULT,
    SNS_SMGR_SENSOR_3_REG_ITEM_TYPE,
    SNS_SMGR_SENSOR_3_REG_ITEM_ID,
    SNS_SMGR_SENSOR_3_CAL_PRI_TYPE,
    SNS_SMGR_SENSOR_3_CAL_PRI_ID,
    SNS_SMGR_SENSOR_3_DEVICE_ID,
    SNS_SMGR_SENSOR_3_FLAGS,
    {SNS_SMGR_SENSOR_3_DATA_TYPE_1, SNS_SMGR_SENSOR_3_DATA_TYPE_2},
    SNS_SMGR_SENSOR_3_GPIO_FIRST,
    SNS_SMGR_SENSOR_3_GPIO_SECOND,
    SNS_SMGR_SENSOR_3_BUS_INSTANCE,
    SNS_SMGR_SENSOR_3_VDD,
    SNS_SMGR_SENSOR_3_VDDIO
   },

   {
    SNS_SMGR_SENSOR_4_DD_FN_LIST_PTR,
    {0},
    (uint16_t)((SNS_SMGR_SENSOR_4_OFF_TO_IDLE/SNS_SMGR_USEC_PER_TICK)+1),
    (uint16_t)((SNS_SMGR_SENSOR_4_IDLE_TO_READY/SNS_SMGR_USEC_PER_TICK)+1),
    SNS_SMGR_SENSOR_4_ENUM_CODE,
    SNS_SMGR_SENSOR_4_BUS_ADDRESS,
    SNS_SMGR_SENSOR_4_RANGE_TYPE,
    SNS_SMGR_SENSOR_4_SENSITIVITY_DEFAULT,
    SNS_SMGR_SENSOR_4_REG_ITEM_TYPE,
    SNS_SMGR_SENSOR_4_REG_ITEM_ID,
    SNS_SMGR_SENSOR_4_CAL_PRI_TYPE,
    SNS_SMGR_SENSOR_4_CAL_PRI_ID,
    SNS_SMGR_SENSOR_4_DEVICE_ID,
    SNS_SMGR_SENSOR_4_FLAGS,
    {SNS_SMGR_SENSOR_4_DATA_TYPE_1, SNS_SMGR_SENSOR_4_DATA_TYPE_2},
    SNS_SMGR_SENSOR_4_GPIO_FIRST,
    SNS_SMGR_SENSOR_4_GPIO_SECOND,
    SNS_SMGR_SENSOR_4_BUS_INSTANCE,
    SNS_SMGR_SENSOR_4_VDD,
    SNS_SMGR_SENSOR_4_VDDIO
   },
   
   {
    SNS_SMGR_SENSOR_5_DD_FN_LIST_PTR,
    {0},
    (uint16_t)((SNS_SMGR_SENSOR_5_OFF_TO_IDLE/SNS_SMGR_USEC_PER_TICK)+1),
    (uint16_t)((SNS_SMGR_SENSOR_5_IDLE_TO_READY/SNS_SMGR_USEC_PER_TICK)+1),
    SNS_SMGR_SENSOR_5_ENUM_CODE,
    SNS_SMGR_SENSOR_5_BUS_ADDRESS,
    SNS_SMGR_SENSOR_5_RANGE_TYPE,
    SNS_SMGR_SENSOR_5_SENSITIVITY_DEFAULT,
    SNS_SMGR_SENSOR_5_REG_ITEM_TYPE,
    SNS_SMGR_SENSOR_5_REG_ITEM_ID,
    SNS_SMGR_SENSOR_5_CAL_PRI_TYPE,
    SNS_SMGR_SENSOR_5_CAL_PRI_ID,
    SNS_SMGR_SENSOR_5_DEVICE_ID,
    SNS_SMGR_SENSOR_5_FLAGS,
    {SNS_SMGR_SENSOR_5_DATA_TYPE_1, SNS_SMGR_SENSOR_5_DATA_TYPE_2},
    SNS_SMGR_SENSOR_5_GPIO_FIRST,
    SNS_SMGR_SENSOR_5_GPIO_SECOND,
    SNS_SMGR_SENSOR_5_BUS_INSTANCE,
    SNS_SMGR_SENSOR_5_VDD,
    SNS_SMGR_SENSOR_5_VDDIO
   },

   {
    SNS_SMGR_SENSOR_6_DD_FN_LIST_PTR,
    {0},
    (uint16_t)((SNS_SMGR_SENSOR_6_OFF_TO_IDLE/SNS_SMGR_USEC_PER_TICK)+1),
    (uint16_t)((SNS_SMGR_SENSOR_6_IDLE_TO_READY/SNS_SMGR_USEC_PER_TICK)+1),
    SNS_SMGR_SENSOR_6_ENUM_CODE,
    SNS_SMGR_SENSOR_6_BUS_ADDRESS,
    SNS_SMGR_SENSOR_6_RANGE_TYPE,
    SNS_SMGR_SENSOR_6_SENSITIVITY_DEFAULT,
    SNS_SMGR_SENSOR_6_REG_ITEM_TYPE,
    SNS_SMGR_SENSOR_6_REG_ITEM_ID,
    SNS_SMGR_SENSOR_6_CAL_PRI_TYPE,
    SNS_SMGR_SENSOR_6_CAL_PRI_ID,
    SNS_SMGR_SENSOR_6_DEVICE_ID,
    SNS_SMGR_SENSOR_6_FLAGS,
    {SNS_SMGR_SENSOR_6_DATA_TYPE_1, SNS_SMGR_SENSOR_6_DATA_TYPE_2},
    SNS_SMGR_SENSOR_6_GPIO_FIRST,
    SNS_SMGR_SENSOR_6_GPIO_SECOND,
    SNS_SMGR_SENSOR_6_BUS_INSTANCE,
    SNS_SMGR_SENSOR_6_VDD,
    SNS_SMGR_SENSOR_6_VDDIO
   },

   {
    SNS_SMGR_SENSOR_7_DD_FN_LIST_PTR,
    {0},
    (uint16_t)((SNS_SMGR_SENSOR_7_OFF_TO_IDLE/SNS_SMGR_USEC_PER_TICK)+1),
    (uint16_t)((SNS_SMGR_SENSOR_7_IDLE_TO_READY/SNS_SMGR_USEC_PER_TICK)+1),
    SNS_SMGR_SENSOR_7_ENUM_CODE,
    SNS_SMGR_SENSOR_7_BUS_ADDRESS,
    SNS_SMGR_SENSOR_7_RANGE_TYPE,
    SNS_SMGR_SENSOR_7_SENSITIVITY_DEFAULT,
    SNS_SMGR_SENSOR_7_REG_ITEM_TYPE,
    SNS_SMGR_SENSOR_7_REG_ITEM_ID,
    SNS_SMGR_SENSOR_7_CAL_PRI_TYPE,
    SNS_SMGR_SENSOR_7_CAL_PRI_ID,
    SNS_SMGR_SENSOR_7_DEVICE_ID,
    SNS_SMGR_SENSOR_7_FLAGS,
    {SNS_SMGR_SENSOR_7_DATA_TYPE_1, SNS_SMGR_SENSOR_7_DATA_TYPE_2},
    SNS_SMGR_SENSOR_7_GPIO_FIRST,
    SNS_SMGR_SENSOR_7_GPIO_SECOND,
    SNS_SMGR_SENSOR_7_BUS_INSTANCE,
    SNS_SMGR_SENSOR_7_VDD,
    SNS_SMGR_SENSOR_7_VDDIO
   }
  };
#else
  SNS_SMGR_UIMAGE_DATA smgr_sensor_cfg_s smgr_sensor_cfg[SNS_SMGR_NUM_SENSORS_DEFINED];
#endif /* ADSP_STANDALONE */

