#ifndef FLASH_PROFILE_LA_H
#define FLASH_PROFILE_LA_H
/*====================================================================
 *
 * FILE: flash_profile_la.h
 *
 * SERVICES: Functions for LA based profiling
 *
 * DESCRIPTION: This file contain initialization and interface APIs
 *              of flash config data, which is used across all NOR
 *              devices
 *
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/dal/flash_profile_la.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 01/03/12     sv     Add Flash profile LA Macros and fn prototypes
 * 02/02/11     bb     Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_profile_common.h"

#ifdef FEATURE_FPGA_DAL
  #include "DDIFPGA.h"
  #include "DALSys.h"
  #include "DALDeviceId.h"
  #include "DALStdErr.h"
#else
  #include "msmhwioreg_port.h"
#endif

#ifndef FEATURE_FPGA_DAL
  #define FLASH_LA_PORT  HWIO_HAPPY_LED_ADDR
#endif

/* We use the Happy LED port which allows a max of 16 markers */
#define FLASH_LA_MAX_MARKERS    16

#define FLASH_LA_MARKER_INIT(x)                     \
  *flash_la_port_ptr = 0;

#define FLASH_LA_MARKER_LOW(x)                      \
  if (flash_la_flag && (x < FLASH_LA_MAX_MARKERS))  \
  {                                                 \
    *flash_la_port_ptr &= ~(1<<x);                  \
  }

#define FLASH_LA_MARKER_HIGH(x)                     \
  if (flash_la_flag && (x < FLASH_LA_MAX_MARKERS))  \
  {                                                 \
    *flash_la_port_ptr |= (1<<x);                   \
  }

/*Initialises the data structures*/
void flash_la_init(void);

/* Marks the start of the logical Analyser profiling */
void flash_la_start(uint8 id, uint32 npages);

/* Marks the end of the logical Analyser profiling */
void flash_la_end(uint8 id);

/* populates and stores the page size and other info */
void flash_la_set_info (flash_client_ctxt *client_ctxt );

/* Sync locking for multipage support */
void flash_la_lock(void);

/* Sync unlocking for multipage support */
void flash_la_unlock(void);

#endif /* FLASH_PROFILE_LA_H */
