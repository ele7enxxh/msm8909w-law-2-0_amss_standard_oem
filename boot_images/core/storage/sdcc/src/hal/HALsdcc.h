#ifndef __HALSDCC_H__
#define __HALSDCC_H__

/**********************************************************************
 * HALsdcc.h
 *
 * Public interface declaration for the SDCC HAL.
 * 
 * This file describes the SDCC HAL external interface, common to all
 * MSMs. The SDCC HAL services calls from SD, MMC and SDIO functions
 * In the documention below, unless specifically mentioned, "SD card"
 * is a generic reference to any SD, MMC, or compatible card.
 *
 * Copyright (c) 2008-2014
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*======================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/hal/HALsdcc.h#1 $
  $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
--------     ---     --------------------------------------------------- 
2014-05-16   sb      Add SDHCi SetCapabilities
2013-09-03   bn      Add DDR support
2010-11-08   rh      Add FIFO block write support
2010-09-14   vj      Resolved compiler warnings
2010-07-21   rh      Add DATA_END interrupt support
2010-03-02   yg      Add enumerator to enum HAL_sdcc_StatusClear
2009-11-13   rh      Add FIFO block read support
2009-09-21   vin     Added  HAL_SDCC_STATUS_TXACTIVE
2009-07-31   vj      Added HAL_SDCC_STATUS_DATA_END
2009-06-30   vin     Added HAL_sdcc_IsIntEnabled and HAL_sdcc_SetInvertOut
2009-06-18   rh      Adding common interface extraction tags
2009-05-07   vin     Moved data timeout definitions to upper layer
2008-11-14   sr      HAL Adaptation for WM6.
2008-09-29   vin     Added HAL_sdcc_GetNumBitsDataLength
2008-04-15   s h     Revised and reformatted.
2008-01-15   uma     Initial version
=======================================================================*/
#include SDCC_OSAL_H

/**  \mainpage SD Card Controller Functional Overview
 *
 * The SD Card Controller driver module consists of four main
 * components:
 *
 * - SDCC hardware abstraction layer (HAL_sdcc)
 * 
 * - SDCC device driver common to all OSs (drivers/sdcc)
 *
 * - The SD Bus implementation for SD Memory and SDIO, for AMSS
 *
 * - Board Support Packages (BSP) for Qualcomm platforms
 * 
 * The SDCC driver and SD stack provide a high-level interface for
 * clients such as Hotplug and WLAN to interact with the SDCC
 * controllers to perform common functions or transfer bulk data.
 * 
 * The BSP for SDCC works with the SDCC driver to define board-level
 * behavior for the SDCC driver.  These platform-specific routines are
 * determined by the specific connections and mappings between the
 * MSM, SD slots, power sources, etc that are unique to that hardware
 * design (Surf, FFA, OEM board.)
 * 
 * The SDCC HAL provides hardware abstraction of the MSM's SD
 * controller to the SDCC driver layer.  The HAL provides a standard
 * API to the SDCC driver, independent of the underlying MSM ASIC
 * hardware.  All hardware access is performed in the HAL.  The HAL is
 * specific to a particular MSM ASIC, and common across all board
 * designs and platforms.
 * 
 * Parameters and data passed to the SDCC HAL are represented in
 * MSM-agnostic values, and must not communicate hardware-specific
 * register values.
 * 
 */

/**
 * @defgroup sdcc SD Card Controller
 *
 * The SDCC software provides block-data transfer and control methods
 * for the MSM SD/MMC Card Controller.
 */

/**
 * @defgroup sdcc_hal Hardware Abstraction Layer (HAL)
 * @ingroup sdcc
 * 
 * The Hardware Abstraction Layer serves to present a common calling
 * interface for all MSM designs that implement an SD Controller
 * block.  The specific attributes of the hardware, such as register
 * addresses, register names, and status bits are all contained within
 * the HAL, and referenced by generic terms between the driver and
 * HAL.
 */

/*@{*/
/**
 * @section error_handling HAL Error Handling
 *
 * No SDCC HAL API call returns an error status.  That is, all
 * functions are assumed to succeed.  Any indication of failure must
 * be detected through additional calls or values passed by reference.
 */

/**
 * @section return_values HAL Return Values
 *
 * Since no success/error status is returned to the caller, most
 * functions have a "void" return type and no return value.
 */

/**
 * @section side_effects HAL "Side Effects"
 *
 * All SDCC HAL functions have "side effects" because they interact
 * with hardware.  That is, there are no "pure functional" HAL calls.
 * The documented purpose of each API call is one and the same with
 * the side-effect.  For this reason, no specific Side Effect, or lack
 * thereof, is stated in the documentation for each function.
 */

/**
 * @section nControllerId SD Controller ID
 *
 * Most SDCC HAL functions take a \c nControllerId parameter as the first
 * argument.  This value represents the physical SD Controller block
 * in the MSM.  These are often referred to as "slot number" or "drive
 * number (driveno)".  Each \c nControllerId refers to a unique hardware
 * block in the MSM, responsible for one SD bus interface.
 *
 * The first controller in an MSM is number "0" and controllers are
 * numbered through N-1, where N is the number of supported
 * controllers in the MSM ASIC.  While the type is uint32, the range
 * of values is usually small -- from 0 to 3 on most 7k targets.
 *
 * @note
 *
 * This value is ASSUMED to be valid by all HAL API calls!  Because
 * these calls are not a public API, and the upper code must already
 * validate the controller number, the HAL will never perform a range
 * or validation check on the controller ID.  The burden is on the
 * Driver layer to call HAL only with valid controller values.  Use of
 * values that are not present in hardware or supported by the HAL
 * will likely result in system failure.  Values must never be passed
 * from user/client code to the HAL without careful inspection.
 */

/*@}*/

/**
 * @defgroup sdcc_hal_bus SDCC Bus Configuration HAL
 * @ingroup sdcc_hal
 *
 * SDCC Bus Configuration HAL
 */
/*@{*/

/** Clock Select for in-coming data and command latch */
enum HAL_sdcc_ClockEdge {
  HAL_SDCC_CLOCK_EDGE_FALLING  = 0, /**< Latch on falling edge of MCIClock */
  HAL_SDCC_CLOCK_EDGE_RISING   = 1, /**< Latch on rising edge of MCIClock */
  HAL_SDCC_CLOCK_EDGE_FEEDBACK = 2, /**< Latch using feedback clock */
  HAL_SDCC_CLOCK_EDGE_LOOPBACK = 3, /**< Latch in loopback mode */
  HAL_SDCC_CLOCK_EDGE_DDR      = 3  /**< Latch in DDR mode */
};

/**
 * Program the SELECT_IN register of MCI_CLK to latch data and command.
 *
 * Incoming Data and Command lines are latched using the clock
 * produced by clock regime of SDCC MCLK. The edge in which the data
 * can be latched is software configurable, and must be carefully
 * chosen to match the speed and timing of the bus.
 *   
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param nClockEdgeIn
 *   Selects the clock edge on which the data and command latches.
 */
void
HAL_sdcc_SetClockEdge (const uint32 nControllerId,
                       enum HAL_sdcc_ClockEdge nClockEdgeIn);

/** Clock Edge Select for outgoing data and command  */
enum HAL_sdcc_InvertOut {
  HAL_SDCC_INVERT_OUT_CLEAR = 0, /**< Data & CMD going out on falling edge */
  HAL_SDCC_INVERT_OUT_SET   = 1, /**< Data & CMD going out on rising edge */
};
/**
 * Program the INVERT_OUT register of MCI_CLK to drive data and command.
 *
 * Outgoing Data and Command lines are latched using the clock
 * produced by clock regime of SDCC MCLK. The edge in which the data
 * can be driven is software configurable, and must be carefully
 * chosen to match the speed and timing of the bus.
 * According to SD Specifications Phys Layer, Ver 2.00 Section 
 * 6.7 and 6.8, low-speed should drive on a rising edge and 
 * high-speed should drive on a falling edge. Hardware default 
 * is '0' for a falling edge. 
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param nClockInvertOut
 *   Selects the clock edge on which the data and command latches.
 */
void
HAL_sdcc_SetInvertOut (const uint32 nControllerId,
                       enum HAL_sdcc_InvertOut nClockInvertOut);

/**
 * Enable or Disable the SD Bus Clock.
 *
 * The MCI_CLK register is programmed to enable or disable the SD bus
 * clock output.  The SD Bus clock should always be turned on during
 * bus operation.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param nClockEnable
 *   TRUE to enable the bus clock.
 */
void
HAL_sdcc_BusClockEnable (const uint32 nControllerId, boolean nClockEnable);

/**
 * Disables / Enables Flow Control (using FLOW_ENA of MCI_CLK).
 *
 * Flow control should be enabled during initialization to avoid
 * aborting in cases where the processor is unable to keep up with the
 * data flow on SD Bus.
 *   
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param nFlowControlEnable
 *   TRUE to enable flow control, FALSE to disable.
 */
void
HAL_sdcc_FlowControl (const uint32 nControllerId,
                      boolean nFlowControlEnable);

/** Clock Select Bus Width configuration   */
enum HAL_sdcc_BusWidth {
  HAL_SDCC_BUS_WIDTH_1_BIT = 0, /**< 1 bit bus */
  HAL_SDCC_BUS_WIDTH_4_BIT = 1, /**< 4 bit bus */
  HAL_SDCC_BUS_WIDTH_8_BIT = 2  /**< 8 bit bus (MMC only) */
};

/**
 * Program the controller for the requested SD bus width.
 *
 * Typical SD bus widths supported are 1, 4 and 8 bits wide.  MMC 4.x
 * can use all three widths, while SD can use 1 or 4-bit widths today.
 *
 * Some SD controllers do not support an 8 bit bus width.  A call to
 * \ref HAL_sdcc_Supports8BitBus() should be made to check for 8 bit
 * bus support prior to calling HAL_sdcc_SetBusWidth() with an 8 bit
 * wide setting.
 *
 * 1- and 4-bit bus settings are supported in all existing SDCC
 * Controllers.  The Driver must send appropriate commands to the card
 * to negotiate the bus width supported by the card BEFORE
 * instructing the controller to change the bus width.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param nBusWidth
 *   Bus width to be set.
 */
void
HAL_sdcc_SetBusWidth (const uint32 nControllerId,
                      enum HAL_sdcc_BusWidth nBusWidth);

/**
 * Check for 8-bit bus width support.
 *
 * This API provides with the information if the underlying controller core
 * supports the 8 bit wide bus support. Some older versions of SDCC
 * controllers can not support 8-bit bus width, this function will make the
 * driver aware of the support to 8-bit interface and the driver can set and
 * configure appropriate bus width.
 *   
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @returns 1 if the controller supports 8-bit bus, zero otherwise.
 */
uint32
HAL_sdcc_Supports8BitBus (const uint32 nControllerId);
/*@}*/

/**
 * @defgroup sdcc_hal_power SDCC Power Control HAL
 * @ingroup sdcc_hal
 *
 * SDCC Power Control HAL
 */
/*@{*/

/**
 * Power control for the Controller Core has three states.  The
 * Power-Up state is used in the transition between Power-Off and
 * Power-On.  This phase is used during the ramp up of VDD from 0 to
 * the required voltage.
 *
 * Power on sequence, starting from VDD Off:
 * 1) VDD ON
 * 2) Issue HAL_SDCC_POWER_UP cmd,
 * 3) delay as needed,
 * 4) Issue HAL_SDCC_POWER_ON cmd.
 */
enum HAL_sdcc_PowerState {
  HAL_SDCC_POWER_OFF = 0,
  HAL_SDCC_POWER_UP  = 1,
  HAL_SDCC_POWER_ON  = 2
};

/**
 * Configure power control.
 *
 * Power control the Controller Core by setting the supported states.
 * The Power-Up state is a state between Power-Off and Power-On.
 * A transition phase during OFF to ON [high state]. This phase is used
 * during the ramp up of VDD from 0 to a full high state.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param nState
 *   This the state to which MCI_POWER should be set for the ASIC.
 */
void
HAL_sdcc_SetPowerState (const uint32 nControllerId,
                        enum HAL_sdcc_PowerState nState);

/**
 * Set the "power save" mode.
 *
 * Disable or Enable the Power Save mode [PWRSAVE of MCI_CLK].  When
 * enabled, the MCI clock will be present (output) only when the clock
 * is enabled and the bus is active.
 *
 * When disabled, (FALSE) the MCI Clock will be always active and
 * driven anytime the Clock is Enabled, even when there is no bus
 * activity.
 *
 * The driver should enable the power save mode to save current
 * when communication is idle.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param nPwrSave
 *   True to enable PWRSAVE and mute the clock during idle.
 */
void
HAL_sdcc_PowerSave (const uint32 nControllerId, boolean nPwrSave);

/*@}*/

/**
 * @defgroup sdcc_hal_cmd SDCC Command and Data 
 * @ingroup sdcc_hal
 * 
 * SDCC Command and Data Transfer
 */
/*@{*/

/** Write or Read direction for data transfer */
enum HAL_sdcc_Dir {
  HAL_SDCC_DIR_WRITE = 0,  /**< controller to card */
  HAL_SDCC_DIR_READ  = 1   /**< card to controller */
};

/** Data transfer mode selection for MCI_DATA_CTL */
enum HAL_sdcc_DataControlMode {
  HAL_SDCC_DCTL_BLK_DATA_XFER    = 0,  /**< block data transfer */
  HAL_SDCC_DCTL_STREAM_DATA_XFER = 1   /**< stream data transfer */
};

/** Data Control register configuration settings */
struct HAL_sdcc_DataControl {
  uint16                          block_size;
  boolean                         dm_enable;
  enum HAL_sdcc_DataControlMode   mode;
  enum HAL_sdcc_Dir               direction;
  boolean                         data_xfer_enable;
};


/** MCI_CMD Configuration structure */
struct HAL_sdcc_SdCmd {
   uint16   dat_cmd;     /**< indicates that this is a Command with Data */
   uint16   prog_ena;    /**< PROG_DONE status bit will be asserted when busy
                              is de-asserted */
   uint16   enable;      /**< If set enables CPSM. */
   uint16   interrupt;   /**< disables cmd timer and waits for int request. */
   uint16   longrsp;     /**< receives a 136-bit long response. */
   uint16   response;    /**< waits for a response. */
   uint16   cmd_index;   /**< Command index. */
   uint32   cmd_arg;     /**< Cmd Argument*/
};

/**
 * Set SD Command values in the SDCC controller.
 *
 * Constructs and sets the MCI_CMD register value to reflect the
 * desired command.  When the command enable flag of HAL_sdcc_SdCmd is
 * selected, the command transfer starts.
 *   
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param pCmd
 *   Command parameter selection structure containing the command
 *   configuration to be programmed to the controller core.
 */ 
void
HAL_sdcc_SetSdCmd (const uint32 nControllerId,
                   const struct HAL_sdcc_SdCmd* pCmd);

/**
 * Get the command's Response from the SD card.
 *
 * The actual content of the SD Response is documented in the SD
 * Specification.
 *
 * There are four different response lengths possible, selected by the
 * nResponseIndex.
 * 
 * The most significant bit of the card status is received first.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param pResp
 *    Pointer to where the SD response will be stored.
 *
 * @param nResponseIndex
 *    Response Index (0 through 3)
 */
void
HAL_sdcc_GetResponse (const uint32 nControllerId,
                      uint32 *pResp,
                      uint32 nResponseIndex);

/**
 * Signal upcoming Abort Command.
 * 
 * Signals that next command will be an abort or a stop command.  This
 * is used to signal the controller after a multi block transfer.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 */
void
HAL_sdcc_SdCmdAbort (const uint32 nControllerId);

/**
 * Set the Data Timeout period.
 *
 * The data timeout period is based on the clock frequency that the
 * controller is running at, and the transfer direction.  The time out
 * involves the clock frequency in KHz and the read / write time out
 * factor.  If this timeout is reached before the start bit is
 * received, it sets the timeout status flag.
 *   
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param data_timer
 *   Timeout value in number of clock cycles
 */  
void
HAL_sdcc_DataTimeout (const uint32 nControllerId, uint32 data_timer);

/**
 * Set data transfer length.
 *
 * Programs data length to MCI_DATA_LENGTH register of controller
 * core.  The MCI_DATA_LENGTH register will contain the number of data
 * bytes to be transfered. This value is loaded into the data counter
 * when data transfer starts.  For block transfers, the data length
 * must be a multiple of the block size.
 *   
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param nDataLengthBytes
 *    Data transfer length in bytes.
 */ 
void
HAL_sdcc_DataLength (const uint32 nControllerId, uint32 nDataLengthBytes);

/**
 * Gets the number of bits in MCI_DATA_LENGTH register
 *
 */
uint32
HAL_sdcc_GetNumBitsDataLength (void);

/**
 * Set block Length.
 *
 * Set data block length in MCI_DATA_CTL register.  Block length is
 * configured as a power-of-two number of bytes.  The maximum allowed
 * block length depends on the specific SD controller: 1 to 4096 on
 * newer sdcc2 controllers, or 1 to 2048 on older sdcc2 controllers.
 * 
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param nBlockSizeBytes
 *   Data block length in bytes.
 */
void
HAL_sdcc_BlockLength (const uint32 nControllerId, uint32 nBlockSizeBytes);

/**
 * Configure the data control register.
 *
 * Program the data control register in the SD controller core. This
 * selects the options to enable data, direction, mode, DM and block
 * size.
 *
 * The data timeout and the data length must be programmed PRIOR to
 * programming this register.
 *
 * @sa HAL_sdcc_DataTimeout
 * @sa HAL_sdcc_DataLength
 * 
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param pDataControl
 *   Pointer to the structure containing the option settings.
 */
void
HAL_sdcc_DataControl (const uint32 nControllerId,
                     const struct HAL_sdcc_DataControl *pDataControl);

/**
 * Configure the data control register.
 *
 * Stop the DPSM
 * 
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 */
void
HAL_sdcc_DataControlStopDPSM (const uint32 nControllerId);


/**
 * Write data to the SD FIFO
 *
 * Write 32 bits of data content to the FIFO register. 
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param nFifoData
 *   32-bit data to be written to the controller.
 */
void
HAL_sdcc_FifoWrite (const uint32 nControllerId, uint32 nFifoData);

/**
 * Read data from the SD FIFO
 *
 * Read data content from the SD FIFO register.
 *   
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @returns
 *   The data word read from the FIFO.
 */
uint32
HAL_sdcc_FifoRead (const uint32 nControllerId);

/**
 * Read block data from the SD FIFO
 *
 * Read data content from the SD FIFO register in block mode.
 * Each block is a fixed 32 bytes.  Reading data in block mode
 * is done to optimize FIFO access.  Instead of calling 
 * HAL_sdcc_FifoRead 8 times to get 32 bytes of data, a single
 * call can remove 32 bytes at a time.  The FIFO must be at least
 * half full before this function can be called.
 *   
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param pDataPt
 *   Pointer to the memory location to put the data in
 *
 */
void
HAL_sdcc_FifoBlockRead (const uint32 nControllerId,uint32 *pDataPt,uint32 *size);

/**
 * Write block data to the SD FIFO
 *
 * Write data content to the SD FIFO register in block mode.
 * Each block is a fixed 32 bytes.  Writing data in block mode
 * is done to optimize FIFO access.  Instead of calling 
 * HAL_sdcc_FifoWrite 8 times to write 32 bytes of data, a single
 * call can move 32 bytes at a time.  The FIFO must be at least
 * half empty before this function can be called.
 *   
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param pDataPt
 *   Pointer to the memory location to put the data in
 *
 */
void
HAL_sdcc_FifoBlockWrite (const uint32 nControllerId,uint32 *pDataPt,uint32 *size);

/**
 * Get physical address for the SD FIFO register.
 *
 * Fetches the physical address of the SD FIFO register.  This address
 * may be required to configure DMA operations to the SD FIFO.
 *   
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @returns
 *   Physical address of the SD FIFO register.
 */
uint32
HAL_sdcc_FifoPhysicalAddress (const uint32 nControllerId);

/**
 * Get the SD FIFO logical register address.
 * 
 * Fetches the (logical) FIFO register address.  This is sometimes
 * required for DMA operations that require the FIFO address may
 * for transfer configuration.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @returns
 *   Logical address of the SD FIFO register
 */
uint32
HAL_sdcc_FifoLogicalAddress (const uint32 nControllerId);

/*@}*/

/**
 * @defgroup sdcc_hal_status SDCC Status
 * @ingroup sdcc_hal
 * 
 * SDCC Status Handling
 */

/*@{*/

#include "HALsdcc_sdcc4.h"

 /**
 * Get entire status register value
 *
 * Read the entire MCI Status register MCI_STATUS from the controller
 * core.   This contains both the Static and Dynamic status flags.
 *
 * The status word value is not directly useable by the HAL client.
 * It is to be passed to HAL_sdcc_StatusCheckFlag() to determine if
 * certain flags are true.  The actual bit contents are "opaque" to
 * the client.
 *   
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @returns Status word value
 */
uint32
HAL_sdcc_StatusNow (const uint32 nControllerId);

/**
 * Check for individual status flags in the status value
 *
 * This function checks for specific flags being set in the hardware
 * status word.  Note that it does not fetch status data, but simply
 * checks the given status word for the presence of a specific flag.
 * 
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param nAllStatus
 *   Full status value from hardware
 *
 * @param nFlag
 *   One flag specifying the status bit to be tested
 *
 * @returns
 *   Zero if the flag is clear, non-zero if the flag is set.
 */
uint32
HAL_sdcc_StatusCheckFlag (const uint32 nControllerId,
                          const uint32 nAllStatus,
                          const uint32 nFlag);

/**
 * Clear a single requested status flag from the static status register.
 *
 * Clears the requested Status bit from the static status register. Static
 * status bits remain asserted until cleared. A write to the MCI_CLEAR
 * register can only clear those set bits.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param nFlag
 *    Which flag in the status to clear
 */ 
void
HAL_sdcc_StatusClear (const uint32 nControllerId,
                      const uint32 nFlag);

/**
 * Clear status flags
 *
 * Clear status flags in the status register. This is a write
 * only register MCI_CLEAR. Setting any bit to 1 in MCI_CLEAR
 * register will clear the corresponding bit in the MCI_STATUS
 * register, which is a read only register.
 *   
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId
 * 
 * @param u32Flag
 *   Flags in the status register to clear
 */
void
HAL_sdcc_StatusClearFlags (const uint32 nControllerId, 
                           const uint32 u32Flag);

/**
 * Clear all static status flags
 *
 * Clear all static status Flags in the status register. This is a write only
 * register MCI_CLEAR. Any write to this register will clear the
 * corresponding static status register in MCI_STATUS which is a read only
 * register.
 *   
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 */
void
HAL_sdcc_StatusClearStaticFlags (const uint32 nControllerId);

/*@}*/

/**
 * @defgroup sdcc_hal_int SDCC Interrupt Control
 * @ingroup sdcc_hal
 * 
 * SDCC Interrupt Control
 */

/*@{*/

/** Cause codes for Interrupt registers */
enum HAL_sdcc_IntSource {
  HAL_SDCC_INT_SOURCE_SDIO          = 0,        /**< SDIO Interrupt */
  HAL_SDCC_INT_SOURCE_PROG          = 1,        /**< Program Done   */
  HAL_SDCC_INT_SOURCE_DATA_TIMEOUT  = 2,        /**< Data Timeout   */
  HAL_SDCC_INT_SOURCE_SDIO_AND_PROG = 3,        /**< SDIO and PROG  */
  HAL_SDCC_INT_SOURCE_DATA_END      = 4,        /**< Data End       */
};

/**
 * Get requested Interrupt state
 *
 * Reads the requested Interrupt mask that provides the monitored
 * interrupt flags.
 * 
 * Two interrupt mask registers are provided by the controller.
 * Specify which one to read by use of nIntMaskNum.
 *   
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param nIntMaskNum
 *   Which interrupt mask to read
 *
 * @returns Interrupt mask value
 */
uint32
HAL_sdcc_GetIntState (const uint32 nControllerId);

/**
 * Enable the requested Interrupt sources.
 * 
 * Two interrupt mask registers are provided by the controller. This
 * function enables interrupts for the given cause code.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param nInt
 *   Source (cause) of the interrupt to be enabled.
 *
 * @param nIntNum
 *   Interrupt number
 */
void
HAL_sdcc_EnableInt (const uint32 nControllerId,
                    enum HAL_sdcc_IntSource nInt);

/**
 * Query if a particular interrupt is currently enabled
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param nInt
 *   Cause of the interrupt to be queried.
 *
 * @param nIntNum
 *   Interrupt number
 *
 * @returns zero if the interrupt is NOT enabled, and 1 if 
 *        the interrupt is enabled
 */
uint32
HAL_sdcc_IsIntEnabled (const uint32 nControllerId,
                       enum HAL_sdcc_IntSource nInt);

/**
 * Disable requested Interrupt flags.
 * 
 * Two interrupt mask registers are provided by the controller. This would
 * disable the interrupts that the current configuration is looking for.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param nInt
 *   Type of interrupt to be disabled.
 *
 * @param nMaskNum
 *   Interrupt Mask Id
 *
 * @param pRegValue
 *   Value to be read
 */ 
void
HAL_sdcc_DisableInt (const uint32 nControllerId,
                     enum HAL_sdcc_IntSource    nInt, 
                     uint32               *pRegValue);

/**
 * Initialize the SD controller base registers.
 * Register Address is virtualized for WM6 Driver, as it runs in
 * user space.
 * 
 */
void 
HAL_sdcc_InitializeRegisters(void);

/**
 * Get interrupt mask state.
 * 
 * Two interrupt mask registers are provided by the controller. This would
 * get the current configuration of interrupts
 *
 * @param nController
 *   SD controller number, as described in \ref nController .
 *
 * @param mask_num
 *   Interrupt Mask Id
 *
 * @param reg_value
 *   Value to be read
 */
void
HAL_sdcc_IntStateGet (const uint32 nController,
                      uint32 * reg_value);

/**
 * sET interrupt mask state.
 * 
 * Two interrupt mask registers are provided by the controller. This would
 * set the current configuration of interrupts
 *
 * @param nController
 *   SD controller number, as described in \ref nController .
 *
 * @param mask_num
 *   Interrupt Mask Id
 *
 * @param reg_value
 *   Value to be set
 */
void
HAL_sdcc_IntStateSet (const uint32 nController,
                      uint32 reg_value);

/*@}*/

/**
 * @defgroup sdcc_hal_mandatory Mandatory HAL Interface
 * @ingroup sdcc_hal
 *
 *
 * HAL Mandatory (Common) API functions.
 * 
 * The HAL specification mandates certain common API calls to be
 * provided by all modules.
 */
/*@{*/

/**
 * Initialize the SD controller core to a known initial state.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param version
 *   Will be set to the SD HAL Version string
 */
void HAL_sdcc_Init (const uint32 nControllerId, char **version);

/**
 * Reset the SD controller core.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 */
void HAL_sdcc_Reset (const uint32 nControllerId);

/**
 * Saves the configuration and state of the SD controller.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @sa HAL_sdcc_Restore
 */
void HAL_sdcc_Save (const uint32 nController, uint32 *state_array);

/**
 * Restores the SD controller state to the last Saved state.
 *
 * HAL_sdcc_Save() must be called prior to calling HAL_sdcc_Restore().
 * 
 * @sa HAL_sdcc_Save
 *
 */
void HAL_sdcc_Restore (const uint32 nController, uint32 *state_array);

 
/**
 * Set the Controller's SDHCi Capabilities.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 */
void HAL_sdhci_SetCapabilities (const uint32 nController);


#endif /* __HALSDCC_H__ */
