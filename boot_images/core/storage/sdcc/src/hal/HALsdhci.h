#ifndef __HALSDHCI_H__
#define __HALSDHCI_H__

/**********************************************************************
 * HALsdhci.h
 *
 * Header file for SD Host Controller Standard Hardware Abstraction Laye(HAL)
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
----------  ---     ---------------------------------------------------- 
2014-11-11  sb      Fixed SetCapabilities
2014-05-16  sb      Added SDHCi's PowerSave.
2014-03-11  bn      Corrected the bit mask of the Bus Voltage field. Increased poll timeout
2014-03-11  pa      code cleanup
2013-09-03  bn      Initial release

======================================================================*/
#include "comdef.h"
#include "HALsdcc.h"

#define HAL_SDHCI_POLL_MAX   100000

#define MCI_HC_MODE_REG_OFFSET 0x78

//offsets for SDHC registers
#define HAL_SDHCI_REG_0_2_OFFSET                           (0x00000000)
#define HAL_SDHCI_REG_4_6_OFFSET                           (0x00000004)
#define HAL_SDHCI_REG_8_A_OFFSET                           (0x00000008)
#define HAL_SDHCI_REG_C_E_OFFSET                           (0x0000000c)
#define HAL_SDHCI_REG_10_12_OFFSET                         (0x00000010)
#define HAL_SDHCI_REG_14_16_OFFSET                         (0x00000014)
#define HAL_SDHCI_REG_18_1A_OFFSET                         (0x00000018)
#define HAL_SDHCI_REG_1C_1E_OFFSET                         (0x0000001c)
#define HAL_SDHCI_REG_20_22_OFFSET                         (0x00000020)
#define HAL_SDHCI_REG_24_26_OFFSET                         (0x00000024)
#define HAL_SDHCI_REG_28_2A_OFFSET                         (0x00000028)
#define HAL_SDHCI_REG_28_OFFSET                            (0x00000028)
#define HAL_SDHCI_REG_29_OFFSET                            (0x00000029)
#define HAL_SDHCI_REG_2C_2E_OFFSET                         (0x0000002c)
#define HAL_SDHCI_REG_2C_OFFSET                            (0x0000002c)
#define HAL_SDHCI_REG_2E_OFFSET                            (0x0000002e)
#define HAL_SDHCI_REG_2F_OFFSET                            (0x0000002f)
#define HAL_SDHCI_REG_30_32_OFFSET                         (0x00000030)
#define HAL_SDHCI_REG_34_36_OFFSET                         (0x00000034)
#define HAL_SDHCI_REG_38_3A_OFFSET                         (0x00000038)
#define HAL_SDHCI_REG_3C_3E_OFFSET                         (0x0000003c)
#define HAL_SDHCI_REG_3E_OFFSET                            (0x0000003E)
#define HAL_SDHCI_REG_40_42_OFFSET                         (0x00000040)
#define HAL_SDHCI_REG_44_46_OFFSET                         (0x00000044)
#define HAL_SDHCI_REG_48_4A_OFFSET                         (0x00000048)
#define HAL_SDHCI_REG_50_52_OFFSET                         (0x00000050)
#define HAL_SDHCI_REG_54_56_OFFSET                         (0x00000054)
#define HAL_SDHCI_REG_58_5A_OFFSET                         (0x00000058)
#define HAL_SDHCI_REG_5C_5E_OFFSET                         (0x0000005c)
#define HAL_SDHCI_REG_60_62_OFFSET                         (0x00000060)
#define HAL_SDHCI_REG_64_66_OFFSET                         (0x00000064)
#define HAL_SDHCI_REG_68_6A_OFFSET                         (0x00000068)
#define HAL_SDHCI_REG_6C_6E_OFFSET                         (0x0000006c)
#define HAL_SDHCI_REG_E0_E2_OFFSET                         (0x000000e0)
#define HAL_SDHCI_REG_FC_FE_OFFSET                         (0x000000fc)
#define HAL_SDHCI_REG_DLL_CONFIG_OFFSET                    (0x00000100)
#define HAL_SDHCI_REG_DLL_TEST_CTL_OFFSET                  (0x00000104)
#define HAL_SDHCI_REG_DLL_STATUS_OFFSET                    (0x00000108)
#define HAL_SDHCI_VENDOR_SPECIFIC_FUNC_OFFSET              (0x0000010c)
#define HAL_SDHCI_VENDOR_SPECIFIC_ADMA_ERR_OFFSET0_OFFSET  (0x00000114)
#define HAL_SDHCI_VENDOR_SPECIFIC_ADMA_ERR_OFFSET1_OFFSET  (0x00000118)
#define HAL_SDHCI_VENDOR_CAPS0_OFFSET                      (0x0000011C)
#define HAL_SDHCI_VENDOR_CAPS1_OFFSET                      (0x00000120)

//BitMasks for Present State Registers
#define HAL_SDHCI_PRESENT_STATE_CMD_LINE_SIGNAL_LEVEL_BMSK          0x1000000
#define HAL_SDHCI_PRESENT_STATE_DAT_3_0_LINE_SIGNAL_LEVEL_BMSK      0xF00000
#define HAL_SDHCI_PRESENT_STATE_WR_PROTECT_SWITCH_PIN_LEVEL_BMSK    0x80000
#define HAL_SDHCI_PRESENT_STATE_CARD_DETECT_PIN_LEVEL_BMSK          0x40000
#define HAL_SDHCI_PRESENT_STATE_CARD_STATE_STABLE_BMSK              0x20000
#define HAL_SDHCI_PRESENT_STATE_CARD_INSERTED_BMSK                  0x10000
#define HAL_SDHCI_PRESENT_STATE_BUF_RD_EN_BMSK                      0x800
#define HAL_SDHCI_PRESENT_STATE_BUF_WR_EN_BMSK                      0x400
#define HAL_SDHCI_PRESENT_STATE_RD_TRANS_ACT_BMSK                   0x200
#define HAL_SDHCI_PRESENT_STATE_WR_TRANS_ACT_BMSK                   0x100
#define HAL_SDHCI_PRESENT_STATE_RETUNING_REQ_BMSK                   0x8
#define HAL_SDHCI_PRESENT_STATE_DAT_LINE_ACT_BMSK                   0x4
#define HAL_SDHCI_PRESENT_STATE_CMD_INHIBIT_DAT_BMSK                0x2
#define HAL_SDHCI_PRESENT_STATE_CMD_INHIBIT_CMD_BMSK                0x1


#define HAL_SDHCI_INTERRUPT_MSK_ALL 0x7FF003F
#define HAL_SDHCI_INTERRUPT_ERROR_MSK_ALL 0x7FF0000

// Bit Masks for Interrupt Status Registers
#define HAL_SDHCI_ERR_INT_STS_VENDOR_SPECIFIC_BMSK         0xF0000000
#define HAL_SDHCI_ERR_INT_STS_TUNING_ERR_BMSK               0x4000000
#define HAL_SDHCI_ERR_INT_STS_ADMA_ERR_BMSK                 0x2000000
#define HAL_SDHCI_ERR_INT_STS_AUTO_CMD_ERR_BMSK             0x1000000
#define HAL_SDHCI_ERR_INT_STS_CURRENT_LIMIT_ERR_BMSK         0x800000
#define HAL_SDHCI_ERR_INT_STS_DATA_END_BIT_ERR_BMSK          0x400000
#define HAL_SDHCI_ERR_INT_STS_DATA_CRC_ERR_BMSK              0x200000
#define HAL_SDHCI_ERR_INT_STS_DATA_TIMEOUT_ERR_BMSK          0x100000
#define HAL_SDHCI_ERR_INT_STS_CMD_INDX_ERR_BMSK               0x80000
#define HAL_SDHCI_ERR_INT_STS_CMD_END_BIT_ERR_BMSK            0x40000
#define HAL_SDHCI_ERR_INT_STS_CMD_CRC_ERR_BMSK                0x20000
#define HAL_SDHCI_ERR_INT_STS_CMD_TIMEOUT_ERR_BMSK            0x10000
#define HAL_SDHCI_NORMAL_INT_STS_ERR_INT_BMSK                  0x8000
#define HAL_SDHCI_NORMAL_INT_STS_RETUNING_EVENT_BMSK           0x1000
#define HAL_SDHCI_NORMAL_INT_STS_INT_C_BMSK                     0x800
#define HAL_SDHCI_NORMAL_INT_STS_INT_B_BMSK                     0x400
#define HAL_SDHCI_NORMAL_INT_STS_INT_A_BMSK                     0x200
#define HAL_SDHCI_NORMAL_INT_STS_CARD_INT_BMSK                  0x100
#define HAL_SDHCI_NORMAL_INT_STS_CARD_REMOVAL_BMSK               0x80
#define HAL_SDHCI_NORMAL_INT_STS_CARD_INSERTION_BMSK             0x40
#define HAL_SDHCI_NORMAL_INT_STS_BUF_RD_READY_BMSK               0x20
#define HAL_SDHCI_NORMAL_INT_STS_BUF_WR_READY_BMSK               0x10
#define HAL_SDHCI_NORMAL_INT_STS_DMA_INT_BMSK                     0x8
#define HAL_SDHCI_NORMAL_INT_STS_BLK_GAP_EVENT_BMSK               0x4
#define HAL_SDHCI_NORMAL_INT_STS_TRANS_COMPLETE_BMSK              0x2
#define HAL_SDHCI_NORMAL_INT_STS_CMD_COMPLETE_BMSK                0x1

/* Bit Masks for Interrupt Status Registers */
#define SDHCI_STATUS_VENDOR_SPECIFIC_BMSK      0xF0000000
#define SDHCI_STATUS_TUNING_ERR_BMSK           0x4000000
#define SDHCI_STATUS_ADMA_ERR_BMSK             0x2000000
#define SDHCI_STATUS_AUTO_CMD_ERR_BMSK         0x1000000
#define SDHCI_STATUS_CURRENT_LIMIT_ERR_BMSK    0x800000
#define SDHCI_STATUS_DATA_END_BIT_ERR_BMSK     0x400000
#define SDHCI_STATUS_DATA_CRC_ERR_BMSK         0x200000
#define SDHCI_STATUS_DATA_TIMEOUT_ERR_BMSK     0x100000
#define SDHCI_STATUS_CMD_INDX_ERR_BMSK         0x80000
#define SDHCI_STATUS_CMD_END_BIT_ERR_BMSK      0x40000
#define SDHCI_STATUS_CMD_CRC_ERR_BMSK          0x20000
#define SDHCI_STATUS_CMD_TIMEOUT_ERR_BMSK      0x10000
#define SDHCI_STATUS_ERR_INT_BMSK              0x8000
#define SDHCI_STATUS_RETUNING_EVENT_BMSK       0x1000
#define SDHCI_STATUS_INT_C_BMSK                0x800
#define SDHCI_STATUS_INT_B_BMSK                0x400
#define SDHCI_STATUS_INT_A_BMSK                0x200
#define SDHCI_STATUS_CARD_INT_BMSK             0x100
#define SDHCI_STATUS_CARD_REMOVAL_BMSK         0x80
#define SDHCI_STATUS_CARD_INSERTION_BMSK       0x40
#define SDHCI_STATUS_BUF_RD_READY_BMSK         0x20
#define SDHCI_STATUS_BUF_WR_READY_BMSK         0x10
#define SDHCI_STATUS_DMA_INT_BMSK              0x8
#define SDHCI_STATUS_BLK_GAP_EVENT_BMSK        0x4
#define SDHCI_STATUS_TRANS_COMPLETE_BMSK       0x2
#define SDHCI_STATUS_CMD_COMPLETE_BMSK         0x1

extern uint32 sdhc_reg_base[];

enum HAL_sdhci_mode {
  HAL_SDHCI_QCT_MODE = 0,
  HAL_SDHCI_SDHC_MODE = 1
};

/* Field enumeration for MCI_HC_MODE */
#define MCI_HC_MODE_ENABLED         0x1
#define MCI_HC_MODE_DISABLED        0x0

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
void HAL_sdcc_HCModeEnable (const uint32 nController, 
                            enum HAL_sdhci_mode mode);

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
void HAL_sdhci_HandlePowerInterrupts (uint32 nController);

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
void HAL_sdhci_SetArg2 (const uint32 nControllerId, uint32 nArg2);

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
void HAL_sdhci_SetBlockSize (const uint32 nControllerId, uint16 nBlockSize);

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
void HAL_sdhci_SetBlockCount (const uint32 nControllerId, uint16 nBlockCount);

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
void HAL_sdhci_SetCmdArg (const uint32 nControllerId, uint32 nArg);

struct HAL_sdhci_TransMode {
   uint8    multiblock;
   uint8    direction;
   uint8    autocmd_en;
   uint8    blk_count_en;
   uint8    dma_en;
};

#define SDCHI_MULTI_BLK_SHIFT         0x5
#define SDHCI_DATA_TRANSFER_DIR_SHIFT 0x4
#define SDHCI_AUTO_CMD_ENABLE_SHIFT   0x2
#define SDHCI_BLK_COUNT_ENABLE_SHIFT  0x1

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
                                const struct HAL_sdhci_TransMode *hal_mode);

struct HAL_sdhci_Cmd {
   uint8   cmd_idx;
   uint8   cmd_type;
   uint8   data_present;
   uint8   idx_check;
   uint8   crc_check;
   uint8   resp_type;
};

#define SDHCI_CMD_INDEX_SHIFT             0x8
#define SDHCI_CMD_TYPE_SHIFT              0x6
#define SDHCI_CMD_DATA_PRESENT_SEL_SHIFT  0x5
#define SDHCI_CMD_IDX_CHK_SHIFT           0x4
#define SDHCI_CMD_IDX_CRC_CHK_SHIFT       0x3

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
                           const struct HAL_sdhci_Cmd *hal_cmd);

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
                            boolean bLongResp);

/**
 * Reg Offset: 0x20. 32-bit register
 * Reads the data from the internal buffer. Used to read the data from
 * the memory device in FIFO mode
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 */
//uint32 HAL_sdhci_ReadDataBufPort (const uint32 nControllerId);
inline uint32 HAL_sdhci_ReadDataBufPort (const uint32 nControllerId)
{
   return in_dword(sdhc_reg_base[nControllerId] + HAL_SDHCI_REG_20_22_OFFSET);
}

/**
 * Reg Offset: 0x20. 32-bit register
 * Writes the data to the internal buffer. Used to write the data to 
 * the memory device in FIFO mode
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param nVal
 *   32-bit value to be written to the internal buffer which will 
 *   be sent to the memory device
 */
//void HAL_sdhci_WriteDataBufPort (const uint32 nControllerId, uint32 nVal);
inline void 
HAL_sdhci_WriteDataBufPort (const uint32 nControllerId, uint32 nVal)
{
   out_dword((sdhc_reg_base[nControllerId] + HAL_SDHCI_REG_20_22_OFFSET), nVal);
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
uint32 HAL_sdhci_GetPresentState  (const uint32 nControllerId);

#define SDHCI_DATA_TRANS_WIDTH_BMSK          0x2
#define SDHCI_EXT_DATA_TRANS_WIDTH_BMSK      0x20
#define SDHCI_DATA_TRANS_WIDTH_1_BIT_BMSK    0x0
#define SDHCI_DATA_TRANS_WIDTH_4_BIT_BMSK    0x2
#define SDHCI_DATA_TRANS_WIDTH_8_BIT_BMSK    0x20

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
                            enum HAL_sdcc_BusWidth BusWidth);

#define SDHCI_HS_EN_BMSK  0x4

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
void HAL_sdhci_HighSpeedEnable (const uint32 nControllerId, boolean bEnable);

#define SDHCI_32_BIT_ADMA2_ENABLE_BMSK  0x10
#define SDHCI_DMA_SEL_BMSK              0x18

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
void HAL_sdhci_ADMAEnable (const uint32 nControllerId, boolean bEnable);

typedef enum HAL_sdhci_BusVolSelect_t {
  HAL_SDHCI_SD_BUS_VOLTAGE_33_BMSK = 0xE,
  HAL_SDHCI_SD_BUS_VOLTAGE_30_BMSK = 0xC,
  HAL_SDHCI_SD_BUS_VOLTAGE_18_BMSK = 0xA
}HAL_sdhci_BusVolSelect;

#define SDHCI_BUS_PWR_BMSK      0x1  /* Bit0 */
#define SDHCI_BUS_VOL_SEL_BMSK  0xE  /* Bits[1:3] */

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
                              HAL_sdhci_BusVolSelect Voltage);

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
void HAL_sdhci_SetPower (uint32 nControllerId, boolean bOn);

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
uint8 HAL_sdhci_GetPowerState (uint32 nControllerId);

/* Reg Offset: 2A. 8-bit Register
 * Block Gap Ctrl - Not implemented
 */

/* Reg Offset: 2B. 8-bit Register
 * Wake Up Control - Not implemented
 */

#define SDHCI_SDCLK_FREQ_SEL_SHFT       0x8
#define SDHCI_SDCLK_FREQ_SEL_MSB_SHFT   0x6
#define SDHCI_GEN_SEL_BMSK              0x20
#define SDHCI_GEN_SEL_SHFT              0x5
#define SDHCI_SDCLK_EN_BMSK             0x4 /* Bit2*/  
#define SDHCI_SDCLK_EN_SHFT             0x2
#define SDHCI_INTERNAL_CLK_STABLE_BMSK  0x2 /* Bit1 */
#define SDHCI_INTERNAL_CLK_STABLE_SHFT  0x1
#define SDHCI_INTERNAL_CLK_EN_BMSK      0x1 /* Bit0 */
#define SDHCI_INTERNAL_CLK_EN_SHFT      0x0

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
void HAL_sdhci_SetClock_10Bits_Mode (uint32 nControllerId, uint32 nDivider);

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
void HAL_sdhci_EnableSDClk (uint32 nControllerId, boolean bEnable);

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
boolean HAL_sdhci_IsClkStable (uint32 nControllerId);

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
void HAL_sdhci_SetTimeout (const uint32 nControllerId,
                           uint8 nTimeoutCounterValue);

enum HAL_sdhci_reset {
  HAL_SDHCI_RESET_ALL     = 0x1,  /* Bit0 */
  HAL_SDHCI_RESET_CMD     = 0x2,  /* Bit1 */
  HAL_SDHCI_RESET_DAT     = 0x4,  /* Bit2 */ 
  HAL_SDHCI_RESET_CMD_DAT = 0x6   /* Bit[1:2] */
};

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
                              enum HAL_sdhci_reset ResetType);

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
uint32 HAL_sdhci_GetInterruptStatus (const uint32 nControllerId);

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
void HAL_sdhci_ResetInterruptStatus (const uint32 nControllerId, uint32 nMask);

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
void HAL_sdhci_InterruptStatusEnable (const uint32 nControllerId,
                                      uint32 nMask, boolean bEnable);

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
void HAL_sdhci_InterruptSignalEnable (const uint32 nControllerId, 
                                      uint32 nMask, boolean bEnable);

/* Reg Offset: 0x3C. 16-bit Register
 * Auto CMD Error Status - Not implemented
 */

#define SDHCI_UHS_MODE_SEL_BMSK   0x7
#define SDHCI_DDR50_ENABLED_BMSK  0x4

/**
 * Reg Offset: 0x3E. 16-bit register
 * Enables DDR50 mode.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 */
void HAL_sdhci_EnableDDR50 (uint32 nControllerId);

//reg 3C: auto cmd err status

struct HAL_sdhci_Capabilities {
   uint32 HAL_sdhci_Reg_40_42;
   uint32 HAL_sdhci_Reg_44_46;
};

// Bit Masks for Capabilities Registers
#define HAL_SDHCI_CAPS_SLOT_TYPE_BMSK                0xc0000000
#define HAL_SDHCI_CAPS_ASYNC_INT_SUPPORT_BMSK        0x20000000
#define HAL_SDHCI_CAPS_SYS_BUS_SUPPORT_64_BIT_BMSK   0x10000000
#define HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_1_8V_BMSK     0x4000000
#define HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_0V_BMSK     0x2000000
#define HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_3V_BMSK     0x1000000
#define HAL_SDHCI_CAPS_SUSPEND_RESUME_SUPPORT_BMSK   0x800000
#define HAL_SDHCI_CAPS_SDMA_SUPPORT_BMSK             0x400000
#define HAL_SDHCI_CAPS_HS_SUPPORT_BMSK               0x200000
#define HAL_SDHCI_CAPS_ADMA2_SUPPORT_BMSK            0x80000
#define HAL_SDHCI_CAPS_SUPPORT_8_BIT_BMSK            0x40000
#define HAL_SDHCI_CAPS_MAX_BLK_LENGTH_BMSK           0x30000
#define HAL_SDHCI_CAPS_BASE_SDCLK_FREQ_BMSK          0xff00
#define HAL_SDHCI_CAPS_TIMEOUT_CLK_UNIT_BMSK         0x80
#define HAL_SDHCI_CAPS_TIMEOUT_CLK_FREQ_BMSK         0x3f
#define HAL_SDHCI_CAPS_CLK_MULTIPLIER_BMSK           0xff0000
#define HAL_SDHCI_CAPS_RETUNING_MODE_BMSK            0xc000
#define HAL_SDHCI_CAPS_USE_TUNING_FOR_SDR50_BMSK     0x2000
#define HAL_SDHCI_CAPS_TIMER_CNT_FOR_RETUNING_BMSK   0xf00
#define HAL_SDHCI_CAPS_DRIVER_TYPE_D_SUPPORT_BMSK    0x40
#define HAL_SDHCI_CAPS_DRIVER_TYPE_C_SUPPORT_BMSK    0x20
#define HAL_SDHCI_CAPS_DRIVER_TYPE_A_SUPPORT_BMSK    0x10
#define HAL_SDHCI_CAPS_DDR_50_SUPPORT_BMSK           0x4
#define HAL_SDHCI_CAPS_SDR_104_SUPPORT_BMSK          0x2
#define HAL_SDHCI_CAPS_SDR_50_SUPPORT_BMSK           0x1

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
void HAL_sdhci_GetCapabilities (const uint32 nControllerId, 
                                struct HAL_sdhci_Capabilities *Capabilities);

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
void HAL_sdhci_SetADMASystemAddress (const uint32 nControllerId,
                                     uint32 nAddress);

/**
 * Reg Offset: 0x5C. 32-bit register
 * Sets the descriptor table address for the 64-bit Address ADMA
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param nAddress
 *   Specifies the address of the descriptor table for 64-bit ADMA 
 */
void HAL_sdhci_SetADMASystemAddress64bit (const uint32 nControllerId, 
                                          uint32 nAddress);

#define HC_REG_DLL_CONFIG_PDN_BMSK  0x20000000
#define HC_REG_DLL_CONFIG_RST_BMSK  0x40000000

/**
 * Register SDCC_HC_REG_DLL_CONFIG
 * Disables the DLL clock
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 */
void HAL_sdhci_DisableDLLClock (const uint32 nControllerId);

#define SDHCI_PWRSAVE_EN_BMSK             0x2 /* Bit1*/  
#define SDHCI_PWRSAVE_ENABLE              0x2 /* Bit1*/  

/**
* Enable or disable Prime-Cell MCI clock output when bus is idle to save power.
*
* @param nControllerId
*   SD controller number, as described in \ref nControllerId .
*
* @param bEnable
*   Set to TRUE enable the Power Save. Otherwise, FALSE to disable
*/

void HAL_sdhci_PowerSave (const uint32 nControllerId, boolean bEnable);  

#endif //__HALSDHCI_H__
