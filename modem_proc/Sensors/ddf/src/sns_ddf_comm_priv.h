
/**
 * @file sns_ddf_comm_priv.h
 * @brief Common private definitions used across the Sensors
 *  	  Device Driver Framework.
 *  
 * Copyright (c) 2014-2015 by Qualcomm Technologies, Inc.  All Rights Reserved
 * Qualcomm Technologies Proprietary and Confidential
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

    when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2015-04-28 jms  Default SPI max clock set to 3.2MHz to comply with slowest slave specs
  2015-04-20 hw   Fix compiler warnings
  2015-02-10 ps   Change SPI max clock to 4 MHz
  2015-01-21 jms  SPI and BMI160 enabled on SLPI 1.0
  2014-07-24 jms  I2C and SPI refactored for uImage
  2014-04-30 asj  Common definitions to be shared internally in the ddf_comm
==============================================================================*/

#ifndef SNS_DDF_COMM_PRIV_H
#define SNS_DDF_COMM_PRIV_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "sns_ddf_comm.h"
#include "I2cDriver.h"
#include "SpiDriver.h"

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

//-------------------------- PRIVATE I2C UIMAGE ------------------------------
sns_ddf_status_e sns_ddf_comm_bus_i2c_read
(
    const sns_ddf_handle_t  handle,
    uint16_t                reg_addr,
    uint8_t*                buffer,
    uint32_t                bytes,
    uint32_t*               read_count
);
sns_ddf_status_e sns_ddf_comm_bus_i2c_write
(
    const sns_ddf_handle_t  handle,
    uint16_t                reg_addr,
    const uint8_t*          buffer,
    uint32_t                bytes,
    uint32_t*               write_count
);

//-------------------------- PRIVATE SPI UIMAGE ------------------------------
sns_ddf_status_e sns_ddf_comm_bus_spi_read
(
    const sns_ddf_handle_t  handle,
    uint16_t                reg_addr,
    uint8_t*                buffer,
    uint32_t                bytes,
    uint32_t*               read_count
);
sns_ddf_status_e sns_ddf_comm_bus_spi_write
(
    const sns_ddf_handle_t  handle,
    uint16_t                reg_addr,
    const uint8_t*          buffer,
    uint32_t                bytes,
    uint32_t*               write_count
);

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/


//-------------------------- PRIVATE I2C ------------------------------
#define SNS_DDF_DEFAULT_I2C_BUS_FREQ   400 //  I2C bus frequency in KHz
#define SNS_DDF_DEFAULT_BYTE_XFER_TMO  2500 // byte xfer timeout in uS

//-------------------------- PRIVATE SPI ------------------------------
#define SNS_DDF_DEFAULT_SPI_BITS_PER_WORD     8
#define SNS_DDF_DEFAULT_SPI_MIN_CLK_FRQ_HZ    (0*1000*1000)
#define SNS_DDF_DEFAULT_SPI_MAX_CLK_FRQ_HZ    (96*100*1000)
#define SNS_DDF_DEFAULT_SPI_DEASSERT_TIME     0
#define SNS_DDF_DEFAULT_SPI_LOOPBACK_MODE     0

//---------------------------------------------------------------------
//#define DDF_COMM_DEBUG

#ifdef DDF_COMM_DEBUG
#if defined SNS_USES_ISLAND
#define DDF_COMM_MSG_0(level,msg)          UMSG(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg)
#define DDF_COMM_MSG_1(level,msg,p1)       UMSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg,p1)
#define DDF_COMM_MSG_2(level,msg,p1,p2)    UMSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg,p1,p2)
#define DDF_COMM_MSG_3(level,msg,p1,p2,p3) UMSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg,p1,p2,p3)
#define DDF_COMM_MSG_4(level,msg,p1,p2,p3,P4) UMSG_4(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg,p1,p2,p3,P4)
#define DDF_COMM_MSG_5(level,msg,p1,p2,p3,P4,P5) UMSG_5(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg,p1,p2,p3,P4,P5)
#elif defined QDSP6
#define DDF_COMM_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg)
#define DDF_COMM_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg,p1)
#define DDF_COMM_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg,p1,p2)
#define DDF_COMM_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg,p1,p2,p3)
#define DDF_COMM_MSG_4(level,msg,p1,p2,p3,P4) MSG_4(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg,p1,p2,p3,P4)
#define DDF_COMM_MSG_5(level,msg,p1,p2,p3,P4,P5) MSG_5(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg,p1,p2,p3,P4,P5)
#endif //QDSP6
#else //DDF_COMM_DEBUG
#define DDF_COMM_MSG_0(level,msg)
#define DDF_COMM_MSG_1(level,msg,p1)
#define DDF_COMM_MSG_2(level,msg,p1,p2)
#define DDF_COMM_MSG_3(level,msg,p1,p2,p3)
#define DDF_COMM_MSG_4(level,msg,p1,p2,p3,P4)
#define DDF_COMM_MSG_5(level,msg,p1,p2,p3,P4,P5)
#endif //DDF_COMM_DEBUG


/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/

//------------------------------- I2C BUS ------------------------------------
typedef struct
{
  sns_ddf_i2c_reg_addr_e reg_addr_type;
  I2cDrv_I2cBus          i2c_bus;
} sns_ddf_sensor_i2c_info_s;

//------------------------------- SPI BUS ------------------------------------
// SPI configuration

typedef struct
{
  spi_device_id_t        dev_id;
  sns_ddf_spi_config_s*  cfg;
} sns_ddf_sensor_spi_info_s;

//------------------------------ BUS HANDLER ---------------------------------
typedef struct
{
  sns_ddf_bus_e bus;
  union
  {
    sns_ddf_sensor_i2c_info_s  i2c_s;
    sns_ddf_sensor_spi_info_s  spi_s;
  };
} sns_ddf_sensor_info_s;

/*----------------------------------------------------------------------------
 * Extern Variables
 * -------------------------------------------------------------------------*/
extern volatile bool EnableI2C;
extern volatile bool EnableSPI;

#endif
