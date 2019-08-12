/*====================================================================
 *
 * FILE:        flash_extdrv_i2c_v2.c
 *
 * SERVICES:    I2C support for all flash drivers
 *
 * DESCRIPTION: Flash I2C core driver
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2010-2014 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/extdrv/config_v2/flash_extdrv_i2c_v2.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ---------   ---    ----------------------------------------------
 * 06/09/14     whc     Fixed for accurate clk support
 * 03/18/14     whc     Add different i2c clk support
 * 11/25/13     eo      Replace busy loop cntr with busywait()
 * 12/17/13     whc     Add RF EEPROM I2C support
 * 09/25/12     eo      Fix missed I2C Clock Setting
 * 09/18/12     eo      Configure I2C Clock based on platform
 * 08/03/12     eo      Change input QUP clock to TCXO
 * 03/26/12     sv      8064 MPQ - 1.0, 1.1 changes
 * 02/17/12     sv      Update to detect and support 8064 - MPQ and APQ
 * 05/17/11     jz      Enable clock branch of GSBI core for I2C
 * 03/21/11     jz      Updated to support 8960 (RUMI)
 * 11/14/10     bn      Added 400KHz I2C clock support.
 * 10/11/10     bn      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_extdrv_i2c_v2.h"
#include FLASH_EXTDRV_I2C_INCLUDE_H

#include "busywait.h"

/* For each write, there're a 4 byte command header.
 * Therefore, max multi-byte write transfer size is 28 bytes for each write. 
 * Max multi-byte read size is 32 bytes for each read. Default to 1 byte 
 * read/write */
#ifdef FLASH_I2C_ENABLE_MULTIBYTE_WRITE
  #define I2C_MAX_WRITE_SIZE     16
#else
  #define I2C_MAX_WRITE_SIZE     1
#endif

#ifdef FLASH_I2C_ENABLE_MULTIBYTE_READ
  #define I2C_MAX_READ_SIZE      32
#else
  #define I2C_MAX_READ_SIZE      1
#endif

/* Initialize the register address variables with default values. For 
 * certain targets in which different flavours might have different 
 * I2C configs for EEPROM, this will be updated appropriately in 
 * i2c_reconfigure(). For all others these addresses will be used as 
 * it is for register read and writes. */
struct i2c_reg_addrs i2c_reg_addr = {
  { 
   (volatile uint32 *)I2C_GSBI_QUP_OUTPUT_FIFO_ADDR,
   (volatile uint32 *)I2C_GSBI_QUP_INPUT_FIFO_ADDR,
   (volatile uint32 *)I2C_GSBI_QUP_CONFIG_ADDR,
   (volatile uint32 *)I2C_GSBI_QUP_IO_MODES_ADDR,
   (volatile uint32 *)I2C_GSBI_QUP_SW_RESET_ADDR,
   (volatile uint32 *)I2C_GSBI_QUP_OPERATIONAL_ADDR,
   (volatile uint32 *)I2C_GSBI_QUP_STATE_ADDR,
   (volatile uint32 *)I2C_GSBI_QUP_ERROR_FLAGS_ADDR,
   (volatile uint32 *)I2C_GSBI_QUP_ERROR_FLAGS_EN_ADDR,
   (volatile uint32 *)I2C_GSBI_QUP_MX_OUTPUT_COUNT_ADDR,  
   (volatile uint32 *)I2C_GSBI_QUP_MX_INPUT_COUNT_ADDR,
   (volatile uint32 *)I2C_GSBI_QUP_MX_WRITE_COUNT_ADDR,  
   (volatile uint32 *)I2C_GSBI_QUP_MX_READ_COUNT_ADDR,   
  },
  {
   (volatile uint32 *)I2C_GSBI_CTRL_REG_ADDR,
   (volatile uint32 *)I2C_GSBI_HCLK_CTL_ADDR,
   (volatile uint32 *)I2C_GSBI_QUP_APPS_MD_ADDR,
   (volatile uint32 *)I2C_GSBI_QUP_APPS_NS_ADDR,
   (volatile uint32 *)I2C_GSBI_I2C_MASTER_CLK_CTL_ADDR,
  },
  {
   (volatile uint32 *)I2C_GPIO_CFG_SDA_ADDR,
   (volatile uint32 *)I2C_GPIO_CFG_CLK_ADDR,
  }
}; 

/* Initialize the config values with default values. For certain targets
 * in which different flavours might have different different I2C configs
 * for EEPROM, this will be updated appropriately in i2c_reconfigure(). 
 * For all others these config values will be used as it is. */
struct i2c_config_data  i2c_cfg_data = {
  {
    I2C_SDA_GPIO_CFG_VAL,
	I2C_CLK_GPIO_CFG_VAL
  }
};

/* The default value (46%) applies to 400KHz. Value here (x) is used to compute 
 * HIGH_TIME_DIVIDER as HIGH_TIME_DIVIDER = x/100 * FS_DIVIDER. HIGH_TIME_DIVIDER 
 * is then used as counter for high period and FS_DIVIDER is used as counter for 
 * low period. */
static uint32 i2c_high_time_divider_factor = I2C_HTD_FACTOR;

/* Default I2C EEPROM Slave Address */
static uint8 eeprom_slave_id = FLASH_I2C_DEVICE_ADDR;
static uint16 i2c_op_freq = I2C_CLOCK_KHZ;

/*
 * Function: Approximately 1 ms delay
 */
static void delay1ms (unsigned long delay_ms)
{
  unsigned long delay_us = delay_ms * 1000;
  busywait(delay_us);
}

/*
 * Function: Check if the QUP core is in a valid state
 */
static int i2c_state_error_check ()
{
  uint32 loop_count = 1000;

  while (loop_count)
  {
    if (*(i2c_reg_addr.qup.QUP_STATE) & I2C_STATE_VALID)
    {
      return FLASH_EXTDRV_I2C_DONE;
    }
    loop_count--;
  }

  return FLASH_EXTDRV_I2C_FAIL;
}

/*
 * Function: Set the QUP core state
 */
static int i2c_core_set_state (enum i2c_core_state state)
{
  /* Only set the QUP core if it's is in a valid state */
  if (FLASH_EXTDRV_I2C_DONE != i2c_state_error_check())
  {
    return FLASH_EXTDRV_I2C_FAIL;
  }

  *(i2c_reg_addr.qup.QUP_STATE) = state;

  /* Confirm that the QUP core is still in a valid state */
  return i2c_state_error_check();
}

/*
 * Function: Initialize the GPIOs used for I2C SLK and SDA pins
 */
void i2c_io_init()
{
  /* System Drivers Clock team are handling cc_qup_core_clk and cc_i2c_apps_clk
   * enablement in clock module.*/
  
  /* set the GSBI I2C GPIOs to Pull Up, 8mA drive mode */
  *(i2c_reg_addr.gpio.GPIO_CFG_CLK) = i2c_cfg_data.gpio.clk_gpio_cfg_val;
  *(i2c_reg_addr.gpio.GPIO_CFG_SDA) = i2c_cfg_data.gpio.sda_gpio_cfg_val;
}

/*
 * Function: Initialize the I2C clocks
 */
static void i2c_clk_init(uint32 nQupClock, uint32 nI2CFsClock)
{
  uint32 fs_divider = 0;
  uint32 reg_value = 0;
  uint32 high_time_divider_value = 0;
  
  /* System Drivers Clock team are handling cc_qup_core_clk and cc_i2c_apps_clk
   * enablement in clock module.*/

  /* The I2C SCL is determined by nI2CFsClock (set to 100/400 for 
   * 100KHz/400KHz respectively). The formula comes from the software 
   * interface document */ 

  /* The equations to calculate Tlow (controlled by FS_DIV) and Thigh (controlled by HTD) are
   * Tlow=TInputclk*(I2C_MASTER_CLK_CTL[FS_DIVIDER_VALUE]+3) 
   * Thigh = TInputclk*(I2C_MASTER_CLK_CTL[HIGH_TIME_DIVIDER_VALUE]+3) 
   * Also FS_DIV + HTD + 6 should add upto TInputClk/I2CBusClkReqKHz. on Fast mode this is 19200/400 i.e 48.
   */
   
  if (I2C_STD_FREQ_KHZ_MAX >= nI2CFsClock)
  {
    fs_divider = I2C_STD_MODE_MIN_TLOW_MSEC * nQupClock - 2;
	reg_value = fs_divider;
  	high_time_divider_value = nQupClock / nI2CFsClock - fs_divider - 6;
  }
  else if (I2C_FAST_FREQ_KHZ_MAX >= nI2CFsClock)
  {
    fs_divider = I2C_FAST_MODE_MIN_TLOW_MSEC * nQupClock -2;
	reg_value = fs_divider;
  	high_time_divider_value = nQupClock / nI2CFsClock - fs_divider - 6;
  }
  else if (I2C_FASTPLUS_FREQ_KHZ_MAX >= nI2CFsClock)
  {
	fs_divider = I2C_FASTPLUS_MODE_MIN_TLOW_MSEC * nQupClock - 2;
	reg_value = fs_divider;
  	high_time_divider_value = nQupClock / nI2CFsClock - fs_divider - 6;;
  }
  else
  {
    fs_divider = 0x7;
    reg_value = fs_divider;
    high_time_divider_value = 0x7;
  }
  
  high_time_divider_value = i2c_high_time_divider_factor * high_time_divider_value / 100;

  /* Program the high_time_divider and fs_divider values */
  reg_value |= ((high_time_divider_value << I2C_HIGH_TIME_DIVIDER_VALUE_SHFT) & 
    I2C_HIGH_TIME_DIVIDER_VALUE_BMSK);
  *(i2c_reg_addr.gsbi.GSBI_MASTER_CLK_CTL) = reg_value;

}

/*
 * Function: Initialize GSBI8 QUP core to operate in I2C Master mode
 */
int i2c_qup_init()
{
  /* Any write resets the entire QUP core and all the mini cores */
  *(i2c_reg_addr.qup.QUP_SW_RESET) = 0x00;

  /* Wait for QUP state valid bit set. Need to ensure the QUP is in 
     RESET_STATE and VALID bit set before further QUP configurations */ 
  if (FLASH_EXTDRV_I2C_DONE != i2c_core_set_state(I2C_RESET_STATE))
  {
    return FLASH_EXTDRV_I2C_FAIL;  
  }
  
  /* Set the QUP mini core to I2C Master, NBit size 16 */
  *(i2c_reg_addr.qup.QUP_CONFIG) = (I2C_MINI_CORE_I2C_MASTER | I2C_N_BITS_16);

  /* Enable Pack and Unpack */
  *(i2c_reg_addr.qup.QUP_IO_MODES) = 
    (I2C_IO_MODES_UNPACK_EN | I2C_IO_MODES_PACK_EN);

  /* Initialize the QUP I2C master SCL to either 100KHz or 400KHz. */
  i2c_clk_init(QUP_INPUT_CLK_TCXO, i2c_op_freq);

  return FLASH_EXTDRV_I2C_DONE;
}

/*
 * Function: Configure the GSBI QUP Mode, FIFOs for each transfer
 */
static int i2c_qup_config(uint32 input_mode, uint32 output_mode,
  uint32 input_count, uint32 output_count)
{
  /* Set Input Mode */
  *(i2c_reg_addr.qup.QUP_IO_MODES) &= ~I2C_INPUT_MODE_BMSK;
  *(i2c_reg_addr.qup.QUP_IO_MODES) |= input_mode;

  /* Set Output Mode */
  *(i2c_reg_addr.qup.QUP_IO_MODES) &= ~I2C_OUTPUT_MODE_BMSK;
  *(i2c_reg_addr.qup.QUP_IO_MODES) |= output_mode;

  /* Input Count for Block and Data Mover Modes */
  *(i2c_reg_addr.qup.QUP_MX_INPUT_CNT) = input_count;

  /* Input Count for FIFO mode */
  *(i2c_reg_addr.qup.QUP_MX_READ_CNT) = input_count;

  /* Output Count for Block and Data Mover Modes */
  *(i2c_reg_addr.qup.QUP_MX_OUTPUT_CNT) = output_count;

  /* Output Count for FIFO mode */
  *(i2c_reg_addr.qup.QUP_MX_WRITE_CNT) = output_count;

  return i2c_core_set_state(I2C_RESET_STATE);
}

/*
 * Function: Check the status of the I2C write
 */
static int i2c_write_complete_check()
{
  uint32 loop_count = I2C_RW_CHECK_LOOP_COUNT;
  
  /* Wait for the OUTPUT_DONE flag. This flag is set once the QUP has done
     shifting the number of bytes specified in I2C_QUP_MX_OUTPUT_CNT */
  while (loop_count)
  {
    if(*(i2c_reg_addr.qup.QUP_OPERATIONAL) & I2C_OUTPUT_DONE_FLAG)
    {
      return FLASH_EXTDRV_I2C_DONE;
    }
    loop_count--;
  }

  return FLASH_EXTDRV_I2C_FAIL;
}

/*
 * Function: Check the status of the I2C read
 */
static int i2c_read_complete_check()
{
  uint32 loop_count = I2C_RW_CHECK_LOOP_COUNT;

  /* Wait for the INPUT_DONE flag. This flag is set once the QUP has received
     the number of bytes specified in I2C_QUP_MX_INPUT_CNT */
  while (loop_count)
  {
    if (*(i2c_reg_addr.qup.QUP_OPERATIONAL) & I2C_INPUT_DONE_FLAG)
    {
      return FLASH_EXTDRV_I2C_DONE;
    }
    loop_count--;
  }

  return FLASH_EXTDRV_I2C_FAIL;
}

/*
 * Function: Initialize Flash I2C core driver
 */
int flash_i2c_init(uint32 slave_id, uint32 clk)
{
/*if device's slave ID is passed, use it instead*/  
  if (slave_id != 0)
  {
  	eeprom_slave_id = (uint8)slave_id;
  }
  
 /*if device's operating frequency is passed, use it instead*/  
  if (clk != 0)
  {
  	i2c_op_freq = (uint16)clk;
  }
   
  
/* Reconfigure the I2C register addresses and configuration values
 * based on the target and revision. These is a stub on some targets
 * which just has a single configuration
 */  
  i2c_reconfigure(&i2c_reg_addr, &i2c_cfg_data);
    
  /* Configure the SCL and SDATA I2C pins to Pull-up mode */
  i2c_io_init();

  /* GSBI8/QUP Initialization */
  if (FLASH_EXTDRV_I2C_DONE == i2c_qup_init())
  {
    return FLASH_EXTDRV_I2C_DONE;
  }
  else
  {
    return FLASH_EXTDRV_I2C_FAIL;
  }
}

/*
 * Function: Read the I2C EEPROM device
 */
int flash_i2c_read(struct nor_i2c_xfer_buff_info *i2c_read_buf_info)
{
  uint32 input_cnt, read_data, index = 0;
  uint32 addr = i2c_read_buf_info->start_addr;
  uint32 byte_cnt = i2c_read_buf_info->length;
  uint8 *read_buff = i2c_read_buf_info->data;

  while (byte_cnt > 0)
  {
    input_cnt = byte_cnt < I2C_MAX_READ_SIZE ? byte_cnt : I2C_MAX_READ_SIZE;

    byte_cnt -= input_cnt;

    /* send out a command (6 byte cmd header), requesting 
       a read of total input_cnt bytes */
    if (FLASH_EXTDRV_I2C_DONE != i2c_qup_config(I2C_INPUT_MODE_BLOCK, 
      I2C_OUTPUT_MODE_BLOCK, input_cnt, 6))
    {
      return FLASH_EXTDRV_I2C_FAIL;     
    }

    if (FLASH_EXTDRV_I2C_DONE != i2c_core_set_state(I2C_RUN_STATE))
    {
      return FLASH_EXTDRV_I2C_FAIL;
    }

    if (FLASH_EXTDRV_I2C_DONE != i2c_core_set_state(I2C_PAUSE_STATE))
    {
      return FLASH_EXTDRV_I2C_FAIL;
    }

    /* Init the clock each time the QUP state changes from RESET to RUN */
    i2c_clk_init(QUP_INPUT_CLK_TCXO, i2c_op_freq);
	
    /* The FIFO is a 32 bit register. A command is 16 bit, so the FIFO
     * can carry up to 2 commands. A command consists of an 8-bit command
     * TAG in the MSB and 8-bit command data in the LSB. The QUP core uses 
     * the command TAG to translates the command to I2C protocol on I2C bus */

    /* Initiate START Write condition + the MSB of the EEPROM address */
    *(i2c_reg_addr.qup.QUP_OUTPUT_FIFO) = ((I2C_BYTE_WRITE((addr & 0xFF00) >> 8) << 16) | \
      I2C_START_WRITE(eeprom_slave_id));

    /* Complete the LSB of the EEPROM address and start reading  the data */
    *(i2c_reg_addr.qup.QUP_OUTPUT_FIFO) = ((I2C_NUM_NOPS(1) << 16) | \
      I2C_BYTE_WRITE(addr & 0xFF));

    /* specify the total of bytes to read */
    *(i2c_reg_addr.qup.QUP_OUTPUT_FIFO) = ((I2C_NUM_READ(input_cnt) << 16) | \
      I2C_START_READ(eeprom_slave_id));

    if (FLASH_EXTDRV_I2C_DONE != i2c_core_set_state(I2C_RUN_STATE))
    {
      return FLASH_EXTDRV_I2C_FAIL;
    }

    /* Kick the watchdog timer */
    KICK_WATCHDOG();

    if ((FLASH_EXTDRV_I2C_DONE != i2c_write_complete_check()) || \
      (FLASH_EXTDRV_I2C_DONE != i2c_read_complete_check()))
    {
      return FLASH_EXTDRV_I2C_FAIL;
    }

    addr += input_cnt;

    while (input_cnt > 0)
    {
      if (input_cnt >= 2)
      {
        /* Read the 32-bit input FIFO register. The received data (2 bytes
           per FIFO read) are located at bits[0:7] and [24:32] */
        read_data = *(i2c_reg_addr.qup.QUP_INPUT_FIFO);

        *(read_buff + index) = (uint8)(read_data & 0xFF);
        *(read_buff + index + 1) = (uint8)((read_data & 0xFF0000) >> 16);

        index += 2;
        input_cnt -= 2;
      }
      else if (input_cnt == 1)
      {
        read_data = *(i2c_reg_addr.qup.QUP_INPUT_FIFO);

        /* Last byte. Retrieve data from Bits[0:7] */
        *(read_buff + index) = (uint8)(read_data & 0xFF);

        index += 1;
        input_cnt -= 1;
      }
    }// while (input_cnt > 0)
  }// while (byte_cnt > 0)

  return FLASH_EXTDRV_I2C_DONE;
}

/*
 * Function: Write the EEPROM
 */
int flash_i2c_write(struct nor_i2c_xfer_buff_info *i2c_write_buf_info)
{
  uint32 output_cnt, index = 0;
  uint32 addr = i2c_write_buf_info->start_addr;
  uint32 byte_cnt = i2c_write_buf_info->length;
  uint8 *write_buff = i2c_write_buf_info->data;

  while (byte_cnt > 0)
  {
    /* 4 bytes are used for the command header. Therefore, each multi
       byte write transaction can transmit upto 28 bytes */
    output_cnt = byte_cnt < I2C_MAX_WRITE_SIZE ? byte_cnt : I2C_MAX_WRITE_SIZE;

    byte_cnt -= output_cnt;

    /* Configure the QUP to send output_cnt bytes + 4 bytes command header */
    if (FLASH_EXTDRV_I2C_DONE != i2c_qup_config(I2C_INPUT_MODE_BLOCK, 
      I2C_OUTPUT_MODE_BLOCK, 0, output_cnt+4))
    {
      return FLASH_EXTDRV_I2C_FAIL;    
    }

    if (FLASH_EXTDRV_I2C_DONE != i2c_core_set_state(I2C_RUN_STATE))
    {
      return FLASH_EXTDRV_I2C_FAIL;
    }

    if (FLASH_EXTDRV_I2C_DONE != i2c_core_set_state(I2C_PAUSE_STATE))
    {
      return FLASH_EXTDRV_I2C_FAIL;
    }

    /* Init the clock each time the QUP state changes from RESET to RUN */
    i2c_clk_init(QUP_INPUT_CLK_TCXO, i2c_op_freq);
  
    /* The FIFO is a 32-bit wide register. Each command is 16 bit, consists of
     * 8-bit comannd TAG in the MSB and 8-bit cmd data in the LSB. Each FIFO
     * write can carry up to 2 commands. The QUP core uses the command TAG to
     * translates the command to I2C protocol on the I2C bus */

    /* Initiate I2C write and MSB of the 16-bit address of the EEPROM memory */
    *(i2c_reg_addr.qup.QUP_OUTPUT_FIFO) = ((I2C_BYTE_WRITE((addr & 0xFF00) >> 8) << 16) | \
      I2C_START_WRITE(eeprom_slave_id));

    /* complete the LSB of the 16-bit address */
    *(i2c_reg_addr.qup.QUP_OUTPUT_FIFO) = ((I2C_NUM_NOPS(1) << 16) | \
      I2C_BYTE_WRITE(addr & 0xFF));

    while (output_cnt > 0)
    {
      if (output_cnt > 2)
      {
        /* There're more than 2 bytes to write, keep writing the output FIFO */
        *(i2c_reg_addr.qup.QUP_OUTPUT_FIFO) = \
          ((I2C_BYTE_WRITE(*(write_buff + index + 1)) << 16) | \
          (I2C_BYTE_WRITE(*(write_buff + index))));

        addr += 2;
        index += 2;
        output_cnt -= 2;
      }
      else if (output_cnt == 2)
      {
        /* Send the LAST_BYTE command which translate to a STOP condition
           on the I2C bus after the last byte written */
        *(i2c_reg_addr.qup.QUP_OUTPUT_FIFO) = \
          ((I2C_LAST_BYTE_WRITE(*(write_buff + index + 1)) << 16) | \
          (I2C_BYTE_WRITE(*(write_buff + index))));

        addr += 2;
        index += 2;
        break;
      }
      else if (output_cnt == 1)
      {
        *(i2c_reg_addr.qup.QUP_OUTPUT_FIFO) = \
          I2C_LAST_BYTE_WRITE(*(write_buff + index));

        addr += 1;
        index += 1;
        break;
      }
    }//while (output_cnt > 0)

    if (FLASH_EXTDRV_I2C_DONE != i2c_core_set_state(I2C_RUN_STATE))
    {
      return FLASH_EXTDRV_I2C_FAIL;
    }

    /* Kick the watchdog timer */
    KICK_WATCHDOG();

    /* EEPROM IC requires 5ms delay between writes */
    delay1ms(I2C_DELAY_WRITE_MIN);

    if (FLASH_EXTDRV_I2C_DONE != i2c_write_complete_check())
    {
      return FLASH_EXTDRV_I2C_FAIL;
    }
  }// while (byte_cnt > 0)

  return FLASH_EXTDRV_I2C_DONE;
}

/*
 * Function: Write to any I2C device that uses 8 bit data address
 */
int flash_i2c_8bitaddr_write(uint8 i2c_dev_id, 
  struct nor_i2c_xfer_buff_info *i2c_write_buf_info)
{
  uint32 output_cnt, index = 0;
  uint32 addr = i2c_write_buf_info->start_addr;
  uint32 byte_cnt = i2c_write_buf_info->length;
  uint8 *write_buff = i2c_write_buf_info->data;

  while (byte_cnt > 0)
  {
    /* 2 bytes are used for the command header. Therefore, each multi
       byte write transaction can transmit upto 30 bytes */
    output_cnt = byte_cnt < I2C_MAX_WRITE_SIZE ? byte_cnt : I2C_MAX_WRITE_SIZE;

    byte_cnt -= output_cnt;

    /* Configure the QUP to send output_cnt bytes + 2 bytes command header */
    if (FLASH_EXTDRV_I2C_DONE != i2c_qup_config(I2C_INPUT_MODE_BLOCK, 
      I2C_OUTPUT_MODE_BLOCK, 0, output_cnt+2))
    {
      return FLASH_EXTDRV_I2C_FAIL;    
    }

    if (FLASH_EXTDRV_I2C_DONE != i2c_core_set_state(I2C_RUN_STATE))
    {
      return FLASH_EXTDRV_I2C_FAIL;
    }

    if (FLASH_EXTDRV_I2C_DONE != i2c_core_set_state(I2C_PAUSE_STATE))
    {
      return FLASH_EXTDRV_I2C_FAIL;
    }

    /* Init the clock each time the QUP state changes from RESET to RUN */
    i2c_clk_init(QUP_INPUT_CLK_TCXO, i2c_op_freq);
  
    /* The FIFO is a 32-bit wide register. Each command is 16 bit, consists of
     * 8-bit comannd TAG in the MSB and 8-bit cmd data in the LSB. Each FIFO
     * write can carry up to 2 commands. The QUP core uses the command TAG to
     * translates the command to I2C protocol on the I2C bus */

    /* Initiate I2C write and MSB of the 16-bit address of the EEPROM memory */
    *(i2c_reg_addr.qup.QUP_OUTPUT_FIFO) = 
      ((I2C_BYTE_WRITE(addr & 0xFF) << 16) | I2C_START_WRITE(i2c_dev_id));

    while (output_cnt > 0)
    {
      if (output_cnt > 2)
      {
        /* There're more than 2 bytes to write, keep writing the output FIFO */
        *(i2c_reg_addr.qup.QUP_OUTPUT_FIFO) = \
          ((I2C_BYTE_WRITE(*(write_buff + index + 1)) << 16) | \
          (I2C_BYTE_WRITE(*(write_buff + index))));

        addr += 2;
        index += 2;
        output_cnt -= 2;
      }
      else if (output_cnt == 2)
      {
        /* Send the LAST_BYTE command which translate to a STOP condition
           on the I2C bus after the last byte written */
        *(i2c_reg_addr.qup.QUP_OUTPUT_FIFO) = \
          ((I2C_LAST_BYTE_WRITE(*(write_buff + index + 1)) << 16) | \
          (I2C_BYTE_WRITE(*(write_buff + index))));

        addr += 2;
        index += 2;
        break;
      }
      else if (output_cnt == 1)
      {
        *(i2c_reg_addr.qup.QUP_OUTPUT_FIFO) = \
          I2C_LAST_BYTE_WRITE(*(write_buff + index));

        addr += 1;
        index += 1;
        break;
      }
    }//while (output_cnt > 0)

    if (FLASH_EXTDRV_I2C_DONE != i2c_core_set_state(I2C_RUN_STATE))
    {
      return FLASH_EXTDRV_I2C_FAIL;
    }

    /* Kick the watchdog timer */
    KICK_WATCHDOG();

    /* EEPROM IC requires 5ms delay between writes */
    delay1ms(I2C_DELAY_WRITE_MIN);

    if (FLASH_EXTDRV_I2C_DONE != i2c_write_complete_check())
    {
      return FLASH_EXTDRV_I2C_FAIL;
    }
  }// while (byte_cnt > 0)

  return FLASH_EXTDRV_I2C_DONE;
}

/*
 * Init/Configure the I2C wrapper APIs
 */
void flash_i2c_configure(void *i2c_configs)
{
  struct nor_i2c_wrapper *wrapper = (struct nor_i2c_wrapper *)i2c_configs;

  if (NULL != wrapper)
  {
    wrapper->init = flash_i2c_init;
    wrapper->read_op = flash_i2c_read;
    wrapper->write_op = flash_i2c_write;
  }
}
