/*==============================================================================

FILE:      BIMC_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/icb/src/9x07/BIMC_error_data.c#1 $ 
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2015/10/15  ddk    Enabled BIMC error fatal.
2015/08/25  rc     Port to the MDM9x07 without error fatal
2014/06/06  tb     Created
 
        Copyright (c) 2015 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "BIMC_error.h"
#include "BIMC_error_HWIO.h"

/*============================================================================
                           TARGET SPECIFIC DATA
============================================================================*/

#define BIMC_INTERRUPT_VECTOR 211

#define BIMC_GLOBAL2_BASE_ADDR BIMC_GLOBAL2_REG_BASE

#define BIMC_DEFAULT_SWAY_BASE_ADDR BIMC_S_DEFAULT_SWAY_REG_BASE

#define BIMC_DDR0_SCMO_BASE_ADDR BIMC_S_DDR0_SCMO_REG_BASE

/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/* NOC Configuration Data*/
const BIMC_BRIC_info_type BRIC_info[] = 
{
//  {name, base_addr, turn_on_intr, err_fatal, intr_status, intr_clear, intr_enable}
    {"BRIC INTERRUPT 1", BIMC_GLOBAL2_REG_BASE, TRUE, TRUE, 0x110, 0x118, 0x11c }
};

const BIMC_DEFAULT_info_type DEFAULT_info[] =
{
//  {name, base_addr, turn_on_intr, err_fatal, intr_status, intr_clear, intr_enable, intr_status0, intr_status1, intr_status2}
    {"DEFAULT SWAY INTERRUPT", BIMC_S_DEFAULT_SWAY_REG_BASE, TRUE, TRUE, 0x100, 0x108, 0x10c, 0x400, 0x410, 0x420 }
};

const BIMC_DDR_SCMO_info_type DDR_SCMO_info[] =
{
//  {name, base_addr, turn_on_intr, err_fatal, intr_status, intr_clear, intr_enable, intr_addr, intr_addr1, intr_packet0, intr_packet1, intr_packet2}
    {"DDR0 SCMO INTERRUPT", BIMC_S_DDR0_SCMO_REG_BASE, TRUE, TRUE, 0x100, 0x108, 0x10c, 0x120, REG_NOT_AVAIL, 0x128, 0x12c, 0x130 }
};

const BIMC_propdata_type BIMC_propdata =
{
    BIMC_INTERRUPT_VECTOR,
    sizeof(BRIC_info)/sizeof(BIMC_BRIC_info_type), 
    BRIC_info,
    sizeof(DEFAULT_info)/sizeof(BIMC_DEFAULT_info_type), 
    DEFAULT_info,
    sizeof(DDR_SCMO_info)/sizeof(BIMC_DDR_SCMO_info_type), 
    DDR_SCMO_info,
};
