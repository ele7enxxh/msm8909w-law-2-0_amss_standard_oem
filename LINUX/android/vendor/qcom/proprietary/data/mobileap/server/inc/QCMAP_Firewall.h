#ifndef _QCMAP_FIREWALL_H_
#define _QCMAP_FIREWALL_H_


/*====================================================

FILE:  QCMAP_Firewall.h

SERVICES:
   QCMAP Connection Manager Backhaul AP-STA Class

=====================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*=====================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.
    when       who        what, where, why
  --------   ---        -------------------------------------------------------
  08/19/14   vm           Created
======================================================*/

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
#include "QCMAP_ConnectionManager.h"

typedef enum {
  CONFIG_FIREWALL_ENABLED = 1,
  CONFIG_FIREWALL_PACKETS_ALLOWED,
}qcmap_firewall_config_enum;


#define SEARCH_FIREWALL_ENTRY -1
#define CONNTRACK_ENTRIES "/etc/conntrack_entries.txt"
#define IPv6_CONNTRACK_FILTER_PATH "/etc/v6conntrack.txt"



class QCMAP_Firewall
{
  private:
    static bool flag;
    static QCMAP_Firewall*object;
    QCMAP_Firewall();

  public:
    qcmap_cm_firewall_conf_t firewall_config;
    static QCMAP_Firewall* Get_Instance(boolean obj_create=false);
     ~QCMAP_Firewall();

    /*Add an Extended Firewall rule to the configuration*/
    static int AddFireWallEntry( qcmap_msgr_firewall_conf_t* firewall_entry,
                                       qmi_error_type_v01 *qmi_err_num);

    int  SetFirewall( qcmap_msgr_firewall_entry_conf_t* firewall_entry,
                         boolean add_rule, qmi_error_type_v01 *qmi_err_num );

    bool DeleteConntrackEntryForDropIPv4FirewallEntries( qcmap_msgr_firewall_entry_conf_t *firewall_entry,
                                      uint8 protocol);

    bool DeleteConntrackEntryForAcceptIPv4FirewallEntries( qcmap_msgr_firewall_entry_conf_t *firewall_entry,
                                                               uint8 protocol_num);

    int  SetFirewallV4( qcmap_msgr_firewall_entry_conf_t* firewall_entry,
                            boolean add_rule, qmi_error_type_v01 *qmi_err_num );

    int  SetFirewallV6( qcmap_msgr_firewall_entry_conf_t* firewall_entry,
                            boolean add_rule, qmi_error_type_v01 *qmi_err_num );

    /*Get an Extended Firewall rule from the configuration*/
    static boolean GetFireWallEntry
                             ( qcmap_msgr_get_firewall_entry_resp_msg_v01* resp,
                               uint32_t handle, qmi_error_type_v01 *qmi_err_num );

    /*Delete extended firewall rule from the configuration*/
    static boolean DeleteFireWallEntry
                                    ( qcmap_msgr_firewall_entry_conf_t* firewall_entry,
                                      int firewall_handle,
                                      qmi_error_type_v01  *qmi_err_num );

    /*Get Firewall rule handles from the configuration*/
    static boolean GetFireWallHandleList
                                   ( qcmap_msgr_firewall_conf_t* firewall_entry,
                                     qmi_error_type_v01 *qmi_err_num );


    static boolean GetSetFirewallConfigFromXML( qcmap_action_type,
                                                         boolean*, boolean*, boolean*);
    boolean ReadFirewallXML();
    boolean GetFirewallEntryFromXML( pugi::xml_node,
                                               qcmap_msgr_firewall_entry_conf_t * );

    static pugi::xml_node searchFirewallByHandleInXML
                                       ( pugi::xml_document*,
                                         qcmap_msgr_firewall_entry_conf_t*,int );

    static pugi::xml_node searchFirewallByIndexInXML
                                       ( pugi::xml_document*,
                                         qcmap_msgr_firewall_entry_conf_t*,int );

    static int addFirewallEntryToXML( pugi::xml_document*,
                                               qcmap_msgr_firewall_entry_conf_t* );

    boolean addFirewallEntryToList( qcmap_msgr_firewall_entry_conf_t*,
                                           qcmap_msgr_firewall_entry_conf_t* );


    /*Read QCMAP extended firewall config from XML */
    static boolean ReadConfigFromFirewallXML(void);

    /*Write QCMAP extended firewall config to XML */
    static boolean WriteConfigToFirewallXML(void);

    static void WriteDefaultsToFirewallXML();

    boolean SetDefaultFirewallRule( qcmap_msgr_ip_family_enum_v01 interface );
    static void SetDefaultFirewall(void);
    boolean EnableIPV6DefaultFirewall(void);
    static boolean SetFirewallConfig( boolean, boolean,  boolean , qmi_error_type_v01* );
    static boolean GetFirewallConfig( boolean*, boolean *,boolean *, qmi_error_type_v01* );
    boolean EnableIPV6Firewall(void);
    void CleanIPv6MangleTable(void);
    void CleanIPv4MangleTable(void);
    static int GetFirewallEntryCountFromXML( pugi::xml_document * );
    void Dump_firewall_conf( qcmap_msgr_firewall_entry_conf_t *firewall_entry );

    void DeleteConntrackEntryForDropIPv6FirewallEntries(qcmap_msgr_firewall_entry_conf_t *firewall_entry,
                                                  uint8 protocol_num);

    void DeleteConntrackEntryForAcceptIPv6FirewallEntries(qcmap_msgr_firewall_entry_conf_t *firewall_entry,
                                                  uint8 protocol_num);
};

#endif

