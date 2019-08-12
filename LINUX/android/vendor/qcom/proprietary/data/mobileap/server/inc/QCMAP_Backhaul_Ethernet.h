#ifndef _QCMAP_BACKHAUL_ETHERNET_H_
#define _QCMAP_BACKHAUL_ETHERNET_H_

/*======================================================

FILE:  QCMAP_Backhaul_Ethernet.h

SERVICES:
   QCMAP Connection Manager Backhaul Ethernet Class

=======================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

======================================================*/
/*======================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.
    when       who        what, where, why
  --------   ---        -------------------------------------------------------
  02/25/16    jc         Created
======================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "qcmap_cm_api.h"
#include "QCMAP_Tethered_Backhaul.h"

class QCMAP_Backhaul_Ethernet : public QCMAP_Tethered_Backhaul
{
private:
   static bool flag;
   static QCMAP_Backhaul_Ethernet *object;

   QCMAP_Backhaul_Ethernet();

public:
  static QCMAP_Backhaul_Ethernet *Get_Instance(boolean obj_create=false);

  ~QCMAP_Backhaul_Ethernet();

  /* --------------------Eth Backhaul Execution---------------------------*/


   /*V4 connection available on Eth*/
   boolean                     eth_v4_available;
   /*V6 connection available on Eth*/
   boolean                     eth_v6_available;
   qcmap_cm_eth_backhaul_conf_t eth_cfg;
   /* Use to store the gateway information from RA when acting as backhaul*/
   struct in6_addr ipv6_gateway_addr;

   static boolean IsEthBackhaulAvailableV4();
   static boolean IsEthBackhaulAvailableV6();
   /* Eth Mode */
   static boolean GetEthBackhaulMode( qcmap_msgr_eth_backhaul_mode_v01 *mode,
                                        qmi_error_type_v01 *qmi_err_num );

   static boolean SetEthBackhaulMode( qcmap_msgr_eth_backhaul_mode_v01 mode,
                                       void *softApHandle,
                                       qmi_error_type_v01 *qmi_err_num );

   /*Handle Address Assignment during Eth Association*/
   static void ProcessEthBackhaulAddrAssign(char* devname,
                                       ip_version_enum_type ip_vsn);

   /* Handle Eth Disassociation event. */
   void ProcessEthBackhaulDisAssoc();
   void DisableEthBackhaul(qmi_error_type_v01 *qmi_err_num);
   void SwitchToEthBackhaul();
};
#endif

