#ifndef __COEX_NV_H__
#define __COEX_NV_H__
/*!
  @file
  coex_nv.h

  @brief
  APIs to manage for coex's NV memory

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/inc/coex_nv.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
04/23/13   tak     Implement range intersection based conflict definition
03/01/13   tak     Remove dependency on LL1 messages and structures
01/10/13   tak     Added new protocol for WCI-2 
09/06/12   cab     Add new protocol version for tx/rx/wcn prio
07/18/12   cab     Updated NV versioning and protocols
05/16/12   cab     Initial version

==========================================================================*/

/*==========================================================================

                             INCLUDE FILES

============================================================================*/
#include <IxErrno.h>
#include "fs_sys_types.h"
#include "fs_public.h"

#include <wci2_uart.h>
#include "cxm_intf_types.h"
#include "cxm.h"
#include "coex_algos.h"

#define CXM_EFS_CONF_FILE_PATH     "/nv/item_files/conf/mcs_cxm_coex.conf"
#define COEX_CONFIG_DATA_FILE_DIR  "/nv/item_files/mcs/cxm"
#define COEX_CONFIG_DATA_FILE_PATH "/nv/item_files/mcs/cxm/coex_config_data"

#define COEX_MAX_EFS_BYTES   20
#define COEX_EFS_NAME_LEN    256


typedef struct
{
  char fname[COEX_EFS_NAME_LEN];     /* to store file name */
  int  fd;                           /* file descriptor to read */
  int return_size;                   /* for error checking */
  struct fs_stat fstat;              /* for checking error conditions */
  uint8 buffer[COEX_MAX_EFS_BYTES];  /* memory area to read the efs item */                   
} coex_efs_type;

/*=============================================================================

  FUNCTION:  coex_nv_init

=============================================================================*/
/*!
    @brief
    This method maintains the internal mapping between the indication method
    id and offset of the message in the message map.

    @return
    int32 offset
*/
/*===========================================================================*/
void coex_nv_init (
  coex_config_params_v8 *coex_nv
);

#endif /* __COEX_NV_H__ */

