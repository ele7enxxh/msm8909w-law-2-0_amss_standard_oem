#ifndef FLASH_EXTDRV_I2C_V2_H
#define FLASH_EXTDRV_I2C_V2_H

/*=======================================================================
 * FILE:        flash_extdrv_i2c_v2.h
 *
 * SERVICES:
 *
 * DESCRIPTION: Header file for I2C configurations
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/extdrv/config_v2/flash_extdrv_i2c_v2.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 03/30/12     sv      Add KICK_WATCHDOG macro
 * 02/15/12     sv      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include "comdef.h"

/* Structure to store adresses of QUP registers */
struct qup_reg_addrs
{
  volatile uint32* QUP_OUTPUT_FIFO;
  volatile uint32* QUP_INPUT_FIFO;
  volatile uint32* QUP_CONFIG;   
  volatile uint32* QUP_IO_MODES;
  volatile uint32* QUP_SW_RESET;
  volatile uint32* QUP_OPERATIONAL;
  volatile uint32* QUP_STATE;
  volatile uint32* QUP_ERROR_FLAGS;
  volatile uint32* QUP_ERROR_FLAGS_EN;
  volatile uint32* QUP_MX_OUTPUT_CNT;
  volatile uint32* QUP_MX_INPUT_CNT;
  volatile uint32* QUP_MX_WRITE_CNT;
  volatile uint32* QUP_MX_READ_CNT;
};

/* Structure to store adresses of GSBI/CLK registers */
struct gsbi_reg_addrs
{
  volatile uint32* GSBI_CTRL;
  volatile uint32* GSBI_HCLK_CTL;
  volatile uint32* GSBI_QUP_APPS_MD;   
  volatile uint32* GSBI_QUP_APPS_NS;
  volatile uint32* GSBI_MASTER_CLK_CTL;
};

/* Structure to store adresses of GPIO registers */
struct gpio_reg_addrs
{
  volatile uint32* GPIO_CFG_SDA;
  volatile uint32* GPIO_CFG_CLK;
};

/* Structure to store all the register addresses needed by I2C */
struct i2c_reg_addrs
{
  struct qup_reg_addrs qup;
  struct gsbi_reg_addrs gsbi;
  struct gpio_reg_addrs gpio;
};  

/* Structure to store the GPIO register config values needed by I2C */
struct i2c_gpio_cfg_data
{
  uint32 sda_gpio_cfg_val;
  uint32 clk_gpio_cfg_val;
};

/* Structure to store all register config values needed by I2C */
struct i2c_config_data
{
  struct i2c_gpio_cfg_data gpio;
};

/* Reset the Watchdog counter */
#if !defined(BUILD_BOOT_CHAIN)
  #define  KICK_WATCHDOG() \
    *((uint32 *)WDOG_RESET_ADDR) = WDOG_RESET_VAL;
#else
  #define KICK_WATCHDOG()
#endif	
  
#endif  /* FLASH_EXTDRV_I2C_V2_H */
