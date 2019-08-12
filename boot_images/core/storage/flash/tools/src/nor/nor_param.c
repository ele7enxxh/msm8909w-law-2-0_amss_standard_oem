/*=======================================================================
 * FILE:        nor_param.c
 *
 * SERVICES:    Flash Programming Support via Lauterbach FLASH_PROG ICD
 *
 * DESCRIPTION
 *   This file exist so that we can explicitly place our prog_param
 *   data in memory since ARM doesn't allow this within a file but rather by
 *   file basis.
 *
 * Copyright (c) 2010 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nor/nor_param.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/10/10     sv      Initial version
 *==================================================================*/

#include "nor_param.h"
struct flash_prog_param prog_param;
