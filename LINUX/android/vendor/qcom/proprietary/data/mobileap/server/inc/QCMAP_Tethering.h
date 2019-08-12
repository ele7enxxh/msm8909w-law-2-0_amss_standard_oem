#ifndef _QCMAP_TETHERING_H_
#define _QCMAP_TETHERING_H_


/*====================================================

FILE:  QCMAP_Firewall.h

SERVICES:
   QCMAP Connection Manager Backhaul AP-STA Class

=====================================================

  Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.
    when       who        what, where, why
  --------   ---        -------------------------------------------------------
  08/19/14   vm           Created
===========================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include "stringl.h"
#include "ds_util.h"
#include "comdef.h"
#include "qcmap_cm_api.h"

#define TETH_IFACE_LL_ADDR1      "169.254.3.1"
#define TETH_IFACE_LL_ADDR2      "169.254.4.1"
#define USB_SUB_NET_MASK                "255.255.255.252"
#define PPP_IFACE "ppp0"

/* Use a dummy MAC for storing DUN client information. */
#define PPP_CLIENT_MAC_ADDR "ff:ff:ff:ff:ff:ff"

class QCMAP_Tethering
{
  private:
    static bool flag;
    static QCMAP_Tethering*object;
    QCMAP_Tethering();

  public:

    ~QCMAP_Tethering();

    uint32  ll_subnet_mask;
    qcmap_cm_tethered_conf_t  tethered_conf[QCMAP_QTI_MAX_TETH_LINK];
    uint8_t  ppp_ipv6_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
    uint8_t  ppp_ipv6_iid[QCMAP_MSGR_IPV6_ADDR_LEN_V01];

    static QCMAP_Tethering* Get_Instance(boolean obj_create=false);
    // Set Usb Neighor IP Addr
    static  void SetUSBNeighIP(uint32 ip);
    static uint32 GetUSBNeighIP(void );

    //Set USB MAC address
    static void SetUSBMac(uint8* usbMac);

    //Get USB MAC
    uint8* GetUSBMac(void );

    //Set Tethering Link enable
    void SetTethLinkEnable (int index, bool isLinkEnabled);

    //set V6 rules on iface
    static void SetTethRouteV6ParamsOnIface(boolean enableV6, char* iface);

    //Start DHCPD on iface
    static void AcceptIPAddrOnIfae(char* iface,
                                 boolean enableV4,
                                 boolean enableV6);

    //Get Tethering Link Status
    static bool GetTethLinkEnable (int index);

    // Delete Conntrack entries related to Connected Clients
    static void DelConntrack(uint32 ipv4addr);

    /* Tethering API for RNDIS/ECM/ETH */
    static boolean SetupTetheredLink(qcmap_qti_tethered_link_type tethered_link,int *err_num);
    static boolean BringDownTetheredLink(qcmap_qti_tethered_link_type tethered_link,
                                             void *softApHandle, int *err_num);

    static void StorePPPIPv6IID(uint8_t ppp_ip[]);

    /*Update the MAC and IP of the client on plug in*/
    static void UpdateMACandIPOnPlugIn (  char *iface );
};
#endif

