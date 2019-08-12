/*==============================================================================

FILE:      ABT_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/icb/src/8909/ABT_data.c#1 $ 
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
09/11/14     rc    changed the SNOC ABT module macro numbering as per the flat
                   file   
06/11/14     rc    Removed the unsupported PNOC ABT module macros  
05/30/14     ms    Created
 
        Copyright (c) 2014 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "ABTimeout.h"
#include "ABTimeout_HWIO.h"

/*============================================================================
                      TARGET AND PLATFORM SPECIFIC DATA
============================================================================*/

/* Base address for devices */
#define ABT_PCNOC_0_BASE_ADDR   PCNOC_0_BUS_TIMEOUT_REG_BASE 
#define ABT_PCNOC_1_BASE_ADDR   PCNOC_1_BUS_TIMEOUT_REG_BASE 
#define ABT_PCNOC_2_BASE_ADDR   PCNOC_2_BUS_TIMEOUT_REG_BASE 
#define ABT_PCNOC_3_BASE_ADDR   PCNOC_3_BUS_TIMEOUT_REG_BASE 
#define ABT_PCNOC_4_BASE_ADDR   PCNOC_4_BUS_TIMEOUT_REG_BASE 
#define ABT_PCNOC_5_BASE_ADDR   PCNOC_5_BUS_TIMEOUT_REG_BASE 
#define ABT_PCNOC_6_BASE_ADDR   PCNOC_6_BUS_TIMEOUT_REG_BASE 
#define ABT_PCNOC_7_BASE_ADDR   PCNOC_7_BUS_TIMEOUT_REG_BASE 
#define ABT_SNOC_0_BASE_ADDR    SNOC_0_BUS_TIMEOUT_REG_BASE
#define ABT_SNOC_1_BASE_ADDR    SNOC_1_BUS_TIMEOUT_REG_BASE

/* Bit Mask for ABT Slaves */
#define ABT_PCNOC_0_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S0_IRQ_SHFT
#define ABT_PCNOC_1_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S1_IRQ_SHFT
#define ABT_PCNOC_2_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S2_IRQ_SHFT
#define ABT_PCNOC_3_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S3_IRQ_SHFT
#define ABT_PCNOC_4_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S4_IRQ_SHFT
#define ABT_PCNOC_5_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S5_IRQ_SHFT
#define ABT_PCNOC_6_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S6_IRQ_SHFT
#define ABT_PCNOC_7_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S7_IRQ_SHFT
#define ABT_SNOC_0_BMSK    HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_SNOC_S0_IRQ_SHFT
#define ABT_SNOC_1_BMSK    HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_SNOC_S1_IRQ_SHFT

/* ABT Slave CLK Name */
/* NOTE: Clock names are not required on TZ platform, Clock API
   enables all the required clocks during ABT driver initialization */
#define ABT_PCNOC_0_CLK    NULL //"gcc_pcnoc_bus_timeout0_ahb_clk"
#define ABT_PCNOC_1_CLK    NULL //"gcc_pcnoc_bus_timeout1_ahb_clk"
#define ABT_PCNOC_2_CLK    NULL //"gcc_pcnoc_bus_timeout2_ahb_clk"
#define ABT_PCNOC_3_CLK    NULL //"gcc_pcnoc_bus_timeout3_ahb_clk"
#define ABT_PCNOC_4_CLK    NULL //"gcc_pcnoc_bus_timeout4_ahb_clk"
#define ABT_PCNOC_5_CLK    NULL //"gcc_pcnoc_bus_timeout5_ahb_clk"
#define ABT_PCNOC_6_CLK    NULL //"gcc_pcnoc_bus_timeout6_ahb_clk"
#define ABT_PCNOC_7_CLK    NULL //"gcc_pcnoc_bus_timeout7_ahb_clk"
#define ABT_SNOC_0_CLK     NULL //"gcc_snoc_bus_timeout0_ahb_clk"
#define ABT_SNOC_1_CLK     NULL //"gcc_snoc_bus_timeout2_ahb_clk" 

/* Timeout Interrupt Register Address */
#define ABT_TIMEOUT_INTR_KPSS_ENABLE   HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ADDR
#define ABT_TIMEOUT_INTR_STATUS        HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR 
#define ABT_TIMEOUT_SLAVE_GLB_EN       HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR

/* TCSR Summary Interrupt Vectors */
#define ABT_TCSR_KPSS_INTR_VECTOR      229

/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/* ABT Configuration Data*/
ABT_slave_info_type ABT_cfgdata[] = 
{ 
//ABT_SLAVE_INFO(  name, sl_en, int_en, to_val)
  ABT_SLAVE_INFO(PCNOC_0,    TRUE,   TRUE,   0xFF),
  ABT_SLAVE_INFO(PCNOC_1,    TRUE,   TRUE,   0xFF),
  ABT_SLAVE_INFO(PCNOC_2,    TRUE,   TRUE,   0xFF),
  ABT_SLAVE_INFO(PCNOC_3,    TRUE,   TRUE,   0xFF),
  ABT_SLAVE_INFO(PCNOC_4,    TRUE,   TRUE,   0xFF),
  ABT_SLAVE_INFO(PCNOC_5,    TRUE,   TRUE,   0xFF),
  ABT_SLAVE_INFO(PCNOC_6,    TRUE,   TRUE,   0xFF),
  ABT_SLAVE_INFO(PCNOC_7,    TRUE,   TRUE,   0xFF),
  ABT_SLAVE_INFO(SNOC_0,     TRUE,   TRUE,   0xFF),
  ABT_SLAVE_INFO(SNOC_1,     TRUE,   TRUE,   0xFF),
};

/* ABT interrupt enable array */
void *intrEnable[] =
{
  (void*)ABT_TIMEOUT_INTR_KPSS_ENABLE,
};

/* ABT interrupt status array */
void *intrStatus[] =
{
  (void*)ABT_TIMEOUT_INTR_STATUS,
};

/* ABT Platform Data type */
ABT_platform_info_type ABT_platform_info =
{
    "TZ",                                     /* Image name */
    intrEnable,                               /* INTR Enable array */
    intrStatus,                               /* INTR Status Register array */
    (void*)ABT_TIMEOUT_SLAVE_GLB_EN,          /* ABT Slave global en address */
    ABT_TCSR_KPSS_INTR_VECTOR,                /* KPSS Summary Interrupt Vector */
    NULL,                                     /* No interrupt priority needed on TZ */
    sizeof(intrEnable)/sizeof(intrEnable[0]), /* Number of interrupt enable/status registers */
};


/* ABT Configuration Property Data*/
ABT_propdata_type ABT_propdata =  
{
    /* Length of the config data array */
    sizeof(ABT_cfgdata)/sizeof(ABT_slave_info_type), 
    /* Pointer to config data array */ 
    ABT_cfgdata,
    /* Pointer to platform info data */ 
    &ABT_platform_info                                    
};

