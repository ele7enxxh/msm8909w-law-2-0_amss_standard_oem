/***********************************************************************
 * tftp_os_wakelocks_modem.c
 *
 * Modem wakelock abstraction.
 *
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/tftp/os/src/tftp_os_wakelocks_modem.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-01-06   vm    Create.

===========================================================================*/

#include "tftp_config_i.h"
#include "tftp_os_wakelocks.h"

#if !defined (TFTP_NHLOS_BUILD)
  #error "This file should be included only for NHLOS Builds"
#endif

void
tftp_os_wakelocks_init(void)
{
}

void
tftp_os_wakelock(void)
{
}

void
tftp_os_wakeunlock(void)
{
}

