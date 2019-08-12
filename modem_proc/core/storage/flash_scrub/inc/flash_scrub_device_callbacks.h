/***********************************************************************
 * flash_scrub_device_callbacks.h
 *
 * API definition for flash scrub tests.
 * Copyright (C) 2014 QUALCOMM Technologies, Inc.
 *
 * This file defines certain APIs used by the test framework to verify
 * the flash scrubbing functionality.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/inc/flash_scrub_device_callbacks.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-01-07   wek   Create

===========================================================================*/
#ifndef __FLASH_SCRUB_DEVICE_CALLBACKS_H__
#define __FLASH_SCRUB_DEVICE_CALLBACKS_H__

#include "DALSysTypes.h"


struct flash_scrub_callback_args
{
  uint32 location; /* Page or block for the operation (read/write/erase) */
  int result;      /* If the callback is called after the operation, result
                    * is the return value of the flash operation (read/write/
                    * erase). 0 means success. Errors defined in flash.h */
};

/* Functions to be called by DalTF to stop scrub at specific points to mimic
 * sudden power downs. */
typedef int (*FLASH_SCRUB_TEST_CALLBACK_FUNC) (
  const char *partition_name, struct flash_scrub_callback_args *args);

void flash_scrub_register_write_callback (
  FLASH_SCRUB_TEST_CALLBACK_FUNC callback);
void flash_scrub_register_read_callback (
  FLASH_SCRUB_TEST_CALLBACK_FUNC callback);
void flash_scrub_register_erase_callback (
  FLASH_SCRUB_TEST_CALLBACK_FUNC callback);

#endif /* not __FLASH_SCRUB_DEVICE_CALLBACKS_H__ */
