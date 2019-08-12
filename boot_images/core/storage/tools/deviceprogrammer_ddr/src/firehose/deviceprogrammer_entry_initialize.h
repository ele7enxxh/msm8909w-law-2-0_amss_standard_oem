/*==================================================================
 *
 * FILE:        deviceprogrammer_entry_initialize.h
 *
 * DESCRIPTION:
 *   
 *
 *        Copyright © 2008-2014 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *               QUALCOMM Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/deviceprogrammer_ddr/src/firehose/deviceprogrammer_entry_initialize.h#1 $ 
 *   $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * YYYY-MM-DD   who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 2014-08-14   wek     Indicate that deviceprogrammer functions don't return.
 * 2014-10-15   sb      Updated for 8909
 * 2013-06-03   ah      Added legal header
 * 2013-05-31   ab      Initial checkin
 */

#ifndef DEVICEPROGRAMMER_ENTRY_INITIALIZE_H
#define DEVICEPROGRAMMER_ENTRY_INITIALIZE_H

#include BOOT_PBL_H
#include "boot_sbl_if.h"

extern boot_pbl_shared_data_type *pbl_shared_global;

void deviceprogrammer_entry(boot_pbl_shared_data_type *pbl_shared) __attribute__((noreturn));

#endif

