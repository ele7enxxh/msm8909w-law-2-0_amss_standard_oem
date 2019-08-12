#ifndef _QCMAP_NATALG_H_
#define _QCMAP_NATALG_H_


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


typedef enum {
  CONFIG_NAT_ENTRY_GENERIC_TIMEOUT = 1,
  CONFIG_NAT_ENTRY_ICMP_TIMEOUT,
  CONFIG_NAT_ENTRY_TCP_ESTABLISHED_TIMEOUT,
  CONFIG_NAT_ENTRY_UDP_TIMEOUT,
  CONFIG_DMZ_IP,
  CONFIG_PPTP_VPN_PASSTHROUGH,
  CONFIG_L2TP_VPN_PASSTHROUGH,
  CONFIG_IPSEC_VPN_PASSTHROUGH,
  CONFIG_RTSP_ALG,
  CONFIG_SIP_ALG,
  CONFIG_NAT_TYPE
}qcmap_nat_config_enum;

class QCMAP_NATALG
{
  private:
    static bool flag;
    static QCMAP_NATALG*object;
    QCMAP_NATALG();

  public:
    qcmap_cm_nat_conf_t nat_config;
    // Data structure to store mask for ALGs Enabled/ Disabled.
    qcmap_msgr_alg_type_mask_v01 alg_enabled_disabled_mask;

    static QCMAP_NATALG* Get_Instance(boolean obj_create=false);

    ~QCMAP_NATALG();

    boolean AddSNATEntryOnApps
                                  ( qcmap_cm_port_fwding_entry_conf_t* nat_entry,
                                    qmi_error_type_v01 *qmi_err_num);

    boolean DeleteSNATEntryOnApps
                                  ( qcmap_cm_port_fwding_entry_conf_t* nat_entry,
                                    qmi_error_type_v01 *qmi_err_num);


    boolean AddDMZOnApps(uint32 dmz_ip, qmi_error_type_v01 *qmi_err_num);
    boolean DeleteDMZOnApps(uint32 dmz_ip, qmi_error_type_v01 *qmi_err_num);

    boolean SetIPSECVpnPassThroughOnApps( boolean enable,
                                                         qmi_error_type_v01 *qmi_err_num);

    boolean SetPPTPVpnPassThroughOnApps( boolean enable,
                                                         qmi_error_type_v01 *qmi_err_num);

    boolean SetL2TPVpnPassThroughOnApps( boolean enable,
                                                        qmi_error_type_v01 *qmi_err_num);

    /* NAT Type */
    static boolean SetNatType( qcmap_msgr_nat_enum_v01 nat_type,
                                    qmi_error_type_v01 *qmi_err_num);

    static boolean GetNatType( qcmap_msgr_nat_enum_v01 * cur_nat_type,
                                    qmi_error_type_v01 *qmi_err_num);

    /* NAT Timeout */
    static boolean SetNatTimeout
                                 ( qcmap_msgr_nat_timeout_enum_v01 timeout_type,
                                   uint32 timeout_value,
                                   qmi_error_type_v01 *qmi_err_num);

    static boolean GetNatTimeout
                                  ( qcmap_msgr_nat_timeout_enum_v01 timeout_type,
                                    uint32 *timeout_value,
                                    qmi_error_type_v01 *qmi_err_num);

    boolean SetNatTimeoutOnApps
                                 ( qcmap_msgr_nat_timeout_enum_v01 timeout_type,
                                   uint32 timeout_value,
                                   qmi_error_type_v01 *qmi_err_num);

    boolean GetNatTimeoutOnApps
                                  ( qcmap_msgr_nat_timeout_enum_v01 timeout_type,
                                    uint32 *timeout_value,
                                    qmi_error_type_v01 *qmi_err_num);

    static boolean GetSetPassthroughFlag( qcmap_nat_config_enum,
                                                   qcmap_action_type, uint8 *,
                                                   qmi_error_type_v01 *);


    /* Add a Static Nat Entry to the configuration and update XML file. */
    static boolean AddStaticNatEntry
                                  ( qcmap_cm_port_fwding_entry_conf_t* nat_entry,
                                    qmi_error_type_v01 *qmi_err_num);

    /* Delete a Static Nat Entry from the configuration and update XML file. */
    static boolean DeleteStaticNatEntry
                                 ( qcmap_cm_port_fwding_entry_conf_t* nat_entry,
                                   qmi_error_type_v01 *qmi_err_num);

    /* Get a Static Nat Entries from the configuration. */
    static boolean GetStaticNatEntries
                                 ( qcmap_msgr_snat_entry_config_v01 *snat_config,
                                   unsigned int* num_entries,
                                   qmi_error_type_v01 *qmi_err_num);

    /* Add a DMZ IP address to the configuration and update XML file. */
    static boolean AddDMZ(uint32 dmz_ip, qmi_error_type_v01 *qmi_err_num);

    /* Get the DMZ IP address from the configuration . */
    static boolean GetDMZ(uint32_t *dmz_ip, qmi_error_type_v01 *qmi_err_num);

    /* Delete a DMZ IP address from the configuration and update XML file. */
    static boolean DeleteDMZ(qmi_error_type_v01 *qmi_err_num);

    /* Set NAT Timeout in the configuration and update XML file. */
    static boolean SetNATEntryTimeout(uint16 timeout);

    /* Get NAT Timeout from the configuration and update XML file. */
    static uint16 GetNATEntryTimeout(void);

    /* Enable/Disable NAT on A5 (In Station mode) */
    boolean EnableNATonApps(void);
    boolean DisableNATonApps(void);


    /*Misc NAT tasks to be done after NAT types is set */
    boolean EnableMiscNATTasks(void);

    static boolean GetSetNATConfigFromXML( qcmap_nat_config_enum conf,
                                                        qcmap_action_type action,
                                                        char *data,
                                                        int data_len);

    boolean AddNATEntryToList(qcmap_cm_port_fwding_entry_conf_t*);

    static boolean GetStaticNATEntriesFromXML
                                             (qcmap_msgr_snat_entry_config_v01*,
                                              unsigned int*);

    static pugi::xml_node searchNATEntryInXML(pugi::xml_document*,
                                                       qcmap_cm_port_fwding_entry_conf_t*);

    boolean ReadNATConfigFromXML(char *);
    static int addNATEntryToXML( pugi::xml_document*,
                                           qcmap_cm_port_fwding_entry_conf_t*);

    /* Enable Algs */
    static boolean EnableAlg( qcmap_msgr_alg_type_mask_v01 alg_type,
                                  qmi_error_type_v01 *qmi_err_num);
    /* Disable Algs */
    static boolean DisableAlg( qcmap_msgr_alg_type_mask_v01 alg_type,
                                  qmi_error_type_v01 *qmi_err_num);
    /* Enable RTSP Alg */
    static boolean EnableRTSPAlg(qmi_error_type_v01 *qmi_err_num);
    /* Disable RTSP Alg */
    static boolean DisableRTSPAlg(qmi_error_type_v01 *qmi_err_num);

    static int GetNATEntryCountFromXML(pugi::xml_document*);
    boolean EnableSIPAlg(qmi_error_type_v01*);
    boolean DisableSIPAlg(qmi_error_type_v01*);


};

long int qcmap_compare_nat_entries(const void *first,const void *second);

#endif
