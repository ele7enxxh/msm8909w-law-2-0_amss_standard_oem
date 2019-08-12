/***********************************************************************
 * fds_config_i.h
 *
 * Private Configuration items for FDS
 * Copyright (C) 2013 QUALCOMM Technologies, Inc.
 *
 * Private Configuration items for Flash Driver Service.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fds/src/fds_config_i.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-09-30   dks   Create

===========================================================================*/

#ifndef __FDS_CONFIG_I_H__
#define __FDS_CONFIG_I_H__

#include "fds_config.h"

/* Maximum number of clients the service can supoort at any given time. */
#ifndef FDS_MAX_CLIENTS_SUPPORTED
  #define FDS_MAX_CLIENTS_SUPPORTED (4)
#endif

/* Maximum events that the service needs to keep track of to send indications
   to registered clients. */
#define FDS_MAX_EVENTS_SUPPORTED (1)


#endif /* not __FDS_CONFIG_I_H__ */
