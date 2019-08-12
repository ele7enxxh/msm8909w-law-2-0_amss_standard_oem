/*==============================================================================

FILE:      NOC_error_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/icb/src/8909/NOC_error_data.c#1 $ 
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2013/10/30  pm     Port to 8916
2012/10/03  av     Created
 
        Copyright (c) 2013 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "NOC_error.h"
#include "NOC_error_HWIO.h"

/*============================================================================
                           TARGET SPECIFIC DATA
============================================================================*/

/* Base address for devices */
#define NOCERR_PCNOC_BASE_ADDR PC_NOC_REG_BASE 
#define NOCERR_SNOC_BASE_ADDR  SYSTEM_NOC_REG_BASE

/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/* NOC Configuration Data*/
NOCERR_info_type NOCERR_cfgdata[] = 
{ 
//NOC_ERR_INFO(name, intr_vector, intr_en, log_filter)
  NOCERR_INFO(PCNOC, 226, TRUE, NOC_ERRLOG2 | NOC_ERRLOG6),/* Peripheral/Config NOC */
  NOCERR_INFO(SNOC,  225, TRUE, NOC_ERRLOG2 | NOC_ERRLOG6),/* System NOC */
};

char *BIMC_reg_names[] = 
{
 "BIMC_S_DDR0_SCMO_INTERRUPT_STATUS",
 "BIMC_S_DDR0_SCMO_ESYN_ADDR",
 "BIMC_S_DDR0_SCMO_ESYN_APACKET_0",
 "BIMC_S_DDR0_SCMO_ESYN_APACKET_1",
 "BIMC_S_DDR0_SCMO_ESYN_APACKET_2",
 "BIMC_S_DEFAULT_SWAY_INTERRUPT_STATUS",
 "BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_0",
 "BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_1",
 "BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_2",
 "BIMC_BRIC_INTERRUPT_STATUS_0",
 "BIMC_BRIC_INTERRUPT_STATUS_1",
 "BIMC_BRIC_INTERRUPT_STATUS_2",
 "BIMC_BRIC_INTERRUPT_STATUS_3",
};

uint32 BIMC_reg_addr[] = 
{
 HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_ADDR,
 HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_ADDR, 
 HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ADDR,
 HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ADDR, 
 HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_ADDR, 
 HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_STATUS_ADDR, 
 HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_0_ADDR, 
 HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_1_ADDR, 
 HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_2_ADDR, 
 HWIO_BIMC_BRIC_INTERRUPT_STATUS_0_ADDR, 
 HWIO_BIMC_BRIC_INTERRUPT_STATUS_1_ADDR, 
 HWIO_BIMC_BRIC_INTERRUPT_STATUS_2_ADDR, 
 HWIO_BIMC_BRIC_INTERRUPT_STATUS_3_ADDR,
};

NOCERR_propdata_type NOCERR_propdata =
{
    /* Length of the config data array */
    sizeof(NOCERR_cfgdata)/sizeof(NOCERR_info_type), 
    /* Pointer to config data array */ 
    NOCERR_cfgdata,
    BIMC_reg_names,
    BIMC_reg_addr,
    sizeof(BIMC_reg_addr)/sizeof(uint32),
};
