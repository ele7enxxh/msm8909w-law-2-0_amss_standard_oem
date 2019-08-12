#ifndef FLASHI_HAL_CONFIG_H
#define FLASHI_HAL_CONFIG_H

/*=======================================================================
 * FILE:        flashi_hal_config.h
 *
 * SERVICES:    None
 *
 * DESCRIPTION: Flash HAL configurable parameters.
 *
 * Copyright (c) 2008-2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/hal/flashi_hal_config.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 05/20/11     sb      To read UUID on 8bit ECC NAND
 * 05/20/11     bb      Flash hal Optimization
 * 10/21/10     sv      Use flash_hal_busywait instead of simulated busywait
 * 09/25/09     mh      Increase FLASH_HAL_NANDC_MAX_BUF for dual nand ctlr
 * 05/29/09     mh      Increase FLASH_HAL_DEVICE_MAX for 2 hal devices/client
 * 04/01/09     mh      Update FLASH_HAL_NANDC_MAX_BUF to 16 for 4K page
 * 03/24/09     bb      Removed external dependency on clk_pause
 * 03/12/09     eo      Update max address cycles to 8
 * 09/03/08     bb      Initial Revision
 *==================================================================*/


/*-----------------------------------------------------------------------
 *           Definitions and Constants
 *----------------------------------------------------------------------*/

/* Log Mechanism */
#ifdef FLASH_LOG_H
  #include FLASH_LOG_H
#else
  #define FLASHLOG(l, x)
#endif

/* Maximum flash clients supported */
#ifndef FLASH_HAL_DEVICE_MAX
  #define FLASH_HAL_DEVICE_MAX     1
#endif

/* Maximum operation sequences */
#ifndef FLASH_HAL_NAND_OP_SEQ_MAX
  #define FLASH_HAL_NAND_OP_SEQ_MAX     20
#endif

/* Wait API */
#ifndef FLASH_HAL_BUSY_WAIT
  #define FLASH_HAL_BUSY_WAIT(pause_time_us) \
    flash_hal_busywait(pause_time_us)
#endif

/* Wait polling time of 1 usec */
#ifndef FLASH_NAND_POLL_TIME
  #define FLASH_NAND_POLL_TIME     1
#endif

/* Maximum address cycles supported */
#ifndef FLASH_HAL_MAX_ADDR_CYCLES
  #define FLASH_HAL_MAX_ADDR_CYCLES 5
#endif

/* Maximum buffer pointers for NAND */
#ifndef FLASH_HAL_NANDC_MAX_BUF
  #define FLASH_HAL_NANDC_MAX_BUF   20
#endif


#endif  /* FLASHI_HAL_CONFIG_H */
