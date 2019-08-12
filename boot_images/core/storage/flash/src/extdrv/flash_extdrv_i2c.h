#ifndef FLASH_EXTDRV_I2C_H
#define FLASH_EXTDRV_I2C_H

/*====================================================================
 *
 * FILE:        flash_extdrv_i2c.h
 *
 * DESCRIPTION: I2C driver interface API's for flash drivers to drive
 * I2C core driver.
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2010-2011, 2014 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/extdrv/flash_extdrv_i2c.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 06/09/14     whc     High time divider factor for accurate clk
 * 03/18/14     whc     Add different i2c clk support
 * 03/21/11     jz      Moved common I2C related defines here
 * 10/28/10     bn      Changed Init function to return pass/fail result
 * 10/11/10     bn      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "comdef.h"

#define FLASH_EXTDRV_I2C_DONE   0    /* Operation passed */
#define FLASH_EXTDRV_I2C_FAIL   (-1) /* Operation failed */

/* The QUP uses the following tags as "command IDs" */
enum i2c_qup_tag
{
  I2C_TAG_NOP = 0,              /* No Operation */
  I2C_TAG_START,                /* Start a Read or Write condition */
  I2C_TAG_MO_DATA,              /* Write a byte out */
  I2C_TAG_MO_STOP,              /* Write the last byte. Create STOP condition*/
  I2C_TAG_MI_REC,               /* Read one or more bytes */                  
  I2C_TAG_UNKNOWN = 0x7FFFFFFF
};

/* QUP core state */
enum i2c_core_state
{
  I2C_RESET_STATE = 0,
  I2C_RUN_STATE,
  I2C_CLEAR_STATE,
  I2C_PAUSE_STATE,
  I2C_UNKNOWN_STATE = 0x7FFFFFFF
};

/* An QUP/I2C operation requires a WORD. The MSB of the WORD contains 
    the TAG (i.e command ID); the LSB contains the command data */  
#define I2C_NUM_NOPS(c)        ((I2C_TAG_NOP     << 8) | c)
#define I2C_START_WRITE(c)     ((I2C_TAG_START   << 8) | (c << 1) | 0)
#define I2C_START_READ(c)      ((I2C_TAG_START   << 8) | (c << 1) | 1)
#define I2C_BYTE_WRITE(c)      ((I2C_TAG_MO_DATA << 8) | c)
#define I2C_LAST_BYTE_WRITE(c) ((I2C_TAG_MO_STOP << 8) | c)
#define I2C_NUM_READ(c)        ((I2C_TAG_MI_REC  << 8) | c)

/* QUP/GSBI bit and field definitions */
#define I2C_N_BITS_16               15
#define I2C_PROTOCOL_I2C_MASTER     (2 << 4) 
#define I2C_MINI_CORE_I2C_MASTER    (2 << 8)
#define I2C_INPUT_MODE_FIFO         (0 << 12)
#define I2C_INPUT_MODE_BLOCK        (1 << 12)
#define I2C_INPUT_MODE_DATA_MOVER   (2 << 12)
#define I2C_OUTPUT_MODE_FIFO        (0 << 10)
#define I2C_OUTPUT_MODE_BLOCK       (1 << 10)
#define I2C_OUTPUT_MODE_DATA_MOVER  (2 << 10)

/* I2C some pre-defined frequencies. With 1.8V Vcc, upto 400KHz is supporteed */
#define I2C_CLK_1KHZ                1
#define I2C_CLK_100KHZ              100
#define I2C_CLK_400KHZ              400
#define I2C_CLK_1MHZ                1000
#define QUP_INPUT_CLK_TCXO          19200
#define QUP_INPUT_CLK               48000

/* I2C frequency limit */
#define I2C_STD_FREQ_KHZ_MAX        100
#define I2C_FAST_FREQ_KHZ_MAX       400
#define I2C_FASTPLUS_FREQ_KHZ_MAX   1000

/* I2C High Time Divider factor from I2C spec. HTD = x/100 * FS_DIVIDER */
#define I2C_HTD_FACTOR      70

#define I2C_STD_MODE_MIN_TLOW_MSEC 0.0047
#define I2C_FAST_MODE_MIN_TLOW_MSEC 0.0013
#define I2C_FASTPLUS_MODE_MIN_TLOW_MSEC 0.0005

/* EEPROM IC needs a 5ms delay between writes. No delay required btwn reads */
#define I2C_DELAY_WRITE_MIN    5

/* I2C read/write buffer information */
struct nor_i2c_xfer_buff_info
{
  uint8 *data;
  uint32 length;
  uint32 start_addr;
};

/* I2C wrapper function pointers */
struct nor_i2c_wrapper
{
  /* Intialize the Flash I2C core driver */
  int (*init)(uint32 slave_id, uint32 clk);

  /* Flash I2C read operation */
  int (*read_op)(struct nor_i2c_xfer_buff_info *i2c_read_buf_info);

  /* Flash I2C write operation */
  int (*write_op)(struct nor_i2c_xfer_buff_info *i2c_write_buf_info);
};

/* Init/Configure the I2C wrapper APIs */
void flash_i2c_configure(void *i2c_configs);

#endif /* FLASH_EXTDRV_I2C_H */
