#ifndef DS_NET_MEM_CONFIG_H
#define DS_NET_MEM_CONFIG_H

/*===========================================================================
  @file DSNetMemConfig.h

  This file provides macros to configure memory buffers for ds Network and
  Sockets libraries during compile time.

  TODO: Write detailed explanation.

  Copyright (c) 2008-2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/inc/ds_Net_MemConfig.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
2008-03-15 pp  CMI: Public/Private split.
2008-06-27 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "dssocket_defs.h"
#include "dss_config.h"

/*===========================================================================

                     PUBLIC DATA DEFINITIONS

===========================================================================*/

/*-------------------------------------------------------------------------
  Max number of network objects available system wide.
  We have DSS_MAX_APPS objects of type ds::Net::NetworkActive.
  We have DSS_MAX_APPS objects of type ds::Net::NetworkMonitored - primary.
--------------------------------------------------------------------------*/
#define MAX_NETWORK_ACTIVE_OBJS    (DSS_MAX_APPS)
#define MAX_NETWORK_MONITORED_OBJS (DSS_MAX_APPS)
#define MAX_NETWORK_OBJS          (MAX_NETWORK_ACTIVE_OBJS + MAX_NETWORK_MONITORED_OBJS)

/*-------------------------------------------------------------------------
  Max number of Default QoS objects available system wide.
--------------------------------------------------------------------------*/
#define MAX_QOS_DEFAULT_OBJS      (MAX_NETWORK_OBJS)

/*-------------------------------------------------------------------------
  Max number of Secondary QoS objects available system wide.
  This number is heuristically determined by the currently expected 
  secondary QoS usage.
--------------------------------------------------------------------------*/
#define MAX_QOS_SECONDARY_OBJS    (32)

/*-------------------------------------------------------------------------
  Max number of MCast Flows available system wide (heuristic estimate).
--------------------------------------------------------------------------*/
#define MAX_MCAST_OBJS            (32)

/*-------------------------------------------------------------------------
  Max number of technology specific objects (heuristic estimate).
--------------------------------------------------------------------------*/
#define MAX_NETWORK_1X_OBJS       (20)
#define MAX_NETWORK_UMTS_OBJS     (20)
#define MAX_NETWORK_MBMS_OBJS     (10)
#define MAX_NETWORK_BCMCS_OBJS    (10)
#define MAX_QOS_1X_OBJS           (MAX_QOS_SECONDARY_OBJS)

/*-------------------------------------------------------------------------
  Max number of phys link objects available system wide.
--------------------------------------------------------------------------*/
#define MAX_PHYS_LINK_OBJS        (MAX_QOS_SECONDARY_OBJS)

/*-------------------------------------------------------------------------
  Max number of network policy objects available system wide.
--------------------------------------------------------------------------*/
#define MAX_POLICY_OBJS           (50)

/*-------------------------------------------------------------------------
  Max number of network policy objects available system wide.
--------------------------------------------------------------------------*/
#define MAX_EVENT_NODE_OBJS       (200)

#endif /* DS_NET_MEM_CONFIG_H */

