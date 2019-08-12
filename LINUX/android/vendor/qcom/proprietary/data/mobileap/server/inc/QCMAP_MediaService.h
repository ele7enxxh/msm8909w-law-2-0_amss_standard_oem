#ifndef _QCMAP_MEDIASERVICE_H_
#define _QCMAP_MEDIASERVICE_H_
/*====================================================

FILE:  QCMAP_MediaService.h

SERVICES:
   QCMAP Connection Manager Service Class

=====================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=====================================================*/
/*=====================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.
    when       who        what, where, why
  --------   ---        -------------------------------------------------------
  07/30/14    us           Created
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
#include "ds_qmi_qcmap_msgr.h"

#define QCMAP_CM_LOG_FUNC_ENTRY()  \
    QCMAP_CM_LOG                   \
    (                              \
        "Entering function %s\n",  \
        __FUNCTION__               \
    )
#define QCMAP_CM_LOG_FUNC_EXIT()   \
    QCMAP_CM_LOG                   \
    (                              \
        "Exiting function %s\n",   \
        __FUNCTION__               \
    )

//Media service names.
#define DLNA_SERVICE "minidlnad"
#define UPNP_SERVICE "miniupnpd"

typedef enum {
CONFIG_UPNP = 1,
CONFIG_DLNA,
CONFIG_MDNS,
CONFIG_DLNAWhitelisting
}qcmap_service_config_enum;


typedef enum {
 MEDIA_SVC_MODE_UP = 1,
 MEDIA_SVC_MODE_DOWN
}qcmap_media_svc_status;

class QCMAP_MediaService
{
private:
  static bool flag;
  static QCMAP_MediaService *object;
  QCMAP_MediaService();
public:
  qcmap_cm_dlna_whitelist_conf_t dlna_whitelist_config;
  static QCMAP_MediaService *Get_Instance(boolean b);
   ~QCMAP_MediaService();

  /* QCMAP CM Config */
  boolean                     qcmap_enable;
  //qcmap_cm_conf_t             cfg;
  qcmap_cm_srvc_conf_t  srvc_cfg;//for storing service specific config


  // Internal UPNP state flag
  qcmap_msgr_upnp_mode_enum_v01                 upnp_state;
  /* Enable UPnP daemon */
  static boolean EnableUPNP(qmi_error_type_v01 *qmi_err_num);
  /* Disable UPnP daemon */
  static boolean DisableUPNP(qmi_error_type_v01 *qmi_err_num);
  /* Interim Disable UPnP daemon */
  boolean InterimDisableUPNP(qmi_error_type_v01 *qmi_err_num);
  /* Return status of the UPnP daemon */
  static boolean GetUPNPStatus(qcmap_msgr_upnp_mode_enum_v01 *status,\
                                      qmi_error_type_v01 *qmi_err_num);

  // Internal DLNA state flag
  qcmap_msgr_dlna_mode_enum_v01                 dlna_state;
  /* Enable DLNA daemon */
  static boolean EnableDLNA(qmi_error_type_v01 *qmi_err_num);
  /* Disable DLNA daemon */
  static boolean DisableDLNA(qmi_error_type_v01 *qmi_err_num);
  /* Interim Disable DLNA daemon */
  boolean InterimDisableDLNA(qmi_error_type_v01 *qmi_err_num);
  /* Restart DLNA daemon */
  boolean RestartDLNA(qmi_error_type_v01 *qmi_err_num);
  /* Return status of the DLNA daemon */
  static boolean GetDLNAStatus(qcmap_msgr_dlna_mode_enum_v01 *status,\
                                      qmi_error_type_v01 *qmi_err_num);
  /* Set the DLNA Media Dir */
  static boolean SetDLNAMediaDir(char media_dir[], qmi_error_type_v01 *qmi_err_num);
  /* Get the DLNA Media Dir */
  static boolean GetDLNAMediaDir(char media_dir[], qmi_error_type_v01 *qmi_err_num);

  // Internal M-DNS state flag
  qcmap_msgr_mdns_mode_enum_v01                 mdns_state;
  /* Enable M-DNS daemon */
  static boolean EnableMDNS(qmi_error_type_v01 *qmi_err_num);
  /* Interim Disable MDNS daemon*/
  boolean InterimDisableMDNS(qmi_error_type_v01 *qmi_err_num);
  /* Disable M-DNS daemon */
  static boolean DisableMDNS(qmi_error_type_v01 *qmi_err_num);
  /* Return status of the UPnP daemon */
  static boolean GetMDNSStatus( qcmap_msgr_mdns_mode_enum_v01 *status,
                                       qmi_error_type_v01 *qmi_err_num);
  /* Set/Get the UPnP notify interval */
  static boolean SetUPNPNotifyInterval( int notify_int,
                                               qmi_error_type_v01 *qmi_err_num);
  static boolean GetUPNPNotifyInterval( int *notify_int,
                                                qmi_error_type_v01 *qmi_err_num);
  /* Set/Get the DLNA notify interval */
  static boolean SetDLNANotifyInterval( int notify_int,
                                               qmi_error_type_v01 *qmi_err_num);
  static boolean GetDLNANotifyInterval( int *notify_int,
                                               qmi_error_type_v01 *qmi_err_num);
  boolean IsDLNAKilled();
  boolean ReadServiceConfigFromXML(void);
 static boolean GetSetServiceConfigFromXML
                                               ( qcmap_service_config_enum conf,
                                                 qcmap_action_type action,
                                                 qcmap_media_svc_status *state);
  boolean IsMediaServiceKilled(char *service);
  static boolean GetDLNAWhitelisting( int *status,
                                     qmi_error_type_v01 *qmi_err_num );

  static boolean SetDLNAWhitelisting
  (
    int status,
    qmi_error_type_v01 *qmi_err_num
  );
  static boolean AddDLNAWhitelistConnection
                                          (
                                            uint32_t dlna_whitelist_ip,
                                            qmi_error_type_v01 *qmi_err_num
                                          );

  boolean AddDLNAWhitelistIPToList (  uint32_t *dlna_whitelist_ip);

  static void AddDLNAWhitelistingIPToXML
                                         (
                                            pugi::xml_document *xml_file,
                                            uint32_t *dlna_whitelist_ip
                                         );
  static pugi::xml_node searchDLNAWhitelistIPInXML
                                           (
                                             pugi::xml_document *xml_file,
                                             uint32_t *dlna_whitelist_ip
                                           );
  static boolean DeleteDLNAWhitelistingIPEntry
                                           (
                                             uint32_t dlna_whitelist_ip,
                                             qmi_error_type_v01 *qmi_err_num
                                           );
  boolean dlna_whitelisting_state;
  boolean ReadDLNAWhitelistIPFromXML(char *xml_path,bool check);
  boolean SetDLNAWhitelistRule( uint32_t *dlna_whitelist_ip ,
                                      boolean add_rule,
                                      qmi_error_type_v01 *qmi_err_num);
  boolean GetDLNAPortNumber( int *portno,
                                       qmi_error_type_v01 *qmi_err_num);
};
#endif

