/*====================================================================
 *
 * FILE: flash_nor_i2c_config.c
 *
 * SERVICES: Functions and data for I2C EEPROM Configurations
 *
 * DESCRIPTION: This file contain initialization and interface APIs
 *              of flash config data, which is used across all NOR
 *              devices
 *
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2010-2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/dal/flash_nor_i2c_config.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ---------   ---     ----------------------------------------------
 * 04/08/15     sb     Update base address
 * 03/18/14     whc    Add different i2c clk support
 * 03/18/14     whc    Add OEM custom EEPROM support
 * 12/17/13     whc    Add RF EEPROM I2C support
 * 09/03/12     sb     Clean up
 * 03/19/12     sv     Adapt to SPI NOR power management changes
 * 05/20/11     bb/sv  Flash driver Optimization
 * 11/24/10     bn     Corrected total pages in the I2C EEPROM partition
 * 08/26/10     bn     Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_nor_config.h"
#include "flash_nor_i2c.h"

#define NUM_ARRAY_ELEMENTS(array, type)  (sizeof(array)/sizeof(type))

/*-----------------------------------------------------------------------
 *           Definitions and Constants
 *----------------------------------------------------------------------*/

/* Supported Regular NAND devices */
static struct flash_nor_params i2c_eeprom_devices[] =
{
  {
    FLASH_NOR_UNKNOWN,          /* NOR family type */
    FLASH_NOR_I2C,              /* Device Technology Type */
    FLASH_TECH_UNKNOWN,         /* Device Bits Per Cell Type */
    FLASH_WIDTH_UNKNOWN,        /* Device Interface Width */
    FLASH_WRITES_SIMPLE,        /* Write style specific to Flash */
    "AT24C128C",                /* Device name string */
    0,                          /* Num ids */
    {0, 0, 0, 0},               /* Device ID */
    1,                          /* Number of total blocks in device */
    512,                        /* Number of pages in a block */
    32,                         /* Logical page size */
    0,                          /* Total density in Mega Bytes */
    32,                         /* Write Buffer size in bytes */
    0,                          /* Device specific flags */
    400,                        /* Device operating frequency in KHz*/
    0x53,                       /* Device base address */
    0,                          /* Total banks */
    0,                          /* Total erase regions */
	0,                          /* Delay for Deep Power Down */
	0,                          /* Delay to release from Deep Power Down */
    {{0, 0} },                  /* Bank matrix */
    {{0, {0, 0}} },             /* Erase region matrix */
    {0, {{0, 0}, {0, 0}, {0, 0} }},
    
  },
  { //RF EEPROM
    FLASH_NOR_UNKNOWN,		  /* NOR family type */
    FLASH_NOR_I2C,			  /* Device Technology Type */
    FLASH_TECH_UNKNOWN,		  /* Device Bits Per Cell Type */
    FLASH_WIDTH_UNKNOWN,		  /* Device Interface Width */
    FLASH_WRITES_SIMPLE,		  /* Write style specific to Flash */
    "AT24C128BN", 			  /* Device name string */
    0,						  /* Num ids */
    {0, 0, 0, 0}, 			  /* Device ID */
    1,						  /* Number of total blocks in device */
    512,						  /* Number of pages in a block */
    32,						  /* Logical page size */
    0,						  /* Total density in Mega Bytes */
    32,						  /* Write Buffer size in bytes */
    0,						  /* Device specific flags */
    400,					  /* Device operating frequency in KHz*/
    0x50, 					  /* Device base address */
    0,						  /* Total banks */
    0,						  /* Total erase regions */
    0,						  /* Delay for Deep Power Down */
    0,						  /* Delay to release from Deep Power Down */
    {{0, 0} },				  /* Bank matrix */
    {{0, {0, 0}} },			  /* Erase region matrix */
    {0, {{0, 0}, {0, 0}, {0, 0} }},
  },
  { //OEM custom EEPROM
    FLASH_NOR_UNKNOWN,		  /* NOR family type */
    FLASH_NOR_I2C,			  /* Device Technology Type */
    FLASH_TECH_UNKNOWN,		  /* Device Bits Per Cell Type */
    FLASH_WIDTH_UNKNOWN,		  /* Device Interface Width */
    FLASH_WRITES_SIMPLE,		  /* Write style specific to Flash */
    "AT24C128BN", 			  /* Device name string */
    0,						  /* Num ids */
    {0, 0, 0, 0}, 			  /* Device ID */
    1,						  /* Number of total blocks in device */
    512,						  /* Number of pages in a block */
    32,						  /* Logical page size */
    0,						  /* Total density in Mega Bytes */
    32,						  /* Write Buffer size in bytes */
    0,						  /* Device specific flags */
    100,					  /* Device operating frequency in KHz*/
    0x50, 					  /* Device base address */
    0,						  /* Total banks */
    0,						  /* Total erase regions */
    0,						  /* Delay for Deep Power Down */
    0,						  /* Delay to release from Deep Power Down */
    {{0, 0} },				  /* Bank matrix */
    {{0, {0, 0}} },			  /* Erase region matrix */
    {0, {{0, 0}, {0, 0}, {0, 0} }},
  }
};

/* Supported Devices */
static struct flash_nor_cfg_data supported_i2c_eeprom_cfgs[] =
{
  { //CDT EEPROM
    FLASH_NOR_I2C,
    0,
    NUM_ARRAY_ELEMENTS(i2c_eeprom_devices[0], flash_nor_params_type),
    &i2c_eeprom_devices[0]
  },
  { //RF EEPROM
    FLASH_NOR_I2C,   
    0,
    NUM_ARRAY_ELEMENTS(i2c_eeprom_devices[1], flash_nor_params_type),
    &i2c_eeprom_devices[1]
  },
  { //OEM custom EEPROM
    FLASH_NOR_I2C,   
    0,
    NUM_ARRAY_ELEMENTS(i2c_eeprom_devices[2], flash_nor_params_type),
    &i2c_eeprom_devices[2]
  },
  {
    FLASH_UNKNOWN,    /* TO MARK THE END - Mandatory - Used in driver */
    0,
    0,
    NULL
  }
};

/*
 * Return all supported I2C configurations
 */
void flash_nor_i2c_get_configs(struct flash_nor_cfg_data **cfg_data)
{
  *cfg_data = (struct flash_nor_cfg_data *) &supported_i2c_eeprom_cfgs;
}

/* NOR Probe table. Driver will be probing for the
   device in the order listed in the below table. */
static flash_probe_tbl nor_probe_table[] =
{
  nor_i2c_probe,
  NULL
};


void flash_nor_i2c_get_probe_table(flash_probe_tbl **probe_tbl)
{
  *probe_tbl = &nor_probe_table[0];
}
