#ifndef SDCC_BSP_H
#define SDCC_BSP_H
 /**********************************************************************
 * sdcc_bsp.h
 *
 * SDCC(Secure Digital Card Controller) driver BSP.
 *
 * This file external data structures, definitions for SDCC BSP used
 * exclusively by the SDCC driver.
 *
 * Copyright (c) 2008-2015
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************

                             Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/bsp/sdcc_bsp.h#1 $
$DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who    what, where, why
----------   ---    --------------------------------------------------------------  
2015-01-28   sb     Fix Klocwork errors
2014-05-16   sb     Added API to initialize busywait for tools
2014-03-11   bn     Added support for slot access (internal or external) config
2012-01-20   rh     Add field for 8 bit v.s. 4 bit memory type
2011-10-20   rh     Code cleanup
2010-06-10   vin    Added sdcc_bsp_get_max_supported_clk 
2010-06-01   vj     Added support for usage of DAL based Interrupt Controller
2010-04-16   rcc    Change an unused definition to reflect that a slot is embedded
2010-03-26   vj     Renamed device to device_id in sdcc_bsp_dmov_properties
2010-02-23   vin    Added sdcc_bsp_get_dmov_chan  
2009-11-19   rcc    Removed all WLAN-specific configuration
2009-10-28   vj     Added API to get the DM properties at Boot
2009-10-07   ah     Added code for WLAN VN699 keypad
2009-07-06   rh     Fix an error in the extraction tag
2009-06-25   rh     Small modification to SDCC_BSP_CPU_SPEED for init call
2009-06-18   rh     Added API to set the VREG level
2009-05-14   vj     Added API and enum for cache operations
2009-05-14   vj     Added API to get cache alignment size
2009-05-15   rh     Adding getting the interrupt number for WM builds
2009-04-06   vin    Added API for getting default SDIO slot
2009-03-17   sr     Added Critical Section for 
                    VREG_GP6 dynamic voting mechanism.
2009-03-16   vj     Added API and enum for changing CPU Speeds
2009-03-02   sr     Hardware Detection Support.
2009-02-18   rh     Remove Alternate-GPIO usage
2009-02-09   vj     OS Abstraction Layer
2009-01-22   rh     Adding an API to control GPIO interface
2008-12-05   sr     HAL Adaptation for WM6.
2008-10-23   vin    Support for harware card detection
2008-06-16   umr    Initial version.
-----------------------------------------------------------------------------*/

/*****************************************************************************/
/*                    INCLUDES                                               */
/*****************************************************************************/
#include SDCC_OSAL_H
#include "sdcc_api.h"

/*SDCC_EXTRACT_COMMON_BEGIN*/
/*****************************************************************************/
/*              DEFINITIONS AND CONSTANTS                                    */
/*****************************************************************************/
#ifndef SDCC_MAX_NUM_CONTROLLER 
#define SDCC_MAX_NUM_CONTROLLER   3
#endif

/* Slot type definitions for different slots on existing on the board in use */
#define SDCC_BSP_NO_SLOT             (0)       /* No physical slot exists. Like a DNI slot */
#define SDCC_BSP_EMBEDDED_MEM_SLOT   (1 << 0)  /* Embedded Memory Slot (eMMC, buried SD) */
#define SDCC_BSP_OPEN_SLOT           (1 << 1)  /* Open Slot where any kind of card can be inserted */
#define SDCC_BSP_WLAN_SLOT           (1 << 2)  /* Exclusive WLAN Slot */
#define SDCC_BSP_UBM_SLOT            (1 << 3)  /* Exclusive UBM slot*/
#define SDCC_BSP_CMMB_SLOT           (1 << 4)  /* Exclusive CMMB slot*/
#define SDCC_BSP_8BIT_SLOT           (1 << 5)  /* Slot is 8 bit in width */

typedef uint32 sdcc_bsp_slot_type;

/* bit-wise definitions for slot type external or internal */
#define SDCC_BSP_SLOT_INVALID        (0)       /* Invalid slot configuration */
#define SDCC_BSP_SLOT_INTERNAL       (1 << 0)  /* Internal Memory Slot */
#define SDCC_BSP_SLOT_EXTERNAL       (1 << 1)  /* External Memory Slot */

typedef uint32 sdcc_bsp_slot_access_type;

/* SDCC BSP Error Types */
typedef enum
{
   SDCC_BSP_NO_ERROR,
   SDCC_BSP_ERR_UNKNOWN,
   SDCC_BSP_ERR_INVALID_PARAM,
   SDCC_BSP_ERR_VOTE_REQUEST_FAILED,
   SDCC_BSP_ERR_FEATURE_UNSUPPORTED
} sdcc_bsp_err_type;

/* Vdd control options for sdcc */
#define SDCC_BSP_VDD_OFF       0
#define SDCC_BSP_VDD_ON        1

/* GPIO control options for sdcc */
#define SDCC_BSP_GPIO_OFF      0
#define SDCC_BSP_GPIO_ON       1

/* Clock control operation */
#define SDCC_BSP_CLK_CONFIG_HCLK   (1 << 0)
#define SDCC_BSP_CLK_CONFIG_RESET  (1 << 1)
#define SDCC_BSP_CLK_CONFIG_MCLK   (1 << 2)

/* Cache control operation */
#define SDCC_BSP_CACHE_INVAL      1
#define SDCC_BSP_CACHE_CLEAN      2
#define SDCC_BSP_MEM_BARRIER      4

#define SDCC_BSP_SLOT_0    0
#define SDCC_BSP_SLOT_1    1
#define SDCC_BSP_SLOT_2    2
#define SDCC_BSP_SLOT_3    3

/*****************************************************************************/
/*                           PROTOTYPES                                      */
/*****************************************************************************/
/*=============================================================================
 * DESCRIPTION   This function determines and returns the slot type that exists
 *               on the board which is use.
 *
 * PARAMETERS    [IN]: slot number
 *
 * RETURN VALUE  sdcc_bsp_slot_type
 *===========================================================================*/
sdcc_bsp_slot_type
sdcc_bsp_get_slot_type (uint32 slot_num);

/*=============================================================================
 * DESCRIPTION   This function determines and returns the slot type whether it 
 *               is external or internal memory that exists on the board.
 *
 * PARAMETERS    [IN]: slot number
 *
 * RETURN VALUE  sdcc_bsp_slot_access_type
 *===========================================================================*/
sdcc_bsp_slot_access_type
sdcc_bsp_get_slot_access_type (uint32 slot_num);

/*=============================================================================
 * DESCRIPTION   This function turns ON/OFF the Vdd source to the card in
 *               use.
 *
 * PARAMETERS    [IN]: slot number
 *               [IN]: state of the VREG setting for the slot
 *
 * RETURN VALUE  sdcc_bsp_err_type.
 *===========================================================================*/
sdcc_bsp_err_type
sdcc_bsp_vdd_control (uint32 slot_num, uint32 state);

/*=============================================================================
 * DESCRIPTION   This function configures any settings like voltage
 *               settings, current settings that may be required to setup
 *               depending on the power management chip that is in use.
 *
 * PARAMETERS    [IN]: slot number
 *
 * RETURN VALUE  sdcc_bsp_err_type.
 *===========================================================================*/
sdcc_bsp_err_type
sdcc_bsp_vdd_configure (uint32 slot_num);

/*=============================================================================
 * DESCRIPTION   This function enables/disables the GPIO interface to the 
 *               card slot.
 *
 * PARAMETERS    [IN]: slot number
 *               [IN]: state of the GPIO interface for the slot
 *
 * RETURN VALUE  sdcc_bsp_err_type.
 *===========================================================================*/
sdcc_bsp_err_type
sdcc_bsp_gpio_control (uint32 slot_num, uint32 state);

/******************************************************************************
* DESCRIPTION   SDCC would call this function to determine the 
*               default slot for a given application.
*    
* PARAMETERS    [IN]: slot type indicating the intended application
*
* RETURN VALUE  slot number
******************************************************************************/
uint32 sdcc_bsp_get_default_slot(sdcc_bsp_slot_type app);

/******************************************************************************
* DESCRIPTION   Performs cache operations on the provided memory region.
*               Used for DMOV related operations only.
*               Clean operation is done for writes to basically push
*               data in the cache to the physical memory
*               Invalidate operation is done for reads to invalidate
*               a memory region in the cache
*    
* PARAMETERS    [IN]: cache operation type
*               [IN]: start address of the memory region
*               [IN]: length of the memory region
*               [IN]: memory barrier requirement
******************************************************************************/
void sdcc_bsp_cache_operation (uint32 op, void *addr, uint32 length);

/******************************************************************************
* DESCRIPTION   Function to get the cache alignment size.
*    
* RETURN VALUE  cache alignment size
******************************************************************************/
uint32 sdcc_bsp_get_cache_align_size(void);

/*=============================================================================
* DESCRIPTION   Set the VREG voltage level
*
* PARAMETERS    [IN]: Voltage to set the VREG to
*===========================================================================*/
void sdcc_bsp_set_vreg_level(uint32 voltageSelection);

/*=============================================================================
* DESCRIPTION   Get the maximum support frequency for the specific slot
*
* PARAMETERS    [IN]: Driver Number
*
* RETURN VALUE  Clock speed in kHz
*===========================================================================*/
uint32 
sdcc_bsp_get_max_supported_clk (uint32 driveno);

/*=============================================================================
* DESCRIPTION   Set the VREG voltage level
*
* PARAMETERS    [IN]: Driver Number
*               [IN]: Clock speed in kHz
*               [IN]: Clock configuration Mode
*===========================================================================*/
void sdcc_bsp_config_clock (uint32 driveno, uint32 *speed, uint32 mode);

/*=============================================================================
* DESCRIPTION   Provides initialization for a busy wait
*
* PARAMETERS    None
*
* RETURN VALUE  None
*===========================================================================*/
void sdcc_bsp_hw_busy_wait_init(void);

/*=============================================================================
* DESCRIPTION   Wait for defined number of uS
*
* PARAMETERS    [IN]: Number of uS to wait
*===========================================================================*/
extern void sdcc_bsp_hw_clk_busy_wait(uint32 us);

#endif /* #ifndef SDCC_BSP_H */

