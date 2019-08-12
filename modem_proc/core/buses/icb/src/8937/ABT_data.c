/*==============================================================================

FILE:      ABT_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/8937/ABT_data.c#1 $ 
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $
$Change: 11985146 $ 

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2014/03/14  ddk    Added support for additional clock voting.
2014/03/10  ddk    Added support for hwio mapping.
2013/11/14  tb     Added support for multiple enable/status registers
2013/11/14  pm     Port to MSM8916
2013/04/16  pm     Added interrupt priority
2012/10/04  av     Support for disabling ABT 
2012/05/31  av     Created
 
        Copyright (c) 2013-2014 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "ABTimeout.h"
#include "ABTimeout_HWIO.h"

/*============================================================================
                      TARGET AND PLATFORM SPECIFIC DATA
============================================================================*/

/* Base address for devices */
#define ABT_MSSCFG_BASE_ADDR  MSS_CONF_BUS_TIMEOUT_REG_BASE


/* Bit Mask for ABT Slaves */
#define ABT_MSSCFG_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_MSS_CONFIG_TIMEOUT_IRQ_SHFT


/* ABT Slave CLK Name */
#define ABT_MSSCFG_CLK    "clk_bus_slave_timeout"


/* Timeout Interrupt Register Address */
#define ABT_TIMEOUT_INTR_MODEM_ENABLE  HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ADDR
#define ABT_TIMEOUT_INTR_STATUS        HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR
#define ABT_TIMEOUT_SLAVE_GLB_EN       HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR

/* TCSR Summary Interrupt Vectors */
#define ABT_TCSR_MODEM_INTR_VECTOR     238

/* Modem Interrupt Priority (Note:  This varies with the processor, as DAL    */
/*                                  passes this through to the underlying     */
/*                                  kernel/OS                                 */
#define ABT_MODEM_INTR_PRIORITY     1


/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/*---------------------------------------------------------------------------*/
/*          Properties data for device ID  = "/dev/ABTimeout"                */
/*---------------------------------------------------------------------------*/

/* ABT HWIO map array */
ABT_platform_hwio_info_type hwio_map_info[] =
{
  {"CORE_TOP_CSR" , 0x0,},
  {"MSS_TOP" , 0x0,},
};

/* ABT Clock Data */
char * clk_name_array_mss_cfg[]=
{
   ABT_MSSCFG_CLK,
};

/* ABT Configuration Data*/
ABT_slave_info_type ABT_cfgdata[] = 
{ 
//ABT_SLAVE_INFO(  name, clk_name_array, sl_en, int_en, to_val)
  ABT_SLAVE_INFO(MSSCFG, 
      clk_name_array_mss_cfg, 
      sizeof(clk_name_array_mss_cfg)/sizeof(clk_name_array_mss_cfg[0]), 
      TRUE,   
      TRUE,   
      0xFF),//  MODEM_CONFIG
};

/* ABT interrupt enable array */
void *intrEnable[] =
{
  (void*)ABT_TIMEOUT_INTR_MODEM_ENABLE,
};

/* ABT interrupt status array */
void *intrStatus[] =
{
  (void*)ABT_TIMEOUT_INTR_STATUS,
};

/* ABT Platform Data type */
ABT_platform_info_type ABT_platform_info =
{
    "MSS",                                    /* Image name */
    intrEnable,                               /* INTR Enable address */
    intrStatus,                               /* INTR Status Register address */
    (void*)ABT_TIMEOUT_SLAVE_GLB_EN,          /* ABT Slave global en address */
    ABT_TCSR_MODEM_INTR_VECTOR,               /* ABT TCSR Summary interrupt vector */
    ABT_MODEM_INTR_PRIORITY,                  /* ABT Modem Interrupt priority */
    sizeof(intrEnable)/sizeof(intrEnable[0]), /* Number of interrupt enable/status registers */
    hwio_map_info,                            /* HWIO map info array  */
    sizeof(hwio_map_info)/sizeof(hwio_map_info[0]), /* Number of hwio region mappings */    
};


/* ABT Configuration Property Data*/
ABT_propdata_type ABT_propdata = 
{
    /* Length of the config  data array */
    sizeof(ABT_cfgdata)/sizeof(ABT_slave_info_type), 
    /* Pointer to config data array */ 
    ABT_cfgdata,
    /* Pointer to platform info data */ 
    &ABT_platform_info                                    
};

