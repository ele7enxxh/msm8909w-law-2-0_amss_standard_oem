/*
===========================================================================

FILE:         HAL_rbcpr_bsp.c

DESCRIPTION:
  Per target rbcpr bsp configurations

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/rbcpr/hal/src/target/8916/HAL_rbcpr_bsp.c#1 $
$Date: 2015/02/25 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
              QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "msmhwio.h"
#include "HAL_rbcpr.h"
#include "HAL_rbcpr_hwio.h"
#include "HAL_rbcpr_qfprom.h"

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

// TODO: CPR0_REG_RED_SEL is QFPROM_RAW_SW_CALIB_ROW0_MSB
const HAL_rbcpr_bsp_type HAL_rbcpr_bsp_data =
{
    .rails = (const HAL_rbcpr_rail_s[])
    {
        {   // VDDCX
            .timer_config =
            {
                .interval = 5,          // time in msecs
                .enable   = TRUE,
            },
            .sensors      = 64,
            .efuse_info   =
            {



 
 
                .target_voltages =                
                {
                    { 
				  .fuse_addresses = (unsigned[])	
					{
	   	            HWIO_ADDR(QFPROM_RAW_CALIB_ROW3_LSB),
	   	            HWIO_ADDR(QFPROM_RAW_CALIB_ROW3_MSB),
					},
					.fuse_index = 1,    // MSB
                        .offset     = 24,
                },

                    { 
			.fuse_addresses = (unsigned[])
	   		          {
	   	           HWIO_ADDR(QFPROM_RAW_CALIB_ROW2_LSB),
                           HWIO_ADDR(QFPROM_RAW_CALIB_ROW2_MSB),
	   		          },
                    .fuse_index = 0,    // LSB
                    .offset     = 0,
                },
		    //turbo    
                { 
				.fuse_addresses = (unsigned[])
	   		       {
		           HWIO_ADDR(QFPROM_RAW_CALIB_ROW1_LSB),
		           HWIO_ADDR(QFPROM_RAW_CALIB_ROW1_MSB),
	   		       },
                    .fuse_index = 1,    // MSB
                        .offset     = 27,
                },
		}
            },
            .efuse_info_rd =
            {

                .target_voltages =                
                {
                    { 
                  .fuse_addresses = (unsigned[])
					{
	   	            HWIO_ADDR(QFPROM_RAW_CALIB_ROW3_LSB),
	   	            HWIO_ADDR(QFPROM_RAW_CALIB_ROW3_MSB),
	   		        },
					.fuse_index = 1,    // MSB
                        .offset     = 24,
                },

                {
				.fuse_addresses = (unsigned[])
	   		      {
	   	           HWIO_ADDR(QFPROM_RAW_CALIB_ROW2_LSB),
                           HWIO_ADDR(QFPROM_RAW_CALIB_ROW2_MSB),
	   		       },
                    .fuse_index = 0,    // LSB
                    .offset     = 0,
                },
		    //turbo    
                {
				 .fuse_addresses = (unsigned[])
	   		         {
		           HWIO_ADDR(QFPROM_RAW_CALIB_ROW1_LSB),
		           HWIO_ADDR(QFPROM_RAW_CALIB_ROW1_MSB),
	   		         },
				  .fuse_index = 1,    // MSB
                        .offset     = 27,
                    },
                },
            },
            .settings =
            {
                .floor          = 0x00,
                .ceiling        = 0x3F,
                .vlevel         = 0x20,
            },
            .name = "vddcx",
            .hw_base_address = RBCPR_WRAPPER_REG_BASE,
        },
    },
    .num_rails = 1,
};

