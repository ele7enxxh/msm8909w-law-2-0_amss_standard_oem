#ifndef __HAL_SEQ_DDR_H__
#define __HAL_SEQ_DDR_H__

/*=============================================================================

                                   DDR SEQ HAL
                            H e a d e r  F i l e
GENERAL DESCRIPTION
This is the header file for ddr HAL. This file is shared across targets

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/common/HAL_SEQ_DDR.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/16/15   sc      Added HAL_SDRAM_PHY_Enable_IO_Cal declaration
05/21/14   aus     Added post clock switch function definition
05/14/14   aus     Initial revision.
=============================================================================*/


/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "HALhwio.h"
#include "ddr_common.h"


/*==============================================================================
                                  MACROS
==============================================================================*/
#define BOOT_HWIO_OUT(a,b,c)  \
        BOOT_HWIO_OUT_2(a,b,c)
#define BOOT_HWIO_OUT_2(hwiosysm, offset, val)  \
        out_dword(HWIO_##hwiosysm##_ADDR + offset, val)

#define BOOT_HWIO_OUTI(a,b,c,d)  \
        BOOT_HWIO_OUTI_2(a,b,c,d)
#define BOOT_HWIO_OUTI_2(hwiosysm, offset, index, val)  \
        out_dword(HWIO_##hwiosysm##_ADDR(index) + offset, val)

#define BOOT_HWIO_OUTM(a,b,c,d)  \
        BOOT_HWIO_OUTM_2(a,b,c,d)
#define BOOT_HWIO_OUTM_2(hwiosysm, offset, mask, val)  \
        out_dword_masked_ns(HWIO_##hwiosysm##_ADDR + offset, mask, val,  \
                            BOOT_HWIO_IN(hwiosysm, offset))

#define BOOT_HWIO_OUTMI(a,b,c,d,e)  \
        BOOT_HWIO_OUTMI_2(a,b,c,d,e)
#define BOOT_HWIO_OUTMI_2(hwiosysm, offset, index, mask, val)  \
        out_dword_masked_ns(HWIO_##hwiosysm##_ADDR(index) + offset, mask, val,  \
                            BOOT_HWIO_INI(hwiosysm, offset, index))

#define BOOT_HWIO_IN(a,b)  \
        BOOT_HWIO_IN_2(a,b)
#define BOOT_HWIO_IN_2(hwiosysm, offset)  \
        in_dword(HWIO_##hwiosysm##_ADDR + offset)

#define BOOT_HWIO_INI(a,d,c)  \
        BOOT_HWIO_INI_2(a,d,c)
#define BOOT_HWIO_INI_2(hwiosysm, offset, index)  \
        in_dword(HWIO_##hwiosysm##_ADDR(index) + offset)

#define BOOT_HWIO_INM(a,b,c)  \
        BOOT_HWIO_INM_2(a,b,c)
#define BOOT_HWIO_INM_2(hwiosysm, offset, mask)  \
        in_dword_masked(HWIO_##hwiosysm##_ADDR + offset, mask)

#define BOOT_HWIO_INMI(a,b,c,d)  \
        BOOT_HWIO_INMI_2(a,b,c,d)
#define BOOT_HWIO_INMI_2(hwiosysm, offset, index, mask)  \
        in_dword_masked(HWIO_##hwiosysm##_ADDR(index) + offset, mask)

#define BOOT_HWIO_ADDR(hwiosysm) \
        BOOT_HWIO_ADDR_2(hwiosysm)
#define BOOT_HWIO_ADDR_2(hwiosysm) HWIO_##hwiosysm##_ADDR

#define BOOT_HWIO_ADDRI(hwiosysm, index) \
        BOOT_HWIO_ADDRI_2(hwiosysm, index)
#define BOOT_HWIO_ADDRI_2(hwiosysm, index) HWIO_ADDRI(hwiosysm, index)

/* Magic value for detecting DDR size. */
#define DDR_DETECT_SIZE_VAL 0xAAAA5555

/* Constant for one megabyte. */
#define MEGABYTE (1024*1024)


/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
/* =============================================================================
**  Function : HAL_SDRAM_Init
** =============================================================================
*/
/**
*   @brief
*   Initialize DDR controller, as well as DDR device.
*
*   @param[in]  interface    Interface to initialize for
*   @param[in]  chip_select  Chip select to initialize for
*   @param[in]  clk_speed    Clock speed (in KHz) to initialize for
*
*   @retval  None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void HAL_SDRAM_Init(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 clk_speed);

/* ============================================================================
**  Function : HAL_SDRAM_Ram_Size_Detection
** ============================================================================
*/
/*!
*   @brief
*   This function will probe various address of ddr to find out the exact
*   configuration of row and column size
*   
*   @details
*   This function will probe various address of ddr to find out the exact
*   configuration of row and column size
*   
*   @param none
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
boolean HAL_SDRAM_Ram_Size_Detection(SDRAM_INTERFACE interface_name);

/* ============================================================================
**  Function : HAL_SDRAM_Enter_Self_Refresh
** ============================================================================
*/
/*!
*   @brief
*   This function triggers the ddr to enter self refresh mode for interface EBI0/1
*   
*   @details
*   This function triggers the ddr to enter self refresh mode for interface EBI0/1
*   
*   
*   @par Dependencies
*   This code must not reside on the ddr that is being put to self refresh.
*   
*   @par Side Effects
*   DDR is in self refresh mode and if both CS is selected, or only 1 CS is physically
*   present, then the ddr clock is turned off, at this point, any writes to ddr will
*   fail since the controller is not going to respond.
*   
*   @retval  None
*   
*   @sa None
*/
void HAL_SDRAM_Enter_Self_Refresh(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select);

/* ============================================================================
**  Function : HAL_SDRAM_Exit_Self_Refresh
** ============================================================================
*/
/*!
*   @brief
*   This function triggers the ddr to exit self refresh mode for interface EBI0/1
*   
*   @details
*   This function triggers the ddr to exit self refresh mode for interface EBI0/1
*   
*   @par Dependencies
*   
*   
*   @par Side Effects
*   
*   @retval  None
*   
*   @sa None
*/
void HAL_SDRAM_Exit_Self_Refresh(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select);

/* ============================================================================
**  Function : HAL_SDRAM_Enter_Deep_Power_Down
** ============================================================================
*/
/*!
*   @brief
*   This function will put the ddr into DPD mode
*   
*   @details
*   This function will trigger the ddr to be put into deep power down mode.
*   In the case where we only have 1 device on the interface (CS0 only), or both
*   chip select are chosen, we will also turn off ddr clock.
*   
*   @param interface_name   -  [IN] Interface to configure DPD for
*   
*   @par Dependencies
*   The desired CS to be configured for Deep Power Down Must be configured first by
*   calling HAL_SDRAM_Chipselect().
*   
*   @par Side Effects
*   After ddr has gone into Deep Power Down, all the data stored will be lost.
*   Also, when returning from deep power down, we must reinitialize the DDR.
*   
*   @retval  None
*   
*   @sa None
*/
void HAL_SDRAM_Enter_Deep_Power_Down(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT);

/* ============================================================================
**  Function : HAL_SDRAM_PHY_Enable_IO_Cal
** ============================================================================
*/
/*!
*   @brief
*   Enable IO Auto Calibration
*   
*   @details
*   
*   @param interface_name   -  [IN] Interface to enable auto IOCAL for
*   
*   @par Dependencies
*      
*   @par Side Effects
*      
*   @retval  None
*   
*   @sa None
*/
void HAL_SDRAM_PHY_Enable_IO_Cal(SDRAM_INTERFACE interface);

/* ============================================================================
**  Function : HAL_SDRAM_Exit_Deep_Power_Down
** ============================================================================
*/
/*!
*   @brief
*   Brief_Description_Goes_here
*   
*   @details
*   Detailed_Description_Goes_here
*   
*   @param clkspeed   -  [IN] Clock speed to reinitialize ddr for
*   
*   @par Dependencies
*   Clock must be setup for initialization speed.
*   LPDDR1 - no limitation
*   LPDDR2 - < 50Mhz
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
void HAL_SDRAM_Exit_Deep_Power_Down(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT, uint32 clkspeed);

/* ============================================================================
**  Function : HAL_SDRAM_Get_Base_Addr
** ============================================================================
*/
/*!
*   @brief
*   This function return the base address of the specific interface and chip select
*   
*   @details
*   This function will return pre determined base address for the particular interface\chipselect requested
*   
*   @param  None
*   
*   @par Dependencies
*   
*   
*   @par Side Effects
*
*   
*   @retval  None
*   
*   @sa None
*/
uint32 HAL_SDRAM_Get_Base_Addr(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select);

/* =============================================================================
**  Function : HAL_SDRAM_Read_MR
** =============================================================================
*/
/**
*   @brief
*   Read DDR mode register.
*
*   @param[in]  interface    Interface to read mode register for
*   @param[in]  chip_select  Chip select to read mode register for
*   @param[in]  mr_addr      Mode register address
*
*   @return
*   Mode register data that is read back.
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
uint32 HAL_SDRAM_Read_MR(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 mr_addr);


/* =============================================================================
**  Function : HAL_SDRAM_Read_MR_Upper
** =============================================================================
*/
/**
*   @brief
*   Read DDR mode register.
*
*   @param[in]  interface    Interface to read mode register for
*   @param[in]  chip_select  Chip select to read mode register for
*   @param[in]  mr_addr      Mode register address
*
*   @retval  None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
uint32 HAL_SDRAM_Read_MR_Upper(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 mr_addr);

/* =============================================================================
**  Function : HAL_SDRAM_Write_MR
** =============================================================================
*/
/**
*   @brief
*   Write DDR mode register.
*
*   @param[in]  interface    Interface to write mode register for
*   @param[in]  chip_select  Chip select to write mode register for
*   @param[in]  mr_addr      Mode register address
*   @param[in]  mr_data      Mode register data to write
*
*   @retval  None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void HAL_SDRAM_Write_MR(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 mr_addr, uint32 mr_data);

/* ============================================================================
**  Function : HAL_SDRAM_DDR_Wait
** ============================================================================
*/
/*!
*   @brief
*   Waits for the given amount of time in us
*   
*   @details
*   waits the given amount of time in us
*   implemented using pause timer when available
*   
*   @param wait_us     -  [IN] Time to wait in terms of microseconds
*   @param clk_speed   -  [IN] Current clock frequency in KHz
*
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
void HAL_SDRAM_DDR_Wait(uint32 wait_us, uint32 clk_speed);

/* ============================================================================
**  Function : HAL_SDRAM_PHY_Get_HW_Major_Revision
** ============================================================================
*/
/*!
*   @brief
*   This function returns the phy hardware major revision id
*
*   @details
*   This function returns the phy hardware major revision id
*
*   @param
*   None
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  uin8 - major revision id
*
*   @sa None
*/
uint8 HAL_SDRAM_PHY_Get_HW_Major_Revision(void);


/* ============================================================================
**  Function : HAL_SDRAM_Pre_Clock_Switch
** ============================================================================
*/
/*!
*   @brief
*   This function is used for changing the core frequency of operating for the memory controller 
*
*   @details
*   This function execute the hardware driven clock frequency switch which is triggered by the memory
*   controller.
*
*   @param[in]  interface        Interface to write mode register for
*   @param[in]  clk_freq_in_khz  Clock frequency
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval None
*
*   @sa None
*/
void HAL_SDRAM_Pre_Clock_Switch (SDRAM_INTERFACE interface, uint32 clk_freq_in_khz);

/* ============================================================================
 **  Function : HAL_SDRAM_Post_Clock_Switch
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function is used for changing  the core frequency of operating for the memory controller
 *
 *   @details
 *   This function execute the hardware driven clock frequency switch which is triggered by the memory
 *  controller.
 **/
void HAL_SDRAM_Post_Clock_Switch (SDRAM_INTERFACE interface, uint32 clk_freq_in_khz);

/* =============================================================================
 **  Function : HAL_SDRAM_RL
 ** =============================================================================
 */
/**
 *   @brief
 *   Return Read Latency according to DDR type and frequency.
 **/
uint32 HAL_SDRAM_RL(SDRAM_INTERFACE interface, uint32 clk_freq_in_khz);


/* =============================================================================
 **  Function : HAL_SDRAM_WL
 ** =============================================================================
 */
/**
 *   @brief
 *   Return Write Latency according to DDR type and frequency.
 **/
uint32 HAL_SDRAM_WL(SDRAM_INTERFACE interface, uint32 clk_freq_in_khz);

#endif /* __HAL_DDR_H__ */
