#ifndef FLASH_CLOCKS_H
#define FLASH_CLOCKS_H

/**********************************************************************
 * flash_clocks.h
 *
 * SERVICES:    Header file for flash operation clocks used in NAND/SPI NOR
 *
 * DESCRIPTION: Header file for flash operation clocks used in NAND/SPI NOR
 * 
 * Copyright (c) 2013,2016 QUALCOMM Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/dal/flash_clocks.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
06/27/16     sb      Cleanup 
05/23/13     eo      Initial Revision
===========================================================================*/
#include "comdef.h"

/* Flash Clocks Init */
void flash_clocks_init(void);

/* Flash Clocks Deinit */
void flash_clocks_deinit(void);    

/* Flash Clocks Request */
void flash_clocks_request(void);

/* Flash Clocks Request Complete */
void flash_clocks_request_complete(void);

#endif /* FLASH_CLOCKS_H */
