#ifndef DS_UTILS_MEM_CONFIG_H
#define DS_UTILS_MEM_CONFIG_H

/*==========================================================================*/ 
/*! 
  @file 
  ds_Utils_MemConfig.h

  @brief
  This file is used to configure the number of buffers used for ds Utils
  items. 

  @details
  The buffer numbers are currently configured to suit the AMSS environment.
  Here, there is only one instance of the DSNET/DSSOCK library that is
  statically linked with all applications. AMSS supports around 20 
  applications and around 50 sockets. 

  For Linux/WinMobile environment, DSSOCK/DSNET are linked with each 
  application as a library. Hence all the buffers would need correct
  configuring for 3rd party OS platforms.
  
  @see ps_mem_get_buf()

  Copyright (c) 2008-2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ds_Utils_MemConfig.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-11-01 rk  Removed feature mobileap.
  2008-06-27 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

/*===========================================================================

                     PUBLIC DATA DEFINITIONS

===========================================================================*/

// TODO: define this number using constant definitions. 

/*! 
  This constant defines the number of signal objects. The same number is
  used for signalctl objects as well 

  @details
  Signals are used by the following objects:
    Object          Num events    Num Objects (approx)
    Network         6             20 - should be DSS_MAX_APPS
    Network1x       3             4
    MTPD            1             2
    NetworkMBMS     1             2
    MCastSession    2             32
    IPFilter        1             20
    QoSDefault      3             20
    QoSSecondary    3             16
    PhysLink        1             16
    Socket          5             50 - should be DSS_MAX_SOCKS

    Since all signals for all objects would not be used at the same time,
    we are currently setting the number of signals to 500 as the safe bet.

    New signals after refactoring:
    1. Some of DSS events handler objects were split to several objects,
    in accordance with the number of events they handle.
    Each of the new objects handle a single event. Therefore, each new object requires a new signal.
      a.	DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SUCCESS_EV: 1 new signal
      b.	DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA_SUCCESS_EV:  1 new signal
      c.	DSS_IFACE_IOCTL_QOS_AWARE_SYSTEM_EV: 1 new signal
      d.	DSSNetworkStateHandler: 3 new signals
      e.	DSSPhysLinkStateHandler: 3 new signals
      Total: 9 new signals

      New signals required: DSS_MAX_APPS * 9

    2. After splitting the dsnet event QDS_EV_STATE_CHANGED.
    The following list includes changes in signal memory allocation:
      a.	1 new signal in DSSNetActive. DSS_MAX_APPS*1 new signals.
      b.	1 new signal in ds::sock::Socket and 1 new signal in ds::sock::NetworkCtl.

      New signals required: DSS_MAX_APPS * 1 + DSS_MAX_SOCKS * 2
*/

#ifndef FEATURE_DATA_PS_LOW_MEM_CHIPSET
  #define MAX_SIGNAL_OBJS           (600 + DSS_MAX_APPS * 10 + DSS_MAX_SOCKS * 2)
#else  /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */
  #define MAX_SIGNAL_OBJS           (300 + DSS_MAX_APPS * 10 + DSS_MAX_SOCKS * 2)
#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

// For each of the classes shown below the number following
// is the number of signal bus members included in a single object
// MAX_SIGNAL_BUS_PER_APP definition includes the sum of signal bus objects appearing in the classes.
// ds::Net::MTPDReg              1
// ds::Net::Network              0 (actually 7 but will be calculated in its inheriting classes)
// ds::Net::NetworkActive        7 (inherited from ds::Net::Network)
// ds::Net::NetworkMonitored     18 or 19 (of them 7 inherited from ds::Net::Network) 7+11+FAST_DORM_STATUS_SIGNAL_OBJ
// ds::Net::NetworkIPv6          1
// ds::Net::Network1X            3
// ds::Net::NetworkIPv6Address   1
// ds::Net::PhysLink             1
// ds::Net::QoSDefault           3
// ds::Net::QoSManager           1
// ds::Net::QoSNetInitiated      2
// ds::Net::QoSNetInitiatedReq   1
// ds::Net::QoSSecondary         3
#ifndef FEATUTE_DATA_PS_MCAST
#define MAX_MCAST_SIGNAL_OBJS    0
#else  /* FEATUTE_DATA_PS_MCAST */
#define MAX_MCAST_SIGNAL_OBJS    6
// ds::Net::MCastMBMSCtrl        1
// ds::Net::MCastSessionPriv     3
// ds::Net::MCastSession         2
#endif /* FEATUTE_DATA_PS_MCAST */
#define FAST_DORM_STATUS_SIGNAL_OBJ 0 // mpSigBusFastDormStatus is not used
//SocketEvent::NUM_EV - READ, WRITE, CLOSE, ACCEPT & PLATFORM_ENABLED
#define ON_DEMAND_SIGNAL_BUS_PER_SOCKET (5)// SocketEvent::NUM_EV

#define MAX_SIGNAL_BUS_PER_APP      (42 + MAX_MCAST_SIGNAL_OBJS + FAST_DORM_STATUS_SIGNAL_OBJ)

//each socket has a single critical section per socket,
// plus five critical sections matching the five on demand signal busses (one per event)
#define CRIT_SECT_EXTRA_SOCKET      (ON_DEMAND_SIGNAL_BUS_PER_SOCKET + 1)
#define CRIT_SECT_EXTRA_SOCKET_TOTAL (CRIT_SECT_EXTRA_SOCKET * DSS_MAX_SOCKS)
#endif /* DS_UTILS_MEM_CONFIG_H */


