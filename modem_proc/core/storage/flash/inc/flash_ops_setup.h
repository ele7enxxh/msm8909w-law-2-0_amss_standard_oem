#ifndef FLASH_OPS_SETUP_H
#define FLASH_OPS_SETUP_H

/**********************************************************************
 * flash_ops_setup.h
 *
 * SERVICES:    Header file for flash operation setup used by EFS
 *
 * DESCRIPTION: Header file for flash operation setup used by EFS
 * 
 * Copyright (c) 2011-2013 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/inc/flash_ops_setup.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
05/03/13     eo      Add flash ops mechanism to enable/disable QPIC clocks
04/02/12     sv      Optimize flash ops mechanism 
03/08/11     sv      Initial Revision
===========================================================================*/
#include "comdef.h"

/* Flash Operation Init */
void flash_ops_init(void);

/* Flash Operations Deinit */
void flash_ops_deinit(void);    

/* Flash Operations start */
void flash_ops_start(void);

/* Flash Operations begin operation */
void flash_ops_begin_op(void);

/* Flash Operations end*/
void flash_ops_end(void);

/* Flash Operations get flash client context */
void flash_ops_get_handle(uint32 *client_ctxt_data);

#endif /* FLASH_OPS_SETUP_H */
