/***********************************************************************
 * tftp_os_wakelocks.h
 *
 * Short description
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * Verbose description.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/tftp/os/inc/tftp_os_wakelocks.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-08-19   nr   Create

===========================================================================*/

#ifndef __TFTP_OS_WAKELOCKS_H__
#define __TFTP_OS_WAKELOCKS_H__

#include "tftp_comdef.h"

void tftp_os_wakelocks_init(void);

void tftp_os_wakelock(void);

void tftp_os_wakeunlock(void);

#endif /* not __TFTP_OS_WAKELOCKS_H__ */
