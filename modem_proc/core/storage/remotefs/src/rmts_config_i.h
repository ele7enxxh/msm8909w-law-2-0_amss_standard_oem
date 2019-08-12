/***********************************************************************
 * rmts_config_i.h
 *
 * Internal configurations for rmts
 * Copyright (C) 2014 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/remotefs/src/rmts_config_i.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-08-08   dks   Increase number of server reconnect retries
2014-07-22   dks   Create

===========================================================================*/

#ifndef __RMTS_CONFIG_I_H__
#define __RMTS_CONFIG_I_H__

#include "rmts_config.h"

#ifndef RMTS_MAX_EXTERNAL_CLIENTS
  #define RMTS_MAX_EXTERNAL_CLIENTS 9
#endif

#ifndef RMTS_MAX_CLIENT_NAME_LENGTH
  #define RMTS_MAX_CLIENT_NAME_LENGTH 16
#endif

#ifndef RFSA_MEM_SIZE
  #define RFSA_MEM_SIZE (64 *1024)
#endif

#ifndef RMTS_MAX_RECONNECT_RETRIES
  #define RMTS_MAX_RECONNECT_RETRIES 10
#endif

#endif /* not __RMTS_CONFIG_I_H__ */
