/**********************************************************************
 * HALsdhci.c
 *
 * Implementation of SD Host Controller Standard. 
 * Hardware Abstraction Laye(HAL)
 *
 * Copyright (c) 2013-2014
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*=======================================================================
                        Edit History

$Header:
$DateTime:

when        who     what, where, why
----------  ----    ---------------------------------------------------- 
2014-05-16  sb      Added SDHCi PowerSave. Fixed 1ms delay to 1us
2014-03-11  bn      Code clean up
2014-03-11  pa      code cleanup
2013-09-03  bn      Initial release

======================================================================*/

#include "HALsdhci.h"
#include "msmhwiobase.h"
#include "sdcc_bsp.h"

/**
 * Register: SDCC_MCI_HC_MODE (SDCC_MCI Base addr + 0x78)
 * Enables or Disables the Host Controller specifications
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param mode
 *   Enables HC Mode or QCT Mode
 */
void HAL_sdcc_HCModeEnable (const uint32 nControllerId, enum HAL_sdhci_mode mode)
{
   uint32 hwio_addr = sdcc_reg_base[nControllerId] + MCI_HC_MODE_REG_OFFSET;
   uint32 reg_value = in_dword(hwio_addr);

   if (mode == HAL_SDHCI_SDHC_MODE)
   {
      reg_value |= HAL_SDHCI_SDHC_MODE;
   }
   else
   {
      reg_value &= ~HAL_SDHCI_SDHC_MODE;
      reg_value |= HAL_SDHCI_QCT_MODE;
   }
   
   out_dword(hwio_addr, reg_value);
}

/**
 * Handle the power interrupts according to the SWI.
 * See the MCI PWRCTR Status/Clear/Control registers in the SWI for details.
 * When there's a bus power event (bus power on/off), the corresponding
 * PWRCTL status bit is set. Software needs to clear the status bits and sets 
 * the corresponding "success" bit in the PWRCLT_CLT register to indiate 
 * the power events have been handled successfully.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 */
void HAL_sdhci_HandlePowerInterrupts (uint32 nController)
{
   uint32 reg_value = 0;
   uint32 count = HAL_SDHCI_POLL_MAX;

   do
   {
      reg_value = HWIO_SDC_REG_IN (MCI_PWRCTL_STATUS, 
                                   sdcc_reg_base[nController]);
      if (0 != reg_value)
      {
         break;
      }
      sdcc_udelay(1);
   } while (count--);

   /* Clear power interrupt and indicate successfull power switch */
   HWIO_SDC_REG_OUT (MCI_PWRCTL_CLEAR, sdcc_reg_base[nController], reg_value);
   
   if (reg_value & (MCI_PWRCTL_CLR_BUS_OFF_BMSK | MCI_PWRCTL_CLR_BUS_ON_BMSK))
   {
      HWIO_SDC_REG_OUT (MCI_PWRCTL_CTL, sdcc_reg_base[nController], 
                        MCI_PWRCTL_CTL_BUS_ON_OFF_SUCCESS_BMSK);
   }
   else
   {
      HWIO_SDC_REG_OUT (MCI_PWRCTL_CTL, sdcc_reg_base[nController],
                        MCI_PWRCTL_CTL_IO_SIG_SWITCH_SUCCESS_BMSK);
   }

   /* wait for interrupt to be cleared */
   count = HAL_SDHCI_POLL_MAX;
   do
   {
      reg_value = HWIO_SDC_REG_IN (MCI_PWRCTL_STATUS, 
                                   sdcc_reg_base[nController]);
      if (0 == reg_value)
      {
         break;
      }
      sdcc_udelay(1);
   } while (count--);

   return;
}

/**
 * Reg Offset: 0x00. 32-bit register 
 * Used with Auto CMD23 feature. it sets the 32-bit block count value to the
 * argument of the CMD23 while executing Auto CMD23
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param nArg2
 *   32-bit value to be set to the Argument 2 register
 */
void HAL_sdhci_SetArg2 (const uint32 nControllerId, uint32 nArg2)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId] + HAL_SDHCI_REG_0_2_OFFSET;
   out_dword(hwio_addr, nArg2);
}

/**
 * Reg Offset: 0x04. 16-bit register
 * Configures number of bytes in a data block
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param nBlockSize
 *   Indicates the transfer block size
 */
void HAL_sdhci_SetBlockSize (const uint32 nControllerId, uint16 nBlockSize)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId] + HAL_SDHCI_REG_4_6_OFFSET;
   out_word(hwio_addr, nBlockSize);
}

/**
 * Reg Offset: 0x06. 16-bit register
 * Configures the number of blocks in the transfer
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param nBlockCount
 *   Indicates the number of blocks in the transfer
 */
void HAL_sdhci_SetBlockCount (const uint32 nControllerId, uint16 nBlockCount)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId] + HAL_SDHCI_REG_4_6_OFFSET + 2;
   out_word(hwio_addr, nBlockCount);
}

/**
 * Reg Offset: 0x08. 32-bit register
 * Sets the SD Argument
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param nArg
 *   Arguments for SD/eMMC commands
 */
void HAL_sdhci_SetCmdArg (const uint32 nControllerId, uint32 nArg)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId] + HAL_SDHCI_REG_8_A_OFFSET;
   out_dword(hwio_addr, nArg);
}

/**
 * Reg Offset: 0x0C. 16-bit register
 * Controls the operation of data transfer. The Host Driver must set the 
 * Transfer Mode Register before issuing a command which transfers data. 
 * Writes to this reigster are ignored when the DAT Command Inhibit bit 
 * in the Present State register is set.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param arg
 *   Arguments for SD/eMMC commands
 */
void HAL_sdhci_SetTransferMode (const uint32 nControllerId, 
                                const struct HAL_sdhci_TransMode *hal_mode)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId] + HAL_SDHCI_REG_C_E_OFFSET;
   uint16 reg_value = 0;

   reg_value = (hal_mode->multiblock) << SDCHI_MULTI_BLK_SHIFT |
               (hal_mode->direction) << SDHCI_DATA_TRANSFER_DIR_SHIFT |
               (hal_mode->autocmd_en) << SDHCI_AUTO_CMD_ENABLE_SHIFT |
               (hal_mode->blk_count_en) << SDHCI_BLK_COUNT_ENABLE_SHIFT |
               (hal_mode->dma_en);

   out_word(hwio_addr, reg_value);
}

/**
 * Reg Offset: 0x0E. 16-bit register
 * Controls the SD/eMMC command generation.
 * Software should check the CMD and DAT Inhibit bits in 
 * the Present State register before issuing the command 
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param hal_cmd
 *   Contains the command index as well as other command attributes
 */
void HAL_sdhci_SetCommand (const uint32 nControllerId, 
                           const struct HAL_sdhci_Cmd *hal_cmd)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId] + HAL_SDHCI_REG_C_E_OFFSET + 2;
   uint16 reg_value = 0;
   reg_value = ((uint16)(hal_cmd->cmd_idx) << SDHCI_CMD_INDEX_SHIFT) |
               (hal_cmd->cmd_type << SDHCI_CMD_TYPE_SHIFT) |
               (hal_cmd->data_present << SDHCI_CMD_DATA_PRESENT_SEL_SHIFT) |
               (hal_cmd->idx_check << SDHCI_CMD_IDX_CHK_SHIFT) |
               (hal_cmd->crc_check << SDHCI_CMD_IDX_CRC_CHK_SHIFT) |
               (hal_cmd->resp_type);

   out_word(hwio_addr, reg_value);
}

/**
 * Reg Offset: 0x10-0x1F. 48-bit register
 * Reads the response from the memory card
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param resp
 *   [output] The response from the card is stored in this buffer
 *
 * @param_resp bLongResp
 *   Indicates to read a short or long response
 */
void HAL_sdhci_GetResponse (const uint32 nControllerId, uint32 *resp, 
                            boolean bLongResp)
{
   uint32 resp0_addr = sdhc_reg_base[nControllerId] + HAL_SDHCI_REG_10_12_OFFSET;
   /* R1,R1b,R3-R6 normal command responses */
   resp[0] = in_dword(resp0_addr);

   if (bLongResp)
   {
      uint32 uR2Resp[4];
      resp[1] = in_dword(resp0_addr + 4);
      resp[2] = in_dword(resp0_addr + 8);
      resp[3] = in_dword(resp0_addr + 12);
      /* the response bits in SDHC mode are offset when compared to 
       * legacy SDCC. To keep the rest of the driver compatible, 
       * adjust the responses to match legacy response.*/
      uR2Resp[0] = ((resp[3] << 8) & 0xFFFFFF00) | ((resp[2] >> 24) & 0x000000FF);
      uR2Resp[1] = ((resp[2] << 8) & 0xFFFFFF00) | ((resp[1] >> 24) & 0x000000FF);
      uR2Resp[2] = ((resp[1] << 8) & 0xFFFFFF00) | ((resp[0] >> 24) & 0x000000FF);
      uR2Resp[3] = ((resp[0] << 8) & 0xFFFFFF00);
      resp[0] = uR2Resp[0];
      resp[1] = uR2Resp[1];
      resp[2] = uR2Resp[2];
      resp[3] = uR2Resp[3];
   }
}

/**
 * Reg Offset: 0x24. 32-bit register
 * Reads the Present State register which gives the status of the host controller
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @return
 *   32-bit Present State register
 */
uint32 HAL_sdhci_GetPresentState (const uint32 nControllerId)
{
   return in_dword(sdhc_reg_base[nControllerId] + HAL_SDHCI_REG_24_26_OFFSET);
}

/**
 * Reg Offset: 0x28. 8-bit register
 * Sets the Bus Width to 1, 4 or 8-bit bus width
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param BusWidth
 */
void HAL_sdhci_SetBusWidth (const uint32 nControllerId,
                            enum HAL_sdcc_BusWidth BusWidth)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId] + HAL_SDHCI_REG_28_2A_OFFSET;
   uint8 reg_value = in_byte (hwio_addr);

   switch (BusWidth)
   {
   case HAL_SDCC_BUS_WIDTH_1_BIT:
      out_byte (hwio_addr, (reg_value & 
                ~(SDHCI_DATA_TRANS_WIDTH_BMSK | SDHCI_EXT_DATA_TRANS_WIDTH_BMSK)) | 
                SDHCI_DATA_TRANS_WIDTH_1_BIT_BMSK);
      break;

   case HAL_SDCC_BUS_WIDTH_4_BIT:
      out_byte (hwio_addr, (reg_value & 
                ~(SDHCI_DATA_TRANS_WIDTH_BMSK | SDHCI_EXT_DATA_TRANS_WIDTH_BMSK)) | 
                SDHCI_DATA_TRANS_WIDTH_4_BIT_BMSK);
      break;
   
   case HAL_SDCC_BUS_WIDTH_8_BIT:
      out_byte (hwio_addr, (reg_value & ~SDHCI_EXT_DATA_TRANS_WIDTH_BMSK) | 
                SDHCI_DATA_TRANS_WIDTH_8_BIT_BMSK);
      break;
   
   default:
      break;
   }
}

/**
 * Reg Offset: 0x28. 8-bit register
 * Enables High Speed or Normal Speed mode
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param bEnable
 *   Set to TRUE to enable High Speed mode. FALSE to enable Normal Speed mode
 */
void HAL_sdhci_HighSpeedEnable (const uint32 nControllerId, boolean bEnable)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId] + HAL_SDHCI_REG_28_2A_OFFSET;
   uint8 reg_value = in_byte (hwio_addr);
   uint8 hs_bit = bEnable ? SDHCI_HS_EN_BMSK : 0;
   out_byte (hwio_addr, (reg_value & ~SDHCI_HS_EN_BMSK) | hs_bit);
}

/**
 * Reg Offset: 0x28. 8-bit register
 * Enables 32-bit ADMA2 mode
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param bEnable
 *   Set to TRUE to enable AMDA2 mode. FALSE to enable SDMA mode
 */
void HAL_sdhci_ADMAEnable (const uint32 nControllerId, boolean bEnable)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId] + HAL_SDHCI_REG_28_2A_OFFSET;
   uint8 reg_value = in_byte (hwio_addr);
   uint8 adma_sel_bit = bEnable ? SDHCI_32_BIT_ADMA2_ENABLE_BMSK : 0;
   out_byte (hwio_addr, (reg_value & ~SDHCI_DMA_SEL_BMSK) | adma_sel_bit);
}

/**
 * Reg Offset: 0x29. 8-bit register
 * Selects the bus voltage level for the memory device. 
 * The supported voltage levels info can be retrieved from the card's 
 * capabilities register 
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param Voltage
 *   bus voltage level to be used
 */
void HAL_sdhci_VoltageSelect (uint32 nControllerId,
                              HAL_sdhci_BusVolSelect Voltage)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId]+HAL_SDHCI_REG_29_OFFSET;
   uint8 val = in_byte(hwio_addr);
   val = (val & ~SDHCI_BUS_VOL_SEL_BMSK) | Voltage;
   out_byte (hwio_addr, val);  
}

/**
 * Reg Offset: 0x29. 8-bit register
 * Turns the bus power on or off
 * Software should set the voltage level before turn on the power
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param bOn
 *   TRUE to turn on the bus power. FALSE to turn it off
 */
void HAL_sdhci_SetPower (uint32 nControllerId, boolean bOn)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId]+HAL_SDHCI_REG_29_OFFSET;
   uint8 val = in_byte(hwio_addr);
   val = (val & ~SDHCI_BUS_PWR_BMSK) | (uint8)bOn;
   out_byte (hwio_addr, val);   
}

/**
 * Reg Offset: 0x29. 8-bit register
 * Reads the current bus power state.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @return
 *   8-bit value of the Power Control Register (offset 0x29)
 */
uint8 HAL_sdhci_GetPowerState (uint32 nControllerId)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId]+HAL_SDHCI_REG_29_OFFSET;
   uint8 val = in_byte(hwio_addr);
   /* HC_REG_29. Bit0 = BusPower. Bit[1:3] = Voltage Select */
   val = val & (SDHCI_BUS_PWR_BMSK | SDHCI_BUS_VOL_SEL_BMSK);
   return val;
}

/**
 * Reg Offset: 0x2C. 16-bit register
 * Set the SD Clock frequency using 10-bit divider
 * The base frequency and the divider are used to set the SD clock.
 * The base frequency is retrieved from the Capabilities register
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @nDivider
 *   10-bit divider of the Power Control Register (offset 0x29)
 */
void HAL_sdhci_SetClock_10Bits_Mode (uint32 nControllerId, uint32 nDivider)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId]+HAL_SDHCI_REG_2C_OFFSET;
   uint16 val = 0;

   /* The LSB bits of divider are Bits[8:15] */
   val = (nDivider & 0xFF) << SDHCI_SDCLK_FREQ_SEL_SHFT;
   /* The MSB bits of the divider are Bits[6:7] */
   val |= ((nDivider & 0x300) >> 8) << SDHCI_SDCLK_FREQ_SEL_MSB_SHFT;
   /* Enable internal clk */
   val |= SDHCI_INTERNAL_CLK_EN_BMSK;
   /* Write 16-bit */
   out_word (hwio_addr, val);  
}

/**
 * Reg Offset: 0x2C. 16-bit register
 * Enables or stops the SD Clock. Software must stop the SD Clock before
 * making any change to the Clock Frequency Select
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param bEnable
 *   TRUE to enable or FALSE to disable SD Clock
 */
void HAL_sdhci_EnableSDClk (uint32 nControllerId, boolean bEnable)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId]+HAL_SDHCI_REG_2C_OFFSET;
   uint8 val = in_byte (hwio_addr);
   val = (val & ~SDHCI_SDCLK_EN_BMSK) | ((uint8)bEnable << SDHCI_SDCLK_EN_SHFT);
   out_byte(hwio_addr, val);
}

/**
 * Reg Offset: 0x2C. 16-bit register
 * Checks if the internal clock is stable. Software must check the internal 
 * clock stable before enabling the SD Clock
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @return
 *   TRUE indicates the internal clock is stable. FALSE otherwise
 */
boolean HAL_sdhci_IsClkStable (uint32 nControllerId)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId]+HAL_SDHCI_REG_2C_OFFSET;
   uint8 val = in_byte (hwio_addr);
   return (0 != (val & SDHCI_INTERNAL_CLK_STABLE_BMSK));
}

/**
 * Reg Offset: 0x2E. 8-bit register
 * Sets the timeout counter for DAT line timeout. Timeout clock frequency 
 * will be generated by dividing the base clock TMCLK value by this value
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param nTimeoutCounterValue
 *   8-bit value for the DAT line timeout counter
 */
void HAL_sdhci_SetTimeout (const uint32 nControllerId, uint8 nTimeoutCounterValue)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId]+HAL_SDHCI_REG_2E_OFFSET;
   out_byte(hwio_addr, nTimeoutCounterValue);
}

/**
 * Reg Offset: 0x2F. 8-bit register
 * Issues software resets for CMD line, DAT line, or all software reset
 * Set to 1 to initiate a reset. Software must wait for the bit to be cleared
 * indicating the reset has completed.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param ResetType
 *   indicates which reset to perform
 */
void HAL_sdhci_SoftwareReset (const uint32 nControllerId,
                              enum HAL_sdhci_reset ResetType)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId]+HAL_SDHCI_REG_2F_OFFSET;
   uint8 val = (uint8)ResetType;
   uint32 count = HAL_SDHCI_POLL_MAX;
   
   /* Set the reset bits and wait until the corresponding reset bits are cleared, */
   /* indicating the reset has completed */
   out_byte(hwio_addr, val);
 
   while (count--)
   {
      if (0 == (in_byte(hwio_addr) & val))
      {
         break;
      }
      sdcc_udelay(1);
   }
}

/**
 * Reg Offset: 0x30-0x32. This API accesses to two 16-bit registers.
 * Read the Normal Interrupt Status Register and 
 * the Error Interrupt Status Register
 * These status bits are enabled by the Interrupt Status Enable Registers.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @returns
 * 32-bit value: 
 * Bits[0-15]: Reg 0x30. Normal Interrupt Status Register
 * Bits[16-31]: Reg 0x32. Error Interrupt Status Register
 */
uint32 HAL_sdhci_GetInterruptStatus(const uint32 nControllerId)
{
   return in_dword(sdhc_reg_base[nControllerId] + HAL_SDHCI_REG_30_32_OFFSET);
}

/**
 * Reg Offset: 0x30-0x32. This API accesses to two 16-bit registers.
 * Clears the Normal Interrupt Status Register and 
 * the Error Interrupt Status Register
 * Writing a 1 clears the bit. Writing 0 keeps the bit unchanged.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param nMask
 *   Bit-wise mask of the bits to be cleared
 */
void HAL_sdhci_ResetInterruptStatus(const uint32 nControllerId, uint32 nMask)
{
   uint32 count = HAL_SDHCI_POLL_MAX;
   uint32 reg_value = 0;

   nMask &= HAL_SDHCI_INTERRUPT_MSK_ALL;
   do
   {
      out_dword((sdhc_reg_base[nControllerId] + 
                HAL_SDHCI_REG_30_32_OFFSET), nMask);

      reg_value = HAL_sdhci_GetInterruptStatus(nControllerId);
      if (0 == (nMask & reg_value))
      {
         break;
      }
      sdcc_udelay(1);
   }
   while (count--);
}

/**
 * Reg Offset: 0x34-0x36. This API accesses to two 16-bit registers.
 * Enables or Disables the status of the events described in 
 * the Interrupt Status Registers. 
 * Writing a 1 to the Interrupt Status Enable bits indicates that when 
 * the events occur the corresponding interrupt status bit is set 
 * (Note that the interrupt is not triggered unless 
 * the corresponding Interrupt Signal Enable bit is set.)
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param mask
 *   Bit-wise mask of the status to be enabled/disabled
 * 
 * @param bEnable
 *   Indicates the operation to be carried out.
 *   If set to 1, enable the status bits that are indicated in the nMask param
 *   If clear to 0, disable the status bits that are indicated in the nMask param.
 */
void HAL_sdhci_InterruptStatusEnable(const uint32 nControllerId,
                                     uint32 nMask, boolean bEnable)
{
   uint32 count = HAL_SDHCI_POLL_MAX;
 
   uint32 hwio_addr = sdhc_reg_base[nControllerId] + HAL_SDHCI_REG_34_36_OFFSET;
   uint32 reg_value = in_dword(hwio_addr);

   /* Disable interrupt status bits */
   if (FALSE == bEnable)
   {
      out_dword(hwio_addr, (reg_value & (~nMask)));
      return;
   }

   /* Enable the interrupt status bits */
   do
   {
      out_dword(hwio_addr, (reg_value & (~nMask)) | nMask);
      /* Make sure the setting sticks */
      reg_value = in_dword(hwio_addr);
      if ((reg_value & nMask) == nMask)
      {
         break;
      }

      sdcc_udelay(1);

   } while (count--);
}

/**
 * Reg Offset: 0x38 - 0x3A. This API accesses to two 16-bit registers.
 * Enables/Disables the Normal Interrupt Signal (0x38) and the Error 
 * Interrupt Signal (0x3A). Writing a 1 to the bits in the Interrupt Signal Enable
 * indicates that if the corresponding Interrupt Status Enable bit is set, 
 * the interrupt would be triggered to the system
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param nMask
 *   Bit-wise mask of the interrupt signal to be enabled or disabled
 * 
 * @param bEnable
 *   Indicates the operation to be carried out.
 *   If set to 1, enable the bit-wise nMask param as the interrupt triggered events
 *   If clear to 0, disable the bit-wise nMask param as the interrupt triggered events
 */
void HAL_sdhci_InterruptSignalEnable(const uint32 nControllerId,
                                     uint32 nMask, uint8 bEnable)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId] + HAL_SDHCI_REG_38_3A_OFFSET;
   uint32 reg_value = in_dword(hwio_addr);
   if (TRUE == bEnable)
   {
      out_dword(hwio_addr, (reg_value & (~nMask)) | nMask);
   }
   else 
   {
      out_dword(hwio_addr, (reg_value & (~nMask)));
   }
}

/* Reg Offset: 0x3C. 16-bit Register
 * Auto CMD Error Status - Not implemented
 */

/**
 * Reg Offset: 0x3E. 16-bit register
 * Enables DDR50 mode.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 */
void HAL_sdhci_EnableDDR50 (uint32 nControllerId)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId]+HAL_SDHCI_REG_3E_OFFSET;
   uint16 val = in_word (hwio_addr);
   val = ((val & ~(uint16)SDHCI_UHS_MODE_SEL_BMSK) | SDHCI_DDR50_ENABLED_BMSK);
   out_word(hwio_addr, val);
}

/**
 * Reg Offset: 0x40. 64-bit register
 * Retrieves the Capabilities. The capabilities provides information 
 * specific to host controller implemenations
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param Capabilities
 *   Host controller capabilities
 */
void HAL_sdhci_GetCapabilities(const uint32 nControllerId, 
                               struct HAL_sdhci_Capabilities *Capabilities)
{
   Capabilities->HAL_sdhci_Reg_40_42 = 
      in_dword (sdhc_reg_base[nControllerId]+HAL_SDHCI_REG_40_42_OFFSET);
   Capabilities->HAL_sdhci_Reg_44_46 = 
      in_dword (sdhc_reg_base[nControllerId]+HAL_SDHCI_REG_44_46_OFFSET);
}

/**
 * Reg Offset: 0x58. 32-bit register
 * Sets the descriptor table address for the 32-bit Address ADMA
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param nAddress
 *   Specifies the address of the descriptor table for 32-bit ADMA
 */
void HAL_sdhci_SetADMASystemAddress(const uint32 nControllerId, uint32 nAddress)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId] + HAL_SDHCI_REG_58_5A_OFFSET;
   out_dword(hwio_addr, nAddress);
}

/**
 * Reg Offset: 0x58. 32-bit register
 * Sets the descriptor table address for the 32-bit Address ADMA
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param nAddress
 *   Specifies the address of the descriptor table for 32-bit ADMA
 */
void HAL_sdhci_SetADMASystemAddress64bit(const uint32 nControllerId,
                                         uint32 nAddress)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId] + 
                      HAL_SDHCI_REG_5C_5E_OFFSET;
   out_dword(hwio_addr, nAddress);
}

/**
 * Register SDCC_HC_REG_DLL_CONFIG
 * Disables the DLL clock
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 */
void HAL_sdhci_DisableDLLClock (const uint32 nControllerId)
{
   uint32 hwio_addr = sdhc_reg_base[nControllerId] + 
                      HAL_SDHCI_REG_DLL_CONFIG_OFFSET;
   uint32 val = in_dword (hwio_addr);
   /* Set PDN bit (Bit29), then DLL_RST bit (Bit30) */ 
   out_dword (hwio_addr, (val | HC_REG_DLL_CONFIG_PDN_BMSK));
   out_dword (hwio_addr, (val | HC_REG_DLL_CONFIG_RST_BMSK | 
              HC_REG_DLL_CONFIG_PDN_BMSK));
}

/**
* Reg Offset: 0x10C. 32-bit register
* Enable or disable SDCLK clock output when bus is idle to save power.
* Enable or disable Prime-Cell MCI clock output when bus is idle to save power.
*
* @param nControllerId
*   SD controller number, as described in \ref nControllerId .
*
* @param bEnable
*   Set to TRUE enable the Power Save. Otherwise, FALSE to disable
*/
void HAL_sdhci_PowerSave (const uint32 nControllerId, boolean bEnable)
{
  uint32 hwio_addr = sdhc_reg_base[nControllerId] +
                      HAL_SDHCI_VENDOR_SPECIFIC_FUNC_OFFSET;
  
  uint32 val = in_dword (hwio_addr);
 
  if (TRUE == bEnable)
  {
    val |= SDHCI_PWRSAVE_EN_BMSK;
  }
  else
  {
    val &= ~SDHCI_PWRSAVE_EN_BMSK;
  }

  out_dword (hwio_addr, val);
}

