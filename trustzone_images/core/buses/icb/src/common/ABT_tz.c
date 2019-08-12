/*==============================================================================

FILE:      ABT_tz.c

DESCRIPTION: This file implements TZ platform specific ABT driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/icb/src/common/ABT_tz.c#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2013/08/30  pm     Split out ABT_Register_Interrupt() from ABT_Init()
2013/05/26  pm     Per JL (of TZ), route all fatal FIQs to all cores 
2012/10/03  av     Added NOC Error Handler
2012/05/31  av     Created

        Copyright (c) 2013, 2015 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

#include "ABTimeout.h"
#include "ABT_internal.h"
#include "ABT_platform.h"
//#include "ClockPIL.h"
#include "ClockTZ.h"
#include "tzbsp_isr.h"
#include "tzbsp_log.h"
#include "tzbsp.h"
#include "NOC_error.h"
#include "BIMC_error.h"

/*============================================================================
                          DEFINEs/MACROs
============================================================================*/


/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

extern ABT_propdata_type ABT_propdata;

/*============================================================================
                                 FUNCTIONS
============================================================================*/

//*============================================================================*/
/**
@brief
      Performs platform specific initialization for AHB Timeout driver.
      It enables clock/interrupts required for AHB timeout slave.

@param[in]  *PropData Pointer to property data structure.

@return
      ABT_error_type indicating success or error.

@dependencies
      None.

@sideeffects
      None.
*/
/*============================================================================*/
ABT_error_type ABT_Platform_Init( ABT_propdata_type * PropData )
{
    // Enable ABT slave clocks
    //Clock_InitBusTimeout();
    Clock_EnableClockGroup(CLOCK_GROUP_ABT);

    // Initialize NOC Error Handler
    NOC_Error_Init();

    // Initialize BIMC Error Handler
    BIMC_Error_Init();

    return ABT_SUCCESS;
}

//*============================================================================*/
/**
@brief 
      Performs platform-specific interrupt registration for AHB Timeout driver.
 
@param[in]  *PropData Pointer to property data structure.

@return    
      ABT_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
ABT_error_type ABT_Platform_Register_Interrupt( ABT_propdata_type * PropData )
{
    // TZ register FIQ handler
    if(0 != tzbsp_register_isr(PropData->platform_info->intr_vector,
                               "ABT",
                               ABT_Handle_Interrupt,
                               NULL,
                               TZBSP_INTF_TRIGGER_LEVEL | TZBSP_INTF_ALL_CPUS,
                               TRUE))
    {
        TZBSP_LOG_ERR("ABT init error !");
        tzbsp_err_fatal(TZBSP_ERR_FATAL_AHB_TIMEOUT);
        return ABT_ERROR_INIT_FAILURE;
    }

    return ABT_SUCCESS;
}

//*============================================================================*/
/**
@brief
      Logs syndrome register infomration using platform specific APIs.

@param[in]  *syndrome Pointer to syndrome info structure

@return
      ABT_error_type indicating success or error.

@dependencies
      None.

@sideeffects
      None.
*/
/*============================================================================*/
ABT_error_type ABT_Platform_Log_Syndrome(ABT_syndrome_info_type *syndrome)
{
    TZBSP_LOG_ERR("ABT %s ID: 0x%08x",
                  syndrome->slave_name, syndrome->id);
    TZBSP_LOG_ERR("ABT %s ADDR0: 0x%08x",
                  syndrome->slave_name, syndrome->addr0);
    TZBSP_LOG_ERR("ABT %s ADDR1: 0x%08x",
                  syndrome->slave_name, syndrome->addr1);
    TZBSP_LOG_ERR("ABT %s HREADY: 0x%08x",
                  syndrome->slave_name, syndrome->hready);
    TZBSP_LOG_ERR("ABT %s Slaves: %d",
                  syndrome->slave_name, syndrome->num_slaves);

    return ABT_SUCCESS;
}

//*============================================================================*/
/**
@brief
      Logs syndrome register infomration using platform specific APIs.

@param[in]  hung_slaves Number of hung slaves.

@return
      ABT_error_type indicating success or error.

@dependencies
      None.

@sideeffects
      None.
*/
/*============================================================================*/
ABT_error_type ABT_Platform_Report_Hang(uint32 hung_slaves)
{
    tzbsp_err_fatal(TZBSP_ERR_FATAL_AHB_TIMEOUT);
    return ABT_SUCCESS;
}

//*============================================================================*/
/**
@brief
      Gets platfrom specific property data structure for configuration.

@param[in]  None.

@return
      ABT_propdata_type* Pointer to property data structure.

@dependencies
      None.

@sideeffects
      None.
*/
/*============================================================================*/
ABT_propdata_type* ABT_Platform_Get_Propdata(void)
{
    return &ABT_propdata;
}


