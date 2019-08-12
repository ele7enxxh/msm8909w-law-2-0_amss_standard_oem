/*==============================================================================

FILE:      NOC_error_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/icb/src/9x07/NOC_error_data.c#1 $ 
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2015/10/15  ddk    Enabled NOC error fatal.
2015/08/25  rc     port to 9x07 without error fatal
2015/06/22  ddk    Dummy changes for 9x07
2013/10/30  pm     Port to 8916
2012/10/03  av     Created
 
        Copyright (c) 2013-2015 Qualcomm Technologies Incorporated.
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

/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/* NOC Configuration Data*/
NOCERR_info_type NOCERR_cfgdata[] = 
{ 
//NOC_ERR_INFO(name, intr_vector, intr_en, log_filter)
  NOCERR_INFO(PCNOC, 226, TRUE, NOC_ERRLOG2 | NOC_ERRLOG6 ),/* Peripheral/Config NOC */
};

const NOCERR_propdata_type NOCERR_propdata =
{
    /* Length of the config data array */
    sizeof(NOCERR_cfgdata)/sizeof(NOCERR_info_type), 
    /* Pointer to config data array */ 
    NOCERR_cfgdata,
    NULL,
    NULL,
    0,
};
